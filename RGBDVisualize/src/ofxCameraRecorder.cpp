/*
 *  ofxCameraRecorder.cpp
 *  RGBDVisualize
 *
 *  Created by Jim on 1/18/12.
 *  Copyright 2012 FlightPhase. All rights reserved.
 *
 */

#include "ofxCameraRecorder.h"
#include "ofxXmlSettings.h"

ofxCameraRecorder::ofxCameraRecorder(){
	camera = NULL;
}

bool ofxCameraRecorder::reset(){
	samples.clear();
}

void ofxCameraRecorder::sample(int frame){
	if(camera == NULL){
		ofLogError("ofxCameraRecorder -- can't sample a null camera");
	}
	if(frame > getLastFrame()){	
		CameraSample c;
		c.frame = frame;
		c.position = camera->getPosition();
		c.orientation = camera->getOrientationQuat();
		samples.push_back(c);
	}
}

string ofxCameraRecorder::getXMLRep(){
	ofxXmlSettings settings;
	settings.addTag("camera");
	settings.pushTag("camera");
	for(int i = 0; i < samples.size(); i++){
		settings.addTag("sample");
		settings.pushTag("sample", i);
		
		settings.addValue("frame", samples[i].frame);
		
		settings.addValue("px", samples[i].position.x);
		settings.addValue("py", samples[i].position.y);
		settings.addValue("pz", samples[i].position.z);
		
		settings.addValue("ox", samples[i].orientation._v.x);
		settings.addValue("oy", samples[i].orientation._v.y);
		settings.addValue("oz", samples[i].orientation._v.z);
		settings.addValue("ow", samples[i].orientation._v.w);
		
		settings.popTag();
	}
	settings.popTag();
	string rep;
	settings.copyXmlToString(rep);
	return rep;
}

void ofxCameraRecorder::loadFromXMLRep(string rep){
	samples.clear();
	ofxXmlSettings settings;
	settings.loadFromBuffer(rep);
	settings.pushTag("camera");
	int numSamples = settings.getNumTags("sample");
	for(int i = 0; i < numSamples; i++){
		CameraSample c;
		settings.pushTag("sample", i);
		c.frame = settings.getValue("frame", 0);
		c.position = ofVec3f(settings.getValue("px", 0.),
							 settings.getValue("py", 0.),
							 settings.getValue("pz", 0.));		
		c.orientation.set(settings.getValue("ox", 0.),
									 settings.getValue("oy", 0.),
									 settings.getValue("oz", 0.),
									 settings.getValue("ow", 1.));
		settings.popTag();
		samples.push_back(c);
	}
	settings.popTag();	
}


void ofxCameraRecorder::writeToFile(string fileName){
	ofxXmlSettings settings;
	string xmlRep = getXMLRep();
	settings.loadFromBuffer(xmlRep);
	settings.saveFile(fileName);
}

void ofxCameraRecorder::loadFromFile(string fileName){
	ofxXmlSettings settings;
	if(settings.loadFile(fileName)){
		string rep;
		settings.copyXmlToString(rep);
		loadFromXMLRep(rep);
	}
	else{
		ofLogError("ofxCameraRecorder -- couldn't load camera file " + fileName);
	}
}

int ofxCameraRecorder::getFirstFrame(){
	return samples.size() == 0 ? 0 : samples[0].frame;
}

int ofxCameraRecorder::getLastFrame(){
	return samples.size() == 0 ? 0 : samples[samples.size()-1].frame;
}

void ofxCameraRecorder::moveCameraToFrame(int frame){
	if(samples.size() == 0){
		ofLogError("ofxCameraRecorder -- no samples to move camera to!");
		return;
	}

	if(camera == NULL){
		ofLogError("ofxCameraRecorder -- can't modify a null camera!");
		return;
	}
	
	//cout << "Sampling at frame " << frame << " with frames ranging between " << samples[0].frame << " - " << samples[samples.size()-1].frame << endl;
	
	CameraSample interp = samples[samples.size()-1];
	for(int i = 0; i < samples.size(); i++){
		if(samples[i].frame == frame){
			interp = samples[i];
		}
		
		if(samples[i].frame > frame){
			if(i != 0){
				interp = interpolateBetween(samples[i-1], samples[i], frame);
			}
			else {
				interp = samples[i];
			}
			break;
		}
	}
	
	camera->setPosition(interp.position);
	camera->setOrientation(interp.orientation);
	
//	cout << "set position to " << camera->getPosition() << endl;
}
				
CameraSample ofxCameraRecorder::interpolateBetween(CameraSample sample1, CameraSample sample2, int frame){
	CameraSample interp;
	float alpha = ofMap(frame, sample1.frame, sample2.frame, 0, 1.0, false);
	interp.frame = frame;
	interp.position = sample1.position.getInterpolated(sample2.position, alpha);
	interp.orientation.slerp(alpha, sample1.orientation, sample2.orientation);
	
	//cout << "interpolating between " << sample1.position << " and " << sample2.position << " with alpha " << alpha << endl; 
	return interp;
}

vector<CameraSample> & ofxCameraRecorder::getSamples(){
	return samples;
}
