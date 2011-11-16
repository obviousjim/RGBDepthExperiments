#pragma once

#include "ofMain.h"
#include "ofxRGBDAlignment.h"
#include "ofxKinect.h"
#include "ofxCvCheckerboardPreview.h"

#include "ofxCVgui.h"
#include "scrGrabCamView3D.h"
#include "ofxEdsdk.h"
#include "scrPointcloudCam.h"

#include "ofxQTKitVideoGrabber.h"

using namespace ofxCv;
using namespace cv;

class testApp : public ofBaseApp{

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
	
	ofxCvCheckerboardPreview kinectCheckerPreview;
	ofxCvCheckerboardPreview cameraCheckerPreview;
	ofxRGBDAlignment depthRGBAlignment;		
	
	ofImage grayscaleKinectCamera;
	ofImage colorExternalCamera;
	ofImage grayscaleExternalCamera;
	
	ofxEdsdk::Camera slr;
	ofxQTKitVideoGrabber grabber;
	
	ofxKinect kinect;
	bool kinectStarted;
    
	ofxCVgui* gui;
	scrGroupTabbed* mainScreen;
	scrGroupGrid* calibrate;
	scrGroupGrid* preview;
	scrDraw2D* kinectView;
	scrDraw2D* depthView;
	scrDraw2D* cameraView;
	//scrGrabCamView3D* pointcloudView;
	scrPointcloudCam* pointcloudView;
	
	ofNode pointcloudNode;
	
	bool fixAlign;
	
	void drawOnKinect(ofRectangle& drawRect);
	void drawOnCamera(ofRectangle& drawRect);
	void drawOnPoint(ofNode& drawNode);
	
	
	
};
