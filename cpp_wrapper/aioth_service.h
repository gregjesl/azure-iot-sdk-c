#ifndef AIOTH_SERVICE_H
#define AIOTH_SERVICE_H

#include <string>
#include <stdexcept>

#include "aioth_client.h"

#include "azure_c_shared_utility/crt_abstractions.h"
#include "azure_c_shared_utility/platform.h"
#include "azure_c_shared_utility/map.h"     // Needed for message properties
#include "azure_c_shared_utility/singlylinkedlist.h" // Needed for feedback info
#include "azure_c_shared_utility/threadapi.h"

#include "iothub_service_client_auth.h"
#include "iothub_messaging.h"

namespace aioth
{
	namespace service
	{
		class client
		{
		public:
			client(const std::string connection_string);
			client(const char *connection_string);
			~client();

			// Open messenger
			static void on_messenger_opened(void *context);

			// Do Work
			void do_work();

			// Send message
			bool send(aioth::message evt, const std::string deviceID);
			static void on_message_sent(void* context, IOTHUB_MESSAGING_RESULT messagingResult);
			void send_backlog();
		private:
			void create_handle(const char* connection_string);
			IOTHUB_SERVICE_CLIENT_AUTH_HANDLE handle;

			// Messaging
			IOTHUB_MESSAGING_CLIENT_HANDLE messenger_handle;
			bool messenger_open = false;
			std::vector<aioth::message> message_backlog;
		};

		class messenger_open_context
		{
		public:
			client client_callback;
		};
	}
}

#endif // !AIOTH_SERVICE_H
