#include <opencv2\opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;

const string window_name1 = "Src";
const string window_name2 = "Output";

int main(int argc, char** argv)
{
	Mat src;
	int histogram[3][256] = { 0 }, cdf[3][256] = { 0 }, HE[3][256] = { 0 };
	if (argc != 2) {
		char path[256];
		cout << "input image path:\n";
		cin >> path;
		src = imread(path, CV_LOAD_IMAGE_COLOR);
	}
	else {
		src = imread(argv[1], CV_LOAD_IMAGE_COLOR);
	}
	if (!src.data) {
		cerr << "could not open or find the image" << endl;
		return -1;
	}

	namedWindow(window_name1, CV_WINDOW_AUTOSIZE);
	moveWindow(window_name1, 100, 100);
	namedWindow(window_name2, CV_WINDOW_AUTOSIZE);
	moveWindow(window_name2, 600, 100);

	imshow(window_name1, src);

	int matSize = src.rows * src.cols;

	for (int i = 0; i < src.rows; i++) {
		for (int j = 0; j < src.cols; j++) {
			for (int k = 0; k < 3; k++) {
				histogram[k][src.at<Vec3b>(i, j)[k]]++;
			}
		}
	}
	
	for (int ch = 0; ch < 3; ch++) {
		cdf[ch][0] = histogram[ch][0];
		HE[ch][0] = cvRound((float)cdf[ch][0] / matSize * 255);
		for (int i = 1; i < 256; i++) {
			cdf[ch][i] = histogram[ch][i] + cdf[ch][i - 1];
			HE[ch][i] = (histogram[ch][i] > 0) ? cvRound((float)cdf[ch][i] / matSize * 255) : 0;
		}

	}

	for (int i = 0; i < src.rows; i++) {
		for (int j = 0; j < src.cols; j++) {
			for (int k = 0; k < 3; k++) {
				src.at<Vec3b>(i, j)[k] = HE[k][src.at<Vec3b>(i, j)[k]];
			}
		}
	}

	imshow(window_name2, src);
	waitKey(0);

	return 0;
}