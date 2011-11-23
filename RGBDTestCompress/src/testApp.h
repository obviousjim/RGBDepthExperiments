#pragma once

#include "ofMain.h"
#include "ofxKinectPointcloudRecorder.h"
#include "ofxRGBDVideoDepthSequence.h"

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
	
	ofxKinectPointcloudRecorder decoder;
	bool pairingFileLoaded;
	ofxRGBDVideoDepthSequence oldPairing;
	ofxRGBDVideoDepthSequence newPairing;
	string pairingFileName;
	
	unsigned short* rawPix;
	unsigned char* pngPixs;
	unsigned char* reloadedPix;
	unsigned short* decompressedPix;
	ofImage preview;
	
	ofImage compressedPreview;
	ofImage recomposedPreview;
	ofImage reloaded;
	int totalDif;
	
};
