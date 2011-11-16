/*
 *  scrGrabCam3d.cpp
 *  RGBDPreview
 *
 *  Created by James George on 11/15/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "scrPointcloudCam.h"

scrPointcloudCam::scrPointcloudCam(string caption, string cameraPositionFile) 
:
//	_node(node),
scrBase(caption)
{
	center = ofVec3f(0,0,500);
	distance = 400;
	
	for (int i=0; i<10; i++)
	{
		positions.push_back(ofVec3f(ofRandom(-10, 10),
									ofRandom(-10, 10),
									ofRandom(-10, 10)));
		
		colours.push_back(ofColor(ofRandom(100) + 100,
								  ofRandom(100) + 100,
								  ofRandom(100) + 100));
	}	
	frozen = false;
	rot = false;
}

void scrPointcloudCam::toggleFreeze(){
	frozen = !frozen;
}

void scrPointcloudCam::drawContent()
{
	ofPushStyle();
	ofSetColor(255);
	
	ofRectangle viewport = getLiveBounds();
	ofRect(	viewport );

	vector<ofVec3f>::iterator p = positions.begin();
	vector<ofColor>::iterator c = colours.begin();
	if(!frozen){
		rot = ofMap(ofGetMouseX(), 0, ofGetWidth(), 0, 360, false);
	}
	camera.setPosition( (center + ofVec3f(distance,0,0) ).getRotated(rot, center, ofVec3f(0,1,0)) );
	camera.lookAt(center, ofVec3f(0,1,0));
				  
	camera.begin(viewport);
	for (; p != positions.end(); p++, c++)
	{
		ofSetColor(*c);
		ofSphere(p->x, p->y, p->z, 1);
	}
	
	float scale = 10.0f;
	float ticks = 5.0f;
	bool labels = false;
	bool x = true;
	bool y = true;
	bool z = true;
	
	float minor = scale / ticks;
	float major =  minor * 2.0f;
	
	
	for (int iDimension=0; iDimension<2; iDimension++)
	{
		for (float yz=-scale; yz<=scale; yz+= minor)
		{
			//major major
			if (fabs(yz) == scale || yz == 0)
				ofSetLineWidth(4);
			
			//major
			else if (yz / major == floor(yz / major) )
				ofSetLineWidth(2);
			
			//minor
			else
				ofSetLineWidth(1);
			if (iDimension==0)
				ofLine(0, yz, -scale, 0, yz, scale);
			else
				ofLine(0, -scale, yz, 0, scale, yz);
		}
	}
	ofPopStyle();

	
	ofNode fakeNode;
	ofNotifyEvent(evtDraw3D, fakeNode);
	
	camera.end();
	
	
	
	
}