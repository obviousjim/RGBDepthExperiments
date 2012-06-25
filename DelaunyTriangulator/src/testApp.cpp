/**
 * Example - Scene With Timeline
 * This example shows how to create a basic RGB+D scene
 * with a timeline to scrub through the video.
 *
 * It requiers ofxGameCamera https://github.com/Flightphase/ofxGameCamera 
 * and ofxTimeline https://github.com/Flightphase/ofxTimeline in addition to ofxRGBDepth
 *
 * James George 2012 
 * Released under the MIT License
 *
 * The RGBDToolkit has been developed with support from the STUDIO for Creative Inquiry and Eyebeam
 */

#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    
    ofSetFrameRate(60);
    ofSetVerticalSync(true);
    ofBackground(25);
    
    //set up the game camera
    cam.setup();
    cam.speed = 20;
    cam.autosavePosition = true;
    cam.targetNode.setPosition(ofVec3f());
    cam.targetNode.setOrientation(ofQuaternion());
    cam.targetXRot = -180;
    cam.targetYRot = 0;
    cam.rotationZ = 0;    
    

    xshift = 0;
    yshift = 0;
    maxFeatures = 500;
    featureQuality = .5;
	minDistance = 10;

    gui.setup("tests");
    gui.add(loadNew.setup("load new scene"));

    gui.add(xshift.setup("xshift", ofxParameter<float>(), -.15, .15));
    gui.add(yshift.setup("yshift", ofxParameter<float>(), -.15, .15));
    gui.add(farClip.setup("far clip", ofxParameter<float>(),1000, 4500));
    
    gui.add(maxFeatures.setup("max features", ofxParameter<int>(),100, 5000));
    gui.add(featureQuality.setup("feature quality", ofxParameter<float>(),.0000001, .01));
    gui.add(minDistance.setup("min distance", ofxParameter<float>(), .0, 200));
    

    gui.loadFromFile("defaultSettings.xml");
    
    calculateRects();
    
    //attemping to load the last scene
    loadDefaultScene();
}

//--------------------------------------------------------------
bool testApp::loadNewScene(){
    ofFileDialogResult r = ofSystemLoadDialog("Select a Scene", true);
    if(r.bSuccess){
        return loadScene(r.getPath());
    }
    return false;
}

//--------------------------------------------------------------
bool testApp::loadDefaultScene(){
    ofxXmlSettings settings;
    if(settings.loadFile("RGBDSimpleSceneDefaults.xml")){
        if(!loadScene(settings.getValue("defaultScene", ""))){
            return loadNewScene();
        }
        return true;
    }
    return loadNewScene();
}

//--------------------------------------------------------------
bool testApp::loadScene(string takeDirectory){
    if(player.setup(takeDirectory, true)){
        ofxXmlSettings settings;
        settings.loadFile("RGBDSimpleSceneDefaults.xml");
        settings.setValue("defaultScene", player.getScene().mediaFolder);
        settings.saveFile();
        
        meshBuilder.setup(player.getScene().calibrationFolder);
        
        //populate
        player.getVideoPlayer().setPosition(.5);
        player.update();
        
        meshBuilder.setXYShift(player.getXYShift());
        //this will compensate if we are using an offline video that is of a different scale
        //meshBuilder.setTextureScaleForImage(player.getVideoPlayer()); 
        //update the first mesh
        meshBuilder.updateMesh(player.getDepthPixels());

        return true;
    }
    return false;
}

//--------------------------------------------------------------
void testApp::update(){
    if(loadNew){
        loadNewScene();
    }
    
    cam.applyRotation = cam.applyTranslation = meshRect.inside(mouseX, mouseY) || triangulatedRect.inside(mouseX, mouseY);
    
    //copy any GUI changes into the mesh builder
    if(meshBuilder.shift.x != xshift || meshBuilder.shift.y != yshift || meshBuilder.farClip != farClip){
        meshBuilder.setXYShift( ofVec2f(xshift,yshift) );
		meshBuilder.farClip = farClip;
        meshBuilder.updateMesh(player.getDepthPixels());
    }
    
    //update the mesh if there is a new depth frame in the player
    player.update();
    if(player.isFrameNew()){
        meshBuilder.updateMesh(player.getDepthPixels());
    }
}

//--------------------------------------------------------------
void testApp::draw(){
    if(player.isLoaded()){
        
        //draw video player...
        player.getVideoPlayer().draw(videoRect);
        
        //...with feature overlay
        ofPushMatrix();
        ofTranslate(videoRect.x, videoRect.y);
        ofScale(videoRect.width / player.getVideoPlayer().getWidth(), 
                videoRect.height / player.getVideoPlayer().getHeight());
        
        ofPushStyle();
        ofNoFill();
        ofSetColor(255, 0, 0);
        triangulate.triangleMesh.drawWireframe();
        ofPopStyle();
        ofPopMatrix();
        
        //draw standard generated mesh
        cam.begin(meshRect);
        glEnable(GL_DEPTH_TEST);
        meshBuilder.draw(player.getVideoPlayer());
        glDisable(GL_DEPTH_TEST);
        cam.end();
        
        //draw triangulated mesh
        cam.begin(triangulatedRect);
        ofPushMatrix();
        ofScale(1,-1, 1);
        glEnable(GL_DEPTH_TEST);
        player.getVideoPlayer().getTextureReference().bind();
        ofSetLineWidth(2);
        triangulatedMesh.drawWireframe();
        player.getVideoPlayer().getTextureReference().unbind();
        glDisable(GL_DEPTH_TEST);
        ofPopMatrix();
        cam.end();
    }
    
    gui.draw();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    if(key == ' '){
        player.togglePlay();
    }
    if(key == 'f'){
		ofShortPixels& pix = player.getDepthPixels();
        
        //1 find features on the color image
        featurePoints.clear();
        ofImage img;
        img.setUseTexture(false);
        img.setFromPixels(player.getVideoPlayer().getPixelsRef());
        img.setImageType(OF_IMAGE_GRAYSCALE);
        goodFeaturesToTrack(toCv(img),
							featurePoints,
							maxFeatures,
							featureQuality,
							minDistance);
		cout << "found " << featurePoints.size() << " features" << endl;
        
        //2 triangulated the features
        triangulate.reset();
//		triangulate.setMaxPoints(featurePoints.size());
        for(int i = 0; i < featurePoints.size(); i++){
    		triangulate.addPoint(featurePoints[i].x,featurePoints[i].y, 0);   
        }
        triangulate.triangulate();
        
        //3 copy them into a 3d mesh
        triangulatedMesh.clear();
        vector<ofVec3f>& trianglePoints = triangulate.triangleMesh.getVertices();
        vector<ofVec2f>& textureCoords = meshBuilder.getMesh().getTexCoords();
        vector<bool> validVertIndeces;
    	for(int i = 0; i < trianglePoints.size(); i++){
            int closestTexCoordIndex  = 0;
            float closestTexCoordDistance = 1000000;
            for(int j = 0; j < textureCoords.size(); j++){
                ofVec2f tri2d(trianglePoints[i].x,trianglePoints[i].y);
                float texCoordDist = tri2d.distanceSquared(textureCoords[j]);
                if(texCoordDist < closestTexCoordDistance){
                	closestTexCoordDistance = texCoordDist;
                    closestTexCoordIndex = j;
                }
            }
            ofVec3f vert = meshBuilder.getMesh().getVertex(closestTexCoordIndex);
            triangulatedMesh.addVertex(vert);
            triangulatedMesh.addTexCoord(meshBuilder.getMesh().getTexCoord(closestTexCoordIndex));
            validVertIndeces.push_back(vert.z < farClip && vert.z > 10);
        }
        
        //copy indices across
        for(int i = 0 ; i < triangulate.triangleMesh.getNumIndices(); i+=3){
            ofIndexType a,b,c;
            a = triangulate.triangleMesh.getIndex(i);
            if(!validVertIndeces[a]) continue;
            
            b = triangulate.triangleMesh.getIndex(i+1);
            if(!validVertIndeces[b]) continue;

            c = triangulate.triangleMesh.getIndex(i+2);
            if(!validVertIndeces[c]) continue;

            triangulatedMesh.addIndex(triangulate.triangleMesh.getIndex(i));
            triangulatedMesh.addIndex(triangulate.triangleMesh.getIndex(i+1));
            triangulatedMesh.addIndex(triangulate.triangleMesh.getIndex(i+2));
            
        }
        
        //4 lattice the wireframe
        
        //unsigned short currentThreshold = abs(mouseX/10)+20;
//        ofxDelaunay triangulate;
//		triangulate.setMaxPoints(points.size());
//        triangulate.addPoints(points);
//        triangulate.triangulate();
//        
//        triangulatedMesh.clear();
//    
//        for(int i = 0; i < triangulate.triangles.size(); i++){
//
//            ofVec3f posA = meshBuilder.getWorldPoint(triangulate.triangles[i].points[0].x,
//                                                     triangulate.triangles[i].points[0].y,
//                                                     pix);
//            if(posA.z > farClip || posA.z == 0) continue;
//            
//            ofVec3f posB = meshBuilder.getWorldPoint(triangulate.triangles[i].points[1].x,
//                                                     triangulate.triangles[i].points[1].y, 
//                                                     pix);
//            if(posB.z > farClip || posB.z == 0) continue;
//            
//            ofVec3f posC = meshBuilder.getWorldPoint(triangulate.triangles[i].points[2].x,
//                                                     triangulate.triangles[i].points[2].y,                                                  pix);
//            
//            if(posC.z > farClip || posC.z == 0) continue;
//            
//            triangulatedMesh.addIndex(triangulatedMesh.getNumVertices());
//            triangulatedMesh.addIndex(triangulatedMesh.getNumVertices()+1);
//            triangulatedMesh.addIndex(triangulatedMesh.getNumVertices()+2);
//            
//            triangulatedMesh.addVertex(posA);
//            triangulatedMesh.addVertex(posB);
//            triangulatedMesh.addVertex(posC);
//            
//            ofVec3f norm = (posA - posC).getCrossed(posB - posC).getNormalized();
//            triangulatedMesh.addNormal(norm);
//            triangulatedMesh.addNormal(norm);
//            triangulatedMesh.addNormal(norm);
//            
//            //cout << "now has " << triangulatedMesh.getNumVertices() << endl;
//        }
    }
}

//--------------------------------------------------------------
void testApp::calculateRects(){
    float rectWidth = ofGetWidth()/2;
    float rectHeight = ofGetWidth()/2 * (9./16.);
    videoRect = ofRectangle(rectWidth,0,rectWidth,rectHeight);
    meshRect = ofRectangle(0,rectHeight,rectWidth,rectHeight);
	triangulatedRect = ofRectangle(rectWidth,rectHeight,rectWidth,rectHeight);
}

//--------------------------------------------------------------
void testApp::exit(){
    gui.saveToFile("defaultSettings.xml");
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

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}