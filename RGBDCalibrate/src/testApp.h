#pragma once

#include "ofMain.h"

#include "ofxCvCheckerboardPreview.h"
#include "ofxMSAInteractiveObjectDelegate.h"
#include "ofxRGBDAlignment.h"
#include "ofxXmlSettings.h"

class testApp : public ofBaseApp, public ofxMSAInteractiveObjectDelegate
{

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

	ofxRGBDAlignment alignment;
	ofxCvCheckerboardPreview leftBoardPreview;
	ofxCvCheckerboardPreview rightBoardPreview;
	
	ofColor downColor;
	ofColor idleColor;
	ofColor hoverColor;
	float topButtonWidth;
	float topButtonHeight;
	
	ofxMSAInteractiveObjectWithDelegate* btnLoadWorkingDirectory;
	ofxMSAInteractiveObjectWithDelegate* btnLoadRGBDirectory;
	ofxMSAInteractiveObjectWithDelegate* btnLoadDepthDiectory;
	ofxMSAInteractiveObjectWithDelegate* btnDiscardCurrentPair;
	ofxMSAInteractiveObjectWithDelegate* btnSaveCalibration;

	ofxMSAInteractiveObjectWithDelegate* btnTestRGBImage;
	ofxMSAInteractiveObjectWithDelegate* btnTestDepthImage;

	string workingDirectory;
	void loadWorkingDirectory();
	void setWorkingDirectory(string path);

	void loadRGBDirectory();
	void loadDepthDirectory();
		
	void objectDidRollOver(ofxMSAInteractiveObject* object, int x, int y);
    void objectDidRollOut(ofxMSAInteractiveObject* object, int x, int y);
	
	void objectDidPress(ofxMSAInteractiveObject* object, int x, int y, int button);
	void objectDidRelease(ofxMSAInteractiveObject* object, int x, int y, int button);
	void objectDidMouseMove(ofxMSAInteractiveObject* object, int x, int y);
	
	ofImage undistortionTest;
};
