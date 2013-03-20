/*
 * TSDRS.h
 *
 * Author: Pedro Cabral
 */

#ifndef TSDRS_H_
#define TSDRS_H_

#include <highgui.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include "ObjDet.h"

using namespace cv;

/*TODO:
 * .falta receber como argumento as coords GPS, erro_GPS e orientação do telemóvel
 * .acabar detecção
 * .fazer reconhecimento/classificação
 * .decidir o que o método computeGPS vai devolver
 */
class TSDRS {
	vector<ObjDet> detObj; //vector with all detected objects
	int n_radii;
	int *radii;
	int n_shapes;
	int *sides;
public:
	TSDRS();
	virtual ~TSDRS();
	vector<ObjDet> detection( Mat );
	vector<ObjDet> recognition( vector<ObjDet> );
	void computeGPS( vector<ObjDet> );
	void drawShape(Mat&,Point,int,int);
};

#endif /* TSDRS_H_ */
