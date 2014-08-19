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

int main(int argc, char** argv) {
	VideoWriter outputVideo("video.avi", CV_FOURCC('I', '4', '2', '0'), 30, Size(640, 480), true);
	if (!outputVideo.isOpened()) {
		cout  << "Could not open the output video for write" << endl;
		return -1;
	}

	MailPhotoSender * mail_photo_sender = new PythonMailPhotoSender("./SendMail.py", "send_mail.conf");

	MotionDetector motion_detector(0, 100);
	if (!motion_detector.is_opened()) {
		cout << "Could not open video device" << endl;
		return -2;
	}
	motion_detector.set_video_writer(outputVideo);
	//motion_detector.set_mail_sender(mail_photo_sender);
	motion_detector.set_save_filename("motion.jpg");

	motion_detector.detect_motion();
	return 0;
}
