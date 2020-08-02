#pragma once

#include <thread>
#include <mutex>
#include <string>

#define ADDRESS "127.0.0.1"
#define OUTPUT_PORT 7000
#define INPUT_PORT 7001
#define OUTPUT_BUFFER_SIZE 1024

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

static void ListenerThread() {
	CustomPacketListener listener;
	UdpListeningReceiveSocket s(
		IpEndpointName(IpEndpointName::ANY_ADDRESS, INPUT_PORT),
		&listener);
	s.Run();
}

void sendEndOfProgramMessage(std::string errorMessage, UdpTransmitSocket* transmitSocket) {
	char buffer[OUTPUT_BUFFER_SIZE];
	osc::OutboundPacketStream p(buffer, OUTPUT_BUFFER_SIZE);

	if (errorMessage == "") {
		p << osc::BeginBundleImmediate << osc::BeginMessage("/Program Complete/") << 1 << " " << osc::EndMessage << osc::EndBundle;		
	}
	else {
		p << osc::BeginBundleImmediate << osc::BeginMessage("/Program Complete/") << 0 << errorMessage.c_str() << osc::EndMessage << osc::EndBundle;
	}
	transmitSocket->Send(p.Data(), p.Size());
}

void sendRecordingStartedMessage(UdpTransmitSocket* transmitSocket) {
	char buffer[OUTPUT_BUFFER_SIZE];
	osc::OutboundPacketStream p(buffer, OUTPUT_BUFFER_SIZE);

	p << osc::BeginBundleImmediate << osc::BeginMessage("/Recording Started/") << osc::EndMessage << osc::EndBundle;
	transmitSocket->Send(p.Data(), p.Size());
}