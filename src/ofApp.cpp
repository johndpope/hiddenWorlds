#include "ofApp.h"

/*
 >>> --- Hidden Worlds --- <<<
 openframeworks app for Creative Coding class
 Carlos Valente - Goldsmiths March 2016
 
 A magic flaslight reveals hidden creatures,
 competing swarm systems float in middair invisible from
 the unaware spectator
 
 THIS IS v7 - GL this shit, from GLpoints with texture example
 .v6 - more than one swarm, attraction points
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
 
 - VSync locks the run speed to the screen framerate
 
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
    
    bReport      = true;     // show system report
    positionEase = 0.05;     // interpolation value for positions
    detailAmount = 15;       // opacity level for beauty pass
    numBoids     = 300;      // number of creatures
    worldBound   = 1000.0f;  // bounding area
    float zDist  = 2000.0f;  // camera distance from center
    
    /* > END OF USER OPTIONS < */
    
    /* ========================================================================== */
    ofSetFrameRate(0);         // program will run as fast as it can
    ofSetVerticalSync(false);  // can produce frame tearing but if feels very satisfying
    //    ofSetFrameRate(120);
    
    /* Useful stuff */
    WIDTH   = ofGetWidth();
    HEIGHT  = ofGetHeight();
    CENTERX = ofGetWidth() / 2;
    CENTERY = ofGetHeight() / 2;
    swarmOrigin  = ofVec3f(0.0f, 0.0f, 0.0f);
    osc.setup(PORT);
    
    bCamFromOSC = false;
    
    /* FBO STUFF */
    scene.allocate(WIDTH, HEIGHT, GL_RGBA32F_ARB);          // allocate FBOs
    sceneDetail.allocate(WIDTH, HEIGHT, GL_RGBA32F_ARB);
    
    scene.begin();                                        // clear FBOs
    ofClear(255, 255, 255, 0);
    scene.end();
    
    sceneDetail.begin();
    ofClear(255, 255, 255, 0);
    sceneDetail.end();
    
    
    /* ==================== TO COME FROM OSC ==================================== */
    
    
    /* CAMERA */
    camDist = ofVec3f(0.0f, 0.0f, zDist);
    cam.setPosition(swarmOrigin - camDist);
    cam.lookAt(swarmOrigin);
    cam.setVFlip(true);
    worldRot = 0.0f;
    
    flashLightPos = cam.getPosition();
    
    ofVec3f pos1 = swarmOrigin;
    ofVec3f pos2 = ofVec3f(ofRandom(worldBound),
                           ofRandom(worldBound),
                           ofRandom(worldBound));
    
    ofVec3f pos3 = ofVec3f(ofRandom(worldBound),
                           ofRandom(worldBound),
                           ofRandom(worldBound));
    
    ofVec3f pos4 = ofVec3f(ofRandom(worldBound),
                           ofRandom(worldBound),
                           ofRandom(worldBound));
    
    light1.setSpecularColor(ofColor::white);
    light1.setScale(1.0f);
    light1.setDiffuseColor(ofColor::white);
    light1.setPosition(pos1);
    light1.setPointLight();
    
    light2.setSpecularColor(ofColor::white);
    light2.setScale(1.0f);
    light2.setDiffuseColor(ofColor::white);
    light2.setPosition(pos2);
    light2.setPointLight();
    
    light3.setSpecularColor(ofColor::white);
    light3.setScale(1.0f);
    light3.setDiffuseColor(ofColor::white);
    light3.setPosition(pos3);
    light3.setPointLight();
    
    light4.setSpecularColor(ofColor::white);
    light4.setScale(1.0f);
    light4.setDiffuseColor(ofColor::white);
    light4.setPosition(pos4);
    light4.setPointLight();
    
    /* STUFF FOR COLONY */
    bounds.setPosition(swarmOrigin);
    bounds.setRadius(worldBound);
    bounds.setResolution(1);
    
    flock1.setup(numBoids, bounds, 100, 2.5);
    flock1.setColor(255, 0 , 0, 10.10f);
    
    flock2.setup(numBoids, bounds, 100, 2.5);
    flock2.setColor(200, 100 , 0, 10.10f);

    flock1.setAttractor(pos1, 100, 200); flock2.setAttractor(pos1, 100, 200);
    flock1.setAttractor(pos2, 100, 200); flock2.setAttractor(pos2, 100, 200);
    flock1.setAttractor(pos3, 100, 200); flock2.setAttractor(pos3, 100, 200);
    flock1.setAttractor(pos4, 100, 200); flock2.setAttractor(pos4, 100, 200);
    glPointSize(2.5);

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
            flashLightPos.x = ( (flashLightPos.x * (1 - positionEase) ) + (pos.x * positionEase) );
            flashLightPos.y = ( (flashLightPos.y * (1 - positionEase) ) + (pos.y * positionEase) );
            
        } // end of while
        
        cam.setPosition(flashLightPos.x, flashLightPos.y, flashLightPos.z);
    }
    
    flock1.update();
    flock2.update();
    worldRot = ofGetElapsedTimeMillis() / 200.0;
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    
    drawScene();           // draw FBOs
    
    ofBackground(0);
    
    ofEnableAlphaBlending();
    ofSetColor(255, 255, 255);
    scene.draw(0, 0);
    ofSetColor(255, 255, 255, detailAmount);
    sceneDetail.draw(0,0);
    
    if (bReport) report(); // report on top
    
}

//--------------------------------------------------------------
void ofApp::drawScene(){
    
    
    float fadeAmnt = 40.0;
    
    scene.begin();
    ofPushStyle();

    ofFill();
    ofSetColor(255, 255, 255, fadeAmnt);
    ofDrawRectangle(0, 0, WIDTH, HEIGHT);
    ofPopStyle();
    cam.begin();
//    light1.enable();
//    ofEnableLighting();
    
    light1.enable();
    light2.enable();
    light3.enable();
    light4.enable();
    ofEnableLighting();
    
    
    ofNoFill();
    ofPushMatrix();
    ofRotateY(worldRot);
    flock1.draw(true, false, false, false); flock2.draw(true, false, false, false);
    ofPopMatrix();
    cam.end();
    scene.end();
    
    sceneDetail.begin();
    ofPushStyle();
    ofFill();
    ofSetColor(255, 255, 255, fadeAmnt);
    ofDrawRectangle(0, 0, WIDTH, HEIGHT);
    ofPopStyle();
    cam.begin();
    //    light1.enable();
    //    ofEnableLighting();
    
    ofNoFill();
    ofPushMatrix();
    ofRotateY(worldRot);
    flock1.draw(false, true, true, true); flock2.draw(false, true, true, false);
    ofPopMatrix();
    cam.end();
    sceneDetail.end();
    
    ofDisableLighting();
    
}
//--------------------------------------------------------------
void ofApp::setupWorld(){

}

//--------------------------------------------------------------
void ofApp::killWorld(){
    
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
    << flock1.getNumVertices()
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
    << cam.getPosition() << endl
    << "Camera rotation: "
    << worldRot << endl;
    
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
            cam.setPosition(swarmOrigin - camDist);
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
