#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){

	ofSystemAlertDialog("Select directory of calibration clips");
	ofFileDialogResult r = ofSystemLoadDialog("Calibration Directory", true);
	if(r.bSuccess){
		movieDirectory = ofDirectory(r.getPath());
		movieDirectory.allowExt("mov");
		movieDirectory.listDir();
		
		currentMovie = 0;
		videoplayer.loadMovie(movieDirectory.getPath(currentMovie));
		videoplayer.play();
	}
		
}

//--------------------------------------------------------------
void testApp::update(){
	videoplayer.update();
}

//--------------------------------------------------------------
void testApp::draw(){
	videoplayer.draw(0,0);
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	if(key == OF_KEY_LEFT){
		videoplayer.close();
		currentMovie--;
		if(currentMovie < 0){
			currentMovie = movieDirectory.numFiles()-1;
		}		
		videoplayer.loadMovie(movieDirectory.getPath(currentMovie));
		videoplayer.play();
	}
	else if(key == OF_KEY_RIGHT){
		currentMovie++;
		if(currentMovie == movieDirectory.numFiles()){
			currentMovie = 0;
		}
		videoplayer.loadMovie(movieDirectory.getPath(currentMovie));
	}
	
	if(key == ' '){
		ofImage frame;
		frame.setFromPixels(videoplayer.getPixelsRef());
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