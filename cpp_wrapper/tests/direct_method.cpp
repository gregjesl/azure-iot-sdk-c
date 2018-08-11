#include "aioth_client.h"
#include "connection.h"
#include <assert.h>

bool callback_received = false;

int callback(std::string name, std::string payload, std::string &response)
{
	response =  "{\"name\":\"" + name + "\"}";
	callback_received = true;
	return 1;
}

int main(void)
{
	// Create the device client
	aioth::client device(TEST_DEVICE_CONNECTION_STRING);

	// Register the callback
	device.set_direct_method_callback(callback);

	// Wait 1 minute for the method
	for (int i = 0; i < 6000; i++)
	{
		ThreadAPI_Sleep(10);
		device.do_work();
		if (callback_received)
		{
			exit(0);
		}
	}
	assert(false);
}