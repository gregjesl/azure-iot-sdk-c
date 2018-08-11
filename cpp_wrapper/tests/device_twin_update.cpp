#include "aioth_client.h"
#include "connection.h"
#include <assert.h>

bool callback_received = false;
bool acknowledgement_sent = false;

void callback(const std::string payload)
{
	callback_received = true;
}

int main(void)
{
	// Create the device client
	aioth::client device(TEST_DEVICE_CONNECTION_STRING);

	// Register the callback
	device.set_twin_update_callback(callback);

	// Do work
	for (int i = 0; i < 6000; i++)
	{
		ThreadAPI_Sleep(10);
		device.do_work();
		if (callback_received)
		{
			assert(device.send_state("{\"Hello\":\"world\"}"));
		}
	}
	assert(false);
}