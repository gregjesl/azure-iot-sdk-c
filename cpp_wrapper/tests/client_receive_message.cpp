#include "connection.h"
#include "aioth_client.h"
#include "aioth_service.h"

#include <assert.h>

bool callback_received = false;

void callback(aioth::message evt)
{
	assert(evt.get_payload() == "Hello world");
	aioth::property_array props = evt.get_properties();
	assert(props.size() == 1);
	assert(props.at(0).key == "Hello");
	assert(props.at(0).value == "world");

	callback_received = true;
}

int main(void)
{
	// Create the cloud service client
	aioth::service::client service(TEST_SERVICE_CONNECTION_STRING);

	// Create the device client
	aioth::client device(TEST_DEVICE_CONNECTION_STRING);

	// Set callback
	device.set_received_message_callback(callback);

	// Create a message
	aioth::message evt("Hello world");
	evt.add_property("Hello", "world");

	// Send the message
	service.send(evt, TEST_DEVICE_ID);

	// Recieve the message
	for (int i = 0; i < 500; i++)
	{
		ThreadAPI_Sleep(10);
		service.do_work();
		device.do_work();
		if (callback_received)
		{
			exit(0);
		}
	}
	assert(false);
}