#include "PythonMailPhotoSender.h"

#include <string>
#include <cstdlib>

using std::string;

string PythonMailPhotoSender::build_command() {
	string result = python_script_file;
	result.append(" -c ");
	result.append(config_file);

	if (!photo_file.empty()) {
		result.append(" -p ");
		result.append(photo_file);
	}
}

void PythonMailPhotoSender::send_message() {
	system(build_command().c_str());
}
