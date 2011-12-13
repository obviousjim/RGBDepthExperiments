#include "testApp.h"

//--------------------------------------------------------------

void testApp::setup(){

	ofEnableAlphaBlending();
	ofSetFrameRate(60);
	
	
	kinect.setRegistration(false);
	kinect.init(true, false);
	kinect.open();
	
	recording = false;
	
    recorder.setRecordLocation("depthframes", "frame_");
    recorder.setup();
	
}

//--------------------------------------------------------------
void testApp::update(){
	kinect.update();
	if(kinect.isFrameNew() && recording){
		recorder.addImage( kinect.getRawDepthPixels() );
		cout << " currently " << recorder.numFramesWaitingSave() << " waiting " << endl;
	}
}

//--------------------------------------------------------------
void testApp::draw(){

	if(recording){
		ofSetColor(255, 0, 0);
		ofRect(0, 0, 640*2, 10);
	}
	
	ofSetColor(255);
	kinect.drawDepth(0, 10);
	kinect.draw(640, 10);
}


//--------------------------------------------------------------
void testApp::keyPressed(int key){
	if(key == ' '){
		recording = !recording;
        if(recording){
            ofImage posterFrame;
            posterFrame.setFromPixels(kinect.getPixels(), kinect.getWidth(), kinect.getHeight(), OF_IMAGE_GRAYSCALE);
            recorder.incrementFolder(posterFrame);
        }
	}
    
    if(key == 'c'){
        string filename = "__CalibFile_" + ofToString(ofGetDay()) + "_" + ofToString(ofGetHours()) + "_" + ofToString(ofGetMinutes()) + "_" + ofToString(ofGetSeconds()) +".png";
        ofImage kinectImage;
        kinectImage.setFromPixels(kinect.getPixels(), 640, 480, OF_IMAGE_GRAYSCALE);
        ofSaveImage( kinectImage, filename);
    }
}

void testApp::exit() {
	kinect.close();
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