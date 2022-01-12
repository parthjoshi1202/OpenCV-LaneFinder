//Lab 9 Parth Joshi , SN- 1126914 
#define _CRT_SECURE_NO_WARNINGS
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
using namespace std;
using namespace cv;

//initial min and max HSV filter volumes
//these will be changed using trackbars
int MinH = 0;
int MaxH = 179;
int MinS = 0;
int MaxS = 255;
int MinV = 0;
int MaxV = 255;
int MinC = 120;
int MaxC = 150;

//window names 
const string windowName = "Original";
const string windowName1 = "HSV Image";
const string windowName2 = "Threashold Image";
const string AdjustmentWindow = "Fine Tuning";

bool Road = true;

//Functions
//Create a window for fine tuning 
void on_trackbar(int,void*) {}; //function called when track bar position gets moved

void setRoad(int MinHue = 70, int MaxHue = 179, int MinSat = 0, int MaxSat = 87, int MinValue = 0, int MaxValue = 77, int MinCanny = 50, int MaxCanny = 150) {

	MinH = MinHue;
	MaxH = MaxHue;
	MinS = MinSat; 
	MaxS = MaxSat; 
	MinV = MinValue;
	MaxV = MaxValue;
	MinC = MinCanny;
	MaxC = MaxCanny;

}

void createtrackBars() {
	if (Road == true) { setRoad(); }
	namedWindow(AdjustmentWindow, 0);
	//create memory to store track bars names on the window
	char TrackBarName[50];
	//create trackbars to insert into a window to fine tune values
	sprintf(TrackBarName,"MinH",MinH);
	sprintf(TrackBarName, "MaxH", MaxH);
	sprintf(TrackBarName, "MinS", MinS);
	sprintf(TrackBarName, "MaxS", MaxS);
	sprintf(TrackBarName, "MinV", MinV);
	sprintf(TrackBarName, "MaxV", MaxV);
	//canny thresholds 
	sprintf(TrackBarName, "MinCanny", MinC);
	sprintf(TrackBarName, "MaxCanny", MaxC);

	//3 parameters are address variables that changes when tracking is changed
	//max and min values tracking
	//function that is called when bars are moved 
	createTrackbar("MinH",AdjustmentWindow,&MinH,MaxH,on_trackbar);
	createTrackbar("MaxH", AdjustmentWindow, &MaxH, MaxH, on_trackbar);
	createTrackbar("MinS", AdjustmentWindow, &MinS, MaxS, on_trackbar);
	createTrackbar("MaxS", AdjustmentWindow, &MaxS, MaxS, on_trackbar);
	createTrackbar("MinV", AdjustmentWindow, &MinV, MaxV, on_trackbar);
	createTrackbar("MaxV", AdjustmentWindow, &MaxV, MaxV, on_trackbar);

	//Canny
	createTrackbar("MinC", AdjustmentWindow, &MinC, MaxC, on_trackbar);
	createTrackbar("MaxC", AdjustmentWindow, &MaxC, MaxC, on_trackbar);
}

int main()
{
	VideoCapture capture("roadLane.mp4");
	//VideoCapture capture("LaneFinder_main/roadLane.mp4");

	if (!capture.isOpened()) {
		cout << "error";
		return -1;
	}

	createtrackBars(); //Calls and create trackerbar slider box

	Mat threshold, capt_Image, HSV_Img, dst;

	while (1) {
		//Step-1 get the video feed
		capture >> capt_Image;
		resize(capt_Image,capt_Image,Size(500,640));

		//defining region of interest box(x,y,b,c)
		//(x,y)=(100,295)
		//and second corner is (x+b,y+c)=(100+220,295+185)
		Rect const box(100,295,320,185);

		//step 2- define a box
		Mat ROI = capt_Image(box);

		//step-3 canny
		//edge detection using canny detector 
		GaussianBlur(ROI,dst,Size(3,3),1.5,1.5);

		//Morphological Operations
		Mat k = getStructuringElement(MORPH_RECT,Size(3,3));
		morphologyEx(ROI,ROI,MORPH_CLOSE,k);
		Canny(ROI,dst,MinC,MaxC,3);

		//Step-4 Converting image into HSV
		cvtColor(ROI,HSV_Img,COLOR_RGB2HSV);

		inRange(ROI,Scalar(70,179,0),Scalar(87,0,77),threshold);

		//Step-5 using vector to get HoughLinesP, applying Hough transform to detect lines in our image 
		vector<Vec4i> lines;
		double rho = 1;
		double theta = CV_PI/180;
		int threshold = 50;
		double minLineLength = 10;
		double maxLineGap = 300;
		HoughLinesP(dst,lines,rho,theta,threshold,minLineLength,maxLineGap);
		//draw actual lines on screen over the road
		int linethickness = 10;
		int lineType = LINE_4;

		for (size_t i = 0; i < lines.size(); i++) {
			Vec4i l = lines[i];
			Scalar greenColor = Scalar(0,250,0); //B,G,R
			line(ROI,Point(l[0],l[1]),Point(l[2],l[3]),greenColor,linethickness,lineType);
		}

		imshow(windowName, capt_Image);
		imshow(windowName1, HSV_Img);
		imshow(windowName2, ROI);

		if (waitKey(20)==27)
		{
			cout << "esc";
			break;
		}

	}
}

