#ifndef MAIN
#define MAIN

#include <QApplication>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/ximgproc.hpp> //For SEEDS
#include <iostream>

using namespace cv;
using namespace std;
using namespace cv::ximgproc;

// --------------- パラメータ ----------------
/*
static int num_superpixels;
int num_iterations = 10;
int prior = 3;
bool double_step = false;
int num_levels = 8;
int num_histogram_bins = 5;
*/
static int superpixels;
const int bin = 5;
const int neighSize = 3;
const float minConf = 0.1;
const float spatialWeight = 0.1; // < 1.0 の値
const int iterations = 2;

// -------------------------------------------

int height, width;

#endif