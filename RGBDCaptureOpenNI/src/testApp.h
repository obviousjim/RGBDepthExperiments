#pragma once

#include "ofMain.h"
#include "ofxDepthImageRecorder.h"
#include "ofxOpenNI.h"
#include "ofxMSAInteractiveObjectDelegate.h"
#include "ofxCvCheckerboardPreview.h"
#include "ofxTimeline.h"
#include "ofxTLDepthImageSequence.h"
#include "ofxGrabCam.h"
#include "ofxGameCamera.h"

typedef enum {
	TabCalibrate,
	TabRecord,
	TabPlayback
} RecorderTab;

typedef enum {
	RenderBW,
	RenderRainbow,
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
	
	void loadDirectory();
	void loadDirectory(string path);
	void loadSequenceForPlayback( int index );
	void updateTakeButtons();
	
	void toggleRecord();
	void captureCalibrationImage();

	void drawPointcloud(bool fullscreen);
	
	string workingDirectory;
	
	bool cameraFound;
	bool fullscreenPoints;
	
	ofColor downColor;
	ofColor idleColor;
	ofColor hoverColor;
	
	float framewidth;
	float frameheight;
	float thirdWidth;
	float btnheight;
	float takeWidth;
	
	ofSoundPlayer highbeeper;
	ofSoundPlayer lowbeeper;
	
	ofxMSAInteractiveObjectWithDelegate* btnSetDirectory;
	
	ofxMSAInteractiveObjectWithDelegate* btnCalibrateTab;
	ofxMSAInteractiveObjectWithDelegate* btnRecordTab;
	ofxMSAInteractiveObjectWithDelegate* btnPlaybackTab;
	
	ofxMSAInteractiveObjectWithDelegate* btnRecordBtn;
	
	ofxMSAInteractiveObjectWithDelegate* btnRenderBW;
	ofxMSAInteractiveObjectWithDelegate* btnRenderRainbow;
	ofxMSAInteractiveObjectWithDelegate* btnRenderPointCloud;
	
	ofxMSAInteractiveObjectWithDelegate* btnCaptureMode;
	
	vector<ofxMSAInteractiveObjectWithDelegate*> btnTakes;
	
	ofxCvCheckerboardPreview calibrationPreview;
	

	ofxGameCamera cam;
	
	ofxTimeline timeline;
	ofxTLDepthImageSequence depthSequence;
	
	RecorderTab currentTab;	
	DepthRenderMode currentRenderMode;
	
	ofxOpenNIContext	recordContext;
	ofxDepthGenerator	recordDepth;
	ofxIRGenerator		recordImage;

	bool inCaptureMode;
	float lastClickTime;
	int clicks;
	
	ofxDepthImageRecorder recorder;
	ofxCvCheckerboardPreview calibrationFinder;
	
	bool recording;
	unsigned short* frame;
	ofImage calibrationImage;

};
