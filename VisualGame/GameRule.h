#pragma once
#ifndef GAMERULE
#define GAMERULE

typedef struct UnitCircle {
	bool isGreen = false;
	Point center;
	vector<int> adjacentItem;
}UnitCircle;

Mat init_game(Mat, Point, vector<Point>, vector<UnitCircle>*);
void color_change(Mat*, vector<UnitCircle>*, int*, int, int);
int find_close(vector<int>, vector<int>, Point);

Mat init_game(Mat video, Point radius, vector<Point> validArea, vector<UnitCircle>* p) {
	Mat img = Mat::zeros(video.rows, video.cols, video.type());
	rectangle(img, validArea[0], validArea[1], Scalar(0, 255, 0), 2);
	// t b l r
	int list[4] = { -1, 1, -4, 4 };
	int nowIdx = 0, value;
	for (int i = 185; i <= video.cols - 140; i += 90) {
		for (int j = 105; j < video.rows - 60; j += 90) {
			circle(img, Point(i, j), radius.x, Scalar(0, 0, 255), 2);
			circle(img, Point(i, j), radius.y, Scalar(125, 125, 125), 2);
			UnitCircle unit;
			unit.isGreen = false;
			unit.center = Point(i, j);
			for (int k = 0; k < 4; k += 1) {
				if (nowIdx % 4 == 3 && k == 1) continue;
				if (nowIdx % 4 == 0 && k == 0) continue;
				value = nowIdx + list[k];
				if (value >= 0 && value < 16) unit.adjacentItem.push_back(value);
			}
			p[0].push_back(unit);
			nowIdx += 1;
		}
	}
	return img;
}

void color_change(Mat* gameStatus, vector<UnitCircle>* circleList, int* judge, int closeIdx, int radius) {
	vector<int> adjIndex = circleList[0][closeIdx].adjacentItem;
	adjIndex.push_back(closeIdx);
	for (int i = 0; i < adjIndex.size(); i += 1) {
		Scalar now = (circleList[0][adjIndex[i]].isGreen) ? (*judge -= 1, Scalar(0, 0, 255)) : (*judge += 1, Scalar(0, 255, 0));
		circle(*gameStatus, circleList[0][adjIndex[i]].center, radius, now, -1);
		circleList[0][adjIndex[i]].isGreen = !circleList[0][adjIndex[i]].isGreen;
	}
	waitKey(200);
	//return gameStatus;
}

int find_close(vector<int> xList, vector<int> yList, Point detectCenter) {
	// return max num: 15
	Point closeCircle = Point(-99, -99);
	int value = 0;
	for (int i = 0; i < 4; i += 1) {
		value = abs(detectCenter.x - xList[i]);
		if (value < 10) closeCircle.x = i;;
		value = abs(detectCenter.y - yList[i]);
		if (value < 10) closeCircle.y = i;
	}
	return 4 * closeCircle.x + closeCircle.y;
}



#endif