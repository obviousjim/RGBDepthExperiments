#include "testApp.h"

//--------------------------------------------------------------

void testApp::setup(){

	ofEnableAlphaBlending();
	ofSetFrameRate(60);
	
    kinectStarted = false;
    fixAlign = false ;
	
//	slr.setup();
	grabber.listDevices();
	grabber.setDeviceID(7);
	grabber.initGrabber(1920, 1080);
	
	kinectCheckerPreview.setup(10,7,4);
	cameraCheckerPreview.setup(10,7,4);
	depthRGBAlignment.setup(10,7,4);
	
	grayscaleExternalCamera.allocate(10, 10, OF_IMAGE_GRAYSCALE);
	grayscaleKinectCamera.allocate(10, 10, OF_IMAGE_GRAYSCALE);
	
	kinectView = new scrDraw2D("Kinect View", grayscaleKinectCamera);
	cameraView = new scrDraw2D("Camera View", grayscaleExternalCamera);
	depthView  = new scrDraw2D("Depth View", kinect.getDepthTextureReference());
	pointcloudView = new scrPointcloudCam("Point Cloud", "PointCamera.xml");
	//pointcloudView = new scrGrabCamView3D("Point Cloud", "PointCamera.xml");
	
	mainScreen = new scrGroupTabbed("", 32);
	
	calibrate = new scrGroupGrid("Calibrate");
	calibrate->push(kinectView);
	calibrate->push(cameraView);
	calibrate->push(depthView);
	
	preview = new scrGroupGrid("Preview");
	preview->push(pointcloudView);
	
	mainScreen->push(calibrate);
	mainScreen->push(preview);
	
	ofAddListener(kinectView->evtDraw, this, &testApp::drawOnKinect);
	ofAddListener(cameraView->evtDraw, this, &testApp::drawOnCamera);
	ofAddListener(pointcloudView->evtDraw3D, this, &testApp::drawOnPoint);
	
	gui = new ofxCVgui();
	gui->init(*mainScreen);

	kinect.setRegistration(true);
	kinect.init(false);
	kinect.open();
	kinectStarted = true;
	
}

//--------------------------------------------------------------
void testApp::update(){

	bool isupdated = false;
	
	//SLR LIVE VIEW
	/*
	slr.update();
	if(slr.isFrameNew()) {

		if(mainScreen->iSelection == 0){
			grayscaleExternalCamera.setFromPixels( slr.getLivePixels() );
			grayscaleExternalCamera.setImageType(OF_IMAGE_GRAYSCALE);
			cameraCheckerPreview.setTestImage( grayscaleExternalCamera.getPixelsRef() );		
		}
		else{
			ofPixelsRef pix = slr.getLivePixels();
			colorExternalCamera.setFromPixels(pix.getPixels(), pix.getWidth(), pix.getHeight(), OF_IMAGE_COLOR);
			depthRGBAlignment.setColorImage( colorExternalCamera );
		}
		isupdated = true;		
	}
    
	if(slr.isLiveReady() && !kinectStarted){
    	kinect.setRegistration(true);
        kinect.init(false);
        kinect.open();
        kinectStarted = true;
    }
    */

	grabber.update();
	if(grabber.isFrameNew()) {
		if(mainScreen->iSelection == 0){
			grayscaleExternalCamera.setFromPixels( grabber.getPixelsRef() );
			grayscaleExternalCamera.setImageType(OF_IMAGE_GRAYSCALE);
			cameraCheckerPreview.setTestImage( grayscaleExternalCamera.getPixelsRef() );		
		}
		else{
			ofPixelsRef pix = grabber.getPixelsRef();
			colorExternalCamera.setFromPixels(pix.getPixels(), pix.getWidth(), pix.getHeight(), OF_IMAGE_COLOR);
			depthRGBAlignment.setColorImage( colorExternalCamera );
		}
		isupdated = true;		
	}
    
//	if(slr.isLiveReady() && !kinectStarted){
//    	kinect.setRegistration(true);
//        kinect.init(false);
//        kinect.open();
//        kinectStarted = true;
//    }
	
	if(fixAlign){
		depthRGBAlignment.yshift = ofMap(ofGetMouseX(), 0, ofGetHeight(), -100, 100, false);
		depthRGBAlignment.xshift = ofMap(ofGetMouseY(), 0, ofGetHeight(), -100, 100, false);	
	}
	
    if(kinectStarted){
        kinect.update();
        if(kinect.isFrameNew()){
            if(mainScreen->iSelection == 0){
                ofPixelsRef p = kinect.getPixelsRef();
    //			cout << " kinect size is " << p.getWidth() << " " << p.getHeight() << endl;
                grayscaleKinectCamera.setFromPixels( kinect.getPixels(), kinect.getWidth(), kinect.getHeight(), OF_IMAGE_COLOR );
                grayscaleKinectCamera.setImageType(OF_IMAGE_GRAYSCALE);
//                grayscaleKinectCamera.setFromPixels( kinect.getPixels(), kinect.getWidth(), kinect.getHeight(), OF_IMAGE_GRAYSCALE );
                kinectCheckerPreview.setTestImage(grayscaleKinectCamera.getPixelsRef());
            }
            else {
                depthRGBAlignment.setDepthImage( kinect.getRawDepthPixels() );
				depthRGBAlignment.updatePointCloud(kinect.getRawDepthPixels(), 640, 480);
//				pointcloudView->center = depthRGBAlignment.getMeshCenter();
//				pointcloudView->distance = depthRGBAlignment.getMeshDistance();
            }
            isupdated = true;
        }
    }

	/*
	rgbcamera.update();
	if(rgbcamera.isFrameNew()){
		depthRGBAlignment.setColorImage( rgbcamera.getPixelsRef() );
		grayCopy.setFromPixels(rgbcamera.getPixelsRef());
		grayCopy.setImageType(OF_IMAGE_GRAYSCALE);
		cameraCheckerPreview.setTestImage( grayCopy.getPixelsRef() );		
		isupdated = true;
	}
	*/	
}

//--------------------------------------------------------------
void testApp::draw(){
//	slr.update();
//	slr.draw(0, 0);

}

void testApp::drawOnKinect(ofRectangle& drawRect){
	kinectCheckerPreview.draw(drawRect);	
}

void testApp::drawOnCamera(ofRectangle& drawRect){
	cameraCheckerPreview.draw(drawRect);
}

void testApp::drawOnPoint(ofNode& drawNode){
//	depthRGBAlignment.drawPointCloud();
	depthRGBAlignment.drawMesh();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	if(key == ' '){
		depthRGBAlignment.addCalibrationImagePair(grayscaleKinectCamera.getPixelsRef(), grayscaleExternalCamera.getPixelsRef());
	}
	
	if(key == 's'){
		depthRGBAlignment.saveCalibration();
	}
	
	if(key == 'l'){
		depthRGBAlignment.loadCalibration();
	}
	
	if(key == '1'){
		pointcloudView->toggleFreeze();
		fixAlign = pointcloudView->frozen;
	}
//	if(key == 'v'){
//		rgbcamera.videoSettings();
//	}
}

void testApp::exit() {
    //slr.stopThread(true);
    kinectCheckerPreview.quit();
    cameraCheckerPreview.quit();
	kinect.close();
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