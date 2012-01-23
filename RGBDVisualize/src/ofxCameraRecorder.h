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
	ofMatrix4x4 matrix;
} CameraSample;

class ofxCameraRecorder {
  public:
	ofxCameraRecorder();
	
	vector<CameraSample> & getSamples();
	void sample(int frame);
	void writeToFile(string fileName);
	void loadFromFile(string fileName);
 	bool reset();
	void moveCameraToFrame(int frame);
	int getFirstFrame();
	int getLastFrame();
	string getXMLRep();
	void loadFromXMLRep(string rep);
	
	ofCamera* camera;
	
  protected:
	vector<CameraSample> samples;
	
	CameraSample interpolateBetween(CameraSample sample1, CameraSample sample2, int frame); 
};