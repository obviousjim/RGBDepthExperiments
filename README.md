# RGBD Depth Library
Library for texturing depth images with simultaneously shot high resolution digital video from an external camera

#Dependencies
ofxRGBDepth (https://github.com/obviousjim/ofxRGBDepth) 

ofxTimeline (https://github.com/Flightphase/ofxTimeline)

ofxRange (https://github.com/Flightphase/ofxRange)

ofxTween (https://github.com/Flightphase/ofxTween)

ofxMSAInteractiveObject (https://github.com/Flightphase/ofxMSAInteractiveObject)

ofxSimpleGuiToo (https://github.com/Flightphase/ofxSimpleGuiToo)

ofxGameCamera (https://github.com/Flightphase/ofxGameCamera)

ofxTextInputField (https://github.com/Flightphase/ofxTextInputField)


# RGBDCaptureOpenNI / RGBDCaptureKinect
**Currently only supports OpenNI**
App for recording data from the sensor to file.  Allows for capturing checkerboard calibration images from the IR camera, manages multiple takes, and supplies a timeline for preview and playback. Also utilizes a simple form of PNG compression for depth images used throughout the library.

# RGBDVideoFrameExtract
A simple utility to extract PNG still frames out of a folder of RGB videos containing checkerboard for use in the calibration app.

# RGBDCalibrate
Combines a series of checkerboard calibration image pairs from SLR and Depth Sensor IR image.  The app visualizes the calibration data allowing you to remove problematic pairs of images, finally outputting a folder of matrices used to describe the intrinsics of each lens and the extrinsic relationship between them

# RGBDPostAlign
A utility to temporally match RGB video to it's corresponding Depth sequence by pegging matching frames. ie the clap finder!

# RGBDVisualize
The final app in the series visualizes the data pairings in 3d space.  The app supports visualizing as mesh, wireframe, or pointcloud, and supports output to image sequence rendering and composition saving.