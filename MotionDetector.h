#ifndef MOTION_DETECTOR_H
#define MOTION_DETECTOR_H

#include "MailPhotoSender.h"

#include <opencv2/opencv.hpp>

#include <string>
#include <fstream>

using cv::VideoCapture;
using cv::Mat;

using std::string;
using std::ofstream;

enum ErrorType {
	NO_ERRORS,

	CAPTURE_INIT_ERROR
};

struct MotionDetector {
	MotionDetector(int capture_device_id, int threshold, int width = 640, int height = 480);

	void set_video_writer(ofstream * video_writer) { this->video_writer = video_writer; }
	void set_standard_deviation(const int standart_deviation) { this->standard_deviation = standard_deviation; }
	void set_save_filename(const string & save_filename) { this->save_filename = save_filename; }
	void set_mail_sender(MailPhotoSender * const mail_sender) { this->mail_sender = mail_sender; send_mail = true; }

	bool is_opened() { return error == NO_ERRORS; }

	void detect_motion();

private:
	static const size_t NO_MOTION_FRAMES = 10;

	VideoCapture cap;

	Mat current_frame;
	Mat previous_frame;

	Mat result;

	Mat diff;

	struct MotionPhotoFrame {
		size_t motion_square;
		Mat photo_frame;

		MotionPhotoFrame():
			motion_square(0)
		{}

	} motion_photo_frame;

	ofstream * video_writer;

	ErrorType error;

	int standard_deviation;
	int threshold_;

	struct Rectangle {
		int min_x;
		int min_y;
		int max_x;
		int max_y;
	} bound_motion;

	string save_filename;

	MailPhotoSender * mail_sender;
	bool send_mail;

	bool is_motion_real();
	size_t get_number_of_changes();

	void clear_bound_rectangle();
	void make_bounds();
};

#endif
