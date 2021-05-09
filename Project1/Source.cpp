#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect.hpp>
#include <iostream>

using namespace cv;
using namespace std;

Mat image_gray, image_blur, image_canny, image_dilated, image_eroded;

Mat preprocessing(Mat image) {
	cvtColor(image, image_gray, COLOR_BGR2GRAY);
	GaussianBlur(image_gray, image_blur, Size(5, 5), 5, 0);
	Canny(image_blur, image_canny, 50, 150);

	Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3));

	dilate(image_canny, image_dilated, kernel);

	return image_dilated;
}

vector<Point> getContours(Mat image) {

	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;

	findContours(image, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

	vector<vector<Point>> conPoly(contours.size());
	vector<Point> my_pts;
	int max_area = 0;

	for (int i = 0; i < contours.size(); i++)
	{
		int area = contourArea(contours[i]);
		cout << area << endl;

		string objectType;

		if (area > 1000)
		{
			float peri = arcLength(contours[i], true);
			approxPolyDP(contours[i], conPoly[i], 0.02 * peri, true);

			if (area > max_area && conPoly[i].size() == 4) {

				max_area = area;
				my_pts = { conPoly[i][0], conPoly[i][1], conPoly[i][2], conPoly[i][3] };
			}
			cout << conPoly[i].size() << endl;

		}
	}
	return my_pts;
}

void drawPoints(Mat image, vector<Point> pts, Scalar color) {

	for (int i = 0; i < pts.size(); i++) {
		circle(image, pts[i], 10, color, FILLED);
		putText(image, to_string(i), pts[i], FONT_HERSHEY_PLAIN, 4, color, 4);
	}

	imshow("Image with Points", image);
}

vector<Point> reorderPoints(vector<Point> pts) {

	vector<Point> new_pts;
	vector<int> sum_pts, sub_pts;

	for (int i = 0; i < 4; i++)
	{
		sum_pts.push_back(pts[i].x + pts[i].y);
		sub_pts.push_back(pts[i].x - pts[i].y);
	}

	new_pts.push_back(pts[min_element(sum_pts.begin(), sum_pts.end()) - sum_pts.begin()]); // 0
	new_pts.push_back(pts[max_element(sub_pts.begin(), sub_pts.end()) - sub_pts.begin()]); // 1
	new_pts.push_back(pts[min_element(sub_pts.begin(), sub_pts.end()) - sub_pts.begin()]); // 2
	new_pts.push_back(pts[max_element(sum_pts.begin(), sum_pts.end()) - sum_pts.begin()]); // 3

	return new_pts;

}

Mat getWarp(Mat image, vector<Point> pts, float w, float h)
{
	Mat image_warped;
	Point2f src[4] = { pts[0], pts[1], pts[2], pts[3] };
	Point2f dst[4] = { {0.0f,0.0f},{w,0.0f},{0.0f,h},{w,h} };

	Mat matrix = getPerspectiveTransform(src, dst);
	warpPerspective(image, image_warped, matrix, Point(w, h));

	return image_warped;
}

int main() {

	// Path to the image
	string path = "paper.jpg";
	Mat image = imread(path);
	Mat image_preprocessed;

	resize(image, image, Size(), 0.5, 0.5);

	image_preprocessed = preprocessing(image);

	vector<Point> my_pts = getContours(image_preprocessed);
	vector<Point> reordered_pts = reorderPoints(my_pts);

	drawPoints(image.clone(), reordered_pts, Scalar(0, 255, 0));
	
	float w = 420, h = 596;
	Mat image_warped = getWarp(image, reordered_pts, w, h);
	
	imshow("Image Output", image);
	imshow("Pre-processed Ouput", image_preprocessed);
	imshow("Final Output", image_warped);
	waitKey(0);



	return 0;

}