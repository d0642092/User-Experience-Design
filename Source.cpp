/*#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/photo.hpp>
#include <time.h>
#include <iostream>
#include <windows.h>

using namespace cv;
using namespace std;

Mat Sourceimage, Spilteimage, Rebuildimage, Dstimage;
int rows, cols;
int Roirows, Roicols;
vector<Point> pointList;
vector<Mat>arraryimage;
void Randarrary(vector<Mat> &vectorMat);    //隨機排列子圖像序列函數
static int vectornumber = 0;
void OnMouseAction(int event, int x, int y, int flags, void *ustc);
//void video(); //攝影機
void video2();
Mat images(int x,int y);
Mat creatImg(int x,int y,int z);

int main(int argc, char** argv)
{
	video2(); //皮膚色
	//video(); //藍色

	//
	//waitKey();
	//images();
	return 0;
}

Mat creatImg(int x,int y,int z) {
	
	Mat img = Mat::zeros(x, y,z);
	int i,j;
	for ( j = 90;j <= y -160;j+=100) {
		for (i = 110; i < x;i+=100) {
			circle(img, Point(i, j), 50, Scalar(0, 0, 255), 2);
			circle(img, Point(i, j), 10, Scalar(125, 125, 125), 2);
			pointList.push_back(Point(i,j));
		}
		
	}
	return img;
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
/*

	//藍色
	int iLowH = 100;
	int iHighH = 140;
	int iLowS = 40;
	int iHighS = 255;
	int iLowV = 40;
	int iHighV = 255;
	cap >> img;
	//Mat test = images(img.rows, img.cols);
	Mat test = creatImg(img.rows, img.cols,img.type());
	Mat out1 = Mat::zeros(img.rows, img.cols, img.type());;
	Mat out2 = Mat::zeros(img.rows, img.cols, img.type());;
	Point p;
	int flag;

	while (1) {

		cap >> img;

		Mat backGround = Mat::zeros(img.rows, img.cols,img.type());
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
			drawContours(dst, hull, i, Scalar(0, 0, 255), 2);
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
		//circle(dst, mc, 50, Scalar(255, 0, 0), 2);

		backGround = img.clone();
		
		absdiff(backGround, dst, d);
		absdiff(test, d, out);
		
		imshow("Thresholded Image", dst);
		
		//imshow("d", d );

		for (int n = 0; n < pointList.size(); n++) {
			Point myPoint = pointList[n];
			//if()
			if (mc.x == 0 & mc.y == 0) {
				break;
			}
			if (mc.x >= myPoint.x-10 & mc.x <=  myPoint.x + 10) {
				if (mc.y >= myPoint.y - 10 & mc.y <= myPoint.y + 10) {
					/*Scalar color = out.at<Scalar>(myPoint.x + 50, myPoint.y + 50);
					if (color == Scalar(0, 0, 255)) {
						
						circle(out1, myPoint, 50, Scalar(0, 0, 255), 2);
						circle(out2, myPoint, 50, Scalar(0, 255, 0), 2);
						subtract(out, out1, out);

						absdiff(out, out1, out);
						break;
					}*/
					
					
					
				/*	circle(out1, myPoint, 50, Scalar(0, 0, 255), 2);
					circle(out2, myPoint, 50, Scalar(0, 255, 0), 2);
					subtract(out, out1, out);
					absdiff(out, out1, out);
					break;
					
				}
			}
	
		}

		absdiff(out, out1, out);
		absdiff(out, out2, out);
	
	
		//imshow("fih", out1);
		//imshow("fih213123", out2);
		//imshow("finish", out);

		char c = (char)waitKey(25); //按esc結束程式
		if (c == 27)
			break;
	}
}*/

/*Mat images(int x, int y) {
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

void OnMouseAction(int event, int x, int y, int flags, void *ustc)
{
	if (event == CV_EVENT_MOUSEMOVE)
	{
		Mat RoiSpilte, RoiSource;
		int rows = (y / Roirows)*Roirows;
		int clos = (x / Roicols)*Roicols;
		RoiSpilte = Spilteimage(Rect(clos, rows, Roicols, Roirows));
		imshow("Slice", RoiSpilte);
		Mat image = Mat::zeros(Sourceimage.rows - Roirows, Sourceimage.cols - Roicols, CV_32FC1);
		matchTemplate(Sourceimage, RoiSpilte, image, 1);
		normalize(image, image, 0, 1, NORM_MINMAX);
		double minV = 0;
		double maxV = 0;
		Point minP, maxP;
		minMaxLoc(image, &minV, &maxV, &minP, &maxP);
		Mat ROIDst = Dstimage(Rect(minP.x, minP.y, Roicols, Roirows));
		addWeighted(ROIDst, 0, RoiSpilte, 1, 0, ROIDst, -1);
		imshow("Jigsaw image", Dstimage);
	}
}*/


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
void OnMouseAction(int event, int x, int y, int flags, void *ustc);
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
			circle(img, Point(i, j), 5, Scalar(125, 125, 125), 2);
			pointList.push_back(Point(i, j));
		}

	}
	return img;
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
			drawContours(dst, hull, i, Scalar(0, 0, 255), 2);
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


		backGround = img.clone();

		absdiff(backGround, dst, d);
		absdiff(test, d, out);


		for (int n = 0; n < pointList.size(); n++) {
			Point myPoint = pointList[n];
			if (mc.x == 0 & mc.y == 0) {
				break;
			}
			if (mc.x >= myPoint.x - 5 & mc.x <= myPoint.x + 5) {
				if (mc.y >= myPoint.y - 5 & mc.y <= myPoint.y + 5) {
					
					circle(out1, myPoint, 50, Scalar(0, 0, 255), -1);


					int x = myPoint.x;
					int y = myPoint.y;
					
					if (x == 110) {
						circle(out1, Point(x + 100, y), 50, Scalar(0, 255, 0), -1);
					}
					else {
						if (x + 100 < img.rows) {
							circle(out1, Point(x - 100, y), 50, Scalar(0, 255, 0), -1);
							circle(out1, Point(x + 100, y), 50, Scalar(0, 255, 0), -1);
						}
						else {
							circle(out1, Point(x - 100, y), 50, Scalar(0, 255, 0), -1);
						}	
					}
						
					if (y  == 90) {
						circle(out1, Point(x, y + 100), 50, Scalar(0, 255, 0), -1);
					}
					else{
						if (y + 100 < img.rows) {
							circle(out1, Point(x , y - 100), 50, Scalar(0, 255, 0), -1);
							circle(out1, Point(x, y + 100), 50, Scalar(0, 255, 0), -1);
						}
						else {
							circle(out1, Point(x , y - 100), 50, Scalar(0, 255, 0), -1);
						}
						
					}
					

					subtract(out, out1, out);
					

				}
			}

		}



		absdiff(out, out1, out);

		//imshow("o", out1);
		imshow("finish", out);
		//imshow("t", test);
		//imshow("Thresholded Image", dst);

		//imshow("d", d );
		//Mat red;
		//inRange(out, Scalar(0, 100, 120), Scalar(10, 255, 255), red);
		//imshow("red", red);


	

		char c = (char)waitKey(25); //按esc結束程式
		if (c == 27)
			break;
	}
}



