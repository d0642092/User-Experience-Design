#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/photo.hpp>
#include <time.h>
#include <iostream>
#include <windows.h>
#include <cstdlib> /* 亂數相關函數 */
#include <ctime>   /* 時間相關函數 */

using namespace cv;
using namespace std;

//變數
vector<Point> pointList;
int PtoP = 104;
int z1 = 20;
//fucntion
void video();
Mat creatImg(int x, int y, int z);

int main(int argc, char** argv)
{
	video();
	return 0;
}

Mat creatImg(int x, int y, int z) {
	srand(time(NULL));
	
	Mat img = Mat::zeros(x, y, z);
	int i, j;
	for (j = 90; j <= y - 160; j += PtoP) {
		for (i = 110; i < x; i += PtoP) {
			
			int random = rand() % 2;
			if (random == 1) {
				circle(img, Point(i, j), 50, Scalar(0, 0, 255), 2);
			}
			else {
				circle(img, Point(i, j), 50, Scalar(0, 255, 0), 2);
			}
			circle(img, Point(i, j), z1, Scalar(125, 125, 125), 2);
			pointList.push_back(Point(i, j));
		}
	}
	return img;
}



void video()
{
	VideoCapture cap(0);
	Mat img, imgHSV;
	int i, j;

	vector< std::vector<cv::Point> > OriginalContours;
	vector<Vec4i > hierarchy;
	vector<vector<Point>> FinalContours;
	vector<Point> hull;

	//藍色
	/*int iLowH = 100;
	int iHighH = 140;
	int iLowS = 40;
	int iHighS = 255;
	int iLowV = 40;
	int iHighV = 255;
	*/
	//綠色
	int iLowH = 78;
	int iHighH = 99;
	int iLowS = 43;
	int iHighS = 255;
	int iLowV = 46;
	int iHighV = 255;
	
	cap >> img;
	
	Mat test = creatImg(img.rows, img.cols, img.type());
	Mat out1 = Mat::zeros(img.rows, img.cols, img.type());;
	Mat out2 = Mat::zeros(img.rows, img.cols, img.type());;
	Mat backGround = Mat::zeros(img.rows, img.cols, img.type());
	
	Mat out;
	Mat d;

	int flag,flag2;
	int successful = 0;
	int X_position=0, Y_position=0;

	while (1) {

		cap >> img;

		if (img.empty())
			break;

		flip(img, img, 1); //左右反轉

		cvtColor(img, imgHSV, COLOR_BGR2HSV);//转为HSV

		Mat imgThresholded;
		inRange(imgHSV, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), imgThresholded);

		Mat element = getStructuringElement(MORPH_RECT, Size(5, 5));
		morphologyEx(imgThresholded, imgThresholded, MORPH_OPEN, element);
		//闭操作 (连接一些连通域)
		morphologyEx(imgThresholded, imgThresholded, MORPH_CLOSE, element);

		vector<vector<Point>> contours;
		findContours(imgThresholded, contours, CV_RETR_EXTERNAL, CHAIN_APPROX_NONE); //找輪廓
		vector<vector<Point>>::iterator cit = contours.begin();
		while (cit != contours.end()) {
			if (contourArea(*cit) < 300) { //過濾面積太小的區塊
				cit = contours.erase(cit);
			}
			else cit++;
		}
		
		//區塊
		Mat dst = img.clone();
		vector<vector<Point>> hull(contours.size());
		cvtColor(imgThresholded, imgThresholded, CV_GRAY2BGR);

		for (int i = 0; i < contours.size(); i++) { //畫出區塊
			convexHull(contours[i], hull[i]);
		//	drawContours(dst, hull, i, Scalar(0, 0, 255), 2);
		}

		//中心點
		vector<Moments> mu(contours.size());
		for (int i = 0; i < contours.size(); i++)
		{
			mu[i] = moments(contours[i], false);
		}

		Point mc(contours.size());
		for (int i = 0; i < contours.size(); i++)
		{
			mc = Point2f(mu[i].m10 / mu[i].m00, mu[i].m01 / mu[i].m00);
		}
		circle(dst, mc, 10, Scalar(255, 0, 0), -1);


		//判斷
		for (int n = 0; n < pointList.size(); n++) {
			Point myPoint = pointList[n];
			successful = 1;
			if (test.at<Vec3b>(myPoint.y+51, myPoint.x)[1] != 255) {
				successful = 0;
				break;
			}
		}

		if (successful == 1) {			
			break;
		}
		
		
		//顏色轉換
		for (int n = 0; n < pointList.size(); n++) {
			
			Point myPoint = pointList[n];
			flag = 0;
			flag2 = 0;
			
			if (mc.x == 0 & mc.y == 0) {
				break;
			}

			// 判斷是否在同一個圈
			if (X_position != 0 & Y_position != 0) {
				if (mc.x >= X_position - z1 & mc.x <= X_position + z1) {
					if (mc.y >= Y_position - z1 & mc.y <= Y_position + z1) {
						break;
					}
				}
			}
			
			
			if (mc.x >= myPoint.x - z1 & mc.x <= myPoint.x + z1) {
				if (mc.y >= myPoint.y - z1 & mc.y <= myPoint.y + z1) {
						
						
					X_position = myPoint.x;
					Y_position = myPoint.y;
					

					int x = myPoint.x;
					int y = myPoint.y;


					// X軸轉換
					if (x == 110) {
						if (test.at<Vec3b>(y, x + 51)[2] == 255) {
							circle(out1, myPoint, 50, Scalar(0, 0, 255), 2);

							if (test.at<Vec3b>(y, x + PtoP - 1)[2] == 255) {
								circle(out1, Point(x + PtoP, y), 50, Scalar(0, 0, 255), 2);
								flag = 1;
							}
							else {
								circle(out1, Point(x + PtoP, y), 50, Scalar(0, 255, 0), 2);
							}

							cv::absdiff(test, out1, out2);
							test = out2.clone();
							cv::absdiff(out1, out1, out1);
								
							circle(out1, myPoint, 50, Scalar(0, 255, 0), 2);
								
							if (flag == 1) {
								circle(out1, Point(x + PtoP, y), 50, Scalar(0, 255, 0), 2);
							}
							else {
								circle(out1, Point(x + PtoP, y), 50, Scalar(0, 0, 255), 2);
							}


						}
						else {
							circle(out1, myPoint, 50, Scalar(0, 255, 0), 2);
								
							if (test.at<Vec3b>(y, x + PtoP - 1)[2] == 255) {
								circle(out1, Point(x + PtoP, y), 50, Scalar(0, 0, 255), 2);
								flag = 1;
							}
							else {
								circle(out1, Point(x + PtoP, y), 50, Scalar(0, 255, 0), 2);
							}

							cv::absdiff(test, out1, out2);
							test = out2.clone();
							cv::absdiff(out1, out1, out1);
								
							circle(out1, myPoint, 50, Scalar(0, 0, 255), 2);

							if (flag == 1) {
								circle(out1, Point(x + PtoP, y), 50, Scalar(0, 255, 0), 2);
							}
							else {
								circle(out1, Point(x + PtoP, y), 50, Scalar(0, 0, 255), 2);
							}
						}

					}
					else {
						if (x + 100 < img.rows) {
							if (test.at<Vec3b>(y, x + 51)[2] == 255) {
								circle(out1, myPoint, 50, Scalar(0, 0, 255), 2);

								if (test.at<Vec3b>(y, x + PtoP - 1)[2] == 255) {
									circle(out1, Point(x + PtoP, y), 50, Scalar(0, 0, 255), 2);
									flag = 1;
								}
								else {
									circle(out1, Point(x + PtoP, y), 50, Scalar(0, 255, 0), 2);
								}

								if (test.at<Vec3b>(y, x - PtoP - 1)[2] == 255) {
									circle(out1, Point(x - PtoP, y), 50, Scalar(0, 0, 255), 2);
									flag2 = 1;
								}
								else {
									circle(out1, Point(x - PtoP, y), 50, Scalar(0, 255, 0), 2);
								}
								cv::absdiff(test, out1, out2);
								test = out2.clone();
								cv::absdiff(out1, out1, out1);

								circle(out1, myPoint, 50, Scalar(0, 255, 0), 2);

								if (flag == 1) {
									circle(out1, Point(x + PtoP, y), 50, Scalar(0, 255, 0), 2);
								}
								else {
									circle(out1, Point(x + PtoP, y), 50, Scalar(0, 0, 255), 2);
								}

								if (flag2 == 1) {
									circle(out1, Point(x - PtoP, y), 50, Scalar(0, 255, 0), 2);
								}
								else {
									circle(out1, Point(x - PtoP, y), 50, Scalar(0, 0, 255), 2);
								}
							}
							else {
								circle(out1, myPoint, 50, Scalar(0, 255, 0), 2);

								if (test.at<Vec3b>(y, x + PtoP - 1)[2] == 255) {
									circle(out1, Point(x + PtoP, y), 50, Scalar(0, 0, 255), 2);
									flag = 1;
								}
								else {
									circle(out1, Point(x + PtoP, y), 50, Scalar(0, 255, 0), 2);
								}

								if (test.at<Vec3b>(y, x - PtoP - 1)[2] == 255) {
									circle(out1, Point(x - PtoP, y), 50, Scalar(0, 0, 255), 2);
									flag2 = 1;
								}
								else {
									circle(out1, Point(x - PtoP, y), 50, Scalar(0, 255, 0), 2);
								}

								cv::absdiff(test, out1, out2);
								test = out2.clone();
								cv::absdiff(out1, out1, out1);

								circle(out1, myPoint, 50, Scalar(0, 0, 255), 2);

								if (flag == 1) {
									circle(out1, Point(x + PtoP, y), 50, Scalar(0, 255, 0), 2);
								}
								else {
									circle(out1, Point(x + PtoP, y), 50, Scalar(0, 0, 255), 2);
								}
								if (flag2 == 1) {
									circle(out1, Point(x - PtoP, y), 50, Scalar(0, 255, 0), 2);
								}
								else {
									circle(out1, Point(x - PtoP, y), 50, Scalar(0, 0, 255), 2);
								}
							}
						}
						else {
							if (test.at<Vec3b>(y, x + 51)[2] == 255) {
								circle(out1, myPoint, 50, Scalar(0, 0, 255), 2);
									
								if (test.at<Vec3b>(y, x - PtoP - 1)[2] == 255) {
									circle(out1, Point(x - PtoP, y), 50, Scalar(0, 0, 255), 2);
									flag = 1;
								}
								else {
									circle(out1, Point(x - PtoP, y), 50, Scalar(0, 255, 0), 2);
								}

								cv::absdiff(test, out1, out2);
								test = out2.clone();
								cv::absdiff(out1, out1, out1);

								circle(out1, myPoint, 50, Scalar(0, 255, 0), 2);
								if (flag == 1) {
									circle(out1, Point(x - PtoP, y), 50, Scalar(0, 255, 0), 2);
								}
								else {
									circle(out1, Point(x - PtoP, y), 50, Scalar(0, 0, 255), 2);
								}
									
							}
							else {
								circle(out1, myPoint, 50, Scalar(0, 255, 0), 2);

								if (test.at<Vec3b>(y, x - PtoP - 1)[2] == 255) {
									circle(out1, Point(x - PtoP, y), 50, Scalar(0, 0, 255), 2);
									flag = 1;
								}
								else {
									circle(out1, Point(x - PtoP, y), 50, Scalar(0, 255, 0), 2);
								}
									
								cv::absdiff(test, out1, out2);
								test = out2.clone();
								cv::absdiff(out1, out1, out1);

								circle(out1, myPoint, 50, Scalar(0, 0, 255), 2);
								
								if (flag == 1) {
									circle(out1, Point(x - PtoP, y), 50, Scalar(0, 255, 0), 2);
								}
								else {
									circle(out1, Point(x - PtoP, y), 50, Scalar(0, 0, 255), 2);
								}
							}

						}
					}


					//y軸轉換
					if (y == 90) {
						if (test.at<Vec3b>(y + PtoP - 1, x)[2] == 255) {
							circle(out1, Point(x, y + PtoP), 50, Scalar(0, 0, 255), 2);
							flag = 1;
						}
						else {
							circle(out1, Point(x, y + PtoP), 50, Scalar(0, 255, 0), 2);
						}
						cv::absdiff(test, out1, out2);
						test = out2.clone();
						cv::absdiff(out1, out1, out1);


						if (flag == 1) {
							circle(out1, Point(x, y + PtoP), 50, Scalar(0, 255, 0), 2);
						}
						else {
							circle(out1, Point(x, y + PtoP), 50, Scalar(0, 0, 255), 2);
						}
					}
					else {
						if (y + 100 < img.rows) {
							if (test.at<Vec3b>(y + PtoP - 1, x)[2] == 255) {
								circle(out1, Point(x, y + PtoP), 50, Scalar(0, 0, 255), 2);
								flag = 1;
							}
							else {
								circle(out1, Point(x, y + PtoP), 50, Scalar(0, 255, 0), 2);
							}
							if (test.at<Vec3b>(y - PtoP - 1, x)[2] == 255) {
								circle(out1, Point(x, y - PtoP), 50, Scalar(0, 0, 255), 2);
								flag2 = 1;
							}
							else {
								circle(out1, Point(x, y - PtoP), 50, Scalar(0, 255, 0), 2);
							}
							cv::absdiff(test, out1, out2);
							test = out2.clone();
							cv::absdiff(out1, out1, out1);


							if (flag == 1) {
								circle(out1, Point(x, y + PtoP), 50, Scalar(0, 255, 0), 2);
							}
							else {
								circle(out1, Point(x, y + PtoP), 50, Scalar(0, 0, 255), 2);
							}
							if (flag2 == 1) {
								circle(out1, Point(x, y - PtoP), 50, Scalar(0, 255, 0), 2);
							}
							else {
								circle(out1, Point(x, y - PtoP), 50, Scalar(0, 0, 255), 2);
							}
						}
						else {
							if (test.at<Vec3b>(y - PtoP - 1, x)[2] == 255) {
								circle(out1, Point(x, y - PtoP), 50, Scalar(0, 0, 255), 2);
								flag = 1;
							}
							else {
								circle(out1, Point(x, y - PtoP), 50, Scalar(0, 255, 0), 2);
							}
							cv::absdiff(test, out1, out2);
							test = out2.clone();
							cv::absdiff(out1, out1, out1);


							if (flag == 1) {
								circle(out1, Point(x, y - PtoP), 50, Scalar(0, 255, 0), 2);
							}
							else {
								circle(out1, Point(x, y - PtoP), 50, Scalar(0, 0, 255), 2);
							}
						}

					}

					break;
				}
			}

		}

		cv::absdiff(test, out1, out2);
		test = out2.clone();
		cv::absdiff(out1, out1, out1);
		

		addWeighted(test, 10.0, dst, 0.5, 1, out);
		//圖形加攝像

		imshow("finish", out);

		char c = (char)waitKey(25); //按esc結束程式
		if (c == 27)
			break;
	}
	
	
	destroyAllWindows();
	Mat fin = imread("img.jpg");
	imshow("congradulation", fin);
	waitKey(0);
	cvDestroyAllWindows();
	return;
}
