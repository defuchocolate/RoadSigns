//
//  ShapeFinder.h
//  RoadSigns
//
//  Created by Fernando Birra on 2/14/13.
//  Copyright (c) 2013 Fernando Birra. All rights reserved.
//

#ifndef __RoadSigns__ShapeFinder__
#define __RoadSigns__ShapeFinder__

#include <iostream>
#include <cv.h>

#include "Shapes.h"

void computeGradients(cv::Mat input, cv::Mat &gx, cv::Mat &gy, cv::Mat &mag);
void normalizeFloatImage(cv::Mat &img);

class ShapeFinder {
public:
    ShapeFinder(cv::Mat &img) : img(img), r(img.rows), c(img.cols) {};
    
    virtual ~ShapeFinder() {};
    
    void prepare(double threshold);
    std::vector<Shape*> findShape(int nsides, const std::vector<int> &rs);
    
    const cv::Mat &getScalarGradient() const { return mag; }
    const cv::Mat &getS() const { return nS; }
    
protected:
    void computeGradients(double threshold);

private:
    int r, c;
    cv::Mat &img;
    cv::Mat gx, gy, mag;
    cv::Mat S;
    cv::Mat nS;
    
    std::vector<Shape*> findCircles(const std::vector<int> &rs);
    std::vector<Shape*> findPolygons(int nsides, const std::vector<int> &rs);
};


#endif /* defined(__RoadSigns__ShapeFinder__) */
