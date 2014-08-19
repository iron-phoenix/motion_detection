#include "MotionDetector.h"
#include "MailPhotoSender.h"

#include <opencv2/opencv.hpp>
#include <cmath>
#include <cstdio>

using cv::VideoCapture;
using cv::Mat;
using cv::absdiff;
using cv::threshold;
using cv::bitwise_and;
using cv::cvtColor;
using cv::Size;
using cv::erode;
using cv::Scalar;
using cv::meanStdDev;
using cv::imwrite;
using cv::MORPH_RECT;
using cv::Point;
using cv::Rect;

MotionDetector::MotionDetector(int capture_device_id, int threshold):
	cap(capture_device_id),
	error(NO_ERRORS),
	erode_kernel(getStructuringElement(MORPH_RECT, Size(2, 2))),
	standard_deviation(20),
	threshold_ (threshold) {
		if (!cap.isOpened()) {
			error = CAPTURE_INIT_ERROR;
			return;
		}

		cap >> previous_frame;
		cap >> current_frame;
		cap >> next_frame;

		cvtColor(previous_frame, previous_frame, CV_BGR2GRAY);
		cvtColor(current_frame, current_frame, CV_BGR2GRAY);
		cvtColor(next_frame, next_frame, CV_BGR2GRAY);

		clear_bound_rectangle();
}

void MotionDetector::detect_motion() {
	size_t no_motion_frames = 0;
	bool need_save_photo = false;

	while (true) {
		previous_frame = current_frame;
		current_frame = next_frame;
		cap >> next_frame;

		result = next_frame;
		cvtColor(next_frame, next_frame, CV_BGR2GRAY);

		absdiff(previous_frame, next_frame, diff1);
		absdiff(next_frame, current_frame, diff2);
		bitwise_and(diff1, diff2, motion);
		threshold(motion, motion, 35, 255, CV_THRESH_BINARY);
		erode(motion, motion, erode_kernel);

		size_t number_of_changes = get_number_of_changes();

		if (number_of_changes > threshold_) {
			no_motion_frames = 0;
			if (video_writer.isOpened())
				video_writer.write(result);
			need_save_photo = true;
		} else {
			++no_motion_frames;
			if (no_motion_frames == NO_MOTION_FRAMES && need_save_photo) {
				if (!save_filename.empty() && motion_photo_frame.photo_frame.data) {
					printf("Hello");
					imwrite(save_filename.c_str(), motion_photo_frame.photo_frame);
					motion_photo_frame.motion_square = 0;
				}

				if (mail_sender != NULL && !save_filename.empty()) {
					mail_sender->set_photo_file(save_filename);
					mail_sender->send_message();
				}
				need_save_photo = false;
			}
		}
	}
}

bool MotionDetector::is_motion_real() {
	Scalar mean;
	Scalar stddev;

	meanStdDev(motion, mean, stddev);

	return stddev[0] >= standard_deviation;
}

size_t MotionDetector::get_number_of_changes() {
	if (is_motion_real()) return 0;

	size_t number_of_changes = 0;

	clear_bound_rectangle();

	for (size_t y = 0; y != motion.rows; y += 2) {
		for (size_t x = 0; x != motion.cols; x += 2) {
			if (static_cast<size_t>(motion.at<uchar>(x, y)) == 255) {
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
	bound_motion.min_x = current_frame.rows;
	bound_motion.min_y = current_frame.cols;
	bound_motion.max_x = 0;
	bound_motion.max_y = 0;
}

void MotionDetector::make_bounds() {
	if (bound_motion.min_x - 5 > 0) bound_motion.min_x -= 5;
	if (bound_motion.min_y - 5 > 0) bound_motion.min_y -= 5;
	if (bound_motion.max_x + 5 < current_frame.cols) bound_motion.max_x += 5;
	if (bound_motion.max_y + 5 < current_frame.rows) bound_motion.max_y += 5;

	Point top_left(bound_motion.min_x, bound_motion.min_y);
	Point floor_right(bound_motion.max_x, bound_motion.max_y);
	Rect rect(top_left, floor_right);

	//Mat cropped = result(rect);
	//cropped.copyTo(result_cropped);
	Scalar color(0, 255, 0);
	rectangle(result, rect, color, 1);

	size_t square = abs(bound_motion.max_x - bound_motion.min_x) * abs(bound_motion.max_y - bound_motion.min_y);
	if (square > motion_photo_frame.motion_square) {
		motion_photo_frame.motion_square = square;
		motion_photo_frame.photo_frame = result;
	}
}
