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
#include "ofxFCPMarker.h"

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
 	
	void updateRenderer(ofVideoPlayer& fromPlayer);
	void processDepthFrame();
	
	bool loadNewProject();
	bool loadDepthSequence(string path);
	bool loadVideoFile(string path);
	bool loadAlignmentMatrices(string path);
	bool loadMarkerFile(string markerPath);
	
	ofxXmlSettings compositionSettings;
	void loadCompositions();
	void newComposition();
	void saveComposition();
	

	ofVideoPlayer hiResPlayer;
	ofVideoPlayer lowResPlayer;
	ofxFCPMarker markers;
	int currentMarker;
	
	ofxRGBDRenderer renderer;
	ofxRGBDVideoDepthSequence sequencer;
	
	unsigned short* depthPixelDecodeBuffer;

	bool allLoaded;

	ofxGameCamera cam;
	
	string videoThumbsPath;
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
	
	bool startRenderMode;
	bool currentlyRendering;
	int currentRenderFrame;
};
