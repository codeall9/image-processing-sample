#include <opencv2\opencv.hpp>
#include <iostream>
#include <cmath>

using namespace std;
using namespace cv;

const string window_name1 = "Src";
const string window_name2 = "Output";

int edge(const Mat& src, const int y, const int x);
double blur(const Mat& src, const int y, const int x);

int main(int argc, char** argv)
{
	Mat src;
	argc = 2;
	argv[1] = "test.png";
	if (argc != 2) {
		char path[256];
		cout << "input image path:\n";
		cin >> path;
		src = imread(path, CV_LOAD_IMAGE_GRAYSCALE);
	}
	else {
		src = imread(argv[1], CV_LOAD_IMAGE_GRAYSCALE);
	}

	namedWindow(window_name1, CV_WINDOW_AUTOSIZE);
	moveWindow(window_name1, 100, 100);
	namedWindow(window_name2, CV_WINDOW_AUTOSIZE);
	moveWindow(window_name2, 600, 100);
	imshow(window_name1, src);

	int minH = src.rows - 1, minW = src.cols - 1;
	Mat dst = src.clone();
	for (int y = 1; y < minH; y++) {
		for (int x = 1, tmp; x < minW; x++) {
			tmp = edge(src, y, x);
			dst.at<uchar>(y, x) = 255 - ((tmp <= 32) ? 16 : (tmp >= 224) ? 224 : tmp);//saturate_cast<uchar>(tmp);
			dst.at<uchar>(y, x) = blur(dst, y, x);
		}
	}

	imshow(window_name2, dst);
	waitKey(0);
	return 0;
}

int edge(const Mat& src, const int y, const int x)
{
	double Gx[3][3] = { { 2, 0, -2 },{ 7.2, 0, -7.2 },{ 2, 0, -2 } },
		Gy[3][3] = { { -1.8, -5, -1.8 },{ 0, 0, 0 },{ 1.8, 5, 1.8 } },
		Gsrc[3][3] = {
			{ src.at<uchar>(y - 1, x - 1), src.at<uchar>(y - 1, x), src.at<uchar>(y - 1, x + 1) },
			{ src.at<uchar>(y, x - 1), src.at<uchar>(y, x), src.at<uchar>(y, x + 1) },
			{ src.at<uchar>(y + 1, x - 1), src.at<uchar>(y + 1, x), src.at<uchar>(y + 1, x + 1) }
	};
	double ansX = 0, ansY = 0;
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			ansX += Gx[i][j] * Gsrc[i][j];
			ansY += Gy[i][j] * Gsrc[i][j];
		}
	}
	return (abs(ansX) + abs(ansY)) / 2;
}

double blur(const Mat & src, const int y, const int x)
{
	double Gx[3][3] = { { 0.0625, 0.125, 0.0625 },{ 0.125, 0.25, 0.125 },{ 0.0625, 0.125, 0.0625 } },
		Gsrc[3][3] = {
			{ src.at<uchar>(y - 1, x - 1), src.at<uchar>(y - 1, x), src.at<uchar>(y - 1, x + 1) },
			{ src.at<uchar>(y, x - 1), src.at<uchar>(y, x), src.at<uchar>(y, x + 1) },
			{ src.at<uchar>(y + 1, x - 1), src.at<uchar>(y + 1, x), src.at<uchar>(y + 1, x + 1) }
	};
	int ansX = 0;
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			ansX += Gx[i][j] * Gsrc[i][j];
		}
	}
	return ansX;
}
