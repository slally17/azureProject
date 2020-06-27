#include <iostream>
#include <fstream>
#include <string>

#include <k4a/k4a.h>
#include <k4arecord/record.h>
#include <k4arecord/playback.h>

#include <fbxsdk.h>

#include "modeOneFunctions.h"

//Syntax: azureProgram.exe (input.mkv) output.fbx
	//If an input is provided program runs in mode 1
	//If an input is not provided program runs in mode 2

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

int main(int argc, char **argv) 
{	
	std::string errorMessage = "";
	
	if (argc == 3) {
		//Run mode 1
		errorMessage = modeOneFunction();
	}
	else if (argc == 2) {
		//Run mode 2
	}
	else {
		//Invalid number of arguments
		errorMessage = "Invalid number of arguments. Use \"azureProgram.exe (input.mkv) output.fbx\".";

		return EXIT_FAILURE;
	}

	if (errorMessage == "") {
		return EXIT_SUCCESS;
	}
	else {
		std::cout << errorMessage << std::endl;
		return EXIT_FAILURE;
	}	
}
