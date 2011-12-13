#pragma once

#include "ofMain.h"
#include "ofxDepthImageRecorder.h"
#include "ofxOpenNI.h"
#include "ofxMSAInteractiveObjectDelegate.h"
#include "ofxCvCheckerboardPreview.h"

typedef enum {
	TabCalibrate,
	TabRecord,
	TabPlayback
} RecorderTab;

typedef enum {
	RenderBW,
	RenderColor,
	RenderPointCloud
} DepthRenderMode;

class testApp : public ofBaseApp, public ofxMSAInteractiveObjectDelegate {

  public:
	
	void setup();
	void update();
	void draw();

	void keyPressed  (int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);
	void exit();
		
	void objectDidRollOver(ofxMSAInteractiveObject* object, int x, int y);
	void objectDidRollOut(ofxMSAInteractiveObject* object, int x, int y);
	void objectDidPress(ofxMSAInteractiveObject* object, int x, int y, int button);
	void objectDidRelease(ofxMSAInteractiveObject* object, int x, int y, int button);
	void objectDidMouseMove(ofxMSAInteractiveObject* object, int x, int y);
	
	
	RecorderTab currentTab;
	DepthRenderMode currentRenderMode;
	
	ofxOpenNIContext	recordContext;
	ofxDepthGenerator	recordDepth;
	ofxIRGenerator		recordImage;
	
	ofxDepthImageRecorder recorder;
	ofxCvCheckerboardPreview calibrationFinder;
	
	bool recording;
	unsigned short* frame;
	ofImage calibrationImage;

};
