#include "ofApp.h"

/*
 >>> --- Hidden Worlds --- <<<
 openframeworks app for Creative Coding class
 Carlos Valente - Goldsmiths March 2016
 
 A magic flaslight reveals hidden creatures,
 competing swarm systems float in middair invisible from
 the unaware spectator
 
 THIS IS v6 - GL this shit, from GLpoints with texture example
 .v5 - swarm extention, create wrapper class, create different rendering modes 
       (blob, lines, vertices, faces)
 .v4 - change implementation to use modified ofxBoids
    // modifications included deleting unused features and
    // transforming all calculations to be vector based
    // create extra setter and getter methods
 .v3 - create swarm creatures
 .v2 - Create spiral monster
 .v1 - create camera system from OSC
 
 to implement:
 - Swarm creature
 - tweak swarm appearence
 - Close in flashlight look
 - Map positions to screen
 - GL shader for swarm?
 
 could be nice
 - ofParameter implementation for OSC
 
 USABILITY NOTES:
 - There are some user options in the beggining of the code
 - Flashlight position comes from OSC "/light/position"
 use twin app, hiddenWorldsUtils or send x, y - 0, 1 values
 to given OSC channel;
 - For this version spectator position is assumed
 
 STUFF IM FINDING <<<
 
 - VSync locks the run speed to the framerate, 2 x framerate makes the program run twice as fast
 
 - ofMeshes are uploaded to the GPU every frame, VBOmeshes only once. Use instancing to make the best of it
 
 DEVELOPMENT <<<
 
 - Everyone complains shadows are tricky, could work on a BW image from a parallel perspective
 https://forum.openframeworks.cc/t/shadows-in-of-for-3d-space/18080
 https://en.wikipedia.org/wiki/Shadow_mapping
 https://github.com/jacres/of-ESMShadowMapping
 
 - Make a shadow advice "put your geometry in a normal vertex shader pixel shader but in the vertex shader you modify the position of vertices by some direction that moves your whole mesh into view  / schange the angle of view and that ios your shadows"
 
 http://www.rastertek.com/dx11tut40.html
 http://www.rastertek.com/tutdx11.html
 
 - triangulation, use camera with plane parallel to projection plane. with laser find brightest spot, second camera finds brightest spot in projection surface.
 http://www.muonics.net/blog/index.php?postid=26
 
 - image mapping
 https://github.com/frauzufall/ofx2DMapping
 
 
 INSPIRATION <<<
 http://www.creativeapplications.net/environment/augmented-shadow-openframeworks/
 http://joonmoon.net/Luzes-Relacionais
 http://www.ericforman.com/
 http://glslsandbox.com/e#28777.0
 http://abstract20122013.gsapp.org/binary-space-partition/
 http://zenbullets.com/images.php
 http://www.novastructura.net/wp/works/meanders/
 
 */

//--------------------------------------------------------------
void ofApp::setup(){

    /* > USER OPTIONS < */
    
    bReport      = true; // show system report
    positionEase = 0.05; // interpolation value for positions
    detailAmount = 55;   // opacity level for beauty pass
    numBoids     = 400;
    worldBound   = 1000;
    regionRadius = 1000;
    
    /* > END OF USER OPTIONS < */
    
    /* ========================================================================== */
    ofSetFrameRate(0);        // program will run at system refresh
    ofSetVerticalSync(true);  // I think.....
    //    ofSetFrameRate(120);
    
    /* Useful stuff */
    WIDTH   = ofGetWidth();
    HEIGHT  = ofGetHeight();
    CENTERX = ofGetWidth() / 2;
    CENTERY = ofGetHeight() / 2;
    swarmOrigin  = ofVec3f(0, 0, -500);
    osc.setup(PORT);
    
    bCamFromOSC = false;
    
    /* FBO STUFF */
    scene.allocate(WIDTH, HEIGHT, GL_RGBA32F_ARB);          // allocate FBOs
    sceneDetail.allocate(WIDTH, HEIGHT, GL_RGBA32F_ARB);
    
    scene.begin();                                        // clear FBOs
    ofClear(255,255,255, 0);
    scene.end();
    
    sceneDetail.begin();
    ofClear(255,255,255, 0);
    sceneDetail.end();
    
    
    flashLightPos = ofVec3f(0, 0, 000);
    
    /* ==================== TO COME FROM OSC ==================================== */
    
    /* TO COME FROM OSC */
    
    /* STUFF FOR COLONY */

    ofSpherePrimitive bounds;
    bounds.setPosition(swarmOrigin);
    bounds.setRadius(regionRadius);
    bounds.setResolution(1);
    
    flock.setup(numBoids, bounds, 100, 2.5);

    
}

//--------------------------------------------------------------
void ofApp::update(){
    
    /* SKETCH UTILS */
    int time = ofGetFrameNum(); // sketch evolution per frame
    // I always want to know the frameRate
    ofSetWindowTitle("Running at " + ofToString(ofGetFrameRate()));
    
    /* OSC RECEIVER */
    if (bCamFromOSC) { // nothing gets updates if we dont need the values
        while(osc.hasWaitingMessages()){
            // get the next message
            ofxOscMessage m;
            osc.getNextMessage(m);
            
            ofVec3f pos;
            if(m.getAddress() == "/light/position") {
                
                pos.x = m.getArgAsFloat(0); // first value for x
                pos.y = m.getArgAsFloat(1); // second value for y
                
            }
            pos.x *= WIDTH;   // re-scale to comp size
            pos.y *= HEIGHT;
            flashLightPos.x = (int)( (flashLightPos.x * (1 - positionEase) ) + (pos.x * positionEase) );
            flashLightPos.y = (int)( (flashLightPos.y * (1 - positionEase) ) + (pos.y * positionEase) );
            
        } // end of while
        
        cam.setPosition(flashLightPos);
        
    }
    
    flock.update();
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    
    drawScene();           // draw FBOs
    
    ofBackground(0);
    
    ofEnableAlphaBlending();
    ofSetColor(255, 255, 255);
    scene.draw(0, 0);
    
    
    if (bReport) report(); // report on top
    

}

//--------------------------------------------------------------
void ofApp::drawScene(){
    
    /* All scenes are unified within camera */
    ofLight light1;
    light1.setSpecularColor(ofColor::wheat);
    light1.setScale(10);
    light1.setPosition(swarmOrigin);
    light1.setAttenuation();

    
    scene.begin();
    ofPushStyle();
    float fadeAmnt = 20.0;
    ofFill();
    ofSetColor(255, 255, 255, fadeAmnt);
    ofDrawRectangle(0, 0, WIDTH, HEIGHT);
    ofPopStyle();
    cam.begin();
    
//    light1.enable();
//    ofEnableLighting();
    
    ofNoFill();
    ofSetColor(ofColor::white, 255);
    
//    flock.draw(true, true, true, true);
    flock.draw(true, false, false, false);
    
    cam.end();
    scene.end();
    
    ofDisableLighting();
    
    
}

//--------------------------------------------------------------
void ofApp::report(){
    ofPushStyle();
    ofSetColor(ofColor::darkRed);
    stringstream reportStr;
    reportStr << "Listening to OSC events on port "
    << PORT << endl
    << endl
    << "Mesh created with "
    << flock.getNumVertices()
    << " vertices" << endl
    << endl
    << "Last position "
    << flashLightPos << endl
    << endl
    << "CONTROLS:" << endl
    << "BACKSPACE - reset camera" << endl
    << "'C'       - toggle mouse control " << endl
    << "'O'       - toggle OSC control " << endl
    << endl
    << "Camera position: "
    << cam.getPosition() << endl;
    
    ofDrawBitmapString(reportStr.str(), 20, 20);
    ofPopStyle();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
    switch(key) {
            
            //        case 'C':
            //        case 'c':
            //            if(cam.getMouseInputEnabled()) cam.disableMouseInput();
            //            else cam.enableMouseInput();
            //            break;
            
        case 'O':
        case 'o':
            bCamFromOSC = !bCamFromOSC;
            break;
            
        case 'F':
        case 'f':
            ofToggleFullscreen();
            break;
            
        case OF_KEY_BACKSPACE:
            cam.setPosition(0, 0, -500);
            cam.lookAt(swarmOrigin);
            break;
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
