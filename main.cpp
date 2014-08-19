#include "MotionDetector.h"
#include "MailPhotoSender.h"
#include "PythonMailPhotoSender.h"

#include <opencv2/opencv.hpp>

#include <iostream>
#include <fstream>

using cv::VideoWriter;
using cv::Size;

using std::cout;
using std::endl;
using std::ofstream;

int main(int argc, char** argv) {
	ofstream outputVideo("video.bgr", ofstream::binary);

	MailPhotoSender * mail_photo_sender = new PythonMailPhotoSender("./SendMail.py", "send_mail.conf");

	MotionDetector motion_detector(0, 100, 320, 240);
	if (!motion_detector.is_opened()) {
		cout << "Could not open video device" << endl;
		return -1;
	}
	motion_detector.set_video_writer(&outputVideo);
	//motion_detector.set_mail_sender(mail_photo_sender);
	motion_detector.set_save_filename("motion.jpg");

	motion_detector.detect_motion();
	return 0;
}
