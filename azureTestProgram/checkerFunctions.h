#pragma once

#include <k4a/k4a.h>
#include <k4arecord/record.h>
#include <k4arecord/playback.h>
#include <k4abt.h>

#include <fstream>
#include <sstream>

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

bool outputFBX(std::string outputPath) {
	std::stringstream fullFileName(outputPath);
	std::string fileName, fileExtension;
	std::getline(fullFileName, fileName, '.');
	std::getline(fullFileName, fileExtension);
	if (fileExtension == "fbx") {
		return true;
	}
	return false;
}

bool outputGLTF(std::string outputPath) {
	std::stringstream fullFileName(outputPath);
	std::string fileName, fileExtension;
	std::getline(fullFileName, fileName, '.');
	std::getline(fullFileName, fileExtension);
	if (fileExtension == "gltf") {
		return true;
	}
	return false;
}