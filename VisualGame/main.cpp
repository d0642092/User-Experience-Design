#include "InitLibrary.h" // must first include, contain external package
#include "GameRule.h"	// contains game rule, init_game, color_change, find_close and circle struct
#include "ImgProcess.h" // contain combine_video and detect color

using namespace std;
bool finish_menu(VideoCapture, Size, Mat, vector<Point>);

int main(int argc, char** argv) {
	vector<vector<Point>> contours;
	vector<Point> validArea = { Point(120, 60), Point(520, 420) };
	vector<UnitCircle> circleList;
	vector<int> xList, yList, adjIndex;
	Point detectCenter = validArea[0];
	Point radius = Point(45, 10);
	Size blurSize = Size(30, 30);

	VideoCapture cap(0);
	Mat video, gameVisual;
	cap >> video;
	Mat initImg = init_game(video, radius, validArea, &circleList);
	Mat gameStatus = Mat::zeros(video.rows, video.cols, video.type());
	Mat element = getStructuringElement(MORPH_RECT, Size(7, 7));
	int closeCircle = 0, judge = 0, bufferChange = 0;

	for (int i = 185; i <= video.cols - 140; i += 90) {
		xList.push_back(i);
	}
	for (int i = 105; i < video.rows - 60; i += 90) {
		yList.push_back(i);
	}
	// press any key will stop
	while (waitKey(1) == -1) {
		cap >> video;
		if (video.empty()) 
			break;
		//imshow("normal", video);
		flip(video, video, 1); //左右反轉
		blur(video, video, blurSize);

		// detect color and erase noise
		contours = detect_color(video, element, validArea);
		// calculate center and mix into video
		gameVisual = combine_video(video, initImg, &detectCenter, validArea, contours);

		// game rule
		closeCircle = find_close(xList, yList, detectCenter);

		if (closeCircle >= 0 && bufferChange <= 0) {
			bufferChange = 10;
			color_change(&gameStatus, &circleList, &judge, closeCircle, radius.x);
		}
		bufferChange -= 1;
		absdiff(gameVisual, gameStatus, gameVisual);
		//imshow("game", gameVisual);
		//Mat test;
		addWeighted(gameVisual, 10.0, video, 0.5, 1, gameVisual); // 遊戲畫面與視訊畫面結合
		imshow("testing", gameVisual);

		// end game
		if (judge == 16) {
			waitKey(300);
			detectCenter = validArea[0];
			destroyAllWindows();
			if (!finish_menu(cap, blurSize, element, validArea))
				break;
			// reset status
			circleList.clear();
			detectCenter = validArea[0];
			judge = 0;
			initImg = init_game(video, radius, validArea, &circleList);
			gameStatus = Mat::zeros(video.rows, video.cols, video.type());
			destroyAllWindows();
		}

	}
	return 0;
}

bool finish_menu(VideoCapture cap, Size blurSize, Mat element, vector<Point> validArea) {
	// default end game
	vector<vector<Point>> contours;
	Point detectCenter = Point(120, 60);
	Mat video, finishVisual;
	cap >> video;
	int margin = 60;
	int xList[2] = { validArea[0].x + margin, validArea[1].x - margin };
	int yList[4] = { validArea[0].y + margin,  video.rows / 2 - margin,
					 video.rows / 2 + margin, validArea[1].y - margin };
	Mat finishMenu = Mat::zeros(video.rows, video.cols, video.type());
	rectangle(finishMenu, validArea[0], validArea[1], Scalar(0, 255, 0), 2);
	rectangle(finishMenu, Point(xList[0], yList[0]), Point(xList[1], yList[1]), Scalar(0, 255, 0), -1);
	rectangle(finishMenu, Point(xList[0], yList[2]), Point(xList[1], yList[3]), Scalar(0, 0, 255), -1);

	while (waitKey(1) == -1) {
		cap >> video;
		if (video.empty()) break;
		flip(video, video, 1); //左右反轉
		blur(video, video, blurSize);
		// detect color and gernerate finish menu
		contours = detect_color(video, element, validArea);
		finishVisual = combine_video(video, finishMenu, &detectCenter, validArea, contours);
		imshow("game", finishVisual);
		// choose restart or endGame;
		if (detectCenter.x > xList[0] && detectCenter.x < xList[1]) {
			if (detectCenter.y > yList[0] && detectCenter.y < yList[1]) 
				return true;
			else if (detectCenter.y > yList[2] && detectCenter.y < yList[3]) 
				return false;
		}
	}
	return false;
}

