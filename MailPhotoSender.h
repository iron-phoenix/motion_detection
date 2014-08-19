#ifndef MAIL_PHOTO_SENDER_H
#define MAIL_PHOTO_SENDER_H

#include <string>

using std::string;

struct MailPhotoSender {
	void set_photo_file(const string & photo_file) { this->photo_file = photo_file; }

	virtual void send_message() = 0;

protected:
	string photo_file;
};

#endif
