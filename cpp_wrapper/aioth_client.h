#ifndef AIOTH_CLIENT_H
#define AIOTH_CLIENT_H

#include <functional>
#include <string>
#include <stdexcept>
#ifdef _WINDOWS
#include <mutex>
#else
#include <pthread.h>
#endif
#include <vector>
#include <malloc.h>

#include "iothub_client.h"
#include "iothub_message.h"
#include "iothubtransportmqtt.h"
#include "iothub_client_options.h"
#include "iothub_client.h"
#include "iothub_message.h"
#include "azure_c_shared_utility/threadapi.h"
#include "azure_c_shared_utility/crt_abstractions.h"
#include "azure_c_shared_utility/platform.h"
#include "iothubtransportmqtt.h"
#include "iothub_client_options.h"
#include "iothub_device_client.h"
#include "azure_c_shared_utility/macro_utils.h"
#include "azure_c_shared_utility/threadapi.h"
#include "azure_c_shared_utility/platform.h"
#include "iothub_device_client.h"
#include "iothub_client_options.h"
#include "iothub.h"
#include "iothub_message.h"

namespace aioth
{
	class platform
	{
	public:
		static bool is_initialized;
	};

	typedef unsigned long int messageID;

	struct key_value_pair
	{
		std::string key;
		std::string value;
	};
	typedef std::vector<key_value_pair> property_array;

#ifdef _WINDOWS
	static std::mutex message_id_mutex;
#else
	static pthread_mutex_t message_id_mutex = PTHREAD_MUTEX_INITIALIZER;
#endif

	class message
	{
	private:
		static messageID message_counter;
		messageID id;
		std::string payload;
	public:
		// Constructors
		message(const std::string payload);
		message(const char *payload);
		message(IOTHUB_MESSAGE_HANDLE handle);
		void init(const std::string payload);
		message(const message &old) { id = old.id; callback = old.callback; payload = old.payload; handle = old.handle; }

		// Getters
		messageID get_id(void);
		inline std::string get_payload(void) { return this->payload; }

		// Public properties
		IOTHUB_MESSAGE_HANDLE handle;
		std::function<void(message *message, bool success)> callback;

		// Properties
		property_array get_properties();
		void add_property(const std::string key, const std::string value);
		void add_property(const key_value_pair kvp);
	};

	// Function prototypes
	typedef std::function<void(message)> message_received_function;
	typedef std::function<int(std::string name, std::string payload, std::string &response)> dm_callback_function;

	class client
	{
	public:
		// Constructors
		inline client() { }
		client(const std::string connection_string);
		client(const char* connection_string);
		~client();

		// Connector and disconnector
		bool connect(const std::string connection_string);
		bool connect(const char* connection_string);
		void disconnect();

		// Work function (call often)
		void do_work(void);

		// Device to cloud message
		void send(message evt);

		// Cloud to device
		static IOTHUBMESSAGE_DISPOSITION_RESULT received_message_handler(IOTHUB_MESSAGE_HANDLE message, void* userContextCallback);
		void set_received_message_callback(message_received_function rcb);

		// Direct method
		static int deviceMethodCallback(const char* method_name, const unsigned char* payload, size_t size, unsigned char** response, size_t* response_size, void* userContextCallback);
		void set_direct_method_callback(dm_callback_function callback);

		// Device twin
		bool send_state(const std::string state);
		static void reported_state_handler(int status_code, void* userContextCallback);
		void set_reported_state_callback(const std::function<void(void)> callback);
		static void deviceTwinCallback(DEVICE_TWIN_UPDATE_STATE update_state, const unsigned char* payLoad, size_t size, void* userContextCallback);
		void set_twin_update_callback(std::function<void(const std::string)> callback);
	private:
		void create_client_handle(const char* connection_string);
		IOTHUB_CLIENT_LL_HANDLE client_handle;

		// Send message
		static void sent_message_handler(IOTHUB_CLIENT_CONFIRMATION_RESULT result, void* userContextCallback);
#ifdef _WINDOWS
		std::mutex message_queue_mutex;
#else
		pthread_mutex_t message_queue_mutex = PTHREAD_MUTEX_INITIALIZER;
#endif
		std::vector<message> message_queue;

		// Received message
		message_received_function received_message_callback;

		// Direct method
		dm_callback_function dm_callback;

		// Device twin
		std::function<void(void)> twin_report_callback;
		std::function<void(const std::string)> twin_update_callback;
	};
}

#endif