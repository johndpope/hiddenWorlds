#include "ofApp.h"

/*
 >>> --- Hidden Worlds --- <<<
 openframeworks app for Creative Coding class
 Carlos Valente - Goldsmiths March 2016
 
 A magic flaslight reveals hidden creatures,
 competing swarm systems float in middair invisible from
 the unaware spectator

 
 USABILITY NOTES:
 - For tweaking it will be necessary to use twin app hiddenWorlds Utils
 - There are some user options in the beggining of the code
 - Flashlight position comes from OSC "/light/position" (NOT IN USE)
 use twin app, hiddenWorldsUtils or send x, y - 0, 1 values
 to given OSC channel;
 
 THIS IS v10 - more flocks, less boids, tweak behaviours (this looks like molecules, lets go with that!)
 .v9 - add sound input control for anaglyph
 .v8 - add OSC control for photoshoot
 .v7 - handle GL stuff, modify looks, look with GL points, meh
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
 - ASCII to replace halftone
 - anaglyph
 - sound input
 - flocks in vector
 - find way to make good meshes
 - swarms avoid each other
 - tweak swarm appearence
 - GL shader for swarm?
 - look into spring behaviours for mesh with some metaballs

 done:
  - implement set colour for meshes
  - create constants header file
  -- populate function called in setup updates all variables
  - two apps fullscreen in two screens
 
 failed:
 - metaballs is a no go
 - instancing with VBO sounds ok but didnt go anywhere
 - Close in flashlight look
 - get orientation from Wii and position from kinect
 - share ofParameters within classes <--
 - ofParameter implementation for OSC (ofParameters didnt prove advantage on transferring data)
 
 >>> STUFF IM FINDING
 
 - VSync locks the run speed to the screen framerate
 
 - ofMeshes are uploaded to the GPU every frame, VBOmeshes only once. Use instancing to make the best of it
 
 - to use meshes that are updates frequently set mesh.setUsage(GL_STREAM_DRAW) or vbo.setUsage(GL_STREAM_DRAW)
 
 - metaballs doesnt quite work with this way of drawing, (needs centroid and radius) cant handle amount of points either
 
 >>> DEVELOPMENT
 
 - Everyone complains shadows are tricky, could work on a BW image from a parallel perspective
 https://forum.openframeworks.cc/t/shadows-in-of-for-3d-space/18080
 https://en.wikipedia.org/wiki/Shadow_mapping
 https://github.com/jacres/of-ESMShadowMapping
 
 - Make a shadow: put geometry in a normal vertex shader pixel shader, in the vertex shader modify the position of vertices by some direction that moves your whole mesh into view  / change the angle of view and that is your shadows"
 http://www.rastertek.com/dx11tut40.html
 http://www.rastertek.com/tutdx11.html
 
 - fairy light look?
 https://forum.openframeworks.cc/t/shaders-are-pretty-hot/7263
 
 - triangulation, use camera with plane parallel to projection plane. with laser find brightest spot, second camera finds brightest spot in projection surface.
 http://www.muonics.net/blog/index.php?postid=26
 
 - image mapping
 https://github.com/frauzufall/ofx2DMapping
 
 - performance 
 https://forum.openframeworks.cc/t/porting-shiffmans-boid-flocking-java-is-spanking-my-c/2100/3
 
 INSPIRATION <<<
 http://www.creativeapplications.net/environment/augmented-shadow-openframeworks/
 http://joonmoon.net/Luzes-Relacionais
 http://www.ericforman.com/
 http://glslsandbox.com/e#28777.0
 http://abstract20122013.gsapp.org/binary-space-partition/
 http://zenbullets.com/images.php
 http://www.novastructura.net/wp/works/meanders/
 http://www.algorithmicdesign.net/columbia/encoded_2012/schizometric/schizometric_6.jpg
 http://code.algorithmicdesign.net/Schizo-Metr-ic
 
 */

//--------------------------------------------------------------
void ofApp::setup(){

    /* > USER OPTIONS < */
    
    bReport         = false;        // show system report
    positionEase    = 0.05;         // interpolation value for positions
    numBoids        = 0500;         // number of boids (for each flock)
    worldBound      = 1200.0f;      // bounding area
    float zDist     = worldBound;   // camera distance from center
    fadeAmnt        = 05.;
    detailAmount    = 100;         // opacity level for beauty pass
    worldRotSpeed   = 0.1f;        // 0 no rotation
    halftoneSize    = 3.0f;         // 3.0 seems to be good
    
    lineWidth       = .9;           // 1 seems to be good
    pointSize       = 46.0;         // 3 seems to be good
    pointOpacity    = 200;
    float shiny     = .1f;          // .1 looks nice
    float lightSize = 1.0f;         // size for point lights
    anaglyphSize    = 15.0f;
    
    bDoListen       = true;         // is audio reactive?

    bDrawWire = false;
    bDrawFace = false;
    bDrawBlob = false;
    
    /* > END OF USER OPTIONS < */
    
    
    
    
    
    
    /* =========================== AUX SETUP ==================================== */
    /* ========================================================================== */
    
    ofSetFrameRate(0);         // program will run as fast as it can
    ofSetVerticalSync(false);  // can produce frame tearing but if feels very satisfying
    ofDisableDepthTest();
    
    /* Useful stuff */
    CONSTANTS.setup();
    swarmOrigin = CONSTANTS.ORIGIN;
    osc.setup(PORT);
    
    bCamFromOSC     = false;
    bNewValues      = true;
    bIsPaused       = false;
    
    /* Flock defaults */
    bNewFlockValues = true;
    separate        = 55.0f;
    distSeparation  = 50.0f;
    align           = 12.0f;
    distAlign       = 100.0f;
    cohesion        = 7.0f;
    distCohesion    = 150.0f;
    attraction      = 10.0f;
    attractionDev   = 1.0f;
    maxSpeed        = 1.0f;
    
    /* FBO STUFF */
    scene.allocate(CONSTANTS.WIDTH, CONSTANTS.HEIGHT, GL_RGBA32F_ARB);          // allocate FBOs
    sceneDetail.allocate(CONSTANTS.WIDTH, CONSTANTS.HEIGHT, GL_RGBA32F_ARB);
    style.allocate(CONSTANTS.WIDTH, CONSTANTS.HEIGHT, GL_RGBA32F_ARB);
    
    scene.begin();                  // clear all FBOs
    ofClear(255, 255, 255, 0);
    scene.end();
    
    sceneDetail.begin();
    ofClear(255, 255, 255, 0);
    sceneDetail.end();
    
    style.begin();
    ofClear(255, 255, 255, 0);
    style.end();
    
    
    /* ======================== OBJECTS INIT ==================================== */
    /* ========================================================================== */

    
    /* > CAMERA < */
    camDist = ofVec3f(0.0f, 0.0f, zDist);
    cam.setVFlip(true);
    cam.setPosition(swarmOrigin - camDist);
    cam.lookAt(swarmOrigin);
    worldRot = 0.0f;
    
    viewPos = cam.getPosition();

    /* > SCENE LIGHTING < */
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
    light1.setScale(lightSize);
    light1.setDiffuseColor(ofColor::darkGray);
    light1.setPosition(pos1);
    light1.setPointLight();
    
    light2.setSpecularColor(ofColor::white);
    light2.setScale(lightSize);
    light2.setDiffuseColor(ofColor::darkGray);
    light2.setPosition(pos2);
    light2.setPointLight();
    
    light3.setSpecularColor(ofColor::white);
    light3.setScale(lightSize);
    light3.setDiffuseColor(ofColor::darkGray);
    light3.setPosition(pos3);
    light3.setPointLight();
    
    light4.setSpecularColor(ofColor::white);
    light4.setScale(lightSize);
    light4.setDiffuseColor(ofColor::darkGray);
    light4.setPosition(pos4);
    light4.setPointLight();
    
    /* > SOUND < */
    int bufferSize = 256;
    left.assign(bufferSize, 0.0);
    right.assign(bufferSize, 0.0);
    
    smoothedVol     = 0.0;
    scaledVol		= 0.0;
    
    soundStream.setup(this, 0, 2, 44100, bufferSize, 4);

    
    /* > STUFF FOR FLOCKS < */
    bounds.setPosition(swarmOrigin);
    bounds.setRadius(worldBound);
    
    flock1.setup(numBoids, bounds, 200, 5.1);
    flock1.setColor(255, 200 , 200, shiny);
    flock1.setDrawValues(pointSize, lineWidth);
    
    flock2.setup(numBoids, bounds, 200, 4.5);
    flock2.setColor(200, 200, 255, shiny);
    flock2.setDrawValues(pointSize, lineWidth);
    
    
    /* Add only one attractor, hopefully for a spherical displacement look */
//    float force = ofRandom(-750, 250);
//    float dist  = ofRandom(1100,  0);
    float force = -10;
    float dist  = 600;
    flock1.setAttractor(pos1, force, dist);
    flock2.setAttractor(pos1, force, dist);
   
    force = 10;
    dist  = 1200;
    flock1.setAttractor(pos1, force, dist);
    flock2.setAttractor(pos1, force, dist);
    
    
    /* > CALL TO DRAW SCENES < */
    updateFlock();
    drawScene();
    
}

//--------------------------------------------------------------
void ofApp::update(){
    
    /* > UTILS < */
    ofSetWindowTitle("Running at " + ofToString(ofGetFrameRate()));
    
    if (!bIsPaused) {
        
        /* > SOUND INPUT < */
        if (bDoListen ) {
            smoothedVol = ofMap(smoothedVol, 0.0, 1.3, 0.05, 1.0, true);
        } else smoothedVol = 0.05f;
        
        /* > OSC RECEIVER < */
        while(osc.hasWaitingMessages()){
            // get the next message
            ofxOscMessage m;
            osc.getNextMessage(m);
        
            /* FLASHLIGHT POSITION X <> Y */
            if(m.getAddress() == "/light/position") {
                float posX, posY;   // TWO floats is faster than a vector ?
            
                posX = m.getArgAsFloat(0); // first value for x
                posY = m.getArgAsFloat(1); // second value for y
                
                /* Re-scale to comp size and ease position difference */
                posX *= CONSTANTS.WIDTH;
                posY *= CONSTANTS.HEIGHT;
                viewPos.x = ( (viewPos.x * (1 - positionEase) ) + (posX * positionEase) );
                viewPos.y = ( (viewPos.y * (1 - positionEase) ) + (posY * positionEase) );
            }
            
            /* APPEARENCE STUFF */
            if(m.getAddress() == "/appearence") {
                fadeAmnt        = m.getArgAsFloat(0); // order from utils
                worldRotSpeed   = m.getArgAsFloat(1);
                halftoneSize    = m.getArgAsFloat(2);
                pointSize       = m.getArgAsFloat(3);
                lineWidth       = m.getArgAsFloat(4);
                detailAmount    = m.getArgAsInt(5);
//                bDrawWire       = m.getArgAsBool(6);
//                bDrawFace       = m.getArgAsBool(7);
//                bDrawBlob       = m.getArgAsBool(8);
                bNewValues      = true;
            }
        
            /* SWARM STUFF*/
            if(m.getAddress() == "/flock") {
            
                separate        = m.getArgAsFloat(0); // order from utils
                distSeparation  = m.getArgAsFloat(1);
                align           = m.getArgAsFloat(2);
                distAlign       = m.getArgAsFloat(3);
                cohesion        = m.getArgAsFloat(4);
                distCohesion    = m.getArgAsFloat(5);
                attraction      = m.getArgAsFloat(6);
                attractionDev   = m.getArgAsFloat(7);
                maxSpeed        = m.getArgAsFloat(8);
                
                bNewFlockValues = true;
            }
        
        
        } // end of while
        
        /* Set camera to new position */
        if (bCamFromOSC) cam.setPosition(viewPos);
        
        /* > Update objects and variables < */
        int time = ofGetElapsedTimeMillis(); // sketch evolution per frame
        if (worldRotSpeed != 0)  worldRot = (time / 100.0) * worldRotSpeed;
    
        /* Update Flock */
        updateFlock();
        
        /* Call for drawing of FBOs */
        if (bNewValues) drawScene(true);
        else drawScene();
    
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    /* Initialize aux */
//    glDepthMask(GL_FALSE); // Not quite sure
    float anaglyph = smoothedVol * anaglyphSize;
    
    /* > Composite FBOs < */

    ofEnableBlendMode(OF_BLENDMODE_ALPHA);              // <--- render background
    ofBackground(0);

    ofEnableBlendMode(OF_BLENDMODE_ALPHA);              // <--- render point cloud
    ofSetColor(255, 0, 0, pointOpacity / 2);
    scene.draw(0, 0);
    ofEnableBlendMode(OF_BLENDMODE_ADD);
    ofSetColor(0, 255, 255, pointOpacity / 2);
    scene.draw(anaglyph, anaglyph);
    

    if (detailAmount > 0) {                             // <--- render beauty pass
        ofEnableBlendMode(OF_BLENDMODE_ALPHA);
        ofSetColor(255, 0, 0, detailAmount / 2);
        sceneDetail.draw(0, 0);
        ofEnableBlendMode(OF_BLENDMODE_ADD);
        ofSetColor(0, 255, 255, detailAmount / 2);
        sceneDetail.draw(anaglyph, anaglyph);
    }
    
    ofEnableBlendMode(OF_BLENDMODE_MULTIPLY);           // <--- render overlay
    ofSetColor(255, 255, 255, 30);
    style.draw(0, 0);
    
    ofEnableBlendMode(OF_BLENDMODE_ALPHA);
    if (bReport) report();                              // <--- render report stream
    
}

//--------------------------------------------------------------
void ofApp::updateFlock(){
    
    if (bNewFlockValues) {
        
        flock1.setSeparation(separate);
        flock1.setSeparationDist(distSeparation);
        
        flock1.setAlign(align);
        flock1.setAlignDist(distAlign);
        
        flock1.setCohesion(cohesion);
        flock1.setCohesionDist(distCohesion);
        
        flock1.setAttraction(attraction);
        flock1.setAttractionDev(attractionDev);
        flock1.setMaxSpeed(maxSpeed);
        
        //======================================================
        
        flock2.setSeparation(separate);
        flock2.setSeparationDist(distSeparation);
        
        flock2.setAlign(align);
        flock2.setAlignDist(distAlign);
        
        flock2.setCohesion(cohesion);
        flock2.setCohesionDist(distCohesion);
        
        flock2.setAttraction(attraction);
        flock2.setAttractionDev(attractionDev);
        flock2.setMaxSpeed(maxSpeed);
        
        bNewFlockValues = false;
    }
    
    flock1.update();
    flock2.update();
    
}

//--------------------------------------------------------------
void ofApp::drawScene(bool bUpdateOverlay){
    
    /* > DRAW POINTS SCENE < */
    if (bUpdateOverlay) {
        flock1.setDrawValues(pointSize, lineWidth);
        flock2.setDrawValues(pointSize, lineWidth);
    }
    
    scene.begin();
    fade();             // fade to black over fadeAmnt
    
        lightsOn();     // call for scene
    
        ofNoFill();
        ofPushMatrix();
    
            ofEnableBlendMode(OF_BLENDMODE_ALPHA);    // overlay both flocks
    
            ofRotateY(worldRot);                    // rotate world over worldRot
            flock1.draw();
            flock2.draw();
    
            ofEnableBlendMode(OF_BLENDMODE_ALPHA);
    
        ofPopMatrix();
        lightsOff();
    ofPopStyle();
    scene.end();
    

    /* > DRAW BEAUTY PASS SCENE < */
    if (detailAmount > 0) { // only draw if it will be visible
        
        sceneDetail.begin();
        fade();
        
        lightsOn();
    
        ofNoFill();
        ofPushMatrix();
            ofEnableBlendMode(OF_BLENDMODE_ALPHA);
            ofRotateY(worldRot);
            flock1.draw(bDrawWire, bDrawFace, bDrawBlob);
            flock2.draw(bDrawWire, bDrawFace, bDrawBlob);
            ofEnableBlendMode(OF_BLENDMODE_ALPHA);
        ofPopMatrix();
        sceneDetail.end();
    
        lightsOff();
    
    }
    
    /* > DRAW OVERLAY SCENE < */
    // gets called at startup and if parameters changed
    if (bUpdateOverlay) drawHalftone();
    
}
//--------------------------------------------------------------
void ofApp::drawHalftone(){
        
    /* Create halftone effect for overlay using halfTone size */
    
    style.begin();
    ofBackground(0, 0, 0);
    glColor3f(1.0, 1.0, 1.0);
    glPointSize(halftoneSize);
    glBegin(GL_POINTS);
    
    float numX = ofGetWidth() / halftoneSize;
    float numY = ofGetHeight() / halftoneSize;
    
    
    float spacingX = ofGetWidth() / numX;  // spacing between objects
    float spacingY = ofGetHeight() / numY;
    
    float startingX = spacingX / 2;               // starting point, half the spacing
    float startingY = spacingY / 2;
    
    for (int x = 0; x < numX; x++) {
        for (int y = 0; y < numY; y++) {
            int locX, locY;
            locX = x * spacingX + startingX;
            locY = y * spacingY + startingY;
            glVertex2f(locX, locY);
        }
    }
    
    glEnd();
    style.end();
    
    bNewValues = false;
}

//--------------------------------------------------------------
void ofApp::audioIn(float * input, int bufferSize, int nChannels){
    
    float curVol = 0.0;
    
    // samples are "interleaved"
    int numCounted = 0;
    
    //lets go through each sample and calculate the root mean square which is a rough way to calculate volume
    for (int i = 0; i < bufferSize; i++){
        left[i]		= input[i * 2]     * 0.5;
        right[i]	= input[i * 2 + 1] * 0.5;
        
        curVol += left[i]  * left[i];
        curVol += right[i] * right[i];
        numCounted += 2;
    }
    
    //this is how we get the mean of rms :)
    curVol /= (float)numCounted;
    
    // this is how we get the root of rms :)
    curVol = sqrt( curVol );
    
    smoothedVol *= 0.98;
    smoothedVol += 0.02 * curVol;
    
}


//--------------------------------------------------------------
void ofApp::lightsOn(){
    
    /* Enable scene lighting - if we are not drawing sprites */
    cam.begin();
    if (!CONSTANTS.bRenderSprite) {
        light1.enable();
        light2.enable();
        light3.enable();
        light4.enable();
        ofEnableLighting();
    }
    
}


//--------------------------------------------------------------
void ofApp::lightsOff(){
    
    /* Disable scene lighting - if we are not drawing sprites */
    if (!CONSTANTS.bRenderSprite) {
        light1.disable();
        light2.disable();
        light3.disable();
        light4.disable();
        ofDisableLighting();
    }
    cam.end();
}

//--------------------------------------------------------------
void ofApp::fade(){
    
    /* Clear Background slowly for fade effect */
    ofPushStyle();
    
        ofFill();
        ofSetColor(0, 0, 0, fadeAmnt);
        ofDrawRectangle(0, 0, CONSTANTS.WIDTH, CONSTANTS.HEIGHT);
    
    ofPopStyle();
    
}

//--------------------------------------------------------------
void ofApp::report(){
    
    /* Reports current state */
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
    << viewPos << endl
    << endl
    << "CONTROLS:" << endl
    << "BACKSPACE   - reset camera" << endl
//    << "'C'       - toggle mouse control " << endl
    << "'F'         - toggle fullscreen " << endl
    << "'O'         - toggle OSC control " << endl
    << "'ARROW UP'  - window at second screen" << endl
    << "'ARROW DOWN'- window at first screen" << endl
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
    /* Handle key events, this are only for debug, there is no UI */
    
    switch(key) {
            
        /* Toggle camera control from OSC */
        case 'O':
        case 'o':
            bCamFromOSC = !bCamFromOSC;
            break;
            
        /* Toggle fullscreen */
        case 'F':
        case 'f':
            ofToggleFullscreen();
            break;
            
        /* Reset camera position */
        case OF_KEY_BACKSPACE:
            cam.setPosition(swarmOrigin - camDist);
            cam.lookAt(swarmOrigin);
            break;
            
        /* Set Window to second screen */
        case OF_KEY_UP:
            if (ofGetWindowMode() == OF_WINDOW){
                ofSetWindowShape(1280, 720);
                ofSetWindowPosition(ofGetScreenWidth(), 0);
            }
            break;
            
        /* Set Window to first screen */
        case OF_KEY_DOWN:
            if (ofGetWindowMode() == OF_WINDOW){
                ofSetWindowShape(1280, 720);
                ofSetWindowPosition(0, 0);
            }
            break;
            
        /* Pause animation */
        case 'P':
        case 'p':
            bIsPaused = !bIsPaused;
            break;
            
        /* Toggle draw wireframe */
        case 'Z':
        case 'z':
            bDrawWire = !bDrawWire;
            break;
            
        /* Toggle draw faces */
        case 'X':
        case 'x':
            bDrawFace = !bDrawFace;
            break;
            
        /* Toggle draw blob */
        case 'V':
        case 'v':
            bDrawBlob = !bDrawBlob;
            break;
            
        /* Toggle Sound reaction */
        case 'S':
        case 's':
            bDoListen = !bDoListen;
            break;
            

    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    
    switch (key) {
        /* Particle Size */
        case '<':
            pointSize -= 5.0;
            bNewValues = true;
            break;
        case '>':
            pointSize += 5.0;
            bNewValues = true;
            break;
    }
    
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

//--------------------------------------------------------------
void ofApp::exit(){
    
    /* Close soundstream on exit */
    soundStream.close();
    
}
