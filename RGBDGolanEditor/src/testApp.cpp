#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	ofSetFrameRate(60);
	
	lastFrame = 0;
	ofxXmlSettings lastdepth;
	if(lastdepth.loadFile("settings.xml")){
		string lastDepthDir = lastdepth.getValue("depthDir", "");
		if(lastDepthDir != ""){
			loadDepthSequence(lastDepthDir);
		}
	}
	
	if(!sequence.isLoaded()){
		loadDepthSequence();
	}
	
	cam.speed = 20;
	cam.autosavePosition = true;
	cam.usemouse = true;
	cam.useArrowKeys = false;
	cam.setFarClip(30000);
	cam.loadCameraPosition();
	
	renderer.setup(ofToDataPath("calibration"));
	
}

//--------------------------------------------------------------
void testApp::loadDepthSequence(){
	ofSystemAlertDialog("Select a directory of depth images");
	ofFileDialogResult r = ofSystemLoadDialog("Load Depth", true);
	if(r.bSuccess){
		loadDepthSequence(r.getPath());
	}
}

void testApp::loadDepthSequence(string sequencePath){
	if(sequence.loadSequence(sequencePath)){
		timeline.setup();
		timeline.setDurationInFrames(sequence.videoThumbs.size());
		timeline.addElement("Depth", &sequence);
		
		ofxXmlSettings save;
		save.loadFile("settings.xml");
		save.setValue("depthDir", sequencePath);
		save.saveFile();
	}
}

//--------------------------------------------------------------
void testApp::update(){
	if(sequence.isLoaded()){
		int currentFrame = timeline.getCurrentFrame();
		if(currentFrame != lastFrame){
			//GOLAN: PROCESS DEPTH FRAME HERE
			
			//DO WHAT YOU WANT
			for(int i = 0; i < 640*480; i++){
				if (sequence.currentDepthRaw[i] > 2000){
					sequence.currentDepthRaw[i] = 0;
				}
			}
				
			renderer.setDepthImage(sequence.currentDepthRaw);
			renderer.update();
			lastFrame = currentFrame;
		}
	}
}

//--------------------------------------------------------------
void testApp::draw(){
	ofBackground(0);
	
	if(sequence.isLoaded()){
		cam.begin();
		ofSetColor(255);
		//GOLAN CHOOSE WHAT TO DRAW HERE
		
		renderer.getMesh();
//		renderer.drawPointCloud();
		renderer.drawWireFrame();
		//renderer.drawMesh();
		
		cam.end();
		
		timeline.draw();
	}
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	if(key == ' '){
		timeline.togglePlay();
	}
	
	if(key == 'l'){
		loadDepthSequence();
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
	timeline.setWidth(w);
}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}