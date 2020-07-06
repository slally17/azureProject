#pragma once

#include <iostream>
#include <string>

#include "windows.h"

std::string modeTwoFunction(const char* output_path) {
	std::string errorMessage = "";
	uint32_t kinectCount = k4a_device_get_installed_count();

	if (kinectCount == 1) { //Run program if Kinect is found
		//Connect to the Kinect
		k4a_device_t device = NULL;
		if (K4A_FAILED(k4a_device_open(K4A_DEVICE_DEFAULT, &device))) {
			errorMessage += "Kinect was found by program, but can't connect. Please try reconnecting.\n";
		}

		//Initialize the Kinect
		k4a_device_configuration_t device_config = K4A_DEVICE_CONFIG_INIT_DISABLE_ALL;
		device_config.camera_fps = K4A_FRAMES_PER_SECOND_30;
		device_config.color_format = K4A_IMAGE_FORMAT_COLOR_MJPG;
		device_config.color_resolution = K4A_COLOR_RESOLUTION_2160P;
		device_config.depth_mode = K4A_DEPTH_MODE_NFOV_UNBINNED;
		k4a_calibration_t sensor_calibration;
		if (K4A_FAILED(k4a_device_get_calibration(device, device_config.depth_mode, K4A_COLOR_RESOLUTION_OFF, &sensor_calibration))) {
			errorMessage += "Get depth camera calibration failed. \n";
		}

		//Create body tracker
		k4abt_tracker_t tracker = NULL;
		k4abt_tracker_configuration_t tracker_config = K4ABT_TRACKER_CONFIG_DEFAULT;
		if (K4A_FAILED(k4abt_tracker_create(&sensor_calibration, tracker_config, &tracker))) {
			errorMessage += "Body tracker initialization failed. \n";
		}

		//Start recording
		if (errorMessage == "" && K4A_FAILED(k4a_device_start_cameras(device, &device_config))) {
			errorMessage += "Kinect camera failed to start, please try reconnecting.\n";
			k4a_device_close(device);
		}

		//Process Kinect recording data
		bool running = true;
		while (running && errorMessage == "") {
			//Press spacebar to stop recording
			if (GetAsyncKeyState(VK_SPACE)) { 
				running = false;
			}

			//Get current frame
			k4a_capture_t sensor_capture;
			k4a_wait_result_t get_capture_result = k4a_device_get_capture(device, &sensor_capture, K4A_WAIT_INFINITE);

			//Process current frame
			if (get_capture_result == K4A_WAIT_RESULT_SUCCEEDED) {
				k4a_wait_result_t queue_capture_result = k4abt_tracker_enqueue_capture(tracker, sensor_capture, K4A_WAIT_INFINITE);
				k4a_capture_release(sensor_capture);
				if (queue_capture_result == K4A_WAIT_RESULT_FAILED)	{
					errorMessage += ("Add capture to tracker process queue failed.\n");
					break;
				}

				//Get skeleton from current frame
				k4abt_frame_t body_frame = NULL;
				k4a_wait_result_t pop_frame_result = k4abt_tracker_pop_result(tracker, &body_frame, K4A_WAIT_INFINITE);
				if (pop_frame_result == K4A_WAIT_RESULT_SUCCEEDED) {
					uint32_t num_bodies = k4abt_frame_get_num_bodies(body_frame);
					if (num_bodies > 0) {
						k4abt_skeleton_t skeleton;
						k4abt_frame_get_body_skeleton(body_frame, 0, &skeleton);
					}		

					//THIS IS WHERE CODE FOR WHAT TO DO WITH SKELETON GOES

					k4abt_frame_release(body_frame);
				}
				else {
					errorMessage += "Pop body frame result failed.\n";
					break;
				}
			}
			else {
				errorMessage += "Get depth capture returned error.\n";
				break;
			}
		}

		//Stop Kinect and release objects
		k4abt_tracker_shutdown(tracker);
		k4abt_tracker_destroy(tracker);
		k4a_device_stop_cameras(device);
		k4a_device_close(device);

		//Create FBX

	}
	else if (kinectCount == 0) { //End program if Kinect isn't found
		errorMessage += "Kinect can't be found by program, please try reconnecting.\n";
	}
	else { //End program if multiple Kinects are found
		errorMessage += "Multiple Kinects, detected. Please unplug additional ones.\n";
	}

	return errorMessage;
}