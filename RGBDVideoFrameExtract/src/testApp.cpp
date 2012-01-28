#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){

	moviesLoaded = false;
	ofSystemAlertDialog("Select directory of calibration clips");
	ofFileDialogResult r = ofSystemLoadDialog("Calibration Directory", true);
	if(r.bSuccess){
		movieDirectory = ofDirectory(r.getPath());
		movieDirectory.allowExt("mov");
		movieDirectory.allowExt("mp4");
		movieDirectory.listDir();
		for(int i = 0; i < movieDirectory.numFiles(); i++){
			ofVideoPlayer p;
			p.loadMovie( movieDirectory.getPath(i) );
			videoplayers.push_back( p );
		}
		currentMovie = 0;
		moviesLoaded = true;
	}
}

//--------------------------------------------------------------
void testApp::update(){
	if(moviesLoaded){
		videoplayers[currentMovie].setFrame(ofMap(ofGetMouseX(), 0, ofGetWidth(), 0, videoplayers[currentMovie].getTotalNumFrames(), true));
		videoplayers[currentMovie].update();
	}
}

//--------------------------------------------------------------
void testApp::draw(){
	ofBackground(0);
	if(moviesLoaded){
		videoplayers[currentMovie].draw(0,0);
		ofDrawBitmapString("Movie #"+ofToString(currentMovie+1) + " " + movieDirectory.getPath(currentMovie), 10, videoplayers[currentMovie].getHeight() + 10);
	}
	else{
		ofSetColor(255);
		ofDrawBitmapString("No Movies Loaded", 10, 10);
	}
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	if(key == OF_KEY_LEFT){
		//videoplayer.close();
		currentMovie--;
		if(currentMovie < 0){
			currentMovie = movieDirectory.numFiles()-1;
		}		
	}
	else if(key == OF_KEY_RIGHT){
		//videoplayer.close();
		currentMovie++;
		if(currentMovie == movieDirectory.numFiles()){
			currentMovie = 0;
		}
	}
	
	if(key == ' ' && moviesLoaded){
		ofImage frame;
		frame.setFromPixels(videoplayers[currentMovie].getPixelsRef());
		frame.saveImage(ofFilePath::removeExt(movieDirectory.getPath(currentMovie)) + "_calib.png");
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

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}