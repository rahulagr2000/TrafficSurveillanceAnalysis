#include <iostream>
#include <fstream>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <C:/Users/rahul/Documents/Visual Studio 2010/Projects/majorProject/major_project_includes.h>

#define DTTMFMT "%S "	// For Date and Time- Here only seconds
#define DTTMSZ 10

//---------------------------------------------------------------------------------------------------

using namespace std;

CvHaarClassifierCascade *cascade;
CvMemStorage            *storage;


// Function Declarations

void detect(IplImage *img);
static char *getDtTm (char *buff); 


// MAIN Below

int main(int argc, char **argv)
{
	CvCapture *capture = 0;
	capture = cvCaptureFromAVI("./dataset/video.avi");
	if(!capture)
	{
		std::cerr << "Cannot open video!" << std::endl;
		return 1;
	}
  
	cascade = (CvHaarClassifierCascade*) cvLoad("cars3.xml");
	storage = cvCreateMemStorage(0);
	assert(cascade && storage && capture);

	ofstream myfile;
	myfile.open ("Statistics.dat");
	
	int resize_factor = 100; // 50% of original image
	int sec=0;				 // Storing time temporarily 
	char buff[DTTMSZ];		 // Buffer of size 10 for storing seconds

	sec=atoi(getDtTm(buff)); // Storing corrent time

	IplImage *frame_aux = cvQueryFrame(capture);
	IplImage *frame = cvCreateImage(cvSize((int)((frame_aux->width*resize_factor)/100) , (int)((frame_aux->height*resize_factor)/100)), frame_aux->depth, frame_aux->nChannels);
  
	// Initializing Background Subtraction Methods
	IBGS *bgs;
	bgs = new PixelBasedAdaptiveSegmenter;
    
	// Initializing Blob Tracking 
	cv::Mat img_blob;
	BlobTracking* blobTracking;
	blobTracking = new BlobTracking;

	// Initializing Vehicle Counting Algorithm
	VehicleCouting* vehicleCouting;
	vehicleCouting = new VehicleCouting;

	int key = 0;
	while(key != 'q')
	{
		frame_aux = cvQueryFrame(capture);
		if(!frame_aux) break;

		cvResize(frame_aux, frame);

		cv::Mat img_input(frame);
		cv::imshow("input", img_input);

		// bgs->process(...) method internally shows the foreground mask image
		cv::Mat img_mask;
		bgs->process(img_input, img_mask);
    
		if(!img_mask.empty())
		{
			// Blob tracking each frame
			blobTracking->process(img_input, img_mask, img_blob);

			// Vehicle counting in each frame
			vehicleCouting->setInput(img_blob);
			vehicleCouting->setTracks(blobTracking->getTracks());
			vehicleCouting->process();
		
			// Haar Classification
			detect(frame);

			//Writing data to a file
			myfile <<(((atoi(getDtTm(buff))-sec)< 0) ?(atoi(getDtTm(buff))+ 60 -sec):(atoi(getDtTm(buff))-sec))<< "\t" <<(vehicleCouting->countAB +vehicleCouting->countBA) << "\n" ;
		}

		key = cvWaitKey(1);
	}

	delete vehicleCouting;
	delete blobTracking;
	delete bgs;

	cvDestroyAllWindows();
	cvReleaseCapture(&capture);
	cvReleaseHaarClassifierCascade(&cascade);
	cvReleaseMemStorage(&storage);
	cvReleaseImage(&frame);
	myfile.close();

	return 0;
}


void detect(IplImage *img)
{
	CvSize img_size = cvGetSize(img);
	CvSeq *object = cvHaarDetectObjects(img,cascade,storage,1.1,1,0,cvSize(0,0),img_size );

	//std::cout << "Total: " << object->total << " cars" << std::endl;
	for(int i = 0 ; i < ( object ? object->total : 0 ) ; i++)
	{
		CvRect *r = (CvRect*)cvGetSeqElem(object, i);
		cvRectangle(img,
		cvPoint(r->x, r->y),
		cvPoint(r->x + r->width, r->y + r->height),
		CV_RGB(255, 0, 0), 2, 8, 0);
	}
	
	cvShowImage("Haar Recognition", img);
}

static char *getDtTm (char *buff) 
{
    time_t t = time (0);
    strftime (buff, DTTMSZ, DTTMFMT, localtime (&t));
    return buff;
}
