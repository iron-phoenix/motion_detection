#ifndef MOTION_DETECTOR_H
#define MOTION_DETECTOR_H

#include "MailPhotoSender.h"

#include <opencv2/opencv.hpp>

#include <string>

using cv::VideoCapture;
using cv::VideoWriter;
using cv::Mat;

using std::string;

enum ErrorType {
	NO_ERRORS,

	CAPTURE_INIT_ERROR
};

struct MotionDetector {
	MotionDetector(int capture_device_id, int threshold);

	void set_video_writer(const VideoWriter & video_writer) { this->video_writer = video_writer; }
	void set_standard_deviation(const int standart_deviation) { this->standard_deviation = standard_deviation; }
	void set_save_filename(const string & save_filename) { this->save_filename = save_filename; }
	void set_mail_sender(MailPhotoSender * const mail_sender) { this->mail_sender = mail_sender; }

	bool is_opened() { return error == NO_ERRORS; }

	void detect_motion();

private:
	static const size_t NO_MOTION_FRAMES = 100;

	VideoCapture cap;

	Mat current_frame;
	Mat previous_frame;
	Mat next_frame;

	Mat result;

	Mat motion;
	Mat diff1;
	Mat diff2;

	Mat erode_kernel;

	struct MotionPhotoFrame {
		size_t motion_square;
		Mat photo_frame;

		MotionPhotoFrame():
			motion_square(0)
		{}

	} motion_photo_frame;

	VideoWriter video_writer;

	ErrorType error;

	int standard_deviation;
	int threshold_;

	struct Rectangle {
		size_t min_x;
		size_t min_y;
		size_t max_x;
		size_t max_y;
	} bound_motion;

	string save_filename;

	MailPhotoSender * mail_sender;

	bool is_motion_real();
	size_t get_number_of_changes();

	void clear_bound_rectangle();
	void make_bounds();
};

#endif
