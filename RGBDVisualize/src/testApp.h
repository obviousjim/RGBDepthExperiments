#pragma once

#include "ofMain.h"
#include "ofxQTKitVideoPlayer.h"
#include "ofxRGBDRenderer.h"
#include "ofxRGBDVideoDepthSequence.h"
#include "ofxDepthImageRecorder.h"
#include "ofxGameCamera.h"
#include "ofxTimeline.h"
#include "ofxTLVideoPlayer.h"
#include "ofxTLDepthImageSequence.h";

#include "ofxSimpleGuiToo.h"

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
 	
	void processDepthFrame();
	
	bool loadNewProject();
	bool loadDepthSequence(string path);
	bool loadVideoFile(string path);
	bool loadAlignmentMatrices(string path);
	
	ofVideoPlayer hiResPlayer;
	ofVideoPlayer lowResPlayer;
	
	ofxRGBDRenderer renderer;
	ofxRGBDVideoDepthSequence sequencer;
	
	unsigned short* depthPixelDecodeBuffer;

	ofImage undistortedImage;
	bool allLoaded;

	ofxGameCamera cam;
		
	string videoPath;
	ofxTimeline timeline;
	ofxTLVideoPlayer videoTimelineElement;
	ofxTLDepthImageSequence depthSequence;
	
	ofxXmlSettings projectsettings;
	
	ofRectangle fboRectangle;
	ofFbo fbo;
	ofImage savingImage;
	string saveFolder;
	int uniqueRand;
	
	bool onRenderMode;
	bool saveCurrentFrame;
};
