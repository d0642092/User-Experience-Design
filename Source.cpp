#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core.hpp>
#include <time.h>
#include <iostream>

using namespace cv;
using namespace std;

Mat Sourceimage, Spilteimage, Rebuildimage, Dstimage;
int rows, cols;
int Roirows, Roicols;
vector<Mat>arraryimage;
void Randarrary(vector<Mat> &vectorMat);    //隨機排列子圖像序列函數
static int vectornumber = 0;
void OnMouseAction(int event, int x, int y, int flags, void *ustc);
void video(); //攝影機
void video2();

int main(int argc, char** argv)
{
	//video2(); //皮膚色
	//video(); //藍色
	Sourceimage = imread("images.jpg");
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
	setMouseCallback("Splite image", OnMouseAction);
	waitKey();
	return 0;
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
	if (event == CV_EVENT_LBUTTONDOWN)
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
}
void video() //皮膚色
{
	VideoCapture cap(0);
	Mat frame, frameHSV;
	Mat Sourceimage = imread("kanahei.jpg");
	vector< std::vector<cv::Point> > OriginalContours;
	vector<Vec4i > hierarchy;
	vector<vector<Point>> FinalContours;
	vector<Point> hull;
	int i, j;
	int Width = cap.get(CV_CAP_PROP_FRAME_WIDTH), Height = cap.get(CV_CAP_PROP_FRAME_HEIGHT);
	float fHeight = ((float)2 / 3 - (float)1 / 7) * Height;
	while (1) {

		cap >> frame;
		if (frame.empty())
			break;

		flip(frame, frame, 1); //左右反轉
		GaussianBlur(frame, frame, Size(7, 7), 2.0, 1.5); //高斯模糊
		cvtColor(frame, frameHSV, CV_BGR2HSV); //通道轉換
		Mat mask(frame.rows, frame.cols, CV_8UC1);
		inRange(frameHSV, Scalar(5, 30, 30), Scalar(40, 170, 256), mask);
		erode(mask, mask, Mat(5, 5, CV_8U), Point(-1, -1), 1); //去除多餘亮點
		
		//Mat grayscaleMat(frame.size(), CV_8U);
		//cvtColor(frame, grayscaleMat, CV_BGR2GRAY);

		
		OriginalContours.clear();
		hierarchy.clear();
		FinalContours.clear();
		findContours(mask, OriginalContours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE); //找輪廓
		// 去除多餘輪廓
		for (i = 0; i < OriginalContours.size(); i++)
		{
			if (fabs(contourArea(Mat(OriginalContours[i]))) > 12000)
			{
				FinalContours.push_back(OriginalContours[i]);
			}
		}
		//cv::drawContours(frame, FinalContours, -1, Scalar(0, 0, 255), 3); //畫輪廓

		//找凸出輪廓
		int hullcount;
		int iNumOfContours = FinalContours.size();
		Point top;


		for (j = 0; j < iNumOfContours; j++)
		{
			convexHull(Mat(FinalContours[j]), hull, true);
			hullcount = (int)hull.size();
			//
			for (i = 0; i < hullcount - 1; i++)
			{
				line(frame, hull[i + 1], hull[i], Scalar(255, 0, 0), 2, CV_AA);
				circle(frame, hull[(hullcount - 1) / 2], 4, Scalar(0, 255, 0), 1); //綠點

			}
			//line(frame, hull[hullcount - 1], hull[0], Scalar(255, 0, 0), 2, CV_AA);
			//line(frame, top, top, Scalar(255, 0, 255), 18, CV_AA);

			/*Point temp;


			for (i = 0; i < (int)hull.size() - 1; i++) //找出綠點位置
			{
				for (j = 0; j < (int)hull.size() - 1; j++)
				{
					if (hull[i].x > hull[i + 1].x || hull[i].y > hull[i + 1].y)
					{
						temp.x = hull[i].x;
						hull[i].x = hull[i + 1].x;
						hull[i + 1].x = temp.x;
					}
				}
			}*/

		}


		imshow("Extracted Frame", frame);
		imshow("Frame", frameHSV);

		char c = (char)waitKey(25); //按esc結束程式
		if (c == 27)
			break;

	}

	for (i = 0; i < (int)hull.size(); i++) //找出綠點位置
		cout << hull[i];

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


	//藍色
	int iLowH = 100;
	int iHighH = 140;
	int iLowS = 40;
	int iHighS = 255;
	int iLowV = 40;
	int iHighV = 255;

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
		namedWindow("Thresholded Image");

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
		imshow("Thresholded Image", dst);



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

void hsvreg(Mat img) {
	//Mat img = imread("蓝色笔筒.jpg", 1);
	Mat imgHSV;
	//蓝色笔筒颜色的HSV范围    
	int iLowH = 100 / 2;
	int iHighH = 120 / 2;
	int iLowS = 50 * 255 / 100;
	int iHighS = 70 * 255 / 100;
	int iLowV = 40 * 255 / 100;
	int iHighV = 50 * 255 / 100;
	cvtColor(img, imgHSV, COLOR_BGR2HSV);//转为HSV
	imwrite("hsv.jpg", imgHSV);
	Mat imgThresholded;
	inRange(imgHSV, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), imgThresholded);
	//Threshold the image
	//开操作 (去除一些噪点)  如果二值化后图片干扰部分依然很多，增大下面的size    
	Mat element = getStructuringElement(MORPH_RECT, Size(5, 5));
	morphologyEx(imgThresholded, imgThresholded, MORPH_OPEN, element);
	//闭操作 (连接一些连通域)
	morphologyEx(imgThresholded, imgThresholded, MORPH_CLOSE, element);
	namedWindow("Thresholded Image", CV_WINDOW_NORMAL);
	imshow("Thresholded Image", imgThresholded);
	//这里是自定义的求取形心函数，当然用连通域计算更好  
	//Point center; 
	//center = GetCenterPoint(imgThresholded);//获取二值化白色区域的形心     
	//circle(img, center, 100, Scalar(0,0,255), 5, 8, 0);//绘制目标位置   
	imwrite("end.jpg", img);
	waitKey(0);

}