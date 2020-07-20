#include <string>

#include "oscpack/osc/OscOutboundPacketStream.h"
#include "oscpack/ip/UdpSocket.h"
#include "oscpack/osc/OscReceivedElements.h"
#include "oscpack/osc/OscPacketListener.h"

#define ADDRESS "127.0.0.1"
#define PORT 7000
#define OUTPUT_BUFFER_SIZE 1024

#include "mkvModeFunctions.h"
#include "realtimeModeFunctions.h"
#include "streamModeFunctions.h"

//Syntax: azureProgram.exe (input.mkv) (output.___)
	//If an input and output are provided program runs in mkv mode
	//If only an output is provided program runs in realtime mode
	//If neither are provided program runs in stream mode

//MKV Mode: Create skeletons from saved mkv file
	//Step 1: Get mkv file
	//Step 2: Convert mkv file to skeletons
	//Step 3: Convert skeletons to fbx or gltf file

//Realtime Mode: Create skeletons from realtime recording
	//Step 1: Initialize the kinect
	//Step 2: Start recording and loop through substeps
		//2A: Get frame from kinect
		//2B: Create skeletons from frame
		//2C: Save skeletons data
	//Step 3: End recording (Press space bar to stop recording)
	//Step 4: Create fbx or gltf from skeleton data

//Stream Mode: Stream Kinect skeletons to Unity
	//Future planning

int main(int argc, char **argv) 
{	
	std::string errorMessage = "";
	UdpTransmitSocket transmitSocket(IpEndpointName(ADDRESS, PORT));
	char buffer[OUTPUT_BUFFER_SIZE];
	osc::OutboundPacketStream p(buffer, OUTPUT_BUFFER_SIZE);
	
	if (argc == 3) {
		//Run mkv mode
		errorMessage = mkvModeFunction(argv[1], argv[2]);
	}
	else if (argc == 2) {
		//Run realtime mode
		errorMessage = realtimeModeFunction(argv[1]);
	}
	else if (argc == 1) {
		//Run stream mode
		errorMessage = streamModeFunction();
	}
	else {
		//Invalid number of arguments
		errorMessage = "Invalid number of arguments. Use \"azureProgram.exe (input.mkv) (output.fbx)\".";
	}

	if (errorMessage == "") {
		//Send osc message when program is complete		
		if (argc == 3) {
			p << osc::BeginBundleImmediate << osc::BeginMessage("Fbx Created") << 1 << osc::EndMessage << osc::EndBundle;
		}
		else if (argc == 2) {
			p << osc::BeginBundleImmediate << osc::BeginMessage("Fbx Created") << 2 << osc::EndMessage << osc::EndBundle;
		}
		transmitSocket.Send(p.Data(), p.Size());		

		return EXIT_SUCCESS;
	}
	else {
		std::cout << errorMessage << std::endl;
		return EXIT_FAILURE;
	}	
}
