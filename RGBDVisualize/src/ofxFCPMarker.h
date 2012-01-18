/*
 *  ofxFCPMarker.h
 *  RGBDVisualize
 *
 *  Created by Jim on 1/17/12.
 *  Copyright 2012 FlightPhase. All rights reserved.
 */

#pragma once
#include "ofMain.h"

typedef struct {
	string Name;
	string Type;
	string MarkerName;
	string Comment;
	string Start;
	string Duration;
	string Color;
	int calculatedFrame;
} FCPMarker;

class ofxFCPMarker
{
  public:
	ofxFCPMarker();
	
	void setVideoFPS(int fps);
	bool parseMarkers(string markerFile);
	void setTypeFilters(vector<string> acceptedTypes);
	bool isLoaded();
	
	vector<FCPMarker> & getMarkers();
	
  protected:
	bool loaded;
	int fpsHint;
	vector<string> acceptedTypes;
	vector<FCPMarker> markers;
	
	bool validateType(string type);
};