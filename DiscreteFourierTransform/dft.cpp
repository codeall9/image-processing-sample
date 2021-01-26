#define _USE_MATH_DEFINES
#include<opencv2\opencv.hpp>
#include<iostream>
#include<cmath>

using namespace cv;
using namespace std;

const string winName_Input = "Input Image";
const string winName_Output1 = "spectrum magnitude";
const string winName_Output2 = "phase";

void mDFT(const Mat &src, Mat &dst);

int main(int argc, char** argv)
{
	Mat InSrc, padded;
	if (argc != 2) {
		char path[256];
		cout << "input image path:\n";
		cin >> path;
		InSrc = imread(path, CV_LOAD_IMAGE_GRAYSCALE);
	}
	else {
		InSrc = imread(argv[1], CV_LOAD_IMAGE_GRAYSCALE);
	}
	if (!InSrc.data) {
		cerr << "could not open or find the image" << endl;
		return -1;
	}
    //expand input image to optimal size
	int m = getOptimalDFTSize(InSrc.rows);
	int n = getOptimalDFTSize(InSrc.cols); // on the border add zero values
	copyMakeBorder(InSrc, padded, 0, m - InSrc.rows, 0, n - InSrc.cols, BORDER_CONSTANT, Scalar::all(0));

	Mat planes[] = { Mat_<float>(padded), Mat::zeros(padded.size(), CV_32F) };
	Mat complexI;
	merge(planes, 2, complexI);         // Add to the expanded another plane with zeros
	mDFT(complexI, complexI);
	//dft(complexI, complexI);            // this way the result may fit in the source matrix

										// compute the magnitude and switch to logarithmic scale
										// => log(1 + sqrt(Re(DFT(I))^2 + Im(DFT(I))^2))

	Mat magI, phaseI;
	split(complexI, planes);                   // planes[0] = Re(DFT(I), planes[1] = Im(DFT(I))
	//magnitude(planes[0], planes[1], magI);// planes[0] = magnitude
	//phase(planes[0], planes[1], phaseI);
	cartToPolar(planes[0], planes[1], magI, phaseI, true);

	magI += Scalar::all(1);                    // switch to logarithmic scale
	log(magI, magI);

	// crop the spectrum, if it has an odd number of rows or columns
	magI = magI(Rect(0, 0, magI.cols & -2, magI.rows & -2));
	phaseI = phaseI(Rect(0, 0, phaseI.cols & -2, phaseI.rows & -2));

	normalize(magI, magI, 0, 1, CV_MINMAX); // Transform the matrix with float values into a
											// viewable image form (float between values 0 and 1).
	normalize(phaseI, phaseI, 0, 1, CV_MINMAX);


	namedWindow(winName_Input, CV_WINDOW_AUTOSIZE);
	moveWindow(winName_Input, 100, 100);
	namedWindow(winName_Output1, CV_WINDOW_AUTOSIZE);
	moveWindow(winName_Output1, 250, 100);
	namedWindow(winName_Output2, CV_WINDOW_AUTOSIZE);
	moveWindow(winName_Output2, 350, 100);

	imshow(winName_Input, InSrc);    // Show the result
	imshow(winName_Output1, magI);
	imshow(winName_Output2, phaseI);

	waitKey();

	return 0;
}

void mDFT(const Mat &src, Mat &dst)
{
	Mat Out[] = { Mat::zeros(src.size(), CV_32F), Mat::zeros(src.size(), CV_32F) };
	float isNeg, angle;
	for (int v = 0; v < dst.rows; v++) {
		for (int u = 0; u < dst.cols; u++) {
			for (int y = 0; y < src.rows; y++) {
				for (int x = 0; x < src.cols; x++) {
					isNeg = (((x + y) & 1 == 1) ? -1.0 : 1.0);
					angle = -2.0 * M_PI * (((float)(x * u) / src.cols) + ((float)(y * v) / src.rows));
					Out[0].at<float>(v, u) += src.at<Vec2f>(y, x)[0] * cos(angle) * isNeg;//real
					Out[1].at<float>(v, u) += src.at<Vec2f>(y, x)[0] * sin(angle) * isNeg;//imag
					//cout << Out[0].at<float>(v, u) << " : " << Out[1].at<float>(v, u) << endl;
				}
			}
			Out[0].at<float>(v, u) /= src.cols;
			Out[1].at<float>(v, u) /= src.rows;
			//cout << Out[0].at<float>(v, u) << " : " << Out[1].at<float>(v, u) << endl;
			cout << "progress: " << fixed << ((float)(v * dst.rows + u) / (src.cols * src.rows)) * 100.0 << "%\r";
		}
	}
	merge(Out, 2, dst);
	cout << "progress: 100% Done!!";
}