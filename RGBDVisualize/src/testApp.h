#pragma once

#include "ofMain.h"
#include "ofxRGBDRenderer.h"
#include "ofxRGBDVideoDepthSequence.h"
#include "ofxDepthImageRecorder.h"
#include "ofxGameCamera.h"
#include "ofxTimeline.h"
#include "ofxTLVideoPlayer.h"
#include "ofxTLVideoDepthAlignmentScrubber.h"
#include "ofxTLDepthImageSequence.h";
#include "ofxMSAInteractiveObjectDelegate.h"
#include "ofxSimpleGuiToo.h"
#include "ofxTLCameraTrack.h"

typedef struct {
	ofxMSAInteractiveObjectWithDelegate* load;
	ofxMSAInteractiveObjectWithDelegate* toggle;
	string fullCompPath;
	bool batchExport;
	bool wasRenderedInBatch;
	string name;
} Comp;

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
 	
	void updateRenderer(ofVideoPlayer& fromPlayer);
	void processDepthFrame();
	void processGeometry();
	void drawGeometry();
	
	bool loadNewProject();
	bool loadDepthSequence(string path);
	bool loadVideoFile(string hiResPath, string lowResPath); //hires can be ""
	bool loadAlignmentMatrices(string path);
	
	ofxXmlSettings projectsettings;
	ofxXmlSettings compositions;
	void loadCompositions();
	void newComposition();
	void saveComposition();
	bool loadCompositionAtIndex(int i);
	bool loadAssetsFromCompositionDirectory(string mediaPath);
	void refreshCompButtons();
	
	//MSA Object delegate
	ofxMSAInteractiveObjectWithDelegate* newCompButton;
	ofxMSAInteractiveObjectWithDelegate* saveCompButton;	
	vector<Comp*> comps;
	
	bool playerElementAdded;
	
 	void objectDidRollOver(ofxMSAInteractiveObject* object, int x, int y);
    void objectDidRollOut(ofxMSAInteractiveObject* object, int x, int y);
	void objectDidPress(ofxMSAInteractiveObject* object, int x, int y, int button);	
	void objectDidRelease(ofxMSAInteractiveObject* object, int x, int y, int button);	
	void objectDidMouseMove(ofxMSAInteractiveObject* object, int x, int y);
	
	void finishRender();
	void toggleCameraPlayback();
	
	void populateTimelineElements();
	void loadTimelineFromCurrentComp();
	
	void startCameraRecord();
	void stopCameraRecord();
	void toggleCameraRecord();
	
	int currentCompIndex;
	string currentCompositionDirectory;
	string mediaBinDirectory;
	ofVideoPlayer* hiResPlayer;
	ofVideoPlayer* lowResPlayer;
		
	bool temporalAlignmentMode;
	bool captureFramePair;
	long currentDepthFrame;
	
	bool viewComps;
	
	unsigned short* depthPixelDecodeBuffer;

	bool allLoaded;

	ofxGameCamera cam;
	ofxTLCameraTrack cameraTrack;
//	ofxCameraTrack cameraRecorder;
	bool sampleCamera;
//	bool playbackCamera;

	
//	string videoThumbsPath;
//	string videoPath;
//	string smallVideoPath;
	
	ofxRGBDRenderer renderer;
//	ofxRGBDVideoDepthSequence sequencer;
	
	ofxTimeline timeline;
	ofxTLVideoPlayer videoTimelineElement;
	ofxTLDepthImageSequence depthSequence;
	ofxTLVideoDepthAlignmentScrubber alignmentScrubber;
	
	ofRectangle fboRectangle;
	ofFbo fbo;
	ofImage savingImage;
	string saveFolder;
	

	float currentXMultiplyShift;
	float currentYMultiplyShift;
	float currentXAdditiveShift;
	float currentYAdditiveShift;
	float currentXScale;
	float currentYScale;
	float currentRotationCompensation;
	
	bool currentLockCamera;
	
	bool shouldResetDuration;
	int currentDuration;
	
	bool currentMirror;
	bool presentMode;
	
	float currentRotateMeshX;
	
	float farClip;
	float currentEdgeCull;
	bool shouldSaveCameraPoint;
	bool shouldClearCameraMoves;
	bool shouldResetCamera;
	
	bool enableVideoInOut;
	float videoInPercent;
	float videoOutPercent;
	
	bool drawPointcloud;
	bool drawWireframe;
	bool drawMesh;
	int pointSize;
	int lineSize;
	int currentSimplify;

	bool hasHiresVideo;
	
	bool startRenderMode;
	bool currentlyRendering;
	int currentRenderFrame;
	int lastRenderFrame;
	int numFramesToRender;
	int numFramesRendered;
	ofImage testImageOne;
	ofImage testImageTwo;
	
	ofVec3f lightpos;
	ofLight light;

	string pathDelim;
};
