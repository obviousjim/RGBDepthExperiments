#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	
	ofSetVerticalSync(true);
	ofSetFrameRate(30);
	ofEnableAlphaBlending();
	
	
	downColor = ofColor(255, 120, 0);
	idleColor = ofColor(220, 200, 200);
	hoverColor = ofColor(255*.2, 255*.2, 30*.2);
	topButtonWidth = 250;
	topButtonHeight = 20;
	
	btnLoadWorkingDirectory = new ofxMSAInteractiveObjectWithDelegate();
	btnLoadWorkingDirectory->setDelegate(this);
	btnLoadWorkingDirectory->setLabel("Load Working Directory");
	btnLoadWorkingDirectory->setDownColor(downColor);
	btnLoadWorkingDirectory->setIdleColor(idleColor);
	btnLoadWorkingDirectory->setHoverColor(hoverColor);
	btnLoadWorkingDirectory->setPosAndSize(0, 0, topButtonWidth, topButtonHeight);
	
	btnLoadDepthDiectory = new ofxMSAInteractiveObjectWithDelegate();
	btnLoadDepthDiectory->setDelegate(this);
	btnLoadDepthDiectory->setLabel("Load Depth Calibration Images");
	btnLoadDepthDiectory->setDownColor(downColor);
	btnLoadDepthDiectory->setIdleColor(idleColor);
	btnLoadDepthDiectory->setHoverColor(hoverColor);
	btnLoadDepthDiectory->setPosAndSize(topButtonWidth, 0, topButtonWidth, topButtonHeight);
	
	btnLoadRGBDirectory = new ofxMSAInteractiveObjectWithDelegate();
	btnLoadRGBDirectory->setDelegate(this);
	btnLoadRGBDirectory->setLabel("Load RGB Calibration Images");
	btnLoadRGBDirectory->setDownColor(downColor);
	btnLoadRGBDirectory->setIdleColor(idleColor);
	btnLoadRGBDirectory->setHoverColor(hoverColor);
	btnLoadRGBDirectory->setPosAndSize(topButtonWidth*2, 0, topButtonWidth, topButtonHeight);
		
	btnDiscardCurrentPair = new ofxMSAInteractiveObjectWithDelegate();
	btnDiscardCurrentPair->setDelegate(this);
	btnDiscardCurrentPair->setLabel("Discard Current Pair");
	btnDiscardCurrentPair->setDownColor(downColor);
	btnDiscardCurrentPair->setIdleColor(idleColor);
	btnDiscardCurrentPair->setHoverColor(hoverColor);
	btnDiscardCurrentPair->setPosAndSize(topButtonWidth*3, 0, topButtonWidth, topButtonHeight);
	
	btnSaveCalibration = new ofxMSAInteractiveObjectWithDelegate();
	btnSaveCalibration->setDelegate(this);
	btnSaveCalibration->setLabel("Save Calibration");
	btnSaveCalibration->setDownColor(downColor);
	btnSaveCalibration->setIdleColor(idleColor);
	btnSaveCalibration->setHoverColor(hoverColor);
	btnSaveCalibration->setPosAndSize(topButtonWidth*4, 0, topButtonWidth, topButtonHeight);
	
	leftBoardPreview.setup(10, 7, 25);
	rightBoardPreview.setup(10, 7, 25);
	alignment.setup(10, 7, 25);
	alignment.setupGui(0, topButtonHeight, ofGetWidth());
	
	ofxXmlSettings curdir;
	if(curdir.loadFile("settings.xml")){
		setWorkingDirectory(curdir.getValue("workingDirectory", ""));
	}

}

//--------------------------------------------------------------
void testApp::update(){
	if(alignment.ready()){
		if(alignment.getCurrentRGBImage().isAllocated()){
			imitate(undistortionTest, alignment.getCurrentRGBImage());
			Calibration& calib = alignment.getDepthCalibration();
			calib.undistort(toCv(alignment.getCurrentRGBImage()), toCv(undistortionTest));
			undistortionTest.update();
		}
	}
}

//--------------------------------------------------------------
void testApp::draw(){
	ofBackground(0);
	ofSetColor(255);
	alignment.drawGui();
	if(undistortionTest.isAllocated()){
		alignment.getCurrentRGBImage().draw(0, 300,320,240);
		alignment.getCurrentDepthImage().draw(320, 300, 320,240);
		undistortionTest.draw(640,300,320,240);
	}
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){
	if(key == OF_KEY_DEL){
		alignment.discardCurrentPair();
		alignment.saveState();
	}
}

//--------------------------------------------------------------
void testApp::loadWorkingDirectory(){
	ofFileDialogResult r = ofSystemLoadDialog("Calibration Settings File", true);
	if(r.bSuccess){
		setWorkingDirectory(r.getPath());
	}
}

//--------------------------------------------------------------
void testApp::setWorkingDirectory(string path){
	workingDirectory = path;
	alignment.loadState(path+"/alignment_config.xml");
	ofxXmlSettings settings;
	settings.addValue("workingDirectory", path);
	settings.saveFile("settings.xml");
}

//--------------------------------------------------------------
void testApp::loadRGBDirectory(){
	ofFileDialogResult r = ofSystemLoadDialog("Load RGB Directory", true);
	if(r.bSuccess){
		alignment.clearRGBImages();
		alignment.addRGBCalibrationDirectory(r.getPath());
		alignment.saveState();
	}		
}	

//--------------------------------------------------------------
void testApp::loadDepthDirectory(){
	ofFileDialogResult r = ofSystemLoadDialog("Load Depth Directory", true);
	if(r.bSuccess){
		alignment.clearDepthImages();
		alignment.addDepthCalibrationDirectory(r.getPath());		
		alignment.saveState();
	}
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
void testApp::objectDidRollOver(ofxMSAInteractiveObject* object, int x, int y){
}

//--------------------------------------------------------------
void testApp::objectDidRollOut(ofxMSAInteractiveObject* object, int x, int y){
	
}

//--------------------------------------------------------------
void testApp::objectDidPress(ofxMSAInteractiveObject* object, int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::objectDidRelease(ofxMSAInteractiveObject* object, int x, int y, int button){
	if(object == btnLoadWorkingDirectory){
		loadWorkingDirectory();
	}
	else if(object == btnLoadRGBDirectory){
		loadRGBDirectory();
	}
	else if(object == btnLoadDepthDiectory){
		loadDepthDirectory();
	}
	else if(object == btnDiscardCurrentPair){
		alignment.discardCurrentPair();
		alignment.saveState();
	}
	else if(object == btnSaveCalibration){
		alignment.saveAlignment(workingDirectory);
	}
}

//--------------------------------------------------------------
void testApp::objectDidMouseMove(ofxMSAInteractiveObject* object, int x, int y){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){
	alignment.setMaxDrawWidth(w);
}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}