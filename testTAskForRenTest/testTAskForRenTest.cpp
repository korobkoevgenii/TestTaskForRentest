#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace cv;

void loadPixel(Mat&res, int x, int y, std::vector<uchar>& buff) {
	if (x > -1 && x<res.cols&&y>-1 && y < res.rows)
		buff.push_back(res.at<uchar>(Point(x, y)));
}

void loadLine(Mat&res, int x, int y, std::vector<uchar>& buff) {
	loadPixel(res, x - 2, y, buff);
	loadPixel(res, x - 1, y, buff);
	loadPixel(res, x , y, buff);
	loadPixel(res, x + 1, y, buff);
	loadPixel(res, x + 2, y, buff);
}

void loadWindow(Mat&res, int x, int y,std::vector<uchar>& buff) {
	loadLine(res, x, y - 2, buff);
	loadLine(res, x, y - 1, buff);
	loadLine(res, x, y, buff);
	loadLine(res, x, y + 1, buff);
	loadLine(res, x, y + 2, buff);
}

void medianFilter(Mat&src, Mat&res) {
	for (int y = 0; y < res.rows; y++) {
		for (int x = 0; x < res.cols; x++) {
			std::vector<uchar> buff;
			loadWindow(src, x, y, buff);
			sort(buff.begin(), buff.end());
			uchar median = buff[buff.size() / 2];
			res.at<uchar>(Point(x, y)) = median;
		}
	}
}

int matrix[3][3] = {{-1,-1,-1},
				    {-1, 9,-1},
				    {-1,-1,-1} };

int clamp(int value, int min, int max){
	if (value < min)
		return min;
	if (value > max)
		return max;
	return value;
}

void sharpnessFilter(Mat &src, Mat & res) {
	for (int y = 0; y < res.rows; y++) {
		for (int x = 0; x < res.cols; x++) {

			float resultColor = 0;

			for (int l = -1; l < 2; l++) {
				for (int k = -1; k < 2; k++) {

					int idx = clamp(x + k, 0, res.cols - 1);
					int idy = clamp(y + l, 0, res.rows - 1);
					int currentPixel= src.at<uchar>(Point(idx, idy));

					resultColor += currentPixel * matrix[l + 1][k + 1];
				}
			}
			res.at<uchar>(Point(x, y)) = clamp((uchar)resultColor, 0, 100);
		}
	}
}

int main(int argc, char* argv[])
{
	std::string image_path = "pictures/Capture.png";
	Mat img = imread(image_path, CV_BGR2GRAY);
	//cvtColor(img, img, CV_BGR2GRAY);

	if (img.empty())
	{
		std::cout << "Could not read the image: " << image_path << std::endl;
		return 1;
	}

	Mat res(img);

	std::cout << "Median filter has started work" << std::endl;
	medianFilter(img, res);
	std::cout << "Median filter has finished work" << std::endl;

	std::cout << "Sharpness filter has started work" << std::endl;
	sharpnessFilter(img, res);
	std::cout << "Sharpness filter has finished work" << std::endl;

	imwrite("pictures/res.jpg", res);
	return 0;
}