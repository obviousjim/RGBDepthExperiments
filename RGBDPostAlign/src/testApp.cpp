#include "testApp.h"
#include "ofxTimeline.h"
#include "ofxTLDepthImageSequence.h"
#include "ofxTLVideoDepthAlignmentScrubber.h"

//--------------------------------------------------------------
void testApp::setup(){
	
	ofSetFrameRate(30);
	ofSetVerticalSync(true);
	ofEnableAlphaBlending();
	
	playerRect = ofRectangle(0,20,640,480);
	
	offset = 20+480;
	timeline = new ofxTimeline();
	timeline->setup();
	timeline->setOffset(ofVec2f(0, offset));
	
	playerElement = NULL;
	depthSequenceElement = NULL;
	
	loadVideoButton = new ofxMSAInteractiveObjectWithDelegate();
	loadVideoButton->disableAppEvents();
	loadVideoButton->setup();
	loadVideoButton->setPosAndSize(0, 0, 640, 20);
	loadVideoButton->setDelegate(this);
	
	loadDepthButton = new ofxMSAInteractiveObjectWithDelegate();
	loadDepthButton->disableAppEvents();
	loadDepthButton->setup();
	loadDepthButton->setPosAndSize(640, 0, 640, 20);
	loadDepthButton->setDelegate(this);

	savePairButton = new ofxMSAInteractiveObjectWithDelegate();
	savePairButton->disableAppEvents();
	savePairButton->setup();
	savePairButton->setPosAndSize(640*2, 0, 200, 20);
	savePairButton->setDelegate(this);

	alignmentScrubber = new ofxTLVideoDepthAlignmentScrubber();
	timeline->addElement("alignment", alignmentScrubber);
	
	timeline->setDurationInFrames(300);
	
	recalculateVideoRects();
	
	shouldRefreshButtons = false;
}

//--------------------------------------------------------------
void testApp::update(){
//	timeline->setOffset(ofVec2f(0, ofGetMouseY()));
	
	if(player.isLoaded()){
		player.update();
	}
	
	if(shouldRefreshButtons){
		refreshAlignmentPairButtons();
	}
	
	if(ofGetKeyPressed('S')){
		offset = ofGetMouseY();
		timeline->setOffset(ofVec2f(0, offset));
		recalculateVideoRects();
	}
	
	if(timeline->getIsPlaying()){
		alignmentScrubber->selectPercent(timeline->getPercentComplete());
	}
	
	loadVideoButton->setPosAndSize(0,0,playerRect.width, 20);
	loadDepthButton->setPosAndSize(playerRect.width, 0, depthRect.width, 20);
	savePairButton->setPos(loadDepthButton->x+loadDepthButton->width,0);
	for(int i = 0; i < alignmentPairButtons.size(); i++){
		alignmentPairButtons[i]->x = savePairButton->x;
	}
}

void testApp::recalculateVideoRects(){
	float frameheight = offset - 20;
	int naturalPlayerWidth = player.isLoaded() ? player.getWidth() : 640;
	int naturalPlayerHeight = player.isLoaded() ? player.getHeight() : 480;
	playerRect = ofRectangle(0, 20, frameheight * naturalPlayerWidth/naturalPlayerHeight, frameheight);
	depthRect = ofRectangle(playerRect.width, 20, frameheight * 640/480, frameheight);	
}

//--------------------------------------------------------------
void testApp::draw(){
	ofBackground(.15*255);
	if (player.isLoaded()) {
		player.draw(playerRect);
	}
	
	if(depthSequenceElement != NULL){
		depthSequenceElement->currentDepthImage.draw(depthRect);
	}
	
	//draw buttons
	ofPushStyle();
	ofSetColor(255, 0, 0);
	ofNoFill();
	
	ofRect( *loadVideoButton );
	ofRect( *loadDepthButton );
	ofRect( *savePairButton );
	
	ofDrawBitmapString( "Load Video Source", ofPoint(loadVideoButton->x+20, loadVideoButton->y+10) );
	ofDrawBitmapString( "Load Depth Source", ofPoint(loadDepthButton->x+20, loadDepthButton->y+10) );
	ofDrawBitmapString( "Save Frame Pair", ofPoint(savePairButton->x+20, savePairButton->y+10) );
	
	for(int i = 0; i < alignmentPairButtons.size(); i++){
		ofRect( *alignmentPairButtons[i] );
		int videoTime = alignmentScrubber->getPairs()[i].videoFrame;
		int depthTime = alignmentScrubber->getPairs()[i].depthFrame;
		
		ofDrawBitmapString("vid. " + ofToString(videoTime) + " dep. " + ofToString(depthTime),
						   ofPoint(alignmentPairButtons[i]->x+20, alignmentPairButtons[i]->y+9));
	}
	
	ofPopStyle();
	
	timeline->draw();
}


void testApp::objectDidRollOver(ofxMSAInteractiveObject* object, int x, int y){
}

void testApp::objectDidRollOut(ofxMSAInteractiveObject* object, int x, int y){

}

void testApp::objectDidPress(ofxMSAInteractiveObject* object, int x, int y, int button){
	
	if(object == loadVideoButton){		
		ofFileDialogResult r = ofSystemLoadDialog("Load Video File", false);
		if(r.bSuccess){
			loadVideoPath(r.getPath());
		}
	}
	else if(object == loadDepthButton){
		ofFileDialogResult r = ofSystemLoadDialog("Load Depth Sequence", true);
		if(r.bSuccess){
			loadDepthPath(r.getPath());
		}		
	}	
	else if(object == savePairButton){
		if(playerElement != NULL && depthSequenceElement != NULL){
//			int videoFrame = playerElement->getSelectedFrame();
//			int depthFrame = depthSequenceElement->getSelectedFrame();
//			alignmentScrubber->addAlignedPair(videoFrame, depthFrame);
			alignmentScrubber->registerCurrentAlignment();
			//refreshAlignmentPairButtons();
			shouldRefreshButtons = true;
		}
	}
	else {
		for(int i = 0; i < alignmentPairButtons.size(); i++){
			if(object == alignmentPairButtons[i]){
				alignmentScrubber->removeAlignmentPair(i);
				//refreshAlignmentPairButtons();
				shouldRefreshButtons = true;
				break;
			}
		}
	}
}

void testApp::objectDidRelease(ofxMSAInteractiveObject* object, int x, int y, int button){
	
}

void testApp::objectDidMouseMove(ofxMSAInteractiveObject* object, int x, int y){
	
}

void testApp::loadVideoPath(string path){
	//if(!player.loadMovie(path, OFXQTVIDEOPLAYER_MODE_PIXELS_AND_TEXTURE)){
	if(!player.loadMovie(path)){
		ofLogError("Couldn't Load video " + path);
		return;
	}
	player.play();
	player.setSpeed(0);
	
	if(playerElement != NULL){
		//timeline->removeElement(playerElement);
		delete playerElement;
	}
	
	playerElement = new ofxTLVideoPlayer();
	playerElement->setup();
	
	timeline->setDurationInFrames(player.getTotalNumFrames());
	timeline->addElement("Video", playerElement);
	playerElement->setVideoPlayer(player, ofFilePath::removeExt( path ) + "/thumbs");
	
	alignmentScrubber->videoSequence = playerElement;
	alignmentScrubber->setXMLFileName(ofFilePath::removeExt(path) + "_pairings.xml");
	alignmentScrubber->load();
	
	refreshAlignmentPairButtons();
	
	//set player rectangle
	playerRect = ofRectangle(0,20,480 * player.getWidth()/player.getHeight(), 480);
	settings.setValue("video", path);
	settings.saveFile("settings.xml");
	
	recalculateVideoRects();
	
}

void testApp::refreshAlignmentPairButtons(){
	for(int i = 0; i < alignmentPairButtons.size(); i++){
		delete alignmentPairButtons[i];
	}
	alignmentPairButtons.clear();
	
	for(int i = 0; i < alignmentScrubber->getPairs().size(); i++){
		ofxMSAInteractiveObjectWithDelegate* pairButton;
		pairButton = new ofxMSAInteractiveObjectWithDelegate();
		pairButton->setup();
		pairButton->setPosAndSize(savePairButton->x, savePairButton->y+savePairButton->height * (i+1), 
								  savePairButton->width, savePairButton->height);
		pairButton->setDelegate(this);		
		alignmentPairButtons.push_back(pairButton);
	}
	shouldRefreshButtons = false;
}

void testApp::loadDepthPath(string path){
	
	if(depthSequenceElement != NULL){
		//timeline->removeElement(depthSequenceElement);
		delete depthSequenceElement;
	}
	
	depthSequenceElement = new ofxTLDepthImageSequence();
	depthSequenceElement->setup();
	
	timeline->addElement("Depth", depthSequenceElement);
	depthSequenceElement->loadSequence(path);
	
	alignmentScrubber->depthSequence = depthSequenceElement;
	
	//set player rectangle
	depthRect = ofRectangle(playerRect.width,20,640,480);
	
	settings.setValue("depth", path);
	settings.saveFile("settings.xml");	
	
	recalculateVideoRects();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	if(key == 'l'){
		if(settings.loadFile("settings.xml")){
			loadVideoPath(settings.getValue("video", ""));
			loadDepthPath(settings.getValue("depth", ""));
		}		
	}
	
	if(key == 'T'){
		if(playerElement != NULL){
			playerElement->toggleThumbs();
		}
		if(depthSequenceElement != NULL){
			depthSequenceElement->toggleThumbs();
		}
	}
	
	if(key == ' '){
		timeline->togglePlay();
	}
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){
	timeline->setWidth(w);
}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}
