#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	
	ofSetFrameRate(60);
	ofSetVerticalSync(true);
	ofEnableAlphaBlending();
	ofBackground(0);
	
	if(projectsettings.loadFile("projectsettings.xml")){
		if(!loadAlignmentMatrices(projectsettings.getValue("alignmentDir", ""))){
			loadNewProject();
		}
		
		if(!loadVideoFile(projectsettings.getValue("videoFile", ""))){
			loadNewProject();
		}
		
		if(!loadDepthSequence(projectsettings.getValue("depthSequence", ""))){
			loadNewProject();
		}
		allLoaded = true;
	}
	else {
		loadNewProject();
	}
	
	renderer.setRGBTexture(player);
		
	cam.speed = 40;
	cam.autosavePosition = true;
	cam.usemouse = true;
	cam.useArrowKeys = false;
	cam.loadCameraPosition();
	cam.setFarClip(30000);
	
	depthPixelDecodeBuffer = depthSequence.currentDepthRaw;
	//depthPixelDecodeBuffer = new unsigned short[640*480];
	
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
	
	player.play();

}

bool testApp::loadNewProject(){
	allLoaded = false;

	//Alignment matrices
	ofFileDialogResult r;
	ofSystemAlertDialog("Select Alignment Directory");
	r = ofSystemLoadDialog("Alignment Directory", true);
	if(!r.bSuccess || !loadAlignmentMatrices(r.getPath())){
		return false;
	}
	
	//Video file
	ofSystemAlertDialog("Select Video File");
	r = ofSystemLoadDialog("Video File", false);
	if(!r.bSuccess || !loadVideoFile(r.getPath())){
		return false;
	}
	
	//depth sequence
	ofSystemAlertDialog("Select Depth Directory");
	r = ofSystemLoadDialog("Depth Directory", true);
	if(!r.bSuccess || !loadDepthSequence(r.getPath())){
		return false;
	}
	
	allLoaded = true;
}

bool testApp::loadDepthSequence(string path){
	projectsettings.setValue("depthSequence", path);
	projectsettings.saveFile();
	
	depthSequence.setup();
	depthSequence.disable();
	return depthSequence.loadSequence(path);
}

bool testApp::loadVideoFile(string path){
	projectsettings.setValue("videoFile", path);
	projectsettings.saveFile();

	videoPath = path;
	//qtRenderer.loadMovie(videoPath, OFXQTVIDEOPLAYER_MODE_TEXTURE_ONLY);
	player.loadMovie(videoPath);
	//qtRenderer.loadMovie(videoPath);
	return sequencer.loadPairingFile(ofFilePath::removeExt(path) + "_pairings.xml");
}

bool testApp::loadAlignmentMatrices(string path){
	projectsettings.setValue("alignmentDir", path);
	projectsettings.saveFile();
	
	return renderer.setup(path);
}

//--------------------------------------------------------------
void testApp::update(){
	if(!allLoaded) return;
	
	player.update();
	
	if(player.isFrameNew()){
		//cout << "new frame?" << endl;
	//qtRenderer.update();
	//if(qtRenderer.isFrameNew()){
		
//		cout << "current frame number " << player.getCurrentFrame() << endl;
		long depthFrame;
		if(sequencer.isSequenceTimebased()){
			long movieMillis = player.getPosition() * player.getDuration()*1000;
			depthFrame = sequencer.getDepthFrameForVideoFrame(movieMillis);
			//cout << "time based getting frame " << depthFrame << " for movie millis " << movieMillis << endl; 
			depthSequence.selectTime(depthFrame);
		}
		else{
			depthFrame = sequencer.getDepthFrameForVideoFrame(player.getCurrentFrame());
			depthSequence.selectFrame(depthFrame);
		}
		
		processDepthFrame();
		
		renderer.setDepthImage(depthPixelDecodeBuffer);
		renderer.update();
	}
}

void testApp::processDepthFrame(){
	
	for(int y = 0; y <	480; y++){
		for(int x = 0; x < 640; x++){
			int index = y*640+x;
//			if(depthPixelDecodeBuffer[index] == 0){
//				depthPixelDecodeBuffer[index] = 5000;
//			}
			//depthPixelDecodeBuffer[index] *= 1.0 - .0*(sin(y/10.0 + ofGetFrameNum()/10.0)*.5+.5); 
		}
	}
}
//--------------------------------------------------------------
void testApp::draw(){
	if(!allLoaded) return;
	
	
	//ofBackground(255*.2);
	ofBackground(255*.0);
	
	cam.begin();
	glEnable(GL_DEPTH_TEST);
//	ofScale(1, -1, 1);
	if(renderer.applyShader){
		renderer.rgbdShader.begin();
	}
	
	//qtRenderer.bind();
	player.getTextureReference().bind();
	//renderer.getMesh().drawFaces();
	renderer.drawPointCloud();
	player.getTextureReference().unbind();
	//qtRenderer.unbind();
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
		if(player.isPlaying()){
			player.stop();
		}
		else{
			player.play();
		}
		
	}
	
	if(key == '='){
		renderer.applyShader = !renderer.applyShader;
	}
	
	if(key == 'l'){
		loadNewProject();
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