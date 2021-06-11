#pragma once

#include <k4a/k4a.h>
#include <k4arecord/record.h>
#include <k4arecord/playback.h>
#include <k4abt.h>

#include "windows.h"
#include <iostream>
#include <string>

std::string videoModeFunction() {
	std::string errorMessage = "";
	uint32_t kinectCount = k4a_device_get_installed_count();

	if (kinectCount == 1 && errorMessage == "") { //Run program if Kinect is found
		//Connect to the Kinect
		k4a_device_t device = NULL;
		if (K4A_FAILED(k4a_device_open(K4A_DEVICE_DEFAULT, &device))) {
			errorMessage += "Kinect was found by program, but can't connect. Please try reconnecting.\n";
		}

		//Initialize the Kinect
		k4a_device_configuration_t device_config = K4A_DEVICE_CONFIG_INIT_DISABLE_ALL;
		device_config.camera_fps = K4A_FRAMES_PER_SECOND_5;
		device_config.color_format = K4A_IMAGE_FORMAT_COLOR_MJPG;
		device_config.color_resolution = K4A_COLOR_RESOLUTION_720P;
		device_config.depth_mode = K4A_DEPTH_MODE_WFOV_2X2BINNED;
		device_config.synchronized_images_only = true;
		k4a_calibration_t sensor_calibration;
		if (errorMessage == "" && K4A_FAILED(k4a_device_get_calibration(device, device_config.depth_mode, device_config.color_resolution, &sensor_calibration))) {
			errorMessage += "Get depth camera calibration failed. \n";
		}
		k4a_transformation_t transformation_handle = NULL;
		transformation_handle = k4a_transformation_create(&sensor_calibration);

		// Create body tracker
		k4abt_tracker_t tracker = NULL;
		k4abt_tracker_configuration_t tracker_config = K4ABT_TRACKER_CONFIG_DEFAULT;
		if (errorMessage == "" && K4A_FAILED(k4abt_tracker_create(&sensor_calibration, tracker_config, &tracker))) {
			errorMessage += "Body tracker initialization failed. \n";
		}

		//Start recording
		if (errorMessage == "" && K4A_FAILED(k4a_device_start_cameras(device, &device_config))) {
			errorMessage += "Kinect camera failed to start, please try reconnecting.\n";
			k4a_device_close(device);
		}

		//Create output path
		std::experimental::filesystem::create_directory("export");
		//CreateDirectory(path.parent_path().string().c_str(), NULL);

		//Process Kinect recording data
		int runTime = -1;
		bool running = true;
		while (running && errorMessage == "") {
			//Increment frame counter
			runTime++;

			//Press spacebar to stop recording
			if (GetAsyncKeyState(VK_SPACE)) {
				running = false;
			}

			//Get current frame
			k4a_capture_t sensor_capture;
			k4a_wait_result_t get_capture_result = k4a_device_get_capture(device, &sensor_capture, K4A_WAIT_INFINITE);

			//Process current frame
			if (get_capture_result == K4A_WAIT_RESULT_SUCCEEDED) {
				// Get color and depth images
				const k4a_image_t color_image = k4a_capture_get_color_image(sensor_capture);
				const k4a_image_t depth_image = k4a_capture_get_depth_image(sensor_capture);

				// Transform depth and ir image
				k4a_image_t transformed_depth_image = NULL;
				int color_image_width_pixels = k4a_image_get_width_pixels(color_image);
				int color_image_height_pixels = k4a_image_get_height_pixels(color_image);
				if (K4A_RESULT_SUCCEEDED != k4a_image_create(K4A_IMAGE_FORMAT_DEPTH16, color_image_width_pixels, color_image_height_pixels, color_image_width_pixels * (int)sizeof(uint16_t), &transformed_depth_image))
				{
					errorMessage += "Failed to create transformed depth image.\n";
				}
				if (K4A_RESULT_SUCCEEDED != k4a_transformation_depth_image_to_color_camera(transformation_handle, depth_image, transformed_depth_image))
				{
					errorMessage += "Failed to compute transformed depth image.\n";
				}

				// Check for skeletons
				k4a_wait_result_t queue_capture_result = k4abt_tracker_enqueue_capture(tracker, sensor_capture, K4A_WAIT_INFINITE);
				if (queue_capture_result == K4A_WAIT_RESULT_FAILED) {
					errorMessage += ("Add capture to tracker process queue failed.\n");
				}
				k4abt_frame_t body_frame = NULL;
				k4a_wait_result_t pop_frame_result = k4abt_tracker_pop_result(tracker, &body_frame, K4A_WAIT_INFINITE);
				if (pop_frame_result == K4A_WAIT_RESULT_SUCCEEDED && errorMessage == "") {
					uint32_t num_bodies = k4abt_frame_get_num_bodies(body_frame);
					if (num_bodies > 0) {
						// Save depth image with body
						std::string depthFileName = "C:\\Users\\Samuel Lally\\OneDrive - Virginia Tech\\Classes\\2021 Summer\\Iceland\\Processing\\researchProject\\data\\depthImageBody" + std::to_string(runTime) + ".txt";
						std::ofstream fw(depthFileName, std::ofstream::out);
						uint16_t* depth_data = (uint16_t*)(void*)k4a_image_get_buffer(transformed_depth_image);
						for (int i = 0; i < k4a_image_get_width_pixels(transformed_depth_image); i++) {
							for (int j = 0; j < k4a_image_get_height_pixels(transformed_depth_image); j++) {
								fw << depth_data[i* k4a_image_get_height_pixels(transformed_depth_image) + j] << "\n";
							}
						}
						fw.close();
						// Save color image with body
						std::string colorFileName = "C:\\Users\\Samuel Lally\\OneDrive - Virginia Tech\\Classes\\2021 Summer\\Iceland\\Processing\\researchProject\\data\\colorImageBody" + std::to_string(runTime) + ".jpg";
						writeToFile(colorFileName.c_str(), k4a_image_get_buffer(color_image), k4a_image_get_size(color_image));
					}
					else {
						// Save depth image without body
						std::string depthFileName = "C:\\Users\\Samuel Lally\\OneDrive - Virginia Tech\\Classes\\2021 Summer\\Iceland\\Processing\\researchProject\\data\\depthImage" + std::to_string(runTime) + ".txt";
						std::ofstream fw(depthFileName, std::ofstream::out);
						uint16_t* depth_data = (uint16_t*)(void*)k4a_image_get_buffer(transformed_depth_image);
						for (int i = 0; i < k4a_image_get_width_pixels(transformed_depth_image); i++) {
							for (int j = 0; j < k4a_image_get_height_pixels(transformed_depth_image); j++) {
								fw << depth_data[i* k4a_image_get_height_pixels(transformed_depth_image) + j] << "\n";
							}
						}
						fw.close();
						// Save color image without body
						std::string colorFileName = "C:\\Users\\Samuel Lally\\OneDrive - Virginia Tech\\Classes\\2021 Summer\\Iceland\\Processing\\researchProject\\data\\colorImage" + std::to_string(runTime) + ".jpg";
						writeToFile(colorFileName.c_str(), k4a_image_get_buffer(color_image), k4a_image_get_size(color_image));
					}
					k4abt_frame_release(body_frame);
				}
				else {
					errorMessage += "Pop body frame result failed.\n";
				}

				//Release capture and images
				k4a_capture_release(sensor_capture);
				k4a_image_release(color_image);
				k4a_image_release(depth_image);
			}
		}

		//Stop Kinect
		k4abt_tracker_shutdown(tracker);
		k4abt_tracker_destroy(tracker);
		k4a_transformation_destroy(transformation_handle);
		k4a_device_stop_cameras(device);
		k4a_device_close(device);
	}
	else if (kinectCount == 0) { //End program if Kinect isn't found
		errorMessage += "Kinect can't be found by program, please try reconnecting.\n";
	}
	else if (errorMessage == "") { //End program if multiple Kinects are found
		errorMessage += "Multiple Kinects, detected. Please unplug additional ones.\n";
	}

	return errorMessage;
}