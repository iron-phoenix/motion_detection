#include "PythonMailPhotoSender.h"

#include <string>
#include <cstdlib>
#include <iostream>

using std::string;

string PythonMailPhotoSender::build_command() {
	string result = python_script_file;
	result.append(" -c ");
	result.append(config_file);

	if (!photo_file.empty()) {
		result.append(" -p ");
		result.append(photo_file);
	}

	return result;
}

void PythonMailPhotoSender::send_message() {
	string command = build_command();
	std::cout << command << std::endl;
	system(command.c_str());
}
