#pragma once

std::string modeOneFunction(const char* input_path, const char* output_path) {
	std::string errorMessage = "";

	//Find the mkv file and check that it exists
	k4a_playback_t playback_handle = nullptr;
	k4a_result_t result = k4a_playback_open(input_path, &playback_handle);
	if (result != K4A_RESULT_SUCCEEDED)	{
		errorMessage = "Cannot open recording.\n";
	}

	//Calibrate the mkv file to be played back
	k4a_calibration_t calibration;
	result = k4a_playback_get_calibration(playback_handle, &calibration);
	if (result != K4A_RESULT_SUCCEEDED)	{
		errorMessage += "Failed to get calibration.\n";
	}

	k4abt_tracker_t tracker = NULL;
	k4abt_tracker_configuration_t tracker_config = K4ABT_TRACKER_CONFIG_DEFAULT;
	if (K4A_RESULT_SUCCEEDED != k4abt_tracker_create(&calibration, tracker_config, &tracker)) 
	{
		errorMessage += "Body tracker initialization failed.\n";
	}

	return errorMessage;
}