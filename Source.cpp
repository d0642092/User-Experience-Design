#include "opencv2/opencv.hpp"  

using namespace cv;
using namespace std;

void skinExtract(const Mat &frame, Mat &skinArea);

int main(int argc, char* argv[])
{
	Mat frame, skinArea;
	VideoCapture capture;

	capture.open(0);
	if (!capture.isOpened())
	{
		cout << "No camera!\n" << endl;
		return -1;
	}

	while (1)
	{
		capture >> frame;
		flip(frame, frame, 1);
		if (frame.empty())
			break;

		skinArea.create(frame.rows, frame.cols, CV_8UC1);
		skinExtract(frame, skinArea);
		Mat show_img;
		frame.copyTo(show_img, skinArea);

		vector<vector<Point> > contours;
		vector<Vec4i> hierarchy;
		 
		findContours(skinArea, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);

		  
		int index;
		double area, maxArea(0);
		for (int i = 0; i < contours.size(); i++)
		{
			area = contourArea(Mat(contours[i]));
			if (area > maxArea)
			{
				maxArea = area;
				index = i;
			}
		}

		//drawContours(frame, contours, index, Scalar(0, 0, 255), 2, 8, hierarchy );  

		Moments moment = moments(skinArea, true);
		Point center(moment.m10 / moment.m00, moment.m01 / moment.m00);
		circle(show_img, center, 8, Scalar(0, 0, 255), CV_FILLED);

		vector<Point> couPoint = contours[index];
		vector<Point> fingerTips;
		Point tmp;
		int max(0), count(0), notice(0);
		for (int i = 0; i < couPoint.size(); i++)
		{
			tmp = couPoint[i];
			int dist = (tmp.x - center.x) * (tmp.x - center.x) + (tmp.y - center.y) * (tmp.y - center.y);
			if (dist > max)
			{
				max = dist;
				notice = i;
			}

			if (dist != max)
			{
				count++;
				if (count > 60)
				{
					count = 0;
					max = 0;
					bool flag = false;
					if (center.y < couPoint[notice].y)
						continue;
					for (int j = 0; j < fingerTips.size(); j++)
					{
						if (abs(couPoint[notice].x - fingerTips[j].x) < 20)
						{
							flag = true;
							break;
						}
					}
					if (flag) continue;
					fingerTips.push_back(couPoint[notice]);
					circle(show_img, couPoint[notice], 6, Scalar(0, 255, 0), CV_FILLED);
					line(show_img, center, couPoint[notice], Scalar(255, 0, 0), 2);
				}
			}
		}

		imshow("show_img", show_img);

		if (cvWaitKey(20) == 'q')
			break;
	}

	return 0;
}

void skinExtract(const Mat &frame, Mat &skinArea)
{
	Mat YCbCr;
	vector<Mat> planes;

	cvtColor(frame, YCbCr, CV_RGB2YCrCb);
	split(YCbCr, planes);

	MatIterator_<uchar> it_Cb = planes[1].begin<uchar>(),
		it_Cb_end = planes[1].end<uchar>();
	MatIterator_<uchar> it_Cr = planes[2].begin<uchar>();
	MatIterator_<uchar> it_skin = skinArea.begin<uchar>();

	for (; it_Cb != it_Cb_end; ++it_Cr, ++it_Cb, ++it_skin)
	{
		if (138 <= *it_Cr &&  *it_Cr <= 170 && 100 <= *it_Cb &&  *it_Cb <= 127)
			*it_skin = 255;
		else
			*it_skin = 0;
	}

	dilate(skinArea, skinArea, Mat(5, 5, CV_8UC1), Point(-1, -1));
	erode(skinArea, skinArea, Mat(5, 5, CV_8UC1), Point(-1, -1));

}