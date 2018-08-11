#include "connection.h"
#include "aioth_client.h"
#include <assert.h>
#include "azure_c_shared_utility/threadapi.h"
#include "azure_c_shared_utility/platform.h"

bool callback_received = false;
aioth::messageID callbackID;

void callback(aioth::message *msg, bool success)
{
	assert(success);
	callback_received = true;
	callbackID = msg->get_id();
}

int main(void)
{
	aioth::client client(TEST_DEVICE_CONNECTION_STRING);

	// No callback
	aioth::message hello("Hello world");
	assert(hello.get_id() == 0);
	client.send(hello);

	aioth::message world("Hello world");
	assert(world.get_id() == 1);
	world.callback = callback;
	client.send(world);

	for (int i = 0; i < 5000; i++)
	{
		ThreadAPI_Sleep(10);
		client.do_work();
		if (callback_received)
		{
			assert(callbackID == world.get_id());
			exit(0);
		}
	}

	// Disconnect and reconnect
	client.disconnect();
	assert(client.connect(TEST_DEVICE_CONNECTION_STRING));
	client.send(world);

	for (int i = 0; i < 5000; i++)
	{
		ThreadAPI_Sleep(10);
		client.do_work();
		if (callback_received)
		{
			assert(callbackID == world.get_id());
			exit(0);
		}
	}

	assert(false);
}