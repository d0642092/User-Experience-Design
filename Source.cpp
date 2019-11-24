#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/photo.hpp>
#include <time.h>
#include <iostream>

using namespace cv;
using namespace std;

Mat Sourceimage, Spilteimage, Rebuildimage, Dstimage;
int rows, cols;
int Roirows, Roicols;
vector<Point> pointList;
vector<Mat>arraryimage;
void Randarrary(vector<Mat> &vectorMat);    //隨機排列子圖像序列函數
static int vectornumber = 0;
//void video(); //攝影機
void video2();
Mat images(int x, int y);
Mat creatImg(int x, int y, int z);

int main(int argc, char** argv)
{
	video2(); //皮膚色
	//video(); //藍色

	//
	//waitKey();
	//images();
	return 0;
}

Mat creatImg(int x, int y, int z) {

	Mat img = Mat::zeros(x, y, z);
	int i, j;
	for (j = 90; j <= y - 160; j += 100) {
		for (i = 110; i < x; i += 100) {
			circle(img, Point(i, j), 50, Scalar(0, 0, 255), 2);
			circle(img, Point(i, j), 30, Scalar(125, 125, 125), 2);
			pointList.push_back(Point(i, j));
		}

	}
	return img;
}

Mat images(int x, int y) {
	Sourceimage = imread("img.jpg");
	//Mat backGround = Mat::zeros(x, y+100, Sourceimage.type());
	resize(Sourceimage, Sourceimage, Size(y, x));
	//seamlessClone(Sourceimage, backGround, Sourceimage, Point(300,250), Sourceimage,NORMAL_CLONE);
	imshow("Source image", Sourceimage);
	rows = atoi("4");
	cols = atoi("4");
	Roirows = Sourceimage.rows / rows;
	Roicols = Sourceimage.cols / cols;
	Spilteimage = Mat::zeros(Sourceimage.rows, Sourceimage.cols, Sourceimage.type());
	Dstimage = Mat::zeros(Sourceimage.rows, Sourceimage.cols, Sourceimage.type());
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			Mat SourceRoi = Sourceimage(Rect(j*Roicols, i*Roirows, Roicols - 1, Roirows - 1));
			arraryimage.push_back(SourceRoi);
		}
	}
	// 随機函數	
	Randarrary(arraryimage);
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			Mat SpilterRoi = Spilteimage(Rect(j*Roicols, i*Roirows, Roicols - 1, Roirows - 1));
			addWeighted(SpilterRoi, 0, arraryimage[vectornumber], 1, 0, SpilterRoi);
			vectornumber++;
			imshow("Splite image", Spilteimage);
			waitKey(150);
		}
	}

	return Spilteimage;

}

void Randarrary(vector<Mat>& vectorMat)
{
	for (int i = 0; i < vectorMat.size(); i++)
	{
		srand(int(time(0)));
		int a = rand() % (vectorMat.size() - i) + i;
		swap(vectorMat[i], vectorMat[a]);
	}
}




void video2()
{
	VideoCapture cap(0);
	Mat img, imgHSV;
	int i, j;

	vector< std::vector<cv::Point> > OriginalContours;
	vector<Vec4i > hierarchy;
	vector<vector<Point>> FinalContours;
	vector<Point> hull;
	/*int iLowH = 100 / 2;
	int iHighH = 120 / 2;
	int iLowS = 50 * 255 / 100;
	int iHighS = 70 * 255 / 100;
	int iLowV = 40 * 255 / 100;
	int iHighV = 50 * 255 / 100;*/

	int flag_color[16];
	for (int i = 0; i < 4; i++)
	{
		flag_color[i] = 0;
	}

	//藍色
	int iLowH = 100;
	int iHighH = 140;
	int iLowS = 40;
	int iHighS = 255;
	int iLowV = 40;
	int iHighV = 255;
	cap >> img;
	//Mat test = images(img.rows, img.cols);
	Mat test = creatImg(img.rows, img.cols, img.type());
	Mat out1 = Mat::zeros(img.rows, img.cols, img.type());;

	int flag;

	while (1) {

		cap >> img;

		imshow("t", test);
		Mat backGround = Mat::zeros(img.rows, img.cols, img.type());
		Mat out;

		Mat d;
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
		//namedWindow("Thresholded Image");

		vector<vector<Point>> contours;
		findContours(imgThresholded, contours, CV_RETR_EXTERNAL, CHAIN_APPROX_NONE); //找輪廓
		vector<vector<Point>>::iterator cit = contours.begin();
		while (cit != contours.end()) {
			if (contourArea(*cit) < 300) { //過濾面積太小的區塊
				cit = contours.erase(cit);
			}
			else cit++;
		}
		Mat dst = img.clone();
		vector<vector<Point>> hull(contours.size());
		cvtColor(imgThresholded, imgThresholded, CV_GRAY2BGR);
		for (int i = 0; i < contours.size(); i++) { //畫出區塊
			convexHull(contours[i], hull[i]);
			//drawContours(dst, hull, i, Scalar(0, 0, 255), 2);
		}

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

		backGround = img.clone();

		absdiff(backGround, dst, d);
		absdiff(test, d, out);

		//imshow("Thresholded Image", dst);

		//imshow("d", d );

		for (int n = 0; n < pointList.size(); n++) {
			Point myPoint = pointList[n];
			//if()
			if (mc.x == 0 & mc.y == 0) {
				break;
			}
			if (mc.x >= myPoint.x - 30 & mc.x <= myPoint.x + 30) {
				if (mc.y >= myPoint.y - 30 & mc.y <= myPoint.y + 30) {
					Scalar color = out.at<Vec3b>(myPoint.x + 50, myPoint.y + 50);

					if (color == Scalar(0, 0, 255)) {
						circle(out1, myPoint, 50, Scalar(0, 0, 255), -1);
						subtract(out, out1, out);
						circle(out, myPoint, 50, Scalar(0, 255, 0), -1);

					}
					circle(out1, myPoint, 50, Scalar(0, 0, 255), -1);



					/***********************************************/    //1
					if (myPoint.x == 110 && myPoint.y == 90)
					{
						if (flag_color[0] == 1)
						{
							circle(out1, Point(110, 90), 50, Scalar(0, 0, 255), -1);
							flag_color[0] = 0;

						}
						else
						{
							flag_color[0] = 1;
							circle(out1, Point(110, 90), 50, Scalar(0, 255, 0), -1);

						}
						if (flag_color[1] == 1)
						{
							circle(out1, Point(210, 90), 50, Scalar(0, 0, 255), -1);
							flag_color[1] = 0;
						}
						else
						{
							circle(out1, Point(210, 90), 50, Scalar(0, 255, 0), -1);
							flag_color[1] = 1;

						}

						if (flag_color[4] == 1)
						{
							circle(out1, Point(110, 190), 50, Scalar(0, 0, 255), -1);
							flag_color[4] = 0;
						}
						else
						{
							circle(out1, Point(110, 190), 50, Scalar(0, 255, 0), -1);
							flag_color[4] = 1;

						}
					}
					/*************************************************************/ // 2  
					else if (myPoint.x == 210 && myPoint.y == 90)
					{
						if (flag_color[1] == 1)
						{
							circle(out1, Point(210, 90), 50, Scalar(0, 0, 255), -1);
							flag_color[1] = 0;


						}
						else
						{
							flag_color[1] = 1;
							circle(out1, Point(210, 90), 50, Scalar(0, 255, 0), -1);


						}

						if (flag_color[0] == 1)
						{
							circle(out1, Point(110, 90), 50, Scalar(0, 0, 255), -1);
							flag_color[0] = 0;
						}
						else
						{
							circle(out1, Point(110, 90), 50, Scalar(0, 255, 0), -1);
							flag_color[0] = 1;

						}

						if (flag_color[2] == 1)
						{
							circle(out1, Point(310, 90), 50, Scalar(0, 0, 255), -1);
							flag_color[2] = 0;
						}
						else
						{
							circle(out1, Point(310, 90), 50, Scalar(0, 255, 0), -1);
							flag_color[2] = 1;

						}
						if (flag_color[5] == 1)
						{
							circle(out1, Point(210, 190), 50, Scalar(0, 0, 255), -1);
							flag_color[5] = 0;
						}
						else
						{
							circle(out1, Point(210, 190), 50, Scalar(0, 255, 0), -1);
							flag_color[5] = 1;

						}

					}
					/*************************************************************/
					/*************************************************************/ // 3 
					else if (myPoint.x == 310 && myPoint.y == 90)
					{
						if (flag_color[2] == 1)
						{
							circle(out1, Point(310, 90), 50, Scalar(0, 0, 255), -1);
							flag_color[2] = 0;


						}
						else
						{
							flag_color[2] = 1;
							circle(out1, Point(310, 90), 50, Scalar(0, 255, 0), -1);



						}
						if (flag_color[1] == 1)
						{
							circle(out1, Point(210, 90), 50, Scalar(0, 0, 255), -1);
							flag_color[1] = 0;
						}
						else
						{
							circle(out1, Point(210, 90), 50, Scalar(0, 255, 0), -1);
							flag_color[1] = 1;

						}

						if (flag_color[3] == 1)
						{
							circle(out1, Point(410, 90), 50, Scalar(0, 0, 255), -1);
							flag_color[3] = 0;
						}
						else
						{
							circle(out1, Point(410, 90), 50, Scalar(0, 255, 0), -1);
							flag_color[3] = 1;

						}
						if (flag_color[6] == 1)
						{
							circle(out1, Point(310, 190), 50, Scalar(0, 0, 255), -1);
							flag_color[6] = 0;
						}
						else
						{
							circle(out1, Point(310, 190), 50, Scalar(0, 255, 0), -1);
							flag_color[6] = 1;

						}

					}
					/*************************************************************/

					/*************************************************************/ // 4
					else if (myPoint.x == 410 && myPoint.y == 90)
					{
						if (flag_color[3] == 1)
						{
							circle(out1, Point(410, 90), 50, Scalar(0, 0, 255), -1);
							flag_color[3] = 0;


						}
						else
						{
							flag_color[3] = 1;
							circle(out1, Point(410, 90), 50, Scalar(0, 255, 0), -1);

						}
						if (flag_color[2] == 1)
						{
							circle(out1, Point(310, 90), 50, Scalar(0, 0, 255), -1);
							flag_color[2] = 0;
						}
						else
						{
							circle(out1, Point(310, 90), 50, Scalar(0, 255, 0), -1);
							flag_color[2] = 1;

						}

						if (flag_color[7] == 1)
						{
							circle(out1, Point(410, 190), 50, Scalar(0, 0, 255), -1);
							flag_color[7] = 0;
						}
						else
						{
							circle(out1, Point(410, 190), 50, Scalar(0, 255, 0), -1);
							flag_color[7] = 1;

						}

					}
					/*************************************************************/


					/*************************************************************/ // 5
					else if (myPoint.x == 110 && myPoint.y == 190)
					{
						if (flag_color[4] == 1)
						{
							circle(out1, Point(110, 190), 50, Scalar(0, 0, 255), -1);
							flag_color[4] = 0;


						}
						else
						{
							flag_color[4] = 1;
							circle(out1, Point(110, 190), 50, Scalar(0, 255, 0), -1);

						}
						if (flag_color[0] == 1)
						{
							circle(out1, Point(110, 90), 50, Scalar(0, 0, 255), -1);
							flag_color[0] = 0;
						}
						else
						{
							circle(out1, Point(110, 90), 50, Scalar(0, 255, 0), -1);
							flag_color[0] = 1;

						}

						if (flag_color[5] == 1)
						{
							circle(out1, Point(210, 190), 50, Scalar(0, 0, 255), -1);
							flag_color[5] = 0;
						}
						else
						{
							circle(out1, Point(210, 190), 50, Scalar(0, 255, 0), -1);
							flag_color[5] = 1;

						}
						if (flag_color[8] == 1)
						{
							circle(out1, Point(110, 290), 50, Scalar(0, 0, 255), -1);
							flag_color[8] = 0;
						}
						else
						{
							circle(out1, Point(110, 290), 50, Scalar(0, 255, 0), -1);
							flag_color[8] = 1;

						}


					}
					/*************************************************************/

					/*************************************************************/ // 6
					else if (myPoint.x == 210 && myPoint.y == 190)
					{
						if (flag_color[5] == 1)
						{
							circle(out1, Point(210, 190), 50, Scalar(0, 0, 255), -1);
							flag_color[5] = 0;

						}
						else
						{
							flag_color[5] = 1;
							circle(out1, Point(210, 190), 50, Scalar(0, 255, 0), -1);

						}
						if (flag_color[1] == 1)
						{
							circle(out1, Point(210, 90), 50, Scalar(0, 0, 255), -1);
							flag_color[1] = 0;
						}
						else
						{
							circle(out1, Point(210, 90), 50, Scalar(0, 255, 0), -1);
							flag_color[1] = 1;

						}

						if (flag_color[4] == 1)
						{
							circle(out1, Point(110, 190), 50, Scalar(0, 0, 255), -1);
							flag_color[4] = 0;
						}
						else
						{
							circle(out1, Point(110, 190), 50, Scalar(0, 255, 0), -1);
							flag_color[4] = 1;

						}
						if (flag_color[6] == 1)
						{
							circle(out1, Point(310, 190), 50, Scalar(0, 0, 255), -1);
							flag_color[6] = 0;
						}
						else
						{
							circle(out1, Point(310, 190), 50, Scalar(0, 255, 0), -1);
							flag_color[6] = 1;

						}if (flag_color[9] == 1)
						{
							circle(out1, Point(210, 290), 50, Scalar(0, 0, 255), -1);
							flag_color[9] = 0;
						}
						else
						{
							circle(out1, Point(210, 290), 50, Scalar(0, 255, 0), -1);
							flag_color[9] = 1;

						}


					}
					/*************************************************************/

					/*************************************************************/ // 7
					else if (myPoint.x == 310 && myPoint.y == 190)
					{
						if (flag_color[6] == 1)
						{
							circle(out1, Point(310, 190), 50, Scalar(0, 0, 255), -1);
							flag_color[6] = 0;

						}
						else
						{
							flag_color[6] = 1;
							circle(out1, Point(310, 190), 50, Scalar(0, 255, 0), -1);

						}
						if (flag_color[2] == 1)
						{
							circle(out1, Point(310, 90), 50, Scalar(0, 0, 255), -1);
							flag_color[2] = 0;
						}
						else
						{
							circle(out1, Point(310, 90), 50, Scalar(0, 255, 0), -1);
							flag_color[2] = 1;

						}

						if (flag_color[5] == 1)
						{
							circle(out1, Point(210, 190), 50, Scalar(0, 0, 255), -1);
							flag_color[5] = 0;
						}
						else
						{
							circle(out1, Point(210, 190), 50, Scalar(0, 255, 0), -1);
							flag_color[5] = 1;

						}
						if (flag_color[7] == 1)
						{
							circle(out1, Point(410, 190), 50, Scalar(0, 0, 255), -1);
							flag_color[7] = 0;
						}
						else
						{
							circle(out1, Point(410, 190), 50, Scalar(0, 255, 0), -1);
							flag_color[7] = 1;

						}if (flag_color[10] == 1)
						{
							circle(out1, Point(310, 290), 50, Scalar(0, 0, 255), -1);
							flag_color[10] = 0;
						}
						else
						{
							circle(out1, Point(310, 290), 50, Scalar(0, 255, 0), -1);
							flag_color[10] = 1;

						}


					}
					/*************************************************************/

					/*************************************************************/ // 8
					else if (myPoint.x == 410 && myPoint.y == 190)
					{
						if (flag_color[7] == 1)
						{
							circle(out1, Point(410, 190), 50, Scalar(0, 0, 255), -1);
							flag_color[7] = 0;

						}
						else
						{
							flag_color[7] = 1;
							circle(out1, Point(410, 190), 50, Scalar(0, 255, 0), -1);

						}
						if (flag_color[3] == 1)
						{
							circle(out1, Point(410, 90), 50, Scalar(0, 0, 255), -1);
							flag_color[3] = 0;
						}
						else
						{
							circle(out1, Point(410, 90), 50, Scalar(0, 255, 0), -1);
							flag_color[3] = 1;

						}

						if (flag_color[6] == 1)
						{
							circle(out1, Point(310, 190), 50, Scalar(0, 0, 255), -1);
							flag_color[6] = 0;
						}
						else
						{
							circle(out1, Point(310, 190), 50, Scalar(0, 255, 0), -1);
							flag_color[6] = 1;

						}
						if (flag_color[11] == 1)
						{
							circle(out1, Point(410, 290), 50, Scalar(0, 0, 255), -1);
							flag_color[11] = 0;
						}
						else
						{
							circle(out1, Point(410, 290), 50, Scalar(0, 255, 0), -1);
							flag_color[11] = 1;

						}


					}
					/*************************************************************/

					/*************************************************************/ // 8
					else if (myPoint.x == 410 && myPoint.y == 190)
					{
						if (flag_color[7] == 1)
						{
							circle(out1, Point(410, 190), 50, Scalar(0, 0, 255), -1);
							flag_color[7] = 0;

						}
						else
						{
							flag_color[7] = 1;
							circle(out1, Point(410, 190), 50, Scalar(0, 255, 0), -1);

						}
						if (flag_color[3] == 1)
						{
							circle(out1, Point(410, 90), 50, Scalar(0, 0, 255), -1);
							flag_color[3] = 0;
						}
						else
						{
							circle(out1, Point(410, 90), 50, Scalar(0, 255, 0), -1);
							flag_color[3] = 1;

						}

						if (flag_color[6] == 1)
						{
							circle(out1, Point(310, 190), 50, Scalar(0, 0, 255), -1);
							flag_color[6] = 0;
						}
						else
						{
							circle(out1, Point(310, 190), 50, Scalar(0, 255, 0), -1);
							flag_color[6] = 1;

						}
						if (flag_color[11] == 1)
						{
							circle(out1, Point(410, 290), 50, Scalar(0, 0, 255), -1);
							flag_color[11] = 0;
						}
						else
						{
							circle(out1, Point(410, 290), 50, Scalar(0, 255, 0), -1);
							flag_color[11] = 1;

						}


					}
					/*************************************************************/

					/*************************************************************/ // 9
					else if (myPoint.x == 110 && myPoint.y == 290)
					{
						if (flag_color[8] == 1)
						{
							circle(out1, Point(110, 290), 50, Scalar(0, 0, 255), -1);
							flag_color[8] = 0;

						}
						else
						{
							flag_color[8] = 1;
							circle(out1, Point(110, 290), 50, Scalar(0, 255, 0), -1);

						}
						if (flag_color[4] == 1)
						{
							circle(out1, Point(110, 190), 50, Scalar(0, 0, 255), -1);
							flag_color[4] = 0;
						}
						else
						{
							circle(out1, Point(110, 190), 50, Scalar(0, 255, 0), -1);
							flag_color[4] = 1;

						}

						if (flag_color[9] == 1)
						{
							circle(out1, Point(210, 290), 50, Scalar(0, 0, 255), -1);
							flag_color[9] = 0;
						}
						else
						{
							circle(out1, Point(210, 290), 50, Scalar(0, 255, 0), -1);
							flag_color[9] = 1;

						}
						if (flag_color[12] == 1)
						{
							circle(out1, Point(110, 390), 50, Scalar(0, 0, 255), -1);
							flag_color[12] = 0;
						}
						else
						{
							circle(out1, Point(110, 390), 50, Scalar(0, 255, 0), -1);
							flag_color[12] = 1;

						}

					}
					/*************************************************************/

					/*************************************************************/ // 10
					else if (myPoint.x == 210 && myPoint.y == 290)
					{
						if (flag_color[9] == 1)
						{
							circle(out1, Point(210, 290), 50, Scalar(0, 0, 255), -1);
							flag_color[9] = 0;

						}
						else
						{
							flag_color[9] = 1;
							circle(out1, Point(210, 290), 50, Scalar(0, 255, 0), -1);

						}
						if (flag_color[5] == 1)
						{
							circle(out1, Point(210, 190), 50, Scalar(0, 0, 255), -1);
							flag_color[5] = 0;
						}
						else
						{
							circle(out1, Point(210, 190), 50, Scalar(0, 255, 0), -1);
							flag_color[5] = 1;

						}

						if (flag_color[8] == 1)
						{
							circle(out1, Point(110, 290), 50, Scalar(0, 0, 255), -1);
							flag_color[8] = 0;
						}
						else
						{
							circle(out1, Point(110, 290), 50, Scalar(0, 255, 0), -1);
							flag_color[8] = 1;

						}
						if (flag_color[10] == 1)
						{
							circle(out1, Point(310, 290), 50, Scalar(0, 0, 255), -1);
							flag_color[10] = 0;
						}
						else
						{
							circle(out1, Point(310, 290), 50, Scalar(0, 255, 0), -1);
							flag_color[10] = 1;

						}
						if (flag_color[13] == 1)
						{
							circle(out1, Point(210, 390), 50, Scalar(0, 0, 255), -1);
							flag_color[13] = 0;
						}
						else
						{
							circle(out1, Point(210, 390), 50, Scalar(0, 255, 0), -1);
							flag_color[13] = 1;

						}

					}
					/*************************************************************/ // 11
					else if (myPoint.x == 310 && myPoint.y == 290)
					{
						if (flag_color[10] == 1)
						{
							circle(out1, Point(310, 290), 50, Scalar(0, 0, 255), -1);
							flag_color[10] = 0;

						}
						else
						{
							flag_color[10] = 1;
							circle(out1, Point(310, 290), 50, Scalar(0, 255, 0), -1);

						}
						if (flag_color[6] == 1)
						{
							circle(out1, Point(310, 190), 50, Scalar(0, 0, 255), -1);
							flag_color[6] = 0;
						}
						else
						{
							circle(out1, Point(310, 190), 50, Scalar(0, 255, 0), -1);
							flag_color[6] = 1;

						}

						if (flag_color[9] == 1)
						{
							circle(out1, Point(210, 290), 50, Scalar(0, 0, 255), -1);
							flag_color[9] = 0;
						}
						else
						{
							circle(out1, Point(210, 290), 50, Scalar(0, 255, 0), -1);
							flag_color[9] = 1;

						}
						if (flag_color[11] == 1)
						{
							circle(out1, Point(410, 290), 50, Scalar(0, 0, 255), -1);
							flag_color[11] = 0;
						}
						else
						{
							circle(out1, Point(410, 290), 50, Scalar(0, 255, 0), -1);
							flag_color[11] = 1;

						}
						if (flag_color[14] == 1)
						{
							circle(out1, Point(310, 390), 50, Scalar(0, 0, 255), -1);
							flag_color[14] = 0;
						}
						else
						{
							circle(out1, Point(310, 390), 50, Scalar(0, 255, 0), -1);
							flag_color[14] = 1;

						}

					}
					/*************************************************************/
					/*************************************************************/ // 12
					else if (myPoint.x == 410 && myPoint.y == 290)
					{
						if (flag_color[11] == 1)
						{
							circle(out1, Point(410, 290), 50, Scalar(0, 0, 255), -1);
							flag_color[11] = 0;

						}
						else
						{
							flag_color[11] = 1;
							circle(out1, Point(410, 290), 50, Scalar(0, 255, 0), -1);

						}
						if (flag_color[10] == 1)
						{
							circle(out1, Point(310, 290), 50, Scalar(0, 0, 255), -1);
							flag_color[10] = 0;
						}
						else
						{
							circle(out1, Point(310, 290), 50, Scalar(0, 255, 0), -1);
							flag_color[10] = 1;

						}

						if (flag_color[7] == 1)
						{
							circle(out1, Point(410, 190), 50, Scalar(0, 0, 255), -1);
							flag_color[7] = 0;
						}
						else
						{
							circle(out1, Point(410, 190), 50, Scalar(0, 255, 0), -1);
							flag_color[7] = 1;

						}
						if (flag_color[15] == 1)
						{
							circle(out1, Point(410, 390), 50, Scalar(0, 0, 255), -1);
							flag_color[15] = 0;
						}
						else
						{
							circle(out1, Point(410, 390), 50, Scalar(0, 255, 0), -1);
							flag_color[15] = 1;

						}


					}
					/***********************************************************/

					/*************************************************************/ // 13
					else if (myPoint.x == 110 && myPoint.y == 390)
					{
						if (flag_color[12] == 1)
						{
							circle(out1, Point(110, 390), 50, Scalar(0, 0, 255), -1);
							flag_color[12] = 0;

						}
						else
						{
							flag_color[12] = 1;
							circle(out1, Point(110, 390), 50, Scalar(0, 255, 0), -1);

						}
						if (flag_color[8] == 1)
						{
							circle(out1, Point(110, 290), 50, Scalar(0, 0, 255), -1);
							flag_color[8] = 0;
						}
						else
						{
							circle(out1, Point(110, 290), 50, Scalar(0, 255, 0), -1);
							flag_color[8] = 1;

						}

						if (flag_color[13] == 1)
						{
							circle(out1, Point(210, 390), 50, Scalar(0, 0, 255), -1);
							flag_color[13] = 0;
						}
						else
						{
							circle(out1, Point(210, 390), 50, Scalar(0, 255, 0), -1);
							flag_color[13] = 1;

						}

					}
					/***********************************************************/

					/*************************************************************/ // 14
					else if (myPoint.x == 210 && myPoint.y == 390)
					{
						if (flag_color[13] == 1)
						{
							circle(out1, Point(210, 390), 50, Scalar(0, 0, 255), -1);
							flag_color[13] = 0;

						}
						else
						{
							flag_color[13] = 1;
							circle(out1, Point(210, 390), 50, Scalar(0, 255, 0), -1);

						}
						if (flag_color[9] == 1)
						{
							circle(out1, Point(210, 290), 50, Scalar(0, 0, 255), -1);
							flag_color[9] = 0;
						}
						else
						{
							circle(out1, Point(210, 290), 50, Scalar(0, 255, 0), -1);
							flag_color[9] = 1;

						}

						if (flag_color[12] == 1)
						{
							circle(out1, Point(110, 390), 50, Scalar(0, 0, 255), -1);
							flag_color[12] = 0;
						}
						else
						{
							circle(out1, Point(110, 390), 50, Scalar(0, 255, 0), -1);
							flag_color[12] = 1;

						}
						if (flag_color[14] == 1)
						{
							circle(out1, Point(310, 390), 50, Scalar(0, 0, 255), -1);
							flag_color[14] = 0;
						}
						else
						{
							circle(out1, Point(310, 390), 50, Scalar(0, 255, 0), -1);
							flag_color[14] = 1;

						}

					}
					/***********************************************************/

					/*************************************************************/ // 15
					else if (myPoint.x == 310 && myPoint.y == 390)
					{
						if (flag_color[14] == 1)
						{
							circle(out1, Point(310, 390), 50, Scalar(0, 0, 255), -1);
							flag_color[14] = 0;

						}
						else
						{
							flag_color[14] = 1;
							circle(out1, Point(310, 390), 50, Scalar(0, 255, 0), -1);

						}
						if (flag_color[10] == 1)
						{
							circle(out1, Point(310, 290), 50, Scalar(0, 0, 255), -1);
							flag_color[10] = 0;
						}
						else
						{
							circle(out1, Point(310, 290), 50, Scalar(0, 255, 0), -1);
							flag_color[10] = 1;

						}

						if (flag_color[13] == 1)
						{
							circle(out1, Point(210, 390), 50, Scalar(0, 0, 255), -1);
							flag_color[13] = 0;
						}
						else
						{
							circle(out1, Point(210, 390), 50, Scalar(0, 255, 0), -1);
							flag_color[13] = 1;

						}
						if (flag_color[15] == 1)
						{
							circle(out1, Point(410, 390), 50, Scalar(0, 0, 255), -1);
							flag_color[15] = 0;
						}
						else
						{
							circle(out1, Point(410, 390), 50, Scalar(0, 255, 0), -1);
							flag_color[15] = 1;

						}

					}
					/***********************************************************/
					/*************************************************************/ // 16
					else if (myPoint.x == 410 && myPoint.y == 390)
					{
						if (flag_color[15] == 1)
						{
							circle(out1, Point(410, 390), 50, Scalar(0, 0, 255), -1);
							flag_color[15] = 0;

						}
						else
						{
							flag_color[15] = 1;
							circle(out1, Point(410, 390), 50, Scalar(0, 255, 0), -1);

						}
						if (flag_color[11] == 1)
						{
							circle(out1, Point(410, 290), 50, Scalar(0, 0, 255), -1);
							flag_color[11] = 0;
						}
						else
						{
							circle(out1, Point(410, 290), 50, Scalar(0, 255, 0), -1);
							flag_color[11] = 1;

						}

						if (flag_color[14] == 1)
						{
							circle(out1, Point(310, 390), 50, Scalar(0, 0, 255), -1);
							flag_color[14] = 0;
						}
						else
						{
							circle(out1, Point(310, 390), 50, Scalar(0, 255, 0), -1);
							flag_color[14] = 1;

						}

					}
					/***********************************************************/


					/********************************************************/
					subtract(out, out1, out);
					//circle(out, myPoint, 50, Scalar(0, 255, 0), 2);
					waitKey(200);
				}
			}

		}



		absdiff(out, out1, out);

		//imshow("o", out1);
		imshow("finish", out);

		Mat red;
		inRange(out, Scalar(0, 100, 120), Scalar(10, 255, 255), red);
		//imshow("red", red);
		int success = 1;
		
		for (int i = 0; i < 15; i++)
		{
			if (flag_color[i] != 1)
				success = 0;
		}
		
		
		if (success == 1)
		{
			Mat success_img = imread("success.png");
			imshow("success", success_img);
			waitKey(20000);
			
		}




		//setMouseCallback("Splite image", OnMouseAction);




		//这里是自定义的求取形心函数，当然用连通域计算更好  
		//Point center; 
		//center = GetCenterPoint(imgThresholded);//获取二值化白色区域的形心     
		//circle(img, center, 100, Scalar(0,0,255), 5, 8, 0);//绘制目标位置   
		//imshow("Extracted Frame", img);

		char c = (char)waitKey(25); //按esc結束程式
		if (c == 27)
			break;
	}
}



