//
//  Main.c
//  RoadSigns
//
//  Created by Fernando Birra on 2/10/13.
//  Copyright (c) 2013 Fernando Birra. All rights reserved.
//


#include <stdio.h>
#include <cv.h>

#include <highgui.h>

#include <iostream>
#include <tclap/CmdLine.h>

#include "ShapeFinder.h"

#define TEMPLATE_SIZE   32.0
#define GRAD_THRESHOLD  150



std::vector<int> &split(const std::string &s, char delim, std::vector<int> &elems) {
    std::stringstream ss(s);
    std::string item;
    int value;
    while(std::getline(ss, item, delim)) {
        istringstream(item) >> value;
        elems.push_back(value);
    }
    return elems;
}


std::vector<int> split(const std::string &s, char delim) {
    std::vector<int> elems;
    return split(s, delim, elems);
}



void HSVtoHUE(cv::Mat &input, cv::Mat &output)
{
    output.create(input.size(), input.depth());
    int ch[] = {1, 0};
    mixChannels(&input, 1, &output, 1, ch ,1);
}



std::string tempName(int length) {
    std::string result;
    
    for(int i=0; i<length; i++) {
        result.push_back(rand()%26+'a');
    }
    return result;
}


double maxV=0.0;
bool debug_mode = false;
bool extract_mode = false;
int frame_nr = 0;
std::string extract_dir;
std::string strand;

bool tri_mode = false;
bool square_mode = false;
bool octagon_mode = false;
bool circle_mode = false;

void captureImages(const std::vector<Shape*> &shapes, cv::Mat &img)
{
    int shape_nr = 0;
    
    for(std::vector<Shape*>::const_iterator it = shapes.begin(); it!=shapes.end(); it++, shape_nr++) {
        // Best place to dump the individual subimages
        if(extract_mode) {
            int left = MAX(0,(*it)->getLeft());
            int top = MAX(0,(*it)->getTop());
            int width = MIN((*it)->getRight(), img.cols) - left;
            int height = MIN((*it)->getBottom(), img.rows) - top;
            
            cv::Mat subimage(img, cv::Rect_<int>(left, top, width, height));
            std::stringstream ss;
            ss << extract_dir << "/" << (*it)->nsides << "_" << strand << frame_nr << "_" << shape_nr << ".jpg";
            if(width==height) {
                cv::resize(subimage, subimage, cv::Size(), TEMPLATE_SIZE/width, TEMPLATE_SIZE/height, CV_INTER_LINEAR);
                cv::imwrite(ss.str(), subimage);
            }
        }
    }
}

void drawShapes(const std::vector<Shape*> &shapes, cv::Mat &img)
{
    for(std::vector<Shape*>::const_iterator it = shapes.begin(); it!=shapes.end(); it++)
        (*it)->drawOn(img);
}

int main(int argc, char* argv[])
{
    float scalex=1.0f, scaley=1.0f;
    int mirror=100;
    bool hsv_mode;
    int width, height;
    
    std::string config_file;
    std::string stream_file;
    std::string color_mode;
    std::string lengths_string;
    std::string types_string;
    
    std::srand((unsigned) std::time(0));
    
    cv::Mat frame;
    
    try {
        TCLAP::CmdLine cmd("Command description message...", ' ', "0.9");
        
        TCLAP::ValueArg<std::string> config_arg("p", "param", "Parameters file", false, "config.xml", "string");
        TCLAP::ValueArg<std::string> mode_arg("c", "color", "Color mode (hsv or grayscale)", false, "hsv", "hsv|gray");
        TCLAP::ValueArg<std::string> mirror_arg("m", "mirror", "Mirror mode (none | horizontal | vertical | both)", false, "none", "string");
        TCLAP::ValueArg<std::string> lengths_arg("l", "lengths", "Lengths of signs", false, "5", "comma separated list of lenths");
        TCLAP::ValueArg<std::string> types_arg("t", "types", "Types of road signs", false, "0", "comma separate list of int values");
        TCLAP::ValueArg<float> scale_arg("s", "scale", "Scaling factor", false, 1.0f, "float");
        TCLAP::ValueArg<int> width_arg("x", "sizex", "Horizontal image size", false, -1, "int");
        TCLAP::ValueArg<int> height_arg("y", "sizey", "Vertical image size", false, -1, "int");
        TCLAP::ValueArg<std::string> stream_arg("i", "input", "Input stream file", true, "0", "filename");
        TCLAP::ValueArg<std::string> extract_arg("e", "extract", "Extract dir", false, "", "extract directory");
        TCLAP::ValueArg<bool> debug_arg("d", "debug", "Debug mode", false, false, "bool");
        
        cmd.add(config_arg);
        cmd.add(mode_arg);
        cmd.add(mirror_arg);
        cmd.add(lengths_arg);
        cmd.add(types_arg);
        cmd.add(scale_arg);
        cmd.add(width_arg);
        cmd.add(height_arg);
        cmd.add(stream_arg);
        cmd.add(extract_arg);
        cmd.add(debug_arg);
        cmd.parse(argc, argv);
        
        config_file = config_arg.getValue();
        stream_file = stream_arg.getValue();
        extract_dir = extract_arg.getValue();
        debug_mode = debug_arg.getValue();
        
        if(mirror_arg.getValue()=="vertical")
            mirror =0;
        else if(mirror_arg.getValue()=="horizontal")
            mirror =1;
        else if(mirror_arg.getValue()=="both")
            mirror =-1;
        else
            mirror =100;
        
        extract_mode = extract_dir != "";
        
        color_mode = mode_arg.getValue();
        hsv_mode = color_mode == "hsv";
        
        scalex = scaley = scale_arg.getValue();
        width = width_arg.getValue();
        height = height_arg.getValue();
        lengths_string = lengths_arg.getValue();
        types_string = types_arg.getValue();
    }
    catch  (TCLAP::ArgException &e)  // catch any exceptions
    {
        std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
    }
    
    cv::VideoCapture cap;
    if(stream_file == "0") cap.open(0);
    else cap.open(stream_file);
    
    if(!cap.isOpened()) {
        return -1;
    }
    
    cap >> frame;
    
    if(width != -1) {
        scalex = width / (float) frame.cols;
        scaley = scalex;
    }
    else if(height != -1) {
        scaley = height / (float) frame.rows;
        scalex = scaley;
    }
    
    cv::namedWindow("stream", CV_WINDOW_AUTOSIZE);
    
    if(debug_mode) {
        cv::namedWindow(color_mode, CV_WINDOW_AUTOSIZE);
        cv::namedWindow("gradient", CV_WINDOW_AUTOSIZE);
        cv::namedWindow("S", CV_WINDOW_AUTOSIZE);
    }
    // Get the length values from the command line
    std::vector<int> lengths = split(lengths_string, ',');
    
    // Get the signs to be considered from the command line
    std::vector<int> types = split(types_string, ',');
    for(std::vector<int>::const_iterator it = types.begin(); it!=types.end(); it++)
        switch(*it) {
            case 0: circle_mode = !circle_mode; break;
            case 3: tri_mode = !tri_mode; break;
            case 4: square_mode = !square_mode; break;
            case 8: octagon_mode = !octagon_mode; break;
        }
    // Generate temporary filename strand
    strand = tempName(5);
    
    while(!frame.empty()) {
        cv::Mat resized, space, hsv;
        
        cv::resize(frame, resized, cv::Size(), scalex, scaley, CV_INTER_LINEAR);
        if(mirror==0 | mirror == -1 | mirror == 1) cv::flip(resized, resized, mirror);
        int r=resized.rows, c=resized.cols;
        
        if(!hsv_mode) {
            cv::cvtColor(resized, space, CV_BGR2GRAY);
        }
        else  {
            cv::cvtColor(resized, hsv, CV_BGR2HSV);
            HSVtoHUE(hsv, space);
        }
        
        cv::normalize(space, space, 0, 255, cv::NORM_MINMAX);
        ShapeFinder sf(space);
        
        // TODO: Change this to a configured parameter
        sf.prepare(GRAD_THRESHOLD);
        
        std::vector<Shape*> c_shapes;
        std::vector<Shape*> t_shapes;
        std::vector<Shape*> s_shapes;
        std::vector<Shape*> o_shapes;
        
        if(circle_mode)     c_shapes = sf.findShape(0,lengths);
        if(tri_mode)        t_shapes = sf.findShape(3,lengths);
        if(square_mode)     s_shapes = sf.findShape(4,lengths);
        if(octagon_mode)    o_shapes = sf.findShape(8,lengths);
        
        if(extract_mode) {
            captureImages(c_shapes, resized);
            captureImages(t_shapes, resized);
            captureImages(s_shapes, resized);
            captureImages(o_shapes, resized);
        }
        
        drawShapes(c_shapes, resized);
        drawShapes(t_shapes, resized);
        drawShapes(s_shapes, resized);
        drawShapes(o_shapes, resized);
        
        
        cv::imshow("stream", resized);
        
        if(debug_mode) {
            cv::imshow(color_mode, space);
            cv::imshow("gradient", sf.getScalarGradient());
            cv::imshow("S", sf.getS());
        }
        
        int k = cv::waitKey(1);
        switch(k) {
            case '3': tri_mode = !tri_mode; break;
            case '4': square_mode = !square_mode; break;
            case '8': octagon_mode = !octagon_mode; break;
            case '0': circle_mode = !circle_mode; break;
                //            case '+': radius++; maxV=0; rs.pop_back(); rs.push_back(radius); break;
                //            case '-': radius--; maxV=0; rs.pop_back(); rs.push_back(radius); break;
            case 'q': return 0;
            case ' ': cv::waitKey(); break;
            case 'm': hsv_mode = !hsv_mode; break;
        }
        
        cap >> frame;
        frame_nr++;
    }
    
    return 0;
}
