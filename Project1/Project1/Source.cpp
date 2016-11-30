#include <Windows.h>
#include <iostream>
#include <fstream>
#include <ctime>
#include <string>
#include <stdio.h>
#include <stdlib.h>

#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/core/core.hpp>
#include <cmath>

using namespace cv;
using namespace std;

void findBall(Mat input, vector<int> & xs, vector<int> & ys, vector<double> & ds);
bool findBall(Vec3b pixel);
void findBall(vector<Mat> & fs);
bool findBall(uchar h, uchar v);

void cutMats(vector<Mat> & fs);
int mostPixel(vector<Mat> & fs);
int countPixel(Mat f);

void cancelNoise(vector<int> & xs, vector<int> & ys, vector<double> & ds);
void xydToXYZ(vector<int> xs, vector<int> ys, vector<double> ds, vector<double> & Xs, vector<double> & Ys, vector<double> & Zs);
void xydToXYZ(int cenX, int cenY, double diameter, double & x, double & y, double & z);
void curveFit(vector <double> & Xs, vector <double> & Ys, vector <double> & Zs, ofstream & f);
void threeUnknownLinear(double constants[3][4], double & x, double & y, double & z);
double addOffset(double z1, double t1, double z2, double t2);
void getSpeed(vector <double> & Xs, vector <double> & Ys, vector <double> & Zs, vector <double> & speeds);

void findLongest(vector<int> & xs, vector<int> & ys, int & x1, int & y1, int & x2, int & y2);
void findShortest(int x, int y, vector<int> & xs, vector<int> & ys, int & x2, int & y2);
void findLongest(int x, int y, vector<int> & xs, vector<int> & ys, int & x2, int & y2);
int dis(int x1, int y1, int x2, int y2);

bool recording = false;

// debuggin method

void findBall(uchar hvs[][1080][2] , vector<int> & xs, vector<int> & ys, vector<double> & ds) {

	vector<int> edgeXs = {};
	vector<int> edgeYs = {};
	int startX = 0;
	int startY = 0;
	int endX = 0;
	int endY = 0;
	int cenX = 0;
	int cenY = 0;

	for (int x = 0; x < 1920; x++) {

		bool startFlag = false;
		bool endFlag = false;
		int ballCount = 0;
		int endCount = 0;


		for (int y = 0; y < 1080; y++) {

			if (!startFlag) {
				if (findBall(hvs[x][y][0], hvs[x][y][1])) {
					ballCount++;
				}
				else {
					ballCount = 0;
				}
			}
			else if (!endFlag) {
				if (!findBall(hvs[x][y][0], hvs[x][y][1])) {
					endCount++;
				}
				else {
					endCount = 0;
				}
			}

			if (!startFlag && ballCount > 10) {
				startFlag = true;
				startY = y - ballCount;
				edgeYs.push_back(startY);
				edgeXs.push_back(x);
			}

			else if (!endFlag && endCount > 10) {
				endFlag = true;
				endY = y - endCount;
				edgeYs.push_back(endY);
				edgeXs.push_back(x);
			}

			else if (endFlag) {
				break;
			}

		}
	}


	for (int y = 0; y < 1080; y++) {

		bool startFlag = false;
		bool endFlag = false;
		int ballCount = 0;
		int endCount = 0;

		for (int x = 0; x < 1920; x++) {


			if (!startFlag) {
				if (findBall(hvs[x][y][0], hvs[x][y][1])) {
					ballCount++;
				}
				else {
					ballCount = 0;
				}
			}
			else if (!endFlag) {
				if (!findBall(hvs[x][y][0], hvs[x][y][1])) {
					endCount++;
				}
				else {
					endCount = 0;
				}
			}

			if (!startFlag && ballCount > 10) {
				startFlag = true;
				startX = x - ballCount;
				edgeXs.push_back(startX);
				edgeYs.push_back(y);
			}

			else if (!endFlag && endCount > 10) {
				endFlag = true;
				endX = x - endCount;
				edgeXs.push_back(endX);
				edgeYs.push_back(y);
			}

			else if (endFlag) {
				break;
			}

		}
	}

	int e = edgeYs.size();

	double aX = 0;
	double aY = 0;
	for (int i = 0; i < e; i++) {
		aX += edgeXs.at(i);
		aY += edgeYs.at(i);
	}
	aX /= e;
	aY /= e;

	vector<int> tempEdgeXs = edgeXs;
	vector<int> tempEdgeYs = edgeYs;

	double aD = 0;
	for (int i = 0; i < e; i++) {
		aD += dis(edgeXs.at(i), edgeYs.at(i), aX, aY);
	}
	aD /= e;

	vector<int> noiseCancelledEdgeXs = {};
	vector<int> noiseCancelledEdgeYs = {};
	for (int i = 0; i < e; i++) {
		int x = edgeXs.at(i);
		int y = edgeYs.at(i);
		if (dis(x, y, aX, aY) < 1.5 * aD) {
			noiseCancelledEdgeXs.push_back(x);
			noiseCancelledEdgeYs.push_back(y);
		}
	}

	double diameter = 0;
	int x1, y1, x2, y2, x3, y3, x4, y4, x5, y5;
	findLongest(noiseCancelledEdgeXs, noiseCancelledEdgeYs, x1, y1, x2, y2);
	int tempCenX = (x1 + x2) / 2;
	int tempCenY = (y1 + y2) / 2;
	findLongest(tempCenX, tempCenY, noiseCancelledEdgeXs, noiseCancelledEdgeYs, x3, y3);
	int invX = tempCenX * 2 - x3;
	int invY = tempCenY * 2 - y3;
	findShortest(invX, invY, noiseCancelledEdgeXs, noiseCancelledEdgeYs, x4, y4);
	cenX = (x3 + x4) / 2;
	cenY = (y3 + y4) / 2;
	findLongest(cenX, cenY, noiseCancelledEdgeXs, noiseCancelledEdgeYs, x5, y5);
	diameter = 2.0 * sqrt(dis(x5, y5, cenX, cenY));

	xs.push_back(cenX);
	ys.push_back(cenY);
	ds.push_back(diameter);
}

void findBall() {

	vector<int> xs = {};
	vector<int> ys = {};
	vector<double> ds = {};

	clock_t start;
	double duration = 0.0;
	start = clock();

	uchar hvs[1920][1080][2];

	string line;
	ifstream myfile("C:\\General use\\Homework\\CS 4710\\data\\log.txt");
	int c = 0;
	int lk, k, j;
	if (myfile.is_open())
	{
		for (int i; getline(myfile, line); i++)
		{

			j = i % (1920 * 1080 * 2);
			k = i / (1920 * 1080 * 2);

			if (lk != k) {
				findBall(hvs, xs, ys, ds);
				duration = ((clock() - start) / (double)CLOCKS_PER_SEC);
			}
			
			uchar horv = atof(line.c_str());
			hvs[j%(1920*2)][j/(1920*2)][j%2] = horv;

			lk = k;
		}

		myfile.close();
	}


	cancelNoise(xs, ys, ds);

	cout << "Noise cancel clear" << endl << endl;

	vector<double> Xs = {};
	vector<double> Ys = {};
	vector<double> Zs = {};

	xydToXYZ(xs, ys, ds, Xs, Ys, Zs);

	cout << "coordinate cancel clear" << endl << endl;

	ofstream f;
	f.open("C:\\General use\\Homework\\CS 4710\\data\\func.txt");

	cout << "here" << endl;

	curveFit(Xs, Ys, Zs, f);

	cout << "curvefit cancel clear" << endl << endl;

	vector<double> speeds = {};
	getSpeed(Xs, Ys, Zs, speeds);

	cout << "speed cancel clear" << endl << endl;

	double as = 0;
	for (int i = 0; i < speeds.size(); i++) {
		as += speeds.at(i);
	}

	f << (double)Xs.size() * 1 / 30 << endl;
	as /= speeds.size();
	f << as << endl;

	duration = ((clock() - start) / (double)CLOCKS_PER_SEC);
	cout << duration << endl << endl;

	f.close();
}

//int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
int main()
{

	VideoCapture vcap;
	Mat image;

	// This works on a D-Link CDS-932L
	const string videoStreamAddress = "rtsp://192.168.100.99:554/1";

	//open the video stream and make sure it's opened
	if (!vcap.open(videoStreamAddress)) {
		cout << "Error opening video stream or file" << endl;
		return -1;
	}

	int frame_width = vcap.get(CV_CAP_PROP_FRAME_WIDTH);
	int frame_height = vcap.get(CV_CAP_PROP_FRAME_HEIGHT);

	VideoWriter video("C:\\General use\\Homework\\CS 4710\\data\\out.avi", CV_FOURCC('m', 'p', '4', 'v'), 30, Size(frame_width, frame_height), true);

	const int count = 150;
	vector<Mat> fs;

	//while(1){
	for (int i = 0; i < count && vcap.isOpened(); i++) {
		Mat frame;
		vcap >> frame;
		//imshow("Frame", frame);
		fs.push_back(frame);

		if (recording)
		{
			video << frame;
			char c = (char)waitKey(33);
			if (c == 27) break;
		}
	}
	
	//if (!recording)
		//findBall(frames);
	clock_t start;
	double duration;
	start = clock();

	cutMats(fs);

	cout << fs.size()<< endl;

	duration = (clock() - start) / (double)CLOCKS_PER_SEC;
	cout << duration << endl;

	for (int i = 0; i < fs.size(); i++) {
		Mat newframe = seeNoise(fs[i]);
		video << newframe;
	}
	
	video.release();

	findBall(fs);

	vcap.release();


	return 0;
}

void findBall(vector<Mat> & frames) {

	vector<int> xs = {};
	vector<int> ys = {};
	vector<double> ds = {};

	clock_t start;
	double duration = 0.0;
	start = clock();

	for (int i = 0; i < frames.size(); i++) {
		findBall(frames[i], xs, ys, ds);
		duration = ((clock() - start) / (double)CLOCKS_PER_SEC);
		cout << i << endl;
		cout << duration << endl << endl;
	}

	cancelNoise(xs, ys, ds);

	cout << "Noise cancel clear" << endl << endl;

	vector<double> Xs = {};
	vector<double> Ys = {};
	vector<double> Zs = {};

	xydToXYZ(xs, ys, ds, Xs, Ys, Zs);

	cout << "coordinate cancel clear" << endl << endl;

	ofstream f;
	f.open("C:\\General use\\Homework\\CS 4710\\data\\func.txt");

	cout << "here" << endl;

	curveFit(Xs, Ys, Zs, f);

	cout << "curvefit cancel clear" << endl << endl;

	vector<double> speeds = {};
	getSpeed(Xs, Ys, Zs, speeds);

	cout << "speed cancel clear" << endl << endl;

	double as = 0;
	for (int i = 0; i < speeds.size(); i++) {
		as += speeds.at(i);
	}

	f << (double)Xs.size() * 1 / 30 << endl;
	as /= speeds.size();
	f << as << endl;

	duration = ((clock() - start) / (double)CLOCKS_PER_SEC);
	cout << duration << endl << endl;

	f.close();
}

void findBall(Mat input, vector<int> & xs, vector<int> & ys, vector<double> & ds) {

	Mat hsv;
	cvtColor(input, hsv, CV_BGR2HSV);

	vector<int> edgeXs = {};
	vector<int> edgeYs = {};
	int startX = 0;
	int startY = 0;
	int endX = 0;
	int endY = 0;
	int cenX = 0;
	int cenY = 0;

	ofstream log;
	log.open("C:\\General use\\Homework\\CS 4710\\data\\log.txt");

	for (int x = 0; x < input.cols; x++) {

		bool startFlag = false;
		bool endFlag = false;
		int ballCount = 0;
		int endCount = 0;

		
		for (int y = 0; y < input.rows; y++) {

			Vec3b pixel = hsv.at<Vec3b>(y, x);
			log << pixel.val[0] << endl; // debug
			log << pixel.val[1] << endl; // debug

			if (!startFlag) {
				if (findBall(pixel)) {
					ballCount++;
				}
				else {
					ballCount = 0;
				}
			}
			else if (!endFlag) {
				if (!findBall(pixel)) {
					endCount++;
				}
				else {
					endCount = 0;
				}
			}

			if (!startFlag && ballCount > 10) {
				startFlag = true;
				startY = y - ballCount;
				edgeYs.push_back(startY);
				edgeXs.push_back(x);
			}

			else if (!endFlag && endCount > 10) {
				endFlag = true;
				endY = y - endCount;
				edgeYs.push_back(endY);
				edgeXs.push_back(x);
			}

			else if (endFlag) {
				break;
			}

		}
	}

	for (int y = 0; y < input.rows; y++) {

		bool startFlag = false;
		bool endFlag = false;
		int ballCount = 0;
		int endCount = 0;

		for (int x = 0; x < input.cols; x++) {

			Vec3b pixel = hsv.at<Vec3b>(y, x);

			if (!startFlag) {
				if (findBall(pixel)) {
					ballCount++;
				}
				else {
					ballCount = 0;
				}
			}
			else if (!endFlag) {
				if (!findBall(pixel)) {
					endCount++;
				}
				else {
					endCount = 0;
				}
			}

			if (!startFlag && ballCount > 10) {
				startFlag = true;
				startX = x - ballCount;
				edgeXs.push_back(startX);
				edgeYs.push_back(y);
			}

			else if (!endFlag && endCount > 10) {
				endFlag = true;
				endX = x - endCount;
				edgeXs.push_back(endX);
				edgeYs.push_back(y);
			}

			else if (endFlag) {
				break;
			}

		}
	}

	int e = edgeYs.size();

	double aX = 0;
	double aY = 0;
	for (int i = 0; i < e; i++) {
		aX += edgeXs.at(i);
		aY += edgeYs.at(i);
	}
	aX /= e;
	aY /= e;

	vector<int> tempEdgeXs = edgeXs;
	vector<int> tempEdgeYs = edgeYs;

	double aD = 0;
	for (int i = 0; i < e; i++) {
		aD += dis(edgeXs.at(i), edgeYs.at(i), aX, aY);
	}
	aD /= e;

	vector<int> noiseCancelledEdgeXs = {};
	vector<int> noiseCancelledEdgeYs = {};
	for (int i = 0; i < e; i++) {
		int x = edgeXs.at(i);
		int y = edgeYs.at(i);
		if (dis(x, y, aX, aY) < 1.5 * aD) {
			noiseCancelledEdgeXs.push_back(x);
			noiseCancelledEdgeYs.push_back(y);
		}
	}

	double diameter = 0;
	int x1, y1, x2, y2, x3, y3, x4, y4, x5, y5;
	findLongest(noiseCancelledEdgeXs, noiseCancelledEdgeYs, x1, y1, x2, y2);
	int tempCenX = (x1 + x2) / 2;
	int tempCenY = (y1 + y2) / 2;
	findLongest(tempCenX, tempCenY, noiseCancelledEdgeXs, noiseCancelledEdgeYs, x3, y3);
	int invX = tempCenX * 2 - x3;
	int invY = tempCenY * 2 - y3;
	findShortest(invX, invY, noiseCancelledEdgeXs, noiseCancelledEdgeYs, x4, y4);
	cenX = (x3 + x4) / 2;
	cenY = (y3 + y4) / 2;
	findLongest(cenX, cenY, noiseCancelledEdgeXs, noiseCancelledEdgeYs, x5, y5);
	diameter = 2.0 * sqrt(dis(x5, y5, cenX, cenY));

	xs.push_back(cenX);
	ys.push_back(cenY);
	ds.push_back(diameter);
}

bool findBall(Vec3b pixel) {
	return (pixel.val[0] > 167 || pixel.val[0] < 6) && pixel.val[1] > 90;
}

bool findBall(uchar h, uchar v) {
	return (h > 167 || h < 6) && v > 90;
}

void cutMats(vector<Mat> & fs) {

	int arround = mostPixel(fs);

	if (arround < 10) {
		fs.erase(fs.begin() + 30, fs.end());
	}

	else if (arround > 279) {
		fs.erase(fs.begin(), fs.end() - 30);
	}

	else {
		fs.erase(fs.begin() + arround + 20, fs.end());
		fs.erase(fs.begin(), fs.begin() + arround - 10);
	}
}

int mostPixel(vector<Mat> & fs) {
	int most = 0;
	int mostI = -1;

	for (int i = 0; i < fs.size(); i += 10) {
		int potential = countPixel(fs.at(i));
		if (potential > most) {
			most = potential;
			mostI = i;
		}
	}

	return mostI == -1 ? 0 : mostI;
}

int countPixel(Mat f) {

	int count = 0;

	Mat hsv;
	cvtColor(f, hsv, CV_BGR2HSV);

	uchar h, v;
	vector<int> is, js;
	for (int i = 0; i < f.rows; ++i) {
		uchar* pixel = hsv.ptr<uchar>(i);
		for (int j = 0; j < f.cols; ++j) {
			h = *pixel++;
			v = *pixel++;
			pixel++;
			if (findBall(h, v)) {
				count++;
			}
		}
	}

	return count;
}

void curveFit(vector <double> & Xs, vector <double> & Ys, vector <double> & Zs, ofstream & f) {

	int n = Xs.size();

	double st = 0;
	double st2 = 0;
	double st3 = 0;
	double st4 = 0;

	double sx = 0;
	double sxt = 0;
	double sxt2 = 0;

	double sy = 0;
	double syt = 0;

	double sz = 0;
	double slnt = 0;
	double szlnt = 0;
	double slnt2 = 0;

	// sum up


	cout << "or here" << endl;

	double offset = addOffset(Zs[0], 1.0 / 30, Zs[1], 2.0 / 30);
	offset = offset < -1/30 ? 0 : offset;


	cout << offset << endl;

	for (double i = 0; i < n; i++) {

		double t = (i + 1) / 30.0;
		double ot = t + offset;
		double lnt = log(ot);
		double x = Xs.at(i);
		double y = Ys.at(i);
		double z = Zs.at(i);

		cout << i << ": " << z << endl;

		st += t;
		st2 += t*t;
		st3 += t*t*t;
		st4 += t*t*t*t;

		sx += x;
		sxt += x * t;
		sxt2 += x * t * t;

		sy += y;
		syt += y * t;

		sz += z;
		slnt += lnt;
		szlnt += z * lnt;
		slnt2 += lnt * lnt;
	}

	// x part

	double constants[3][4];

	constants[0][0] = st4;
	constants[0][1] = st3;
	constants[0][2] = st2;
	constants[0][3] = -sxt2;

	constants[1][0] = st3;
	constants[1][1] = st2;
	constants[1][2] = st;
	constants[1][3] = -sxt;

	constants[2][0] = st2;
	constants[2][1] = st;
	constants[2][2] = n;
	constants[2][3] = -sx;

	double xa, xb, xc;
	threeUnknownLinear(constants, xa, xb, xc);

	// y part

	double ay = sy / n;
	double at = st / n;
	double yb = (syt - n * ay * at) / (st2 - n * at * at);
	double ya = ay - yb * at;

	// z part
	cout << "slnt: " << slnt << endl;
	double zb = (n * szlnt - sz * slnt) / (n * slnt2 - slnt * slnt);
	cout << "zb: " << zb << endl;
	int kk;
	cin >> kk;
	double za = (sz - zb * slnt) / n;

	// curve fitting
	//for (double i = 0; i < n; i++) {
	//	double t = (i + 1) / 30.0;
	//	Xs.at(i) = xa*t*t + xb*t + xc;
	//	Ys.at(i) = ya + yb*t;
	//	Zs.at(i) = za + zb*log(t + offset);
	//}

	f << xa << endl << xb << endl << xc << endl;
	f << ya << endl << yb << endl;
	f << za << endl << zb << endl << offset << endl;
}

void threeUnknownLinear(double constants[3][4], double & x, double & y, double & z) {

	double a, b, c, d, l, m, n, k, p, D, q, r, s;

	/*
	ax+by+cz+d=0
	lx+my+nz+k=0
	px+qy+rz+s=0
	*/

	a = constants[0][0];
	b = constants[0][1];
	c = constants[0][2];
	d = constants[0][3];

	l = constants[1][0];
	m = constants[1][1];
	n = constants[1][2];
	k = constants[1][3];

	p = constants[2][0];
	q = constants[2][1];
	r = constants[2][2];
	s = constants[2][3];

	D = (a*m*r + b*p*n + c*l*q) - (a*n*q + b*l*r + c*m*p);
	x = ((b*r*k + c*m*s + d*n*q) - (b*n*s + c*q*k + d*m*r)) / D;
	y = ((a*n*s + c*p*k + d*l*r) - (a*r*k + c*l*s + d*n*p)) / D;
	z = ((a*q*k + b*l*s + d*m*p) - (a*m*s + b*p*k + d*l*q)) / D;
	//cout << "x:" << x  << "\ny:" << y  << "\nz:" << z ;
}

double addOffset(double z1, double t1, double z2, double t2) {
	double k = (z2 - z1) / (t2 - t1);
	double c = z1 - k * t1;
	return c / k;
}

void getSpeed(vector <double> & Xs, vector <double> & Ys, vector <double> & Zs, vector <double> & speeds) {

	unsigned long n = Xs.size() - 1;
	for (int i = 0; i < n; i++) {
		speeds.push_back(
			sqrt((Xs.at(i + 1) - Xs.at(i))*(Xs.at(i + 1) - Xs.at(i)) +
			(Ys.at(i + 1) - Ys.at(i))*(Ys.at(i + 1) - Ys.at(i)) +
				(Zs.at(i + 1) - Zs.at(i))*(Zs.at(i + 1) - Zs.at(i))) * 0.03 * 2.23694);
	}
}

void cancelNoise(vector<int> & xs, vector<int> & ys, vector<double> & ds) {

	int n = xs.size();

	int theS = 0; // start
	int theE = 0; // end
	int thisS = 0;

	for (int i = 1; i < n; i++) {

		if (xs.at(i) < xs.at(i - 1)) {

			if (xs.at(theE) - xs.at(theS) < xs.at(i) - xs.at(thisS)) {
				theS = thisS;
				theE = i;
			}

			thisS = i;
		}
	}

	if ((theS | theE | thisS) != 0) {

		xs.erase(xs.begin() + theE, xs.end());
		ys.erase(ys.begin() + theE, ys.end());
		ds.erase(ds.begin() + theE, ds.end());

		xs.erase(xs.begin(), xs.begin() + theS + 1);
		ys.erase(ys.begin(), ys.begin() + theS + 1);
		ds.erase(ds.begin(), ds.begin() + theS + 1);

	}
}

void xydToXYZ(vector<int> xs, vector<int> ys, vector<double> ds, vector<double> & Xs, vector<double> & Ys, vector<double> & Zs) {
	for (int i = 0; i < xs.size(); i++) {
		double X = 0;
		double Y = 0;
		double Z = 0;
		xydToXYZ(xs.at(i), ys.at(i), ds.at(i), X, Y, Z);
		Xs.push_back(X);
		Ys.push_back(Y);
		Zs.push_back(Z);
	}
}

void xydToXYZ(int cenX, int cenY, double diameter, double & x, double & y, double & z) {

	/*
	Baseball diameter: 74.68 mm
	ZCam FOV : 122¢X / 58¢X
	cols: 1920
	rows: 1080
	iPhone FOV: 58.040¢X
	*/

	double fov_v = 28 * 3.1415926535 / 180;
	double fov = fov_v * 1920 / 1080;
	double bbd = 74.68;

	double depth = 0;
	double angleV = 0;
	double angleH = 0;

	if (diameter != 0) {
		depth = bbd / 2 / sin(fov * diameter / 1920 / 2);
		angleH = fov * (double)cenX / 1920;
		angleV = fov_v * (double)cenY / 1080;
	}

	x = -depth * cos(fov_v / 2 - angleV) * sin(fov / 2 - angleH);
	y = depth * sin(fov_v / 2 - angleV);
	z = depth * cos(fov_v / 2 - angleV) * cos(fov / 2 - angleH);
}

void findLongest(vector<int> & xs, vector<int> & ys, int & x1, int & y1, int & x2, int & y2) {
	for (int i = 0; i < xs.size(); i++) {
		for (int j = i; j < xs.size(); j++) {
			if (dis(xs.at(i), ys.at(i), xs.at(j), ys.at(j)) > dis(x1, y1, x2, y2)) {
				x1 = xs.at(i);
				y1 = ys.at(i);
				x2 = xs.at(j);
				y2 = ys.at(j);
			}
		}
	}
}

void findShortest(int x, int y, vector<int> & xs, vector<int> & ys, int & x2, int & y2) {

	int closestX = -4000;
	int closestY = -4000;

	for (int i = 0; i < xs.size(); i++) {
		if (dis(xs.at(i), ys.at(i), x, y) < dis(closestX, closestY, x, y)) {
			closestX = xs.at(i);
			closestY = ys.at(i);
		}
	}

	if (closestX == -4000) {
		x2 = 0;
		y2 = 0;
	}
	else {
		x2 = closestX;
		y2 = closestY;
	}
}

void findLongest(int x, int y, vector<int> & xs, vector<int> & ys, int & x2, int & y2) {

	int furthestX = x;
	int furthestY = y;

	for (int i = 0; i < xs.size(); i++) {
		if (dis(xs.at(i), ys.at(i), x, y) > dis(furthestX, furthestY, x, y)) {
			furthestX = xs.at(i);
			furthestY = ys.at(i);
		}
	}

	if (furthestX == 4000) {
		x2 = 0;
		y2 = 0;
	}
	else {
		x2 = furthestX;
		y2 = furthestY;
	}
}

int dis(int x1, int y1, int x2, int y2) {
	return (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2);
}