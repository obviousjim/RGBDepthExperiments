/*
 *  ofxFCPMarker.h
 *  RGBDVisualize
 *
 *  Created by Jim on 1/17/12.
 *  Copyright 2012 FlightPhase. All rights reserved.
 *
 */
#include "ofxFCPMarker.h"

ofxFCPMarker::ofxFCPMarker(){
	loaded = false;
	fpsHint = 30;
}

void ofxFCPMarker::setVideoFPS(int fps){
	fpsHint = fps;
}

bool ofxFCPMarker::parseMarkers(string markerFile){
	ofFile f = ofFile(markerFile);
	if(!f.exists()){
		ofLogError("ofxFCPMarker -- File " + markerFile + " doesn't exist!");
		return false;
	}
	
	ofBuffer buf(f);
	string firstLine = buf.getNextLine();
	//cout << "first line is " << endl;
	
	while (!buf.isLastLine()) {
		string line = buf.getNextLine();
		vector<string> splitLine = ofSplitString(line, "	", false, true);
		if(splitLine.size() == 7){
			FCPMarker m;
			m.Name = splitLine[0];
			m.Type = splitLine[1];
			if(!validateType(m.Type)){
				continue;
			}
			m.MarkerName = splitLine[2];
			m.Comment = splitLine[3];
			m.Start = splitLine[4];
			m.Duration = splitLine[5];
			m.Color = splitLine[6];
			
			//start looks like timecode eg: 01:04:52:01
			vector<string> timecode = ofSplitString(m.Start, ":", false, true);
			
			m.calculatedFrame = (ofToInt(timecode[0])-1) * 60 * 60 * fpsHint + 
								  ofToInt(timecode[1]) * 60 * fpsHint + 
								  ofToInt(timecode[2]) * fpsHint +
								  ofToInt(timecode[3]);
			markers.push_back(m);
			   
			//cout << "timecode of " << m.Start << " comes out to frame " << m.calculatedFrame << endl;
		}

		//cout << line << endl;
	}
	
	loaded = true;	
	return true;
}

vector<FCPMarker>& ofxFCPMarker::getMarkers(){
	return markers;
}

bool ofxFCPMarker::validateType(string type){
	
	if(acceptedTypes.size() == 0){
		return true;
	}
	
	for(int i = 0; i < acceptedTypes.size(); i++){
		if(type == acceptedTypes[i]){
			return true;
		}
	}
	return false;
}

void ofxFCPMarker::setTypeFilters(vector<string> types){
	acceptedTypes = types;
}

bool ofxFCPMarker::isLoaded(){
	return loaded;
}
