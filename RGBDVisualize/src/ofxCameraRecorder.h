/*
 *  ofxCameraRecorder.h
 *  RGBDVisualize
 *
 *  Created by Jim on 1/18/12.
 *  Copyright 2012 FlightPhase. All rights reserved.
 *
 */

#pragma once
#include "ofMain.h"

typedef struct{
	int frame;
	ofVec3f position;
	ofQuaternion orientation;
} CameraSample;

class ofxCameraRecorder {
  public:
	ofxCameraRecorder();
	void sample(int frame);
	void writeToFile(string fileName);
	void loadFromFile(string fileName);
 	bool reset();
	void moveCameraToFrame(int frame);
	int getFirstFrame();
	int getLastFrame();
	
	ofCamera* camera;
	
  protected:
	vector<CameraSample> samples;
	
	CameraSample interpolateBetween(CameraSample sample1, CameraSample sample2, int frame); 
};