#pragma once

#include "ofMain.h"
#include "ofxRGBDRenderer.h"
#include "ofxRGBDVideoDepthSequence.h"
#include "ofxDepthImageRecorder.h"
#include "ofxGameCamera.h"
#include "ofxTimeline.h"
#include "ofxTLVideoPlayer.h"
#include "ofxTLDepthImageSequence.h";
#include "ofxMSAInteractiveObjectDelegate.h"
#include "ofxSimpleGuiToo.h"
#include "ofxFCPMarker.h"
#include "ofxCameraRecorder.h"

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
 	
	void updateRenderer(ofVideoPlayer& fromPlayer);
	void processDepthFrame();
	
	bool loadNewProject();
	bool loadDepthSequence(string path);
	bool loadVideoFile(string path);
	bool loadAlignmentMatrices(string path);
	bool loadMarkerFile(string markerPath);
	
	ofxXmlSettings projectsettings;
	ofxXmlSettings compositions;
	void loadCompositions();
	void newComposition();
	void saveComposition();
	
	void refreshCompButtons();
	
	//MSA Object delegate
	ofxMSAInteractiveObjectWithDelegate* newCompButton;
	ofxMSAInteractiveObjectWithDelegate* saveCompButton;
	vector<ofxMSAInteractiveObjectWithDelegate*> compbuttons;
	vector<string> fullCompPaths;
	bool playerElementAdded;
	
 	void objectDidRollOver(ofxMSAInteractiveObject* object, int x, int y);
    void objectDidRollOut(ofxMSAInteractiveObject* object, int x, int y);
	void objectDidPress(ofxMSAInteractiveObject* object, int x, int y, int button);	
	void objectDidRelease(ofxMSAInteractiveObject* object, int x, int y, int button);	
	void objectDidMouseMove(ofxMSAInteractiveObject* object, int x, int y);
	
	string currentCompositionDirectory;
	string mediaBinDirectory;
	string currentMediaFolder;
	ofVideoPlayer* hiResPlayer;
	ofVideoPlayer* lowResPlayer;
	ofxFCPMarker markers;
	int currentMarker;
	ofxCameraRecorder cameraRecorder;
	bool sampleCamera;
	bool playbackCamera;
	
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
	
	ofRectangle fboRectangle;
	ofFbo fbo;
	ofImage savingImage;
	string saveFolder;
	
	float currentXShift;
	float currentYShift;
	
	bool drawPointcloud;
	bool drawWireframe;
	bool drawMesh;
	int pointSize;
	int lineSize;
	
	bool startRenderMode;
	bool currentlyRendering;
	int currentRenderFrame;
};
