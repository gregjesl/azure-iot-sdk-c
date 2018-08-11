#include "aioth_service.h"

aioth::service::client::client(const std::string connection_string)
{
	this->create_handle(connection_string.c_str());
}

aioth::service::client::client(const char* connection_string)
{
	this->create_handle(connection_string);
}

aioth::service::client::~client()
{
	IoTHubMessaging_Close(this->messenger_handle);
	IoTHubMessaging_Destroy(this->messenger_handle);
	IoTHubServiceClientAuth_Destroy(this->handle);
}

void aioth::service::client::create_handle(const char* connection_string)
{
	if (!aioth::platform::is_initialized)
	{
		if (platform_init() != 0)
		{
			throw std::runtime_error("Failed to initialize the platform.\r\n");
		}
		aioth::platform::is_initialized = true;
	}

	if ((this->handle = IoTHubServiceClientAuth_CreateFromConnectionString(connection_string)) == NULL)
	{
		throw std::runtime_error("Could not open connection");
	}
	if ((this->messenger_handle = IoTHubMessaging_Create(this->handle)) == NULL)
	{
		throw std::runtime_error("Could not create messenger");
	}
	if (IoTHubMessaging_Open(this->messenger_handle, aioth::service::client::on_messenger_opened, this) != IOTHUB_MESSAGING_OK)
	{
		throw std::runtime_error("Could not open messenger");
	}
}

void aioth::service::client::do_work()
{
	// IoTHubMessaging_LL_DoWork(this->messenger_handle);
}

bool aioth::service::client::send(aioth::message evt, const std::string deviceID)
{
	// evt.wtf = std::string(deviceID);

	if (!this->messenger_open)
	{
		this->message_backlog.push_back(evt);
	}
	aioth::message *evt_ptr = new aioth::message(evt);
	return IoTHubMessaging_SendAsync(this->messenger_handle, deviceID.c_str(), evt.handle, aioth::service::client::on_message_sent, evt_ptr) == IOTHUB_MESSAGING_OK;
}

void aioth::service::client::on_message_sent(void* context, IOTHUB_MESSAGING_RESULT messagingResult)
{
	aioth::message* evt = (aioth::message*)context;
	if (evt->callback != NULL)
	{
		evt->callback(evt, messagingResult == IOTHUB_MESSAGING_OK);
	}
	delete evt;
}

void aioth::service::client::on_messenger_opened(void *context)
{
	aioth::service::client *callback_client = (aioth::service::client*)context;
	callback_client->send_backlog();
}

void aioth::service::client::send_backlog()
{
	this->messenger_open = true;
	for (size_t i = 0; i < this->message_backlog.size(); i++)
	{
		// this->send(this->message_backlog.at(i), this->message_backlog.at(i).wtf);
	}
}