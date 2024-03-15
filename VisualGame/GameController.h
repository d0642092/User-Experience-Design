#pragma once
#ifndef GAME_CONTROLLER
#define GAME_CONTROLLER

#include "GameParameter.h"

class GameControl
{
private:
	GameParameter parameter;
	Mat combine_video(Mat, Mat, vector<vector<Point>>);
	int find_close();
	void color_change(int);
public:
	GameControl();
	void init_game(Mat);
	void reset_detect_center();
	bool isFinish(Mat*);
	int isRestart();
	Mat get_now_status(Mat*, Mat*);

};

GameControl::GameControl(){}

Mat GameControl::combine_video(Mat video, Mat img, vector<vector<Point>> contours) {
	// combine video, color position and img
	vector<Moments> mu(contours.size());
	Mat mixtureVisual = video.clone();
	int x = 0;
	for (int i = 0; i < contours.size(); i += 1)
	{
		// detect valid point
		mu[i] = moments(contours[i], false);
		x = (int)(mu[i].m10 / mu[i].m00);
		if (x < parameter.validArea[0].x || x > parameter.validArea[1].x) continue;
		parameter.detectCenter = Point(x, (int)(mu[i].m01 / mu[i].m00));
	}
	circle(mixtureVisual, parameter.detectCenter, 10, Scalar(255, 0, 0), -1);
	// black background add init image
	absdiff(video.clone(), mixtureVisual, mixtureVisual);
	absdiff(img, mixtureVisual, mixtureVisual);
	return mixtureVisual;
}
int GameControl::find_close() {
	// return max num: 15
	Point closeCircle = Point(-99, -99);
	for (int i = 0; i < 4; i += 1) {
		if (abs(parameter.detectCenter.x - parameter.xList[i]) < 10) 
			closeCircle.x = i;
		if (abs(parameter.detectCenter.y - parameter.yList[i]) < 10) 
			closeCircle.y = i;
	}
	return 4 * closeCircle.x + closeCircle.y;
}
void GameControl::color_change(int closeIdx) {
	vector<int> adjIndex = parameter.circleList[closeIdx].adjacentItem;
	adjIndex.push_back(closeIdx);
	for (int i = 0; i < adjIndex.size(); i += 1) {
		Scalar now = (parameter.circleList[adjIndex[i]].isGreen) ? (parameter.judge -= 1, Scalar(0, 0, 255)) : (parameter.judge += 1, Scalar(0, 255, 0));
		circle(parameter.gameStatus, parameter.circleList[adjIndex[i]].center, parameter.radius.x, now, -1);
		parameter.circleList[adjIndex[i]].isGreen = !parameter.circleList[adjIndex[i]].isGreen;
	}
	waitKey(200);
	//return gameStatus;
}

void GameControl::init_game(Mat video) {
	parameter = GameParameter();
	parameter.setting(Point(video.rows, video.cols), video.type());
}
void GameControl::reset_detect_center() {
	parameter.detectCenter = parameter.validArea[0];
}
bool GameControl::isFinish(Mat* gameVisual) {
	int closeCircle = find_close();

	if (closeCircle >= 0 && parameter.bufferChange <= 0) {
		parameter.bufferChange = 3;
		color_change(closeCircle);
	}
	parameter.bufferChange -= 1;
	absdiff(*gameVisual, parameter.gameStatus, *gameVisual);
	if (parameter.judge == 16) {
		parameter.isFinish = true;
		return parameter.isFinish;
	}
	return parameter.isFinish;
}
int GameControl::isRestart() {
	if (parameter.detectCenter.x > parameter.xFinishList[0] && parameter.detectCenter.x < parameter.xFinishList[1]) {
		if (parameter.detectCenter.y > parameter.yFinishList[0] && parameter.detectCenter.y < parameter.yFinishList[1])
			return 1;
		else if (parameter.detectCenter.y > parameter.yFinishList[2] && parameter.detectCenter.y < parameter.yFinishList[3])
			return 0;
	}
	return -1;
}
Mat GameControl::get_now_status(Mat* video, Mat* validVisual) {
	// image process
	flip(*video, *video, 1);
	blur(*video, *video, parameter.blurSize);
	// detect color and erase noise
	vector<vector<Point>> contours;
	Mat videoHSV;
	cvtColor(*video, videoHSV, COLOR_BGR2HSV);
	inRange(videoHSV, parameter.lowHsv, parameter.highHsv, *validVisual);
	// Dilation and Erosion
	morphologyEx(*validVisual, *validVisual, MORPH_OPEN, parameter.element);
	morphologyEx(*validVisual, *validVisual, MORPH_DILATE, parameter.element);

	// find all point and add valid area rectangle
	findContours(*validVisual, contours, CV_RETR_EXTERNAL, CHAIN_APPROX_NONE);
	rectangle(*validVisual, parameter.validArea[0], parameter.validArea[1], Scalar(255, 255, 0), 2);

	// calculate center and mix into video
	Mat combineImg = (parameter.isFinish) ? parameter.finishImg : parameter.initImg;
	return combine_video(*video, combineImg, contours);
}


#endif