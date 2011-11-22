#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	
	allLoaded = false;

	alignment.setup(10, 7, 2.5);
	
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
	cam.useArrowKeys = false;
	cam.loadCameraPosition();
	
	depthPixelDecodeBuffer = new unsigned short[640*480];
}

//--------------------------------------------------------------
void testApp::update(){
	if(!allLoaded) return;
	if(player.isPaused()){
		player.setFrame(ofMap(ofGetMouseX(), 0, ofGetWidth(), 0, player.getTotalNumFrames(), true));
	}
	player.update();
	if(player.isFrameNew()){
		int depthFrame = sequencer.getDepthFrameForVideoFrame(player.getCurrentFrame());
		depthFrame = ofClamp(depthFrame, 0, depthImages.numFiles()-1);
		//cout << "loading depth frame " << depthFrame << " for video frame " << player.getCurrentFrame() << endl;
		alignment.update( decoder.readCompressedPng(depthImages.getPath(depthFrame), depthPixelDecodeBuffer) );
	}
}

//--------------------------------------------------------------
void testApp::draw(){
	if(!allLoaded) return;
	
	ofBackground(255*.2);
	
	drawAsScanlines();

}

void testApp::drawAsTriangleMesh(){
	cam.begin();
	ofPushMatrix();
	ofScale(1, -1, 1);
	
	
	qtRenderer.draw(0, 0);
	
	qtRenderer.bind();
	
	vector<ofVec3f> & vertices = alignment.getMesh().getVertices();
	vector<ofIndexType> & indices = alignment.getMesh().getIndices();
	vector<ofVec2f> & texcoords = alignment.getMesh().getTexCoords();
	
	glEnable(GL_DEPTH_TEST);
	
	glBegin(GL_TRIANGLES);
	for(int i = 0; i < indices.size(); i+=3){
		ofVec3f& a = vertices[ indices[i+0] ];
		ofVec3f& b = vertices[ indices[i+1] ];
		ofVec3f& c = vertices[ indices[i+2] ];
		if(a.z > 5 && b.z > 5 && c.z > 5){
			ofVec2f& ta = texcoords[ indices[i+0] ];
			ofVec2f& tb = texcoords[ indices[i+1] ];
			ofVec2f& tc = texcoords[ indices[i+2] ];
			
			glTexCoord2f(ta.x, ta.y);
			glVertex3f(a.x,a.y,a.z);
			glTexCoord2f(tb.x, tb.y);
			glVertex3f(b.x,b.y,b.z);
			glTexCoord2f(tc.x, tc.y);
			glVertex3f(c.x,c.y,c.z);
		}
	}
	
	glEnd();
	glDisable(GL_DEPTH_TEST);
	
	qtRenderer.unbind();
	
	//	alignment.drawMesh();
	//	alignment.drawPointCloud();
	
	ofPopMatrix();
	
	cam.end();
}

void testApp::drawAsScanlines(){
	cam.begin();
	ofPushMatrix();
	ofScale(1, -1, 1);
	
	
	qtRenderer.draw(0, 0);
	
	qtRenderer.bind();
	
	vector<ofVec3f> & vertices = alignment.getMesh().getVertices();
	vector<ofIndexType> & indices = alignment.getMesh().getIndices();
	vector<ofVec2f> & texcoords = alignment.getMesh().getTexCoords();
	
	glEnable(GL_DEPTH_TEST);
	
	glBegin(GL_TRIANGLES);
	for(int i = 0; i < indices.size(); i+=3){
		
		ofVec3f& a = vertices[ indices[i+0] ];
		ofVec3f& b = vertices[ indices[i+1] ];
		ofVec3f& c = vertices[ indices[i+2] ];
		
		if(a.z > 5 && b.z > 5 && c.z > 5 &&
		   a.z < 1500 && b.z < 1500 && b.z < 1500){
//		if(a.z > 5 && b.z > 5 && c.z > 5 &&
//		   (int(a.y) % 10) > 5 &&
//		   (int(b.y) % 10) > 5 &&
//		   (int(c.y) % 10) > 5){
			
			ofVec2f& ta = texcoords[ indices[i+0] ];
			ofVec2f& tb = texcoords[ indices[i+1] ];
			ofVec2f& tc = texcoords[ indices[i+2] ];
			
			glTexCoord2f(ta.x, ta.y);
			glVertex3f(a.x,a.y,a.z);
			
			glTexCoord2f(tb.x, tb.y);
			glVertex3f(b.x,b.y,b.z);
			
			glTexCoord2f(tc.x, tc.y);
			glVertex3f(c.x,c.y,c.z);
		}
	}
	
	cout << ofGetMouseY()*3 << endl;
	
	glEnd();
	glDisable(GL_DEPTH_TEST);
	
	qtRenderer.unbind();
	
	ofPopMatrix();
	
	cam.end();
	
}
						

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	if(key == ' '){
		if(qtRenderer.isPlaying()){
			qtRenderer.stop();
		}
		else{
			qtRenderer.play();
		}
		
	}
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){
	if(key == OF_KEY_UP){
		alignment.yshift++;
		alignment.update();
	}
	if(key == OF_KEY_DOWN){
		alignment.yshift--;
		alignment.update();
	}
	if(key == OF_KEY_RIGHT){
		alignment.xshift++;
		alignment.update();
	}
	if(key == OF_KEY_LEFT){
		alignment.xshift--;
		alignment.update();
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
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}