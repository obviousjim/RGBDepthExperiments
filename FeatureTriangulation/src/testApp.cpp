#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    
    ofSetFrameRate(60);
    ofSetVerticalSync(true);
    ofBackground(25);
    
    
    //set up a standard timeline with a default duration to begin with
    timeline.setup();
    timeline.getColors().loadColors("defaultColors.xml");
	timeline.setOffset(ofVec2f(0,0));
    timeline.setDurationInFrames(300);
    timeline.setPageName("Main");
    
    //set up a video timeline element so we can see the video frames
    videoTimelineElement = new ofxTLVideoPlayer();
    timeline.addElement("Video", videoTimelineElement);

    player.shouldCreateMesh = false;
    
    //set up the game camera
    cam.setup();
    cam.speed = 20;
    cam.autosavePosition = true;
    //cam.loadCameraPosition();
    cam.targetNode.setPosition(ofVec3f());
    cam.targetNode.setOrientation(ofQuaternion());
    cam.targetXRot = -180;
    cam.targetYRot = 0;
    cam.rotationZ = 0;    

    //load 
    loadDefaultScene();
}

bool testApp::loadNewScene(){
    ofFileDialogResult r = ofSystemLoadDialog("Select a Scene", true);
    if(r.bSuccess){
        return loadScene(r.getPath());
    }
    return false;
}

bool testApp::loadDefaultScene(){
    ofxXmlSettings settings;
    if(settings.loadFile("RGBDSimpleSceneDefaults.xml")){
        return loadScene(settings.getValue("defaultScene", ""));
    }
    return loadNewScene();
}

bool testApp::loadScene(string takeDirectory){
    if(player.setup(takeDirectory)){
        ofxXmlSettings settings;
        settings.loadFile("RGBDSimpleSceneDefaults.xml");
        settings.setValue("defaultScene", player.getScene().mediaFolder);
        settings.saveFile();

        videoTimelineElement->setVideoPlayer(player.getVideoPlayer(), player.getScene().videoThumbsPath);
        timeline.setDurationInFrames(player.getDurationInFrames());
    }
}

//--------------------------------------------------------------
void testApp::update(){
    player.update();
}

//--------------------------------------------------------------
void testApp::draw(){
    if(player.isLoaded()){
        cam.begin();
        
        ofEnableLighting();
        light.setPointLight();
        light.setPosition(0, 0, 0);
        light.enable();
        
        glEnable(GL_DEPTH_TEST);

        player.getRenderer().bindRenderer();
        //testMesh.drawWireframe();
        testMesh.draw();
        player.getRenderer().unbindRenderer();
        
        glDisable(GL_DEPTH_TEST);
        
        ofDisableLighting();
        cam.end();
    }

    timeline.draw();
}

void testApp::retriangulate(){

	testMesh.clear();
    ofxDelaunay triangulate;
    
    player.getRenderer().undistortImages();
    ofShortPixels& pix = player.getCurrentDepthPixels();
    player.getRenderer().farClip = 1200;

    vector<ofPoint> points;
    unsigned short currentThreshold = abs(mouseX/10)+1;
    for(int y = 0; y < pix.getHeight(); y++){
        unsigned short currentDepthDelta = 0;
        for(int x = 1; x < pix.getWidth()-1; x++){
            unsigned short curZ = pix.getPixels()[y*pix.getWidth()+x]; 
            unsigned short nextZ = pix.getPixels()[y*pix.getWidth()+x+1];
            currentDepthDelta += abs(curZ - nextZ);
            if(nextZ == 0 || currentDepthDelta > currentThreshold){
                currentDepthDelta %= currentThreshold;
                points.push_back(ofPoint(x,y));
            }
        }
    }
    
    triangulate.setMaxPoints(points.size());
    for(int i = 0; i < points.size(); i++) triangulate.addPoint(points[i]);
    triangulate.triangulate();
    
    for(int i = 0; i < triangulate.triangles.size(); i++){
        
        ofVec3f posA = player.getRenderer().getWoldPoint(triangulate.triangles[i].points[0].x,
                                                         triangulate.triangles[i].points[0].y);
        if(posA.z == 0) continue;
        
        ofVec3f posB = player.getRenderer().getWoldPoint(triangulate.triangles[i].points[1].x,
                                                         triangulate.triangles[i].points[1].y);
        if(posB.z == 0) continue;
        
        ofVec3f posC = player.getRenderer().getWoldPoint(triangulate.triangles[i].points[2].x,
                                                         triangulate.triangles[i].points[2].y);
        if(posC.z == 0) continue;
        
        testMesh.addIndex(testMesh.getNumVertices());
        testMesh.addIndex(testMesh.getNumVertices()+1);
        testMesh.addIndex(testMesh.getNumVertices()+2);
        
        testMesh.addVertex(posA);
        testMesh.addVertex(posB);
        testMesh.addVertex(posC);
        
        ofVec3f norm = (posA - posC).getCrossed(posB - posC).getNormalized();
        testMesh.addNormal(norm);
        testMesh.addNormal(norm);
        testMesh.addNormal(norm);
        
    }
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    if(key == ' '){
        player.togglePlay();
    }
    
    if(key == 'r'){
        retriangulate();
    }
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y){

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