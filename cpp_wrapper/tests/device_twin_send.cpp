#include "aioth_client.h"
#include "connection.h"
#include <assert.h>

bool callback_received = false;

void callback(void)
{
	callback_received = true;
}

int main(void)
{
	// Create the device client
	aioth::client device(TEST_DEVICE_CONNECTION_STRING);

	// Register the callback
	device.set_reported_state_callback(callback);

	// Report the state
	assert(device.send_state("{\"Hello\":\"world\"}"));

	// Do work
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