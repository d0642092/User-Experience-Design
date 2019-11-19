#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core.hpp>
#include <time.h>

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


int main(int argc, char** argv)
{
	//video();
	Sourceimage = imread("kanahei.jpg");
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
	}	// 随機函數	Randarrary( arraryimage); 	
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
void video()
{
	VideoCapture cap(0);
	Mat Sourceimage = Mat::zeros(Size(800, 600), CV_8UC3);
	Mat output = Mat::zeros(Size(800, 600), CV_8UC3);

	Sourceimage = imread("kanahei.jpg");
	Point p1(100, 100);
	Point p2(50, 50);
	while (1) {
		Mat frame = Mat::zeros(Size(800,600), CV_8UC3);
		// Capture frame-by-frame
		cap >> frame;
		frame = Mat::zeros(Size(800, 600), CV_8UC3);
		line(frame, p1, p2, Scalar(0, 0, 255));
		// If the frame is empty, break immediately
		if (frame.empty())
			break;
		//addWeighted(frame, 0.7, mix, 0.3, 0.0, output);
		// Display the resulting frame
		imshow("output", output);
		imshow("Extracted Frame", frame); // Press ESC on keyboard to exit
		char c = (char)waitKey(25);
		if (c == 27)
			break;
	}

 }