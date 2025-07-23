#include "opencv2/opencv.hpp"
#include <iostream>

using namespace cv;
using namespace std;

int main()
{
	cout << "OpenCV version: " << CV_VERSION << endl;

	Mat image = imread("lenna.bmp"); //imread ÇÔ¼ö 

	// Check if the image was loaded successfully
	if (image.empty()) {
		cerr << "Error: Could not open or find the image!" << endl;
		return -1;
	}
		
	namedWindow("Image");
	imshow("Image", image);

	waitKey(); // Wait for a key press indefinitely
	
	return 0;
}