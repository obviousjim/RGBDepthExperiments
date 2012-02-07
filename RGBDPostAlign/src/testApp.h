#pragma once

#include "ofMain.h"

#include "ofxXmlSettings.h"
#include "ofxMSAInteractiveObjectDelegate.h"


class ofxTLVideoPlayer;
class ofxTLDepthImageSequence;
class ofxTLVideoDepthAlignmentScrubber;
class ofxTimeline;

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
	
	ofxTimeline* timeline;
	ofxTLVideoPlayer* playerElement;
	ofxTLDepthImageSequence* depthSequenceElement;
	ofxTLVideoDepthAlignmentScrubber* alignmentScrubber;
	
	ofVideoPlayer player;
	ofRectangle playerRect;
	ofRectangle depthRect;
	
	ofxMSAInteractiveObjectWithDelegate* loadVideoButton;
	ofxMSAInteractiveObjectWithDelegate* loadDepthButton;
	ofxMSAInteractiveObjectWithDelegate* savePairButton;
	vector<ofxMSAInteractiveObjectWithDelegate*> alignmentPairButtons;
	void refreshAlignmentPairButtons();
	void recalculateVideoRects();
	
	ofxXmlSettings settings;
	
	int offset;
	void loadVideoPath(string path);
	void loadDepthPath(string path);
	
	void objectDidRollOver(ofxMSAInteractiveObject* object, int x, int y);
    void objectDidRollOut(ofxMSAInteractiveObject* object, int x, int y);
	
	void objectDidPress(ofxMSAInteractiveObject* object, int x, int y, int button);
	void objectDidRelease(ofxMSAInteractiveObject* object, int x, int y, int button);
	void objectDidMouseMove(ofxMSAInteractiveObject* object, int x, int y);

	bool shouldRefreshButtons;
};
