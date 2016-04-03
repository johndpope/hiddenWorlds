#include "ofApp.h"

/*
 >>> --- Hidden Worlds --- <<<
 openframeworks app for Creative Coding class
 Carlos Valente - Goldsmiths March 2016
 
 > Staring at digital skies <
 
 A magic flaslight reveals hidden creatures,
 competing swarm systems float in middair invisible from
 the unaware spectator

 
 USABILITY NOTES:
 - For tweaking it will be necessary to use twin app hiddenWorlds Utils
 - There are some user options in the beggining of the code
 - Flashlight position comes from OSC "/light/position" (NOT IN USE)
 use twin app, hiddenWorldsUtils or send x, y - 0, 1 values
 to given OSC channel;
 
 THIS IS .v11 tweak behaviours (this looks like molecules, lets go with that!)
 .v10 - more flocks! shader this!
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
 - find way to make good meshes
 - swarms avoid each other
 - looks like bounds is a cube?

 done:
  - flocks in vector
  - add force field to particles
  - make colour pallette
  - implement set colour for meshes
  - create constants header file
  -- populate function called in setup updates all variables
  - two apps fullscreen in two screens
  - anaglyph
  - sound input
  - GL shader for swarm
 
 failed:
  - ASCII to replace halftone (could be worth a second go)
  - metaballs is a no go, double tried
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
    
    int numFlocks   = 80;          // only little flocks might handle metaballs
    numBoids        = 020;         // number of boids (for each flock)
    worldBound      = 1200.0f;      // bounding area
    float zDist     = worldBound * 2;   // camera distance from center
    fadeAmnt        = 11.5;
    detailAmount    = 000;         // opacity level for beauty pass
    worldRotSpeed   = 01.0f;       // 0 no rotation
    halftoneSize    = 4.0f;       // 4.0 seems to be good
    
    lineWidth       = .9;           // 1 seems to be good
    pointSize       = 25.0;         // 25 seems to be good
    pointOpacity    = 255;
    float shiny     = .1f;          // .1 looks nice
    float lightSize = 1.0f;         // size for point lights
    anaglyphSize    = 15.0f;
    
    bDoListen       = true;         // is audio reactive?

    bDrawWire = false;
    bDrawFace = false;
    bDrawBlob = false;

    /* Add a couple of colours */
//    palette.push_back(ofColor(240, 240, 255));  // sanguine
//    palette.push_back(ofColor(255, 240, 240));  // steel
    palette.push_back(ofColor(89,  113,  150));  // blue
    palette.push_back(ofColor(46,  189, 179));  // turquoise
    palette.push_back(ofColor(141, 219, 186));  // aqua
    palette.push_back(ofColor(255, 202, 192));  // straw
    palette.push_back(ofColor(252, 76,  67));  // orange
    palette.push_back(ofColor(0, 76,  200));  // blue
    
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
    
    anaglyph        = 1.0f;
    
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
    maxSpeed        = 1.3f;
    
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
    int numLigths = 4;
    
    ofLight light1;                         // default light in center
    light1.setSpecularColor(ofColor::white);
    light1.setScale(lightSize);
    light1.setDiffuseColor(ofColor::darkGray);
    light1.setPosition(pos1);
    light1.setPointLight();
    
    lights.push_back(light1);
    
    for (int i = 0; i < numLigths; i++){
        ofVec3f pos = ofVec3f(ofRandom(-worldBound, worldBound),
                              ofRandom(-worldBound, worldBound),
                              ofRandom(-worldBound, worldBound));
        
        ofLight l;
        l.setSpecularColor(ofColor::white);
        l.setScale(lightSize);
        l.setDiffuseColor(ofColor::darkGray);
        l.setPosition(pos);
        l.setPointLight();
        
        lights.push_back(l);
        
    }
    
    /* ASCII OVERLAY - from example */
    font.load("Courier New Bold.ttf", 15);
    asciiCharacters =  string("  ..,,,'''``--_:;^^**""=+<>iv%&xclrs)/){}I?!][1taeo7zjLunT#@JCwfy325Fp6mqSghVd4EgXPGZbYkOA8U$KHDBWNMR0Q");
    
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
    
    /* !!! I am not sure why did I have to create object and initialize separate */
    for (int i = 0; i < numFlocks; i++) {
        MyFlock f;
        flocks.push_back(f);
    }
    
    for (int i = 0; i < flocks.size(); i++) {
        float repelForce = -3;
        float repelDist  = 600;
        float pushForce  = 2;
        float pushDist   = 800;
        ofColor color    = palette[ofRandom(palette.size())];
        
        flocks[i].setup(numBoids, bounds, worldBound / 3.0, 5.1, pointSize);    // !!add deviation
        flocks[i].setColor(color.r, color.g, color.b, shiny);
        flocks[i].setDrawValues(pointSize, lineWidth);
        flocks[i].setAttractor(pos1, repelForce, repelDist);
        flocks[i].setAttractor(pos1, pushForce, pushDist);
    }
    
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
        if (bDoListen )
            anaglyph = ofMap(smoothedVol, 0.1, 0.5, 0.05, 1.0, true);

        
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
    anaglyph = anaglyph * anaglyphSize;
    
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
    ofSetColor(255, 255, 255, 20);
    style.draw(0, 0);
    
    ofEnableBlendMode(OF_BLENDMODE_ALPHA);
    if (bReport) report();                              // <--- render report stream
    
}

//--------------------------------------------------------------
void ofApp::updateFlock(){
    
    /* Updates all flocks in vector */
    
    for (int i = 0; i < flocks.size(); i++) {
        if (bNewFlockValues) {
            flocks[i].setSeparation(separate);
            flocks[i].setSeparationDist(distSeparation);
            
            flocks[i].setAlign(align);
            flocks[i].setAlignDist(distAlign);
            
            flocks[i].setCohesion(cohesion);
            flocks[i].setCohesionDist(distCohesion);
            
            flocks[i].setAttraction(attraction);
            flocks[i].setAttractionDev(attractionDev);
            flocks[i].setMaxSpeed(maxSpeed);
        }
        flocks[i].update();
    }
    bNewFlockValues = false;

}

//--------------------------------------------------------------
void ofApp::drawScene(bool bUpdateOverlay){
    
    /* > DRAW POINTS SCENE < */
    if (bUpdateOverlay) {
        for (int i = 0; i < flocks.size(); i++) {
            flocks[i].setDrawValues(pointSize, lineWidth);
        }
    }
    
    scene.begin();
    fade();             // fade to black over fadeAmnt
    
        lightsOn();     // call for scene
    
        ofNoFill();
        ofPushMatrix();
    
            ofEnableBlendMode(OF_BLENDMODE_ALPHA);    // overlay both flocks
    
            ofRotateY(worldRot);                    // rotate world over worldRot
            for (int i = 0; i < flocks.size(); i++) {
                flocks[i].draw();
            }
    
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
            for (int i = 0; i < flocks.size(); i++) {
                flocks[i].draw(bDrawWire, bDrawFace, bDrawBlob);
            }
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
void ofApp::drawASCII(){

    /* Create ASCII effect for overlay - DIDNT WORK */

    style.begin();
    ofBackground(0, 0, 0);
    ofSetColor(255);
    
    for (int i = 0; i < CONSTANTS.WIDTH; i+= 7){
        for (int j = 0; j < CONSTANTS.HEIGHT; j+= 9){
            int rnd = ofRandom(asciiCharacters.size() + 1);
            font.drawString(ofToString(asciiCharacters[rnd]), i, j);
        }
    }
    
    style.end();
    
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
    smoothedVol *= 0.995;
    smoothedVol += 0.005 * curVol;
    
}


//--------------------------------------------------------------
void ofApp::lightsOn(){
    
    /* Enable scene lighting - if we are not drawing sprites */
    cam.begin();
    if (!CONSTANTS.bRenderSprite) {
        for (int i = 0; i < lights.size(); i++){
            lights[i].enable();
            lights[i].enable();
            lights[i].enable();
            lights[i].enable();
            ofEnableLighting();
        }
    }
    
}

//--------------------------------------------------------------
void ofApp::lightsOff(){
    
    /* Disable scene lighting - if we are not drawing sprites */
    if (!CONSTANTS.bRenderSprite) {
        for (int i = 0; i < lights.size(); i++){
            lights[i].disable();
            lights[i].disable();
            lights[i].disable();
            lights[i].disable();
            ofDisableLighting();
        }

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
