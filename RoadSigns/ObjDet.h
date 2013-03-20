/*
 * ObjDet.h
 *
 *  Author: Pedro Cabral
 */

#ifndef OBJDET_H_
#define OBJDET_H_

#include <cv.h>
#include "GPSCoord.h"

using namespace cv;

class ObjDet {
	Mat ROI;
	int radius;
	int n_sides;
	GPSCoord coord;

public:
	ObjDet();
	ObjDet(Mat,int,int);
	ObjDet(Mat,int,int,GPSCoord);
	ObjDet(const ObjDet&);
	virtual ~ObjDet();
	Mat getDetectedObject() const;
	int getRadius() const;
	int getNSides() const;
	void setCoordinates(double lat, double lon);
	GPSCoord getCoordinates() const;
	ObjDet& operator=(const ObjDet&);
};

#endif /* OBJDET_H_ */
