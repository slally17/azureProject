#pragma once

#include <iostream>
#include <string>

std::string modeOneFunction() {
	std::string errorMessage = "";
	uint32_t kinectCount = k4a_device_get_installed_count();

	if (kinectCount == 1) { //Run program if kinect is found
		//Connect to the Kinect
		k4a_device_t device = NULL;
		if (K4A_FAILED(k4a_device_open(K4A_DEVICE_DEFAULT, &device)))
		{
			errorMessage = "Kinect was found by program, but can't connect. Please try reconnecting.";
		}

		//Initialize the Kinect
		k4a_device_configuration_t config = K4A_DEVICE_CONFIG_INIT_DISABLE_ALL;
		config.camera_fps = K4A_FRAMES_PER_SECOND_30;
		config.color_format = K4A_IMAGE_FORMAT_COLOR_MJPG;
		config.color_resolution = K4A_COLOR_RESOLUTION_2160P;
		config.depth_mode = K4A_DEPTH_MODE_NFOV_UNBINNED;

		//Start recording
		if (K4A_FAILED(k4a_device_start_cameras(device, &config)))
		{
			errorMessage = "Kinect camera failed to start, please try reconnecting.";
			k4a_device_close(device);
		}

		//Stop Kinect and release objects
		k4a_device_stop_cameras(device);
		k4a_device_close(device);

		//Create FBX

	}
	else if (kinectCount == 0) { //End program if Kinect isn't found
		errorMessage = "Kinect can't be found by program, please try reconnecting.";
	}
	else { //End program if multiple Kinects are found
		errorMessage = "Multiple Kinects, detected. Please unplug additional ones.";
	}

	return errorMessage;
}