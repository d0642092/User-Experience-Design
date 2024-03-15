#pragma once
#ifndef GAME_PARAMETER
#define GAME_PARAMETER

#include <iostream>
#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/imgproc/imgproc.hpp>
using namespace std;
using namespace cv;

typedef struct UnitCircle {
	bool isGreen = false;
	Point center;
	vector<int> adjacentItem;
}UnitCircle;

class GameParameter
{
public:
	GameParameter();
	vector<UnitCircle> circleList;
	vector<Point> validArea;
	vector<int> xList, yList, xFinishList, yFinishList;
	Scalar lowHsv, highHsv;
	Point radius, detectCenter;
	Size blurSize;
	Mat initImg, finishImg, element, gameStatus;
	bool isFinish;
	int videoRows, videoCols, videoType;
	int judge, bufferChange;

	//func
	void setting(Point, int);
	void set_circle_list(Point, vector<int>);
	void generate_finish_img();
	void init_start_img();
};

GameParameter::GameParameter() {
	element = getStructuringElement(MORPH_RECT, Size(7, 7));
	validArea = { Point(120, 60), Point(520, 420) };
	radius = Point(45, 10);
	detectCenter = validArea[0];
	highHsv = Scalar(140, 200, 200);
	lowHsv = Scalar(100, 100, 100);
	blurSize = Size(30, 30);
	judge = 0;
	bufferChange = 0;
	isFinish = false;
}

void GameParameter::setting(Point videoSize, int type) {
	videoRows = videoSize.x;
	videoCols = videoSize.y;
	videoType = type;
	for (int i = 185; i <= videoCols - 140; i += 90) {
		xList.push_back(i);
	}
	for (int i = 105; i < videoRows - 60; i += 90) {
		yList.push_back(i);
	}
	gameStatus = Mat::zeros(videoRows, videoCols, videoType);
	generate_finish_img();
	init_start_img();
}

void GameParameter::init_start_img() {
	initImg = Mat::zeros(videoRows, videoCols, videoType);
	rectangle(initImg, validArea[0], validArea[1], Scalar(0, 255, 0), 2);
	// t b l r
	int list[4] = { -1, 1, -4, 4 };
	int nowIdx = 0, value;
	for (int i = 185; i <= videoCols - 140; i += 90) {
		for (int j = 105; j < videoRows - 60; j += 90) {
			circle(initImg, Point(i, j), radius.x, Scalar(0, 0, 255), 2);
			circle(initImg, Point(i, j), radius.y, Scalar(125, 125, 125), 2);
			UnitCircle unit;
			unit.isGreen = false;
			unit.center = Point(i, j);
			for (int k = 0; k < 4; k += 1) {
				if (nowIdx % 4 == 3 && k == 1) continue;
				if (nowIdx % 4 == 0 && k == 0) continue;
				value = nowIdx + list[k];
				if (value >= 0 && value < 16) unit.adjacentItem.push_back(value);
			}
			circleList.push_back(unit);
			nowIdx += 1;
		}
	}
}

void GameParameter::generate_finish_img() {
	int margin = 60;
	xFinishList.push_back(validArea[0].x + margin);
	xFinishList.push_back(validArea[1].x - margin);
	yFinishList.push_back(validArea[0].y + margin);
	yFinishList.push_back(videoRows / 2 - margin);
	yFinishList.push_back(videoRows / 2 + margin);
	yFinishList.push_back(validArea[1].y - margin);

	finishImg = Mat::zeros(videoRows, videoCols, videoType);
	rectangle(finishImg, validArea[0], validArea[1], Scalar(0, 255, 0), 2);
	rectangle(finishImg, Point(xFinishList[0], yFinishList[0]), Point(xFinishList[1], yFinishList[1]), Scalar(0, 255, 0), -1);
	rectangle(finishImg, Point(xFinishList[0], yFinishList[2]), Point(xFinishList[1], yFinishList[3]), Scalar(0, 0, 255), -1);

	putText(finishImg, "Restart", Point(validArea[0].x + 20, yFinishList[0] - 20), FONT_HERSHEY_DUPLEX, 1, Scalar(255,255,255), 2);
	putText(finishImg, "Finish", Point(validArea[0].x + 20, yFinishList[2] - 20), FONT_HERSHEY_DUPLEX, 1, Scalar(255,255,255), 2);
	
}	
#endif
