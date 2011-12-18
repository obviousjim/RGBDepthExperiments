#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	
	ofSetFrameRate(60);
	ofSetVerticalSync(true);
	
	allLoaded = false;

	//renderer.setup(10, 7, 2.5);
	player.setPlayer(ofPtr<ofBaseVideoPlayer>(&qtRenderer));
	ofFileDialogResult r;
	ofSystemAlertDialog("Select Calibration Directory");
	
	r = ofSystemLoadDialog("Calibration Directory", true);
	if(r.bSuccess){
		//renderer.loadCalibration(r.getPath());
		renderer.setup(r.getPath());

	}
	else{
		return;
	}

	ofSystemAlertDialog("Select Video File");
	r = ofSystemLoadDialog("Video File", false);
	if(r.bSuccess){
		videoPath = r.getPath();
		qtRenderer.loadMovie(videoPath, OFXQTVIDEOPLAYER_MODE_TEXTURE_ONLY);
		//qtRenderer.loadMovie(videoPath);
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
	
		
	allLoaded = true;
	
	
	//renderer.setColorTexture(player);
	renderer.setRGBTexture(player);
	
	player.play();
	
	cam.speed = 10;
	cam.autosavePosition = true;
	cam.usemouse = true;
	cam.useArrowKeys = false;
	cam.loadCameraPosition();
	
	depthPixelDecodeBuffer = new unsigned short[640*480];
	
	
	string videoThumbsPath = ofFilePath::removeExt(videoPath);
	if(!ofDirectory(videoThumbsPath).exists()){
		ofDirectory(videoThumbsPath).create(true);
	}

	timeline.setup();
	videoTimelineElement.setup();
	
	timeline.setDurationInFrames(player.getTotalNumFrames());
	timeline.addElement("Video", &videoTimelineElement);
	
	videoTimelineElement.toggleThumbs();
	videoTimelineElement.setVideoPlayer(player, videoThumbsPath);
}

//--------------------------------------------------------------
void testApp::update(){
	if(!allLoaded) return;
	
	//player.update();
	qtRenderer.update();
	if(qtRenderer.isFrameNew()){
		
//		cout << "current frame number " << player.getCurrentFrame() << endl;
		
		int depthFrame = sequencer.getDepthFrameForVideoFrame(player.getCurrentFrame());
		depthFrame = ofClamp(depthFrame, 0, depthImages.numFiles()-1);
		//cout << "loading depth frame " << depthFrame << " for video frame " << player.getCurrentFrame() << endl;
		decoder.readCompressedPng(depthImages.getPath(depthFrame), depthPixelDecodeBuffer);
		//processDepthFrame();
		renderer.setDepthImage(depthPixelDecodeBuffer);
		renderer.update();
	}
}

void testApp::processDepthFrame(){
	
	for(int y = 0; y <	480; y++){
		for(int x = 0; x < 640; x++){
			int index = y*640+x;
			if(depthPixelDecodeBuffer[index] == 0){
				depthPixelDecodeBuffer[index] = 5000;
			}
			depthPixelDecodeBuffer[index] *= 1.0 - .0*(sin(y/10.0 + ofGetFrameNum()/10.0)*.5+.5); 
		}
	}
}
//--------------------------------------------------------------
void testApp::draw(){
	if(!allLoaded) return;
	
	
	ofBackground(255*.2);
	
	cam.begin();
	glEnable(GL_DEPTH_TEST);
	ofScale(1, -1, 1);
	if(renderer.applyShader){
		renderer.rgbdShader.begin();
	}
	
	qtRenderer.bind();
	renderer.getMesh().drawFaces();
	qtRenderer.unbind();
	if(renderer.applyShader){
		renderer.rgbdShader.end();
	}
	
	glDisable(GL_DEPTH_TEST);
	cam.end();
	
	timeline.draw();

	ofSetColor(255);
	ofDrawBitmapString(ofToString(ofGetFrameRate()), ofPoint(20,20));
}

void testApp::drawWireframe(){
	
	cam.begin();
	
	ofPushMatrix();
	ofScale(1, -1, 1);
	glEnable(GL_DEPTH_TEST);

	qtRenderer.bind();
	renderer.getMesh().drawWireframe();
	qtRenderer.unbind();
	
	glDisable(GL_DEPTH_TEST);
	ofPopMatrix();
	
	cam.end();
	
}

void testApp::drawAsTriangleMesh(){
	cam.begin();
	ofPushMatrix();
	ofScale(1, -1, 1);
	
	
//	qtRenderer.draw(0, 0);
	qtRenderer.bind();
//	player.getTextureReference().bind();
	
	vector<ofVec3f> & vertices = renderer.getMesh().getVertices();
	vector<ofIndexType> & indices = renderer.getMesh().getIndices();
	vector<ofVec2f> & texcoords = renderer.getMesh().getTexCoords();
	
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
	//player.getTextureReference().unbind();
	
	//	renderer.drawMesh();
	//	renderer.drawPointCloud();
	
	ofPopMatrix();
	
	cam.end();
}

void testApp::drawAsScanlines(){
	cam.begin();
	ofPushMatrix();
	ofScale(1, -1, 1);
	
	
	qtRenderer.draw(0, 0);
	
	qtRenderer.bind();
//	player.getTextureReference().bind();
	
	vector<ofVec3f> & vertices = renderer.getMesh().getVertices();
	vector<ofIndexType> & indices = renderer.getMesh().getIndices();
	vector<ofVec2f> & texcoords = renderer.getMesh().getTexCoords();
	
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
//	player.getTextureReference().unbind();
	
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
	
	if(key == '='){
		renderer.applyShader = !renderer.applyShader;
	}
	
	if(key == OF_KEY_UP){
		renderer.yshift++;
		renderer.update();
		cout << "shifts: " << renderer.xshift << " " << renderer.yshift << endl;
	}
	if(key == OF_KEY_DOWN){
		renderer.yshift--;
		renderer.update();
		cout << "shifts: " << renderer.xshift << " " << renderer.yshift << endl;
	}
	if(key == OF_KEY_RIGHT){
		renderer.xshift++;
		renderer.update();
		cout << "shifts: " << renderer.xshift << " " << renderer.yshift << endl;
	}
	if(key == OF_KEY_LEFT){
		renderer.xshift--;
		renderer.update();
		cout << "shifts: " << renderer.xshift << " " << renderer.yshift << endl;
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