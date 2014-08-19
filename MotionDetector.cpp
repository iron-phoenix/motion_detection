#include "MotionDetector.h"
#include "MailPhotoSender.h"

#include <opencv2/opencv.hpp>
#include <cmath>
#include <cstdio>

using cv::VideoCapture;
using cv::Mat;
using cv::absdiff;
using cv::threshold;
using cv::cvtColor;
using cv::Size;
using cv::Scalar;
using cv::meanStdDev;
using cv::imwrite;
using cv::MORPH_RECT;
using cv::Point;
using cv::Rect;
using cv::GaussianBlur;
using cv::waitKey;

MotionDetector::MotionDetector(int capture_device_id, int threshold, int width, int height):
	cap(capture_device_id),
	error(NO_ERRORS),
	standard_deviation(50),
	threshold_ (threshold),
	send_mail(false) {
		if (!cap.isOpened()) {
			error = CAPTURE_INIT_ERROR;
			return;
		}

		cap.set(CV_CAP_PROP_FRAME_WIDTH, width);
		cap.set(CV_CAP_PROP_FRAME_HEIGHT, height);

		cap >> previous_frame;
		cap >> current_frame;

		cvtColor(previous_frame, previous_frame, CV_BGR2GRAY);
		cvtColor(current_frame, current_frame, CV_BGR2GRAY);

		clear_bound_rectangle();
}

void MotionDetector::detect_motion() {
	size_t no_motion_frames = 0;

	while (true) {
		previous_frame = current_frame;
		cap >> current_frame;

		result = current_frame;
		cvtColor(current_frame, current_frame, CV_BGR2GRAY);

		GaussianBlur(current_frame, current_frame, Size(3, 3), 1);
		absdiff(previous_frame, current_frame, diff);
		threshold(diff, diff, 30, 255, CV_THRESH_BINARY);

		size_t number_of_changes = get_number_of_changes();

		if (number_of_changes > threshold_) {
			no_motion_frames = 0;
			video_writer->write((char *) (result.data), result.rows * result.cols * 3);
		} else {
			++no_motion_frames;
			if (no_motion_frames == NO_MOTION_FRAMES && motion_photo_frame.photo_frame.data) {
				if (!save_filename.empty()) {
					imwrite(save_filename.c_str(), motion_photo_frame.photo_frame);
					motion_photo_frame.motion_square = 0;
					motion_photo_frame.photo_frame.release();
				}

				if (send_mail && !save_filename.empty()) {
					mail_sender->set_photo_file(save_filename);
					mail_sender->send_message();
				}
			}
		}

		waitKey(10);
	}
}

bool MotionDetector::is_motion_real() {
	Scalar mean;
	Scalar stddev;

	meanStdDev(diff, mean, stddev);

	return stddev[0] >= standard_deviation;
}

size_t MotionDetector::get_number_of_changes() {
	if (is_motion_real()) return 0;

	size_t number_of_changes = 0;

	clear_bound_rectangle();

	for (size_t y = 0; y < diff.rows; y += 2) {
		for (size_t x = 0; x < diff.cols; x += 2) {
			if (static_cast<size_t>(diff.at<uchar>(y, x)) == 255) {
				++number_of_changes;

				if (bound_motion.min_x > x) bound_motion.min_x = x;
				if (bound_motion.min_y > y) bound_motion.min_y = y;
				if (bound_motion.max_x < x) bound_motion.max_x = x;
				if (bound_motion.max_y < y) bound_motion.max_y = y;
			}
		}
	}

	if (number_of_changes > threshold_)
		make_bounds();

	return number_of_changes;
}

void MotionDetector::clear_bound_rectangle() {
	bound_motion.min_x = current_frame.cols;
	bound_motion.min_y = current_frame.rows;
	bound_motion.max_x = 0;
	bound_motion.max_y = 0;
}

void MotionDetector::make_bounds() {
	if (bound_motion.min_x - 5 > 0) bound_motion.min_x -= 5;
	if (bound_motion.min_y - 5 > 0) bound_motion.min_y -= 5;
	if (bound_motion.max_x + 5 < current_frame.cols - 1) bound_motion.max_x += 5;
	if (bound_motion.max_y + 5 < current_frame.rows - 1) bound_motion.max_y += 5;

	Point top_left(bound_motion.min_x, bound_motion.min_y);
	Point floor_right(bound_motion.max_x, bound_motion.max_y);
	Rect rect(top_left, floor_right);

	Scalar color(0, 255, 0);
	rectangle(result, rect, color, 1);

	size_t square = abs(bound_motion.max_x - bound_motion.min_x) * abs(bound_motion.max_y - bound_motion.min_y);
	if (square > motion_photo_frame.motion_square) {
		motion_photo_frame.motion_square = square;
		motion_photo_frame.photo_frame = result;
	}
}
