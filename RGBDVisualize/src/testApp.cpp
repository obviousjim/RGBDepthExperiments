#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	
	allLoaded = false;

	
	ofFileDialogResult r;
	ofSystemAlertDialog("Select Calibration Directory");
	
	r = ofSystemLoadDialog("Calibration Directory", true);
	if(r.bSuccess){
		alignment.loadCalibration(r.getPath());
	}
	else{
		return;
	}

	ofSystemAlertDialog("Select Video File");
	r = ofSystemLoadDialog("Video File", false);
	if(r.bSuccess){
		qtRenderer.loadMovie(r.getPath(), OFXQTVIDEOPLAYER_MODE_TEXTURE_ONLY);
		sequencer.loadPairingFile(ofFilePath::removeExt(r.getPath()) + "_pairings.xml");
	}
	else{
		return;
	}
	
	ofSystemAlertDialog("Select Depth Directory");
	r = ofSystemLoadDialog("Depth Directory", true);
	if(r.bSuccess){
		depthImages = ofDirectory(r.getPath());
		depthImages.allowExt("png");
		//depthImages.allowExt("xkcd");
		depthImages.listDir();
	}
	else{
		return;
	}
	
	
//	ofSystemAlertDialog("Select Pairing File");
//	r = ofSystemLoadDialog("Select Pairing File", false);
//	if(r.bSuccess){
//		sequencer.loadPairingFile(r.getPath());
//	}
//	else{
//		return;
//	}
		
	allLoaded = true;
	
	player.setPlayer(ofPtr<ofBaseVideoPlayer>(&qtRenderer));
	alignment.setColorTexture(player);
	
	player.play();
	
	cam.speed = 10;
	cam.autosavePosition = true;
	cam.usemouse = true;
	cam.loadCameraPosition();
	
	depthPixelDecodeBuffer = new unsigned short[640*480];
}

//--------------------------------------------------------------
void testApp::update(){
	if(!allLoaded) return;
	
	player.update();
	if(player.isFrameNew()){
		int depthFrame = sequencer.getDepthFrameForVideoFrame(player.getCurrentFrame());
		depthFrame = ofClamp(depthFrame, 0, depthImages.numFiles()-1);
		//cout << "loading depth frame " << depthFrame << " for video frame " << player.getCurrentFrame() << endl;
		
		alignment.update( decoder.readCompressedPng(depthImages.getPath(depthFrame), depthPixelDecodeBuffer) );
		//alignment.update( decoder.readDepthFrame(depthImages.getPath(depthFrame), depthPixelDecodeBuffer)  );
	}
}

//--------------------------------------------------------------
void testApp::draw(){
	if(!allLoaded) return;
	cam.begin();
	qtRenderer.draw(0, 0);
	//alignment.drawMesh();
	alignment.drawPointCloud();
	cam.end();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

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