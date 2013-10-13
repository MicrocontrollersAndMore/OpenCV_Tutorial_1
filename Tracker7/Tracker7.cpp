// Tracker7.cpp

#include<opencv/cvaux.h>
#include<opencv/highgui.h>
#include<opencv/cxcore.h>

#include<stdio.h>
#include<stdlib.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[]) {


	CvSize size640x480 = cvSize(640, 480);			// use a 640 x 480 size for all windows, also make sure your webcam is set to 640x480 !!

	CvCapture* p_capWebcam;						// we will assign our web cam video stream to this later . . .

	IplImage* p_imgOriginal;			// pointer to an image structure, this will be the input image from webcam
	IplImage* p_imgProcessed;			// pointer to an image structure, this will be the processed image
										/* IPL is short for Intel Image Processing Library, this is the structure used in OpenCV 1.x to work with images */

	CvMemStorage* p_strStorage;			// necessary storage variable to pass into cvHoughCircles()

	CvSeq* p_seqCircles;				// pointer to an OpenCV sequence, will be returned by cvHough Circles() and will contain all circles
										// call cvGetSeqElem(p_seqCircles, i) will return a 3 element array of the ith circle (see next variable)
	
	float* p_fltXYRadius;				// pointer to a 3 element array of floats
										// [0] => x position of detected object
										// [1] => y position of detected object
										// [2] => radius of detected object

	int i;								// loop counter
	char charCheckForEscKey;			// char for checking key press (Esc exits program)

	p_capWebcam = cvCaptureFromCAM(0);	// 0 => use 1st webcam, may have to change to a different number if you have multiple cameras

	if(p_capWebcam == NULL) {			// if capture was not successful . . .
		printf("error: capture is NULL \n");	// error message to standard out . . .
		getchar();								// getchar() to pause for user see message . . .
		return(-1);								// exit program
	}

											// declare 2 windows
	cvNamedWindow("Original", CV_WINDOW_AUTOSIZE);		// original image from webcam
	cvNamedWindow("Processed", CV_WINDOW_AUTOSIZE);		// the processed image we will use for detecting circles

	p_imgProcessed = cvCreateImage(size640x480,			// 640 x 480 pixels (CvSize struct from earlier)
								   IPL_DEPTH_8U,		// 8-bit color depth
								   1);					// 1 channel (grayscale), if this was a color image, use 3

	while(1) {								// for each frame . . .
		p_imgOriginal = cvQueryFrame(p_capWebcam);		// get frame from webcam
		
		if(p_imgOriginal == NULL) {					// if frame was not captured successfully . . .
			printf("error: frame is NULL \n");		// error message to std out
			getchar();
			break;
		}

		cvInRangeS(p_imgOriginal,				// function input
				   CV_RGB(175,  0,  0),			// min filtering value (if color is greater than or equal to this)
				   CV_RGB(256,100,100),			// max filtering value (if color is less than this)
				   p_imgProcessed);				// function output

		p_strStorage = cvCreateMemStorage(0);	// allocate necessary memory storage variable to pass into cvHoughCircles()

										// smooth the processed image, this will make it easier for the next function to pick out the circles
		cvSmooth(p_imgProcessed,		// function input
				 p_imgProcessed,		// function output
				 CV_GAUSSIAN,			// use Gaussian filter (average nearby pixels, with closest pixels weighted more)
				 9,						// smoothing filter window width
				 9);					// smoothing filter window height

													// fill sequential structure with all circles in processed image
		p_seqCircles = cvHoughCircles(p_imgProcessed,		// input image, nothe that this has to be grayscale (no color)
									  p_strStorage,			// provide function with memory storage, makes function return a pointer to a CvSeq
									  CV_HOUGH_GRADIENT,	// two-pass algorithm for detecting circles, this is the only choice available
									  2,					// size of image / 2 = "accumulator resolution", i.e. accum = res = size of image / 2
									  p_imgProcessed->height / 4,	// min distance in pixels between the centers of the detected circles
									  100,						// high threshold of Canny edge detector, called by cvHoughCircles
									  50,						// low threshold of Canny edge detector, called by cvHoughCircles
									  10,						// min circle radius, in pixels
									  400);						// max circle radius, in pixels

		for(i=0; i < p_seqCircles->total; i++) {		// for each element in sequential circles structure (i.e. for each object detected)

			p_fltXYRadius = (float*)cvGetSeqElem(p_seqCircles, i);	// from the sequential structure, read the ith value into a pointer to a float

			printf("ball position x = %f, y = %f, r = %f \n", p_fltXYRadius[0],		// x position of center point of circle
															  p_fltXYRadius[1],		// y position of center point of circle
															  p_fltXYRadius[2]);	// radius of circle

										// draw a small green circle at center of detected object
			cvCircle(p_imgOriginal,										// draw on the original image
					 cvPoint(cvRound(p_fltXYRadius[0]), cvRound(p_fltXYRadius[1])),		// center point of circle
					 3,													// 3 pixel radius of circle
					 CV_RGB(0,255,0),									// draw pure green
					 CV_FILLED);										// thickness, fill in the circle
			
										// draw a red circle around the detected object
			cvCircle(p_imgOriginal,										// draw on the original image
					 cvPoint(cvRound(p_fltXYRadius[0]), cvRound(p_fltXYRadius[1])),		// center point of circle
					 cvRound(p_fltXYRadius[2]),							// radius of circle in pixels
					 CV_RGB(255,0,0),									// draw pure red
					 3);												// thickness of circle in pixels
		}	// end for

		cvShowImage("Original", p_imgOriginal);			// original image with detectec ball overlay
		cvShowImage("Processed", p_imgProcessed);		// image after processing

		cvReleaseMemStorage(&p_strStorage);				// deallocate necessary storage variable to pass into cvHoughCircles

		charCheckForEscKey = cvWaitKey(10);				// delay (in ms), and get key press, if any
		if(charCheckForEscKey == 27) break;				// if Esc key (ASCII 27) was pressed, jump out of while loop
	}	// end while

	cvReleaseCapture(&p_capWebcam);					// release memory as applicable

	cvDestroyWindow("Original");
	cvDestroyWindow("Processed");

	return(0);
}