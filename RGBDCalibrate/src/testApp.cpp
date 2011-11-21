#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	
	ofSetVerticalSync(true);
	ofSetFrameRate(30);
	ofEnableAlphaBlending();
	
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
	
	leftBoardPreview.setup(10, 7, 2.5);
	rightBoardPreview.setup(10, 7, 2.5);
	alignment.setup(10, 7, 2.5);

	loadDirectories();
}

//--------------------------------------------------------------
void testApp::update(){
	
	addPair->setPosAndSize(0, 0, ofGetWidth()/2, 20);
	saveCalibrationButton->setPosAndSize(ofGetWidth()/2, 0, ofGetWidth()/2, 20);
	
	leftFrame->setPosAndSize(0,20, ofGetWidth()/2, ofGetHeight()-40);
	rightFrame->setPosAndSize(ofGetWidth()/2,20, ofGetWidth()/2, ofGetHeight()-40);
	
	leftSubpixelRefine->setPosAndSize(0, ofGetHeight()-20, ofGetWidth()/2, 20);
	rightSubpixelRefine->setPosAndSize(ofGetWidth()/2, ofGetHeight()-20, ofGetWidth()/2, 20);
}

//--------------------------------------------------------------
void testApp::draw(){
	
	ofBackground(0);
	
	ofSetColor(255);
	if(leftLoaded){
		leftImage.draw(*leftFrame);
		leftBoardPreview.draw(*leftFrame);
	}
	
	if(rightLoaded){
		rightImage.draw(*rightFrame);
		rightBoardPreview.draw(*rightFrame);
	}
	
	ofNoFill();
	ofSetColor(255, 0, 0);

	ofRect( *addPair );
	ofRect( *leftFrame );
	ofRect( *rightFrame );
	
	ofRect( *leftSubpixelRefine );
	ofRect( *rightSubpixelRefine );
	
	ofDrawBitmapString("Save Pair", ofPoint(20,9));
	ofDrawBitmapString("Export Calibration File", ofPoint(ofGetWidth()/2+20,9));
	ofDrawBitmapString("Load Kinect Checkerboard Image", ofPoint(20,29));
	ofDrawBitmapString("Load External Checkerboard Image", ofPoint(ofGetWidth()/2+20,29));
	
	ofDrawBitmapString("Left Subpixel Refine: " + ofToString(leftBoardPreview.getSmallestSquarePixelsize()), ofPoint(20,ofGetHeight()-9));
	ofDrawBitmapString("Right Subpixel Refine: " + ofToString(rightBoardPreview.getSmallestSquarePixelsize()), ofPoint(ofGetWidth()/2+20,ofGetHeight()-9));
	
	
	if(leftLoaded && rightLoaded){
		//draw an error rectangle
		if(leftDir.numFiles() != rightDir.numFiles()){
			ofSetColor(255, 100, 0, 50);
			ofRect(0,20, ofGetWidth(), ofGetHeight()-40);
			ofSetColor(255, 0, 0);
			ofDrawBitmapString("WARNING: Directory sizes do not match", ofVec2f(40, ofGetHeight()/2));
		}
		ofSetColor(255);
		ofDrawBitmapString("Reprojection Error: " + ofToString(alignment.getKinectCalibration().getReprojectionError(), 4), 
						   20, ofGetHeight()-40);
		ofDrawBitmapString("Reprojection Error: " + ofToString(alignment.getExternalCalibration().getReprojectionError(), 4), 
						   ofGetWidth()/2+20, ofGetHeight()-40);
	}
	
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){
	if(leftLoaded && rightLoaded){
		
		if(key == OF_KEY_LEFT){
			currentIndex--;
			if(currentIndex < 0){
				currentIndex = leftDir.numFiles()-1;
			}
			updateImages();
		}
		else if(key == OF_KEY_RIGHT){
			currentIndex = (currentIndex + 1 ) % leftDir.numFiles();
			updateImages();
		}
	}
	
	if(key == 'c'){
		alignment.getKinectCalibration().clean();
		alignment.getExternalCalibration().clean();
	}
}

void testApp::updateImages(){
	bool success;
	success = leftImage.loadImage(leftDir.getPath(currentIndex));
	success = success && rightImage.loadImage(rightDir.getPath(currentIndex));
	if(success){
		leftImage.setImageType(OF_IMAGE_GRAYSCALE);
		rightImage.setImageType(OF_IMAGE_GRAYSCALE);
		leftBoardPreview.setTestImage(leftImage.getPixelsRef());
		rightBoardPreview.setTestImage(rightImage.getPixelsRef());
	}
}

//--------------------------------------------------------------
void testApp::saveDirectories(){
	ofxXmlSettings settings;
	settings.addValue("left", leftDir.getOriginalDirectory());
	settings.addValue("right", rightDir.getOriginalDirectory());
	settings.saveFile("settings.xml");
}

//--------------------------------------------------------------
void testApp::loadDirectories(){
	ofxXmlSettings settings;
	if(settings.loadFile("settings.xml")){
		leftDir.open(settings.getValue("left", ""));
		rightDir.open(settings.getValue("right", ""));
		if(leftDir.exists() && rightDir.exists()){
			
			leftDir.allowExt("png");
			leftDir.allowExt("jpg");
			leftDir.listDir();
			leftLoaded = true;
			
			rightDir.allowExt("png");
			rightDir.allowExt("jpg");
			rightDir.listDir();
			rightLoaded = true;
			currentIndex = 0;
			
			updateImages();
		}
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
	bool alreadyLoaded = leftLoaded && rightLoaded;
	if(object == leftFrame){
		ofFileDialogResult r = ofSystemLoadDialog("Load Kinect Directory", true);
		if(r.bSuccess){
			alreadyLoaded = false;
			leftDir.allowExt("png");
			leftDir.allowExt("jpg");
			leftDir.listDir(r.getPath());
			leftLoaded = true;
			saveDirectories();
		}
	}
	else if(object == rightFrame){
		ofFileDialogResult r = ofSystemLoadDialog("Load External Camera Directory", true);
		if(r.bSuccess){
			alreadyLoaded = false;
			rightDir.allowExt("png");
			rightDir.allowExt("jpg");
			rightDir.listDir(r.getPath());
			rightLoaded = true;
			saveDirectories();
		}
	}
	else if(object == addPair){
		if(leftLoaded && rightLoaded){
			alignment.addCalibrationImagePair(leftImage.getPixelsRef(), rightImage.getPixelsRef());
		}
	}
	else if(object == saveCalibrationButton){
		saveCalibration();
	}
	
	if(object == leftSubpixelRefine){
		leftBoardPreview.setSmallestSquarePixelsize(ofMap(x, object->x, object->x+object->width, 1, 20, true));
	}
	if(object == rightSubpixelRefine){
		rightBoardPreview.setSmallestSquarePixelsize(ofMap(x, object->x, object->x+object->width, 1, 20, true));
	}
	
	if(!alreadyLoaded && leftLoaded && rightLoaded){
		currentIndex = 0;
		updateImages();
	}
}

void testApp::saveCalibration(){
	if(alignment.ready()){
		alignment.saveCalibration();
	}
}

//--------------------------------------------------------------
void testApp::objectDidRelease(ofxMSAInteractiveObject* object, int x, int y, int button){
	
}

//--------------------------------------------------------------
void testApp::objectDidMouseMove(ofxMSAInteractiveObject* object, int x, int y){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}