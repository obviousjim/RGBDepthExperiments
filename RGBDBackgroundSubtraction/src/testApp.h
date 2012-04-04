#pragma once

#include "ofMain.h"
#include "ofxDepthImageCompressor.h"

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

	bool depthDirectoryLoaded;
	
	ofDirectory inputDirectory;
	ofxDepthImageCompressor compressor;
	ofShortPixels testFrameUncompressed;
	ofShortPixels testFrameUncompressedTresholded;
	ofImage testFramePNGRaw;
	ofImage testFrame8Bit;
	ofImage testFrameReconverted;
	int clip;
	int lastClip;
	

	bool exporting;
	int currentExportFrame;

	string exportDirectory;
};
