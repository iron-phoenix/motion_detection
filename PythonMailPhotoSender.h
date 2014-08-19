#ifndef PYTHON_MAIL_PHOTO_SENDER_H
#define PYTHON_MAIL_PHOTO_SENDER_H

#include "MailPhotoSender.h"

#include <string>

using std::string;

struct PythonMailPhotoSender: public MailPhotoSender {
	PythonMailPhotoSender(const string & python_script_file, const string & config_file):
			python_script_file(python_script_file),
			config_file(config_file)
	{}

	void send_message();

private:
	string python_script_file;
	string config_file;

	string build_command();
};

#endif
