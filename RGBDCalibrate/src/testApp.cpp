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
	
	/*
	leftLoaded = false;
	rightLoaded = false;
	
	leftFrame = new ofxMSAInteractiveObjectWithDelegate();
	leftFrame->disableAppEvents();
	leftFrame->setup();
	leftFrame->setDelegate(this);
	
	rightFrame = new ofxMSAInteractiveObjectWithDelegate();
	rightFrame->disableAppEvents();
	rightFrame->setup();
	rightFrame->setDelegate(this);
	
	addPair = new ofxMSAInteractiveObjectWithDelegate();
	addPair->disableAppEvents();
	addPair->setup();
	addPair->setDelegate(this);	

	saveCalibrationButton = new ofxMSAInteractiveObjectWithDelegate();;
	saveCalibrationButton->disableAppEvents();
	saveCalibrationButton->setup();
	saveCalibrationButton->setDelegate(this);	
	
	leftSubpixelRefine = new ofxMSAInteractiveObjectWithDelegate();
	leftSubpixelRefine->disableAppEvents();
	leftSubpixelRefine->setup();
	leftSubpixelRefine->setDelegate(this);	
	
	rightSubpixelRefine = new ofxMSAInteractiveObjectWithDelegate();
	rightSubpixelRefine->disableAppEvents();
	rightSubpixelRefine->setup();
	rightSubpixelRefine->setDelegate(this);	
	*/
	
	leftBoardPreview.setup(10, 7, 2.5);
	rightBoardPreview.setup(10, 7, 2.5);
	alignment.setup(10, 7, 2.5);
	alignment.setupGui(0, topButtonHeight, ofGetWidth());
	
	ofxXmlSettings curdir;
	if(curdir.loadFile("settings.xml")){
		setWorkingDirectory(curdir.getValue("workingDirectory", ""));
	}
	
//	loadDirectories();
}

//--------------------------------------------------------------
void testApp::update(){
	
}

//--------------------------------------------------------------
void testApp::draw(){
	ofBackground(0);
	ofSetColor(255);
	alignment.drawGui();
	
	
//	if(leftLoaded){
//		leftImage.draw(*leftFrame);
//		leftBoardPreview.draw(*leftFrame);
//	}
//	
//	if(rightLoaded){
//		rightImage.draw(*rightFrame);
//		rightBoardPreview.draw(*rightFrame);
//	}
	
//	ofNoFill();
//	ofSetColor(255, 0, 0);

//	ofRect( *addPair );
//	ofRect( *leftFrame );
//	ofRect( *rightFrame );
	
//	ofRect( *leftSubpixelRefine );
//	ofRect( *rightSubpixelRefine );
	
//	ofDrawBitmapString("Save Pair", ofPoint(20,9));
//	ofDrawBitmapString("Export Calibration File", ofPoint(ofGetWidth()/2+20,9));
//	ofDrawBitmapString("Load Kinect Checkerboard Image", ofPoint(20,29));
//	ofDrawBitmapString("Load External Checkerboard Image", ofPoint(ofGetWidth()/2+20,29));
//	
//	ofDrawBitmapString("Left Subpixel Refine: " + ofToString(leftBoardPreview.getSmallestSquarePixelsize()), ofPoint(20,ofGetHeight()-9));
//	ofDrawBitmapString("Right Subpixel Refine: " + ofToString(rightBoardPreview.getSmallestSquarePixelsize()), ofPoint(ofGetWidth()/2+20,ofGetHeight()-9));
	
	
//	if(leftLoaded && rightLoaded){
//		//draw an error rectangle
//		if(leftDir.numFiles() != rightDir.numFiles()){
//			ofSetColor(255, 100, 0, 50);
//			ofRect(0,20, ofGetWidth(), ofGetHeight()-40);
//			ofSetColor(255, 0, 0);
//			ofDrawBitmapString("WARNING: Directory sizes do not match", ofVec2f(40, ofGetHeight()/2));
//		}
//		ofSetColor(255);
//		ofDrawBitmapString("Reprojection Error: " + ofToString(alignment.getKinectCalibration().getReprojectionError(), 4), 
//						   20, ofGetHeight()-40);
//		ofDrawBitmapString("Reprojection Error: " + ofToString(alignment.getExternalCalibration().getReprojectionError(), 4), 
//						   ofGetWidth()/2+20, ofGetHeight()-40);
//	}
	
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){
	
}

//void testApp::updateImages(){
//	bool success;
//	success = leftImage.loadImage(leftDir.getPath(currentIndex));
//	success = success && rightImage.loadImage(rightDir.getPath(currentIndex));
//	if(success){
//		leftImage.setImageType(OF_IMAGE_GRAYSCALE);
//		rightImage.setImageType(OF_IMAGE_GRAYSCALE);
//		leftBoardPreview.setTestImage(leftImage.getPixelsRef());
//		rightBoardPreview.setTestImage(rightImage.getPixelsRef());
//	}
//}

//--------------------------------------------------------------
//void testApp::saveDirectories(){
//	ofxXmlSettings settings;
//	settings.addValue("left", leftDir.getOriginalDirectory());
//	settings.addValue("right", rightDir.getOriginalDirectory());
//	settings.saveFile("settings.xml");
//}

//--------------------------------------------------------------

//--------------------------------------------------------------
void testApp::loadWorkingDirectory(){
	ofFileDialogResult r = ofSystemLoadDialog("Calibration Settings File", true);
	if(r.bSuccess){
		setWorkingDirectory(r.getPath());
	}
}

void testApp::setWorkingDirectory(string path){
	workingDirectory = path;
	alignment.loadState(path+"/alignment_config.xml");
	ofxXmlSettings settings;
	settings.addValue("workingDirectory", path);
	settings.saveFile("settings.xml");
	
	
}

void testApp::loadRGBDirectory(){
	ofFileDialogResult r = ofSystemLoadDialog("Load RGB Directory", true);
	if(r.bSuccess){
		alignment.clearRGBImages();
		alignment.addRGBCalibrationDirectory(r.getPath());
		
	}		
}	

void testApp::loadDepthDirectory(){
	ofFileDialogResult r = ofSystemLoadDialog("Load RGB Directory", true);
	if(r.bSuccess){
		alignment.clearDepthImages();
		alignment.addDepthCalibrationDirectory(r.getPath());		
	}		
	
}

	
//	ofxXmlSettings settings;
//	if(settings.loadFile("settings.xml")){
//		leftDir.open(settings.getValue("left", ""));
//		rightDir.open(settings.getValue("right", ""));
//		if(leftDir.exists() && rightDir.exists()){
//			
//			leftDir.allowExt("png");
//			leftDir.allowExt("jpg");
//			leftDir.listDir();
//			leftLoaded = true;
//			
//			rightDir.allowExt("png");
//			rightDir.allowExt("jpg");
//			rightDir.listDir();
//			rightLoaded = true;
//			currentIndex = 0;
//			
//			updateImages();
//		}
//	}
//}

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