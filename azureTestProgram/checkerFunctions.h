#pragma once

#include <k4a/k4a.h>
#include <k4arecord/record.h>
#include <k4arecord/playback.h>
#include <k4abt.h>

#include <fstream>

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

bool fileExists(const char* output_path) {
	std::ifstream file(output_path);
	bool result = true;
	if (!file) {
		result = false;
	}
	return result;
}