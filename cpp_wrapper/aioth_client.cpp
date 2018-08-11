#include "aioth_client.h"

bool aioth::platform::is_initialized = false;
aioth::messageID aioth::message::message_counter = 0;

/* Message */

aioth::message::message(const std::string payload)
{
	this->init(payload);
	/*
	if ((this->handle = IoTHubMessage_CreateFromString(payload.c_str())) == NULL)
	{
		throw std::runtime_error("Could not create message");
	}
	*/
}

aioth::message::message(const char *payload)
{
	this->init(std::string(payload));
}

aioth::message::message(IOTHUB_MESSAGE_HANDLE handle)
{
	this->payload = std::string(IoTHubMessage_GetString(handle));
	this->handle = handle;
}

void aioth::message::init(const std::string msg_payload)
{
#ifdef _WINDOWS
	aioth::message_id_mutex.lock();
	this->id = aioth::message::message_counter++;
	aioth::message_id_mutex.unlock();
#else
	pthread_mutex_lock(&aioth::message_id_mutex);
	this->id = aioth::message::message_counter++;
	pthread_mutex_unlock(&aioth::message_id_mutex);
#endif
	this->payload = msg_payload;
	this->callback = NULL;
}

aioth::messageID aioth::message::get_id(void)
{
	return this->id;
}

void aioth::message::add_property(const std::string key, const std::string value)
{
	IoTHubMessage_SetProperty(this->handle, key.c_str(), value.c_str());
}

void aioth::message::add_property(const key_value_pair kvp)
{
	this->add_property(kvp.key, kvp.value);
}

aioth::property_array aioth::message::get_properties()
{
	MAP_HANDLE map = IoTHubMessage_Properties(this->handle);
	if (map != NULL)
	{
		aioth::property_array result;
		const char*const* keys;
		const char*const* values;
		size_t propertyCount = 0;
		if (Map_GetInternals(map, &keys, &values, &propertyCount) == MAP_OK)
		{
			if (propertyCount > 0)
			{
				size_t index;

				for (index = 0; index < propertyCount; index++)
				{
					aioth::key_value_pair pair;
					pair.key = std::string(keys[index]);
					pair.value = std::string(values[index]);
				}
				
			}
		}
	}
	throw std::runtime_error("Could not get properties");
}

/* end message */

/* client */

void aioth::client::sent_message_handler(IOTHUB_CLIENT_CONFIRMATION_RESULT result, void* userContextCallback)
{
	aioth::message *msg= (aioth::message*)userContextCallback;
	if (msg->callback != NULL)
	{
		msg->callback(msg, result == IOTHUB_CLIENT_CONFIRMATION_OK);
	}
	IoTHubMessage_Destroy(msg->handle);
	delete msg;
}

aioth::client::client(const std::string connection_string)
{
	if (!this->connect(connection_string))
	{
		delete this;
	}
}

aioth::client::client(const char *connection_string)
{
	if (!this->connect(connection_string))
	{
		delete this;
	}
}

bool aioth::client::connect(const std::string connection_string)
{
	return this->connect(connection_string.c_str());
}

bool aioth::client::connect(const char* connection_string)
{
	try
	{
		this->create_client_handle(connection_string);
	}
	catch (std::exception)
	{
		return false;
	}
	return true;
}

void aioth::client::disconnect()
{
	IoTHubClient_LL_Destroy(this->client_handle);
}

void aioth::client::set_received_message_callback(aioth::message_received_function rcb)
{
	if (IoTHubClient_LL_SetMessageCallback(this->client_handle, aioth::client::received_message_handler, this) != IOTHUB_CLIENT_OK)
	{
		throw std::runtime_error("Could not set received message callback");
	}
	this->received_message_callback = rcb;
}

aioth::client::~client()
{
	this->disconnect();
}

void aioth::client::do_work(void)
{
	// Check for messages to send
#ifdef _WINDOWS
	this->message_queue_mutex.lock();
#else
	pthread_mutex_lock(&this->message_queue_mutex);
#endif
	if (this->message_queue.size() > 0)
	{
		aioth::message *msg;

		for (size_t i = 0; i < this->message_queue.size(); i++)
		{
			msg = new aioth::message(this->message_queue[i]);
			if ((msg->handle = IoTHubMessage_CreateFromString(msg->get_payload().c_str())) == NULL)
			{
				throw std::runtime_error("Could not create message handle");
			}

			// Send it! 
			if (IoTHubClient_LL_SendEventAsync(this->client_handle, msg->handle, aioth::client::sent_message_handler, msg) != IOTHUB_CLIENT_OK)
			{
				throw std::runtime_error("Could not send message");
			}
		}

		this->message_queue.clear();
	}
#ifdef _WINDOWS
	this->message_queue_mutex.unlock();
#else
	pthread_mutex_unlock(&this->message_queue_mutex);
#endif

	IoTHubClient_LL_DoWork(this->client_handle);
}

void aioth::client::create_client_handle(const char *connection_string)
{
	// Initialize the platform if not already initialized
	if (!aioth::platform::is_initialized)
	{
		if (platform_init() != 0)
		{
			throw std::runtime_error("Failed to initialize the platform.\r\n");
		}
		aioth::platform::is_initialized = true;
	}

	if ((this->client_handle = IoTHubClient_LL_CreateFromConnectionString(connection_string, MQTT_Protocol)) == NULL)
	{
		throw std::runtime_error("Could not create client handle");
	}
	if (IoTHubClient_LL_SetMessageCallback(this->client_handle, aioth::client::received_message_handler, NULL) != IOTHUB_CLIENT_OK)
	{
		throw std::runtime_error("Could not create received message handler");
	}
}

void aioth::client::send(const aioth::message message)
{
#ifdef _WINDOWS
	this->message_queue_mutex.lock();
#else
	pthread_mutex_lock(&this->message_queue_mutex);
#endif
	this->message_queue.push_back(message);
#ifdef _WINDOWS
	this->message_queue_mutex.unlock();
#else
	pthread_mutex_unlock(&this->message_queue_mutex);
#endif
}

IOTHUBMESSAGE_DISPOSITION_RESULT aioth::client::received_message_handler(IOTHUB_MESSAGE_HANDLE message, void* userContextCallback)
{
	const char* messageId;

	if (userContextCallback == NULL)
	{
		return IOTHUBMESSAGE_ACCEPTED;
	}
	
	// Get the client
	aioth::client* ptr = (aioth::client*)userContextCallback;

	if (ptr->received_message_callback == NULL)
	{
		return IOTHUBMESSAGE_ACCEPTED;
	}

	// Message properties
	if ((messageId = IoTHubMessage_GetMessageId(message)) == NULL)
	{
		messageId = "<null>";
	}

	try
	{
		ptr->received_message_callback(aioth::message(message));
	}
	catch (std::exception)
	{
		return IOTHUBMESSAGE_ABANDONED;
	}
	
	return IOTHUBMESSAGE_ACCEPTED;
}

int aioth::client::deviceMethodCallback(const char* method_name, const unsigned char* payload, size_t payload_size, unsigned char** response, size_t* response_size, void* userContextCallback)
{
	aioth::client *ptr = (aioth::client*)userContextCallback;
	if (ptr == NULL)
	{
		return -1;
	}

	int result;
	std::string str_response;

	try
	{
		std::string name(method_name);
		std::string str_payload = std::string((char*)payload).substr(0, payload_size);
		result = ptr->dm_callback(name, str_payload, str_response);
	}
	catch (std::exception ex)
	{
		printf("Error: %s\n", ex.what());
		std::string error = "{\"error\":\"Direct method callback function threw an exception\"}";
		*response = (unsigned char *)malloc(error.size() * sizeof(unsigned char));
		memcpy(*response, (unsigned char*)error.c_str(), error.size() * sizeof(unsigned char));
		*response_size = error.size();
		return -1;
	}

	*response = (unsigned char *)malloc(str_response.size() * sizeof(unsigned char));
	memcpy(*response, (unsigned char*)str_response.c_str(), str_response.size() * sizeof(unsigned char));
	*response_size = str_response.size();

	return result;
}

void aioth::client::set_direct_method_callback(aioth::dm_callback_function callback)
{
	if (IoTHubClient_LL_SetDeviceMethodCallback(this->client_handle, aioth::client::deviceMethodCallback, this) != IOTHUB_CLIENT_OK)
	{
		throw std::runtime_error("Could not set direct method callback");
	}
	this->dm_callback = callback;
}

bool aioth::client::send_state(const std::string state)
{
	if (this->twin_report_callback != NULL)
	{
		return IoTHubClient_LL_SendReportedState(this->client_handle, (const unsigned char*)state.c_str(), state.size(), aioth::client::reported_state_handler, this) == IOTHUB_CLIENT_OK;
	}
	else
	{
		return IoTHubClient_LL_SendReportedState(this->client_handle, (const unsigned char*)state.c_str(), state.size(), NULL, NULL) == IOTHUB_CLIENT_OK;
	}
}

void aioth::client::reported_state_handler(int status_code, void *userContextCallback)
{
	(void)status_code;
	aioth::client *ptr = (aioth::client*)userContextCallback;
	if (ptr == NULL)
	{
		return;
	}

	if (ptr->twin_report_callback != NULL)
	{
		ptr->twin_report_callback();
	}
}

void aioth::client::set_reported_state_callback(std::function<void(void)> callback)
{
	this->twin_report_callback = callback;
}

void aioth::client::deviceTwinCallback(DEVICE_TWIN_UPDATE_STATE update_state, const unsigned char* payload, size_t size, void* userContextCallback)
{
	(void)update_state;

	aioth::client *ptr = (aioth::client*)userContextCallback;
	if (ptr == NULL)
	{
		return;
	}

	if (ptr->twin_update_callback != NULL)
	{
		std::string str_payload = std::string((char*)payload).substr(0, size);
		ptr->twin_update_callback(str_payload);
	}
}

void aioth::client::set_twin_update_callback(std::function<void(const std::string)> callback)
{
	if (IoTHubClient_LL_SetDeviceTwinCallback(this->client_handle, aioth::client::deviceTwinCallback, this) != IOTHUB_CLIENT_OK)
	{
		throw std::runtime_error("Could not set direct method callback");
	}
	this->twin_update_callback = callback;
}