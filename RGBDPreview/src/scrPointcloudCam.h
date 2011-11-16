/*
 *  scrPointcloudCam.h
 *  RGBDPreview
 *
 *  Created by James George on 11/15/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "ofMain.h"
#include "ofxCVgui.h"

class scrPointcloudCam : public scrBase
{
public:
	scrPointcloudCam(string caption, string cameraPositionFile);
	
	void			keyPressed(ofKeyEventArgs & args);
	//	ofNode&			getNodeReference();
	
	float			pointSize;
	ofEvent<ofNode>	evtDraw3D; ///< differs from standard event, in that it is triggered after the transforms have been applied
	
	ofVec3f center;
	float distance;
	void toggleFreeze();
	bool frozen;
	
protected:
	float rot;
	vector<ofVec3f> positions;
	vector<ofColor> colours;
	
	virtual void	drawContent();
	
    ofCamera camera;
};