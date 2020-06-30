#include <iostream>
#include <fstream>
#include <string>

#include <k4a/k4a.h>
#include <k4arecord/record.h>
#include <k4arecord/playback.h>
#include <k4abt.h>

#include <fbxsdk.h>

#include "modeOneFunctions.h"
#include "modeTwoFunctions.h"
#include "modeThreeFunctions.h"

//Syntax: azureProgram.exe (input.mkv) (output.fbx)
	//If an input and output are provided program runs in mode 1
	//If only an output is provided program runs in mode 2
	//If neither are provided program runs in mode 3

//Mode 1: Create skeletons from saved mkv file
	//Step 1: Get mkv file
	//Step 2: Convert mkv file to skeletons
	//Step 3: Convert skeletons to fbx file

//Mode 2: Create skeletons from realtime recording
	//Step 1: Initialize the kinect
	//Step 2: Start recording and loop through substeps
		//2A: Get frame from kinect
		//2B: Create skeletons from frame
		//2C: Save skeletons data
	//Step 3: End recording (Press space bar to stop recording)
	//Step 4: Create fbx from skeleton data

//Mode 3: Stream Kinect skeletons to Unity
	//Future planning

int main(int argc, char **argv) 
{	
	std::string errorMessage = "";
	
	if (argc == 3) {
		//Run mode 1
		errorMessage = modeOneFunction(argv[1], argv[2]);
	}
	else if (argc == 2) {
		//Run mode 2
		errorMessage = modeTwoFunction();
	}
	else if (argc == 1) {
		//Run mode 3
		errorMessage = modeThreeFunction();
	}
	else {
		//Invalid number of arguments
		errorMessage = "Invalid number of arguments. Use \"azureProgram.exe (input.mkv) (output.fbx)\".";
	}

	if (errorMessage == "") {
		return EXIT_SUCCESS;
	}
	else {
		std::cout << errorMessage << std::endl;
		return EXIT_FAILURE;
	}	
}
