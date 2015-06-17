#ifndef _MY_OPENCV_LIB_H_
#define _MY_OPENCV_LIB_H_

#define _CRT_SECURE_NO_WARNINGS

#include <opencv2/opencv.hpp>

#if _DEBUG
#pragma comment(lib, "opencv_calib3d248d.lib")
#pragma comment(lib, "opencv_contrib248d.lib")
#pragma comment(lib, "opencv_core248d.lib")
#pragma comment(lib, "opencv_features2d248d.lib")
#pragma comment(lib, "opencv_flann248d.lib")
#pragma comment(lib, "opencv_gpu248d.lib")
#pragma comment(lib, "opencv_highgui248d.lib")
#pragma comment(lib, "opencv_imgproc248d.lib")
#pragma comment(lib, "opencv_legacy248d.lib")
#pragma comment(lib, "opencv_ml248d.lib")
#pragma comment(lib, "opencv_nonfree248d.lib")
#pragma comment(lib, "opencv_objdetect248d.lib")
#pragma comment(lib, "opencv_ocl248d.lib")
#pragma comment(lib, "opencv_photo248d.lib")
#pragma comment(lib, "opencv_stitching248d.lib")
#pragma comment(lib, "opencv_superres248d.lib")
#pragma comment(lib, "opencv_ts248d.lib")
#pragma comment(lib, "opencv_video248d.lib")
#pragma comment(lib, "opencv_videostab248d.lib")
#else
#pragma comment(lib, "opencv_calib3d248.lib")
#pragma comment(lib, "opencv_contrib248.lib")
#pragma comment(lib, "opencv_core248.lib")
#pragma comment(lib, "opencv_features2d248.lib")
#pragma comment(lib, "opencv_flann248.lib")
#pragma comment(lib, "opencv_gpu248.lib")
#pragma comment(lib, "opencv_highgui248.lib")
#pragma comment(lib, "opencv_imgproc248.lib")
#pragma comment(lib, "opencv_legacy248.lib")
#pragma comment(lib, "opencv_ml248.lib")
#pragma comment(lib, "opencv_nonfree248.lib")
#pragma comment(lib, "opencv_objdetect248.lib")
#pragma comment(lib, "opencv_ocl248.lib")
#pragma comment(lib, "opencv_photo248.lib")
#pragma comment(lib, "opencv_stitching248.lib")
#pragma comment(lib, "opencv_superres248.lib")
#pragma comment(lib, "opencv_ts248.lib")
#pragma comment(lib, "opencv_video248.lib")
#pragma comment(lib, "opencv_videostab248.lib")
#endif 

#endif