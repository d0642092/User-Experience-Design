#pragma once
#ifndef IMG_PROCESS
#define IMG_PROCESS

Mat combine_video(Mat, Mat, Point*, vector<Point>, vector<vector<Point>>);
vector<vector<Point>> detect_color(Mat, Mat, vector<Point>);

vector<vector<Point>> detect_color(Mat video, Mat element, vector<Point> validArea) {
	vector<vector<Point>> contours;
	Scalar lowHsv = Scalar(100, 100, 100);
	Scalar highHsv = Scalar(140, 200, 200);
	Mat colorRange, videoHSV;

	cvtColor(video, videoHSV, COLOR_BGR2HSV);
	inRange(videoHSV, lowHsv, highHsv, colorRange);
	// Dilation and Erosion
	morphologyEx(colorRange, colorRange, MORPH_OPEN, element);
	morphologyEx(colorRange, colorRange, MORPH_DILATE, element);

	// find all point and add valid area rectangle
	findContours(colorRange, contours, CV_RETR_EXTERNAL, CHAIN_APPROX_NONE);
	rectangle(colorRange, validArea[0], validArea[1], Scalar(255, 255, 0), 2);
	imshow("Valid Area", colorRange);
	return contours;
}


// combine video, color position and img
Mat combine_video(Mat video, Mat initImg, Point* detectCenter, vector<Point> validArea, vector<vector<Point>> contours) {
	vector<Moments> mu(contours.size());
	Mat mixtureVisual = video.clone();
	int x = 0;
	for (int i = 0; i < contours.size(); i += 1)
	{
		// detect valid point
		mu[i] = moments(contours[i], false);
		x = (int)(mu[i].m10 / mu[i].m00);
		if (x < validArea[0].x || x > validArea[1].x) continue;
		*detectCenter = Point(x, (int)(mu[i].m01 / mu[i].m00));
	}
	circle(mixtureVisual, *detectCenter, 10, Scalar(255, 0, 0), -1);
	// black background add init image
	absdiff(video.clone(), mixtureVisual, mixtureVisual);
	absdiff(initImg, mixtureVisual, mixtureVisual);
	return mixtureVisual;
}

#endif