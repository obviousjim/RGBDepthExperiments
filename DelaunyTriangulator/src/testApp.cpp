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
    cam.setScale(1,-1,1);

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

    gui.add(lightX.setup("lightX", ofxParameter<float>(),-1000, 1000));
    gui.add(lightY.setup("lightY", ofxParameter<float>(),-1000, 1000));
    gui.add(lightZ.setup("lightZ", ofxParameter<float>(),400, 2000));

    gui.add(maxFeatures.setup("max features", ofxParameter<int>(),100, 5000));
    gui.add(featureQuality.setup("feature quality", ofxParameter<float>(),.0000001, .01));
    gui.add(minDistance.setup("min distance", ofxParameter<float>(), .0, 200));
	gui.add(renderMode.setup("render", ofxParameter<bool>()));

    gui.loadFromFile("defaultSettings.xml");
    renderMode = false;
    
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
    
    if(renderMode){
//        if(!player.getVideoPlayer().isPlaying()){
//            player.getVideoPlayer().play();
//            player.getVideoPlayer().setSpeed(0);
//        }

        player.getVideoPlayer().setFrame( player.getVideoPlayer().getCurrentFrame() + 1);
        player.update();
	    createTriangulation();
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
        previewFBO.begin();
        ofClear(0,0,0,0);
        cam.begin();
        glEnable(GL_DEPTH_TEST);
        meshBuilder.draw(player.getVideoPlayer());
        glDisable(GL_DEPTH_TEST);
        cam.end();
        previewFBO.end();
        previewFBO.getTextureReference().draw(meshRect);
        
        renderFBO.begin();
        ofClear(0,0,0,0);
        
        //draw triangulated mesh
        cam.begin();
        ofPushMatrix();
        ofPushStyle();
        ofScale(1,-1, 1);
        glEnable(GL_DEPTH_TEST);
        player.getVideoPlayer().getTextureReference().bind();
        ofEnableAlphaBlending();
        /*
        ofSetColor(255, 255, 255, 255);
        glShadeModel(GL_FLAT);
        ofEnableLighting();
        ofLight l;
        l.setPosition(lightX,lightY,lightZ);
        l.setAttenuation(0,.01,0);
        l.enable();
         */

//        triangulatedMesh.draw();
//        triangulatedMesh.drawWireframe();
//        ofDisableLighting();
        
        ofSetLineWidth(1);
//s        ofBlendMode(OF_BLENDMODE_ADD);
//        triangulatedMesh.drawWireframe();
        player.getVideoPlayer().getTextureReference().unbind();

        latticeMesh.drawWireframe();
//        glBegin(GL_POINTS);
//        glColor3f(1.0,0,0);
//        for(int i = 0; i < innerPoints.size(); i++) glVertex3f(innerPoints[i].x, innerPoints[i].y, innerPoints[i].z);
//        glEnd();
        
//        //draw face normals
//        ofSetLineWidth(1);
//        for(int i = 0; i < faceNormals.size(); i++){
//            ofSetColor( (faceNormals[i].x*.5+.5) * 255,
//                        (faceNormals[i].y*.5+.5) * 255,
//                        (faceNormals[i].z*.5+.5) * 255);
//                       
//            ofLine(faceCenters[i], faceCenters[i] + faceNormals[i]*6);
//        }

        glDisable(GL_DEPTH_TEST);

        ofPopStyle();
        ofPopMatrix();
        
        cam.end();
        

        renderFBO.end();
        
        renderFBO.getTextureReference().draw(triangulatedRect);
//        cam.setPosition(cam.getPosition() + cam.getSideDir().normalized() * .5);

        if(renderMode){
            ofImage saveImage;
            renderFBO.getTextureReference().readToPixels(saveImage.getPixelsRef());
            char filename[1024];
            sprintf(filename, "videframe_%05d.png", player.getVideoPlayer().getCurrentFrame());
            saveImage.saveImage(filename);
        }
    }
    
    gui.draw();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    if(key == ' '){
        player.togglePlay();
    }
    if(key == 'f'){
    	createTriangulation();
    }        

}

//--------------------------------------------------------------
void testApp::createTriangulation(){
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
    faceNormals.clear();	
    faceCenters.clear();

	map<ofIndexType, vector<int> > vertexIndexToFaceIndex;
    for(int i = 0 ; i < triangulate.triangleMesh.getNumIndices(); i+=3){
        ofIndexType a,b,c;
        a = triangulate.triangleMesh.getIndex(i);
        if(!validVertIndeces[a]) continue;
        
        b = triangulate.triangleMesh.getIndex(i+1);
        if(!validVertIndeces[b]) continue;
        
        c = triangulate.triangleMesh.getIndex(i+2);
        if(!validVertIndeces[c]) continue;
        
        triangulatedMesh.addIndex(triangulate.triangleMesh.getIndex(i  ));
        triangulatedMesh.addIndex(triangulate.triangleMesh.getIndex(i+1));
        triangulatedMesh.addIndex(triangulate.triangleMesh.getIndex(i+2));

        //keep track of which faces belong to which vertices
    	vertexIndexToFaceIndex[a].push_back(faceNormals.size());
        vertexIndexToFaceIndex[b].push_back(faceNormals.size());
        vertexIndexToFaceIndex[c].push_back(faceNormals.size());
        
        //calculate the face normal
        ofVec3f& va = triangulatedMesh.getVertices()[a];
        ofVec3f& vb = triangulatedMesh.getVertices()[b];
        ofVec3f& vc = triangulatedMesh.getVertices()[c];
        ofVec3f faceNormal = (vb-va).getCrossed(vc-va).normalized();
        faceNormals.push_back( faceNormal );
        faceCenters.push_back( (va + vb + vc) / 3.);

    }
    
    //now go through and average the normals into the vertices
    triangulatedMesh.getNormals().resize(triangulatedMesh.getNumVertices());
    map<ofIndexType, vector<int> >::iterator it;
    for(it = vertexIndexToFaceIndex.begin(); it != vertexIndexToFaceIndex.end(); it++) {
        ofVec3f average(0,0,0);
		vector<int>& faceNormalIndices = it->second;
        for(int i = 0 ; i < faceNormalIndices.size(); i++){
            average += faceNormals[ faceNormalIndices[i] ];
        }
        average.normalize();
        triangulatedMesh.setNormal(it->first, average); 
    }
    
    //Create a lattice structure
    latticeMesh.clear();
    
    //copy the main vertices into the lattice mesh
    for(int i = 0; i < triangulatedMesh.getNumVertices(); i++){
        latticeMesh.addVertex(triangulatedMesh.getVertex(i));
        latticeMesh.addNormal(triangulatedMesh.getNormal(i));
    }
    
    innerPoints.clear();
    backInnerPoints.clear();
    backPoints.clear();

    
    
    
    //for each triangle, find the centroid and create 3 new vertices that move a fixed distane towards the center
    //then stitch them

    for(int i = 0 ; i < triangulatedMesh.getNumIndices(); i+=3){
        
        ofIndexType o1 = triangulatedMesh.getIndex(i);
        ofIndexType o2 = triangulatedMesh.getIndex(i+1);        
        ofIndexType o3 = triangulatedMesh.getIndex(i+2);
        
        ofVec3f& va = triangulatedMesh.getVertices()[o1];
        ofVec3f& vb = triangulatedMesh.getVertices()[o2];
        ofVec3f& vc = triangulatedMesh.getVertices()[o3];
        
        ofVec3f& center = faceCenters[i/3];
        ofVec3f& normal = faceNormals[i/3];
        
        ofVec3f innerA = va + (center - va).normalized() * 2;
        ofVec3f innerB = vb + (center - vb).normalized() * 2;
        ofVec3f innerC = vc + (center - vc).normalized() * 2;
        
        innerPoints.push_back(innerA);
        innerPoints.push_back(innerB);
        innerPoints.push_back(innerC);
        
    
        backPoints.push_back(va - triangulatedMesh.getNormal(o1) * 2);
        backPoints.push_back(vb - triangulatedMesh.getNormal(o2) * 2);
        backPoints.push_back(vc - triangulatedMesh.getNormal(o3) * 2);

        backInnerPoints.push_back(innerA - normal*2);
        backInnerPoints.push_back(innerB - normal*2);
        backInnerPoints.push_back(innerC - normal*2);

        //get the indices of the inner points
        ofIndexType i1 = latticeMesh.getNumVertices();
        ofIndexType i2 = i1+1;
        ofIndexType i3 = i1+2;
        
        //add the inner points to the mesh
        latticeMesh.addVertex(innerA);
        latticeMesh.addVertex(innerB);
        latticeMesh.addVertex(innerC);
        
        latticeMesh.addNormal(normal);
        latticeMesh.addNormal(normal);
        latticeMesh.addNormal(normal);
        
        //stitch the 3 quads around the inner mesh
        latticeMesh.addIndex(o1);latticeMesh.addIndex(o2);latticeMesh.addIndex(i2);
        latticeMesh.addIndex(i2);latticeMesh.addIndex(i1);latticeMesh.addIndex(o1);
        
        latticeMesh.addIndex(o2);latticeMesh.addIndex(o3);latticeMesh.addIndex(i3);
        latticeMesh.addIndex(i3);latticeMesh.addIndex(i2);latticeMesh.addIndex(o2);

        latticeMesh.addIndex(o3);latticeMesh.addIndex(o1);latticeMesh.addIndex(i1);
        latticeMesh.addIndex(i1);latticeMesh.addIndex(i3);latticeMesh.addIndex(o3);
    
        //add back vertices
        ofIndexType bo1 = latticeMesh.getNumVertices();
        ofIndexType bo2 = bo1+1;
        ofIndexType bo3 = bo1+2;
        
        latticeMesh.addVertex(innerA);
        latticeMesh.addVertex(innerB);
        latticeMesh.addVertex(innerC);

    }
    
    
    
}

//--------------------------------------------------------------
void testApp::calculateRects(){
    float rectWidth = ofGetWidth()/2;
    float rectHeight = ofGetWidth()/2 * (9./16.);
    videoRect = ofRectangle(rectWidth,0,rectWidth,rectHeight);
    meshRect = ofRectangle(0,rectHeight,rectWidth,rectHeight);
	triangulatedRect = ofRectangle(rectWidth,rectHeight,rectWidth,rectHeight);
    renderFBO.allocate(rectWidth, rectHeight, GL_RGB, 4);
    previewFBO.allocate(rectWidth, rectHeight, GL_RGB, 4);
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