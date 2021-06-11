#include <string>

#include "oscpack/osc/OscOutboundPacketStream.h"
#include "oscpack/ip/UdpSocket.h"
#include "oscpack/osc/OscReceivedElements.h"
#include "oscpack/osc/OscPacketListener.h"

#include "mkvModeFunctions.h"
#include "realtimeModeFunctions.h"
#include "streamModeFunctions.h"
#include "imageModeFunctions.h"
#include "videoModeFunctions.h"

//Syntax: azureProgram.exe -mode (input.mkv) (output.___)
	//If an input and output are provided program runs in mkv mode
	//If only an output is provided program runs in realtime mode
	//If neither are provided program runs in image mode

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

//Image Mode: Save color and transformed depth image
	//Step 1: Capture color and depth image
	//Step 2: Transform depth image to aline with color image
	//Step 3: Save both images

//Stream Mode: Stream Kinect skeletons to Unity
	//Future planning

int main(int argc, char **argv) 
{	
	UdpTransmitSocket transmitSocket(IpEndpointName(ADDRESS, OUTPUT_PORT));
	std::string errorMessage = "";
	std::string mode = argv[1];
	
	if (mode == "-mkv" && argc == 4) {
		//Run mkv mode
		errorMessage = mkvModeFunction(argv[2], argv[3]);
	}
	else if (mode == "-realtime" && argc == 3) {
		//Start thread for receiving end recording message
		std::thread lt = std::thread(ListenerThread);

		//Run realtime mode
		errorMessage = realtimeModeFunction(argv[2], &transmitSocket);

		lt.detach();
	}
	else if (mode == "-image" && argc == 2) {
		// Run image mode
		errorMessage = imageModeFunction();
		
		//Run stream mode
		//errorMessage = streamModeFunction();
	}
	else if (mode == "-video" && argc == 2) {
		errorMessage = videoModeFunction();
	}
	else {
		//Invalid number of arguments
		errorMessage = "Invalid number of arguments. Use \"azureProgram.exe -mode (input.mkv) (output.fbx)\".";
	}

	//Send end of program osc message
	sendEndOfProgramMessage(errorMessage, &transmitSocket);

	//End program
	if (errorMessage == "") {
		std::cout << "Program Success!" << std::endl;
		return EXIT_SUCCESS;
	}
	else {
		std::cout << errorMessage << std::endl;
		return EXIT_FAILURE;
	}	
}
