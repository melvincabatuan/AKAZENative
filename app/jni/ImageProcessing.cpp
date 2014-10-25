/*
*  ImageProcessing.cpp
* package com.cabatuan.kazenative;
*/
#include <jni.h>

#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "AKAZE.h"

using namespace std;
using namespace cv;
using namespace libAKAZE;

// Default options
const float DEFAULT_SCALE_OFFSET = 1.60;              // Base scale offset (sigma units)
const float DEFAULT_OCTAVE_MAX = 4.0;                 // Maximum octave evolution of the image 2^sigma (coarsest scale sigma units)
const int DEFAULT_NSUBLEVELS = 4;                     // Default number of sublevels per scale level
const float DEFAULT_DETECTOR_THRESHOLD = 0.001;       // Detector response threshold to accept point
const float DEFAULT_MIN_DETECTOR_THRESHOLD = 0.00001; // Minimum Detector response threshold to accept point
const float DEFAULT_SIGMA_SMOOTHING_DERIVATIVES = 1.0;
//const int DEFAULT_DIFFUSIVITY_TYPE = 1;               // 0 -> PM G1, 1 -> PM G2, 2 -> Weickert
//const int DEFAULT_DESCRIPTOR_MODE = 1;                // Descriptor Mode 0->SURF, 1->M-SURF
//const bool DEFAULT_UPRIGHT = false;                  // Upright descriptors, not invariant to rotation
//const bool DEFAULT_EXTENDED = false;                 // Extended descriptor, dimension 128
//const bool DEFAULT_SAVE_SCALE_SPACE = false;         // For saving the scale space images
//const bool DEFAULT_VERBOSITY = false;                // Verbosity level (0->no verbosity)
//const bool DEFAULT_SAVE_KEYPOINTS = false;           // For saving the list of keypoints


Mat * temp = NULL;

extern "C"
jboolean
Java_com_cabatuan_kazenative_CameraPreview_ImageProcessing(
		JNIEnv* env, jobject thiz,
		jint width, jint height,
		jbyteArray NV21FrameData, jintArray outPixels)
{
	jbyte * pNV21FrameData = env->GetByteArrayElements(NV21FrameData, 0);
	jint * poutPixels = env->GetIntArrayElements(outPixels, 0);

	if ( temp == NULL )
    	{
    		temp = new Mat(height, width, CV_8UC1);
    	}

	Mat mGray(height, width, CV_8UC1, (unsigned char *)pNV21FrameData);
	Mat mResult(height, width, CV_8UC4, (unsigned char *)poutPixels);

	Mat KazeImg = *temp;
    vector<KeyPoint> kpts;
    AKAZEOptions options;

    /// Convert the image to float to extract features.
      Mat mGray_32;
      mGray.convertTo(mGray_32, CV_32F, 1.0/255.0,0);

    /// Set options and Detect features
        options.img_width = mGray.cols;
        options.img_height = mGray.rows;
        options.omax = DEFAULT_OCTAVE_MAX;
        options.nsublevels = DEFAULT_NSUBLEVELS;
        options.dthreshold = DEFAULT_DETECTOR_THRESHOLD;
        options.sderivatives = DEFAULT_SIGMA_SMOOTHING_DERIVATIVES;
        options.descriptor_channels = 1;
        options.descriptor_size = 100;
        options.save_scale_space = false;
        options.verbosity = false;
        options.save_keypoints = false;
        AKAZE evolution(options);

        evolution.Create_Nonlinear_Scale_Space(mGray_32);
        evolution.Feature_Detection(kpts);

    /// Detect A-KAZE features in the input image
    //akz(mGray, noArray(), keypts, KazeImg, false);

    drawKeypoints( mGray, kpts, KazeImg, Scalar( 0, 255, 0));

	cvtColor(KazeImg, mResult, CV_BGR2BGRA);

	env->ReleaseByteArrayElements(NV21FrameData, pNV21FrameData, 0);
	env->ReleaseIntArrayElements(outPixels, poutPixels, 0);
	return true;
}
