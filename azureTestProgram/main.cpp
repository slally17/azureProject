#include <iostream>
#include <fstream>
#include <string>

#include <k4a/k4a.h>
#include <k4arecord/record.h>
#include <k4arecord/playback.h>

#include <fbxsdk.h>

#include "testFile.h"

int main(int argc, char **argv) 
{	
	testFunction();

	uint32_t count = k4a_device_get_installed_count();

	std::cout << count << std::endl;

	return 0;
}
