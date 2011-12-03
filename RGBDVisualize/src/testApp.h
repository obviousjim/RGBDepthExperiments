#pragma once

#include "ofMain.h"
#include "ofxQTKitVideoPlayer.h"
#include "ofxRGBDAlignment.h"
#include "ofxRGBDVideoDepthSequence.h"
#include "ofxKinectPointcloudRecorder.h"
#include "ofxGameCamera.h"
#include "ofxTimeline.h"
#include "ofxTLVideoPlayer.h"

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

	void drawAsTriangleMesh();
	void drawAsScanlines();
	void drawWireframe();
	
	void processDepthFrame();
	
	ofVideoPlayer player;
	ofxQTKitVideoPlayer qtRenderer;
	
	ofxRGBDAlignment alignment;
	ofxKinectPointcloudRecorder decoder;
	ofxRGBDVideoDepthSequence sequencer;
	unsigned short* depthPixelDecodeBuffer;
	
	ofDirectory depthImages;
	bool allLoaded;

	ofxGameCamera cam;
	
	string videoPath;
	ofxTimeline timeline;
	ofxTLVideoPlayer videoTimelineElement;
};
