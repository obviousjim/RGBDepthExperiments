#pragma once

#include "ofMain.h"
#include "ofxTimeline.h"
#include "ofxXmlSettings.h"
#include "ofxTLVideoPlayer.h"
#include "ofxGameCamera.h"
#include "ofxRGBDPlayer.h"
//#include "ofxTriangle.h"
#include "ofxDelaunay.h"

class testApp : public ofBaseApp{
  public:
    void setup();
    void update();
    void draw();
    
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y);
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
    ofxRGBDPlayer player;
    
    ofxGameCamera cam;
    ofxTimeline timeline;
    ofxTLVideoPlayer* videoTimelineElement;

    bool loadNewScene();
    bool loadDefaultScene();
    bool loadScene(string takeDirectory);
  
    void retriangulate();
    
    ofMesh testMesh;
    
    ofLight light;
    

};
