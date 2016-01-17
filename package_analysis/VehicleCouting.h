#pragma once

//#include <boost/config/user.hpp>
#include <boost\config\user.hpp>
#include <boost\lexical_cast.hpp>
#include <iostream>
#include <opencv\cv.h>
#include <opencv\highgui.h>
//#include "../package_tracking/cvblob/cvblob.h"

#include <c:/Users/rahul/Documents/Visual Studio 2010/Projects/majorProject/package_tracking/cvblob/cvblob.h>

//int globalCount=0;

enum LaneOrientation
{
  LO_NONE       = 0,
  LO_HORIZONTAL = 1,
  LO_VERTICAL   = 2
};

enum VehiclePosition
{
  VP_NONE = 0,
  VP_A  = 1,
  VP_B  = 2
};

class VehicleCouting
{
private:
  bool firstTime;
  bool showOutput;
  int key;
  cv::Mat img_input;
  cvb::CvTracks tracks;
  std::map<cvb::CvID, std::vector<CvPoint2D64f>> points;
  LaneOrientation LaneOrientation;
  std::map<cvb::CvID, VehiclePosition> positions;

  int img_w;
  int img_h;
  int showAB;

public:
  VehicleCouting();
  ~VehicleCouting();

  void setInput(const cv::Mat &i);
  void setTracks(const cvb::CvTracks &t);
  void process();
  long countAB;
  long countBA;

private:
  VehiclePosition getVehiclePosition(const CvPoint2D64f centroid);

  void saveConfig();
  void loadConfig();
};
