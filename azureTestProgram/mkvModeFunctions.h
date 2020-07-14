#pragma once

#include "fbxFunctions.h"

bool check_depth_image_exists(k4a_capture_t capture)
{
	k4a_image_t depth = k4a_capture_get_depth_image(capture);
	if (depth != nullptr) {
		k4a_image_release(depth);
		return true;
	}
	else {
		return false;
	}
}

std::string mkvModeFunction(const char* input_path, const char* output_path) {
	std::vector<k4abt_skeleton_t> skeletons;
	std::string errorMessage = "";

	//Find the mkv file and check that it exists
	k4a_playback_t playback_handle = nullptr;
	k4a_result_t result = k4a_playback_open(input_path, &playback_handle);
	if (K4A_RESULT_SUCCEEDED != result)	{
		errorMessage += "Cannot open recording.\n";
	}

	//Calibrate the mkv file to be played back
	k4a_calibration_t calibration;
	result = k4a_playback_get_calibration(playback_handle, &calibration);
	if (errorMessage == "" && K4A_RESULT_SUCCEEDED != result)	{
		errorMessage += "Failed to get calibration.\n";
	}

	//Create body tracker
	k4abt_tracker_t tracker = NULL;
	k4abt_tracker_configuration_t tracker_config = K4ABT_TRACKER_CONFIG_DEFAULT;
	if (errorMessage == "" && K4A_RESULT_SUCCEEDED != k4abt_tracker_create(&calibration, tracker_config, &tracker)) {
		errorMessage += "Body tracker initialization failed.\n";
	}

	//Process mkv recording data
	bool running = true;
	while (running && errorMessage == "") {
		//Get current frame
		k4a_capture_t capture_handle = nullptr;
		k4a_stream_result_t stream_result = k4a_playback_get_next_capture(playback_handle, &capture_handle);

		//Process current frame
		if (stream_result == K4A_STREAM_RESULT_SUCCEEDED) {
			if (check_depth_image_exists(capture_handle)) {
				k4a_wait_result_t queue_capture_result = k4abt_tracker_enqueue_capture(tracker, capture_handle, K4A_WAIT_INFINITE);
				k4a_capture_release(capture_handle);
				if (queue_capture_result == K4A_WAIT_RESULT_FAILED) {
					errorMessage += ("Add capture to tracker process queue failed.\n");
				}

				//Get skeleton from current frame
				k4abt_frame_t body_frame = NULL;
				k4a_wait_result_t pop_frame_result = k4abt_tracker_pop_result(tracker, &body_frame, K4A_WAIT_INFINITE);
				if (pop_frame_result == K4A_WAIT_RESULT_SUCCEEDED && errorMessage == "") {
					uint32_t num_bodies = k4abt_frame_get_num_bodies(body_frame);
					if (num_bodies > 0) {
						k4abt_skeleton_t skeleton;
						k4abt_frame_get_body_skeleton(body_frame, 0, &skeleton);
						skeletons.push_back(skeleton);
					}
					k4abt_frame_release(body_frame);
				}
				else {
					errorMessage += "Pop body frame result failed.\n";
				}
			}
		}
		else if (stream_result == K4A_STREAM_RESULT_EOF) {
			running = false;
		}
		else {
			errorMessage += "Failed to read current frame.\n";
		}	
	}

	//Release tracker and recording
	k4abt_tracker_shutdown(tracker);
	k4abt_tracker_destroy(tracker);
	k4a_playback_close(playback_handle);

	//Create FBX from skeletons vector
	bool success = true;
	if (true) { //Replace true with errorMessage == ""
		success = createFBX(skeletons, output_path);
	}	
	if (!success) {
		errorMessage += "An error occurred while creating the fbx..\n";
	}

	return errorMessage;
}