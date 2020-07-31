#pragma once

#include <thread>
#include <mutex>

std::mutex oscMutex;
bool endRecording = false;

class CustomPacketListener : public osc::OscPacketListener {
protected:

	virtual void ProcessMessage(const osc::ReceivedMessage& m,
		const IpEndpointName& remoteEndpoint)
	{
		(void)remoteEndpoint; // suppress unused parameter warning		
		if (std::strcmp(m.AddressPattern(), "/End Recording/") == 0) {
			oscMutex.lock();
			endRecording = true;
			oscMutex.unlock();
		}
	}
};

void ListenerThread() {
	CustomPacketListener listener;
	UdpListeningReceiveSocket s(
		IpEndpointName(IpEndpointName::ANY_ADDRESS, INPUT_PORT),
		&listener);
	s.Run();
}