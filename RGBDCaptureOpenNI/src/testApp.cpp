#include "testApp.h"

//--------------------------------------------------------------

void testApp::setup(){

	ofEnableAlphaBlending();
	ofSetFrameRate(60);
	
	
	recordContext.setup();	// all nodes created by code -> NOT using the xml config file at all
	recordDepth.setup(&recordContext);
	recordImage.setup(&recordContext);
	
	recording = false;
	
    recorder.setRecordLocation("depthframes", "frame_");
    recorder.setup();
	
	currentTab = TabCalibrate;
	
}

//--------------------------------------------------------------

void testApp::update(){
	recordContext.update();
	if(currentTab == TabCalibrate){
		recordImage.update();
	}
	else if(currentTab == TabRecord){
		recordDepth.update();	
	}
	
	
	
//	if(kinect.isFrameNew() && recording){
//		recorder.addImage( kinect.getRawDepthPixels() );
//		cout << " currently " << recorder.numFramesWaitingSave() << " waiting " << endl;
//	}
	
}

void testApp::objectDidRollOver(ofxMSAInteractiveObject* object, int x, int y){
}

void testApp::objectDidRollOut(ofxMSAInteractiveObject* object, int x, int y){
}
void testApp::objectDidPress(ofxMSAInteractiveObject* object, int x, int y, int button){

}
void testApp::objectDidRelease(ofxMSAInteractiveObject* object, int x, int y, int button){

}
void testApp::objectDidMouseMove(ofxMSAInteractiveObject* object, int x, int y){

}

//--------------------------------------------------------------
void testApp::draw(){
	
	if(currentTab == TabCalibrate){
		recordImage.draw(640, 0, 640, 480);
	}
	else if(currentTab == TabRecord){
		//TODO render modes
		recordDepth.draw(0,0,640,480);
		calibrationImage.setFromPixels(recordImage.getIrPixels(), 640, 480, OF_IMAGE_GRAYSCALE);
	}
	else {
		//draw timeline
	}


	if(recording){
		ofSetColor(255, 0, 0);
		ofRect(0, 0, 640*2, 10);
	}
	
	ofSetColor(255);
}


//--------------------------------------------------------------
void testApp::keyPressed(int key){
	if(key == ' '){
		if(currentTab == TabRecord){
			recording = !recording;
//			if(recording){
//				ofImage posterFrame;
//				posterFrame.setFromPixels(kinect.getPixels(), kinect.getWidth(), kinect.getHeight(), OF_IMAGE_GRAYSCALE);
//				recorder.incrementFolder(posterFrame);
//			}
		}
		else if(currentTab == TabCalibrate){
			string filename = "__CalibFile_" + ofToString(ofGetDay()) + "_" + ofToString(ofGetHours()) + "_" + ofToString(ofGetMinutes()) + "_" + ofToString(ofGetSeconds()) +".png";
			
			ofSaveImage( calibrationImage, filename);			
		}
	}
    
    if(key == 'c'){
    }
}

void testApp::exit() {
//	kinect.close();
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

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}