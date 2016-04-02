#pragma once

#include "ofMain.h"

#include "ofxOsc.h"    // standard OSC
#define PORT 12345

#include "myFlock.h"   // my wrapper for boids code

#include "CONSTANTS.h"



class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
    
        /* Application specific methods */
        void report();                                  // builds report string
        void drawScene(bool bUpdateOverlay = false);    // draw call for FBOs
    
        /* Methods for unified appearence */
        void lightsOn();            // turn on scene lighting
        void lightsOff();           // kill scene lightign
        void fade();                // fade background

        void drawHalftone();        // draws halftone overlay
    
		void keyPressed(int key);
		void keyReleased(int key);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
        void exit();
    
    /* > Useful stuff < */
    CONSTANTS CONSTANTS;
    bool bReport, bCamFromOSC;
    bool bIsPaused;
    float positionEase;
    float worldRotSpeed;
    float worldRot;
    float halftoneSize;
    int detailAmount;
    int worldBound;
    
    
    /* > OSC Stuff < */
    ofxOscReceiver osc;
    bool bNewValues;
    
    /* > AUDIO REACTIVE < */
    void audioIn(float * input, int bufferSize, int nChannels);
    ofSoundStream soundStream;
    float smoothedVol, scaledVol;
    vector <float> left;
    vector <float> right;
    bool bDoListen;
    
    /* > FBOs < */
    ofFbo scene, sceneDetail, style;
    float fadeAmnt;
    
    /* > 3D pretty stuff < */
    ofEasyCam cam;
    ofVec3f camDist;
    ofShader shader;
    ofTexture texture;
    bool bDrawWire, bDrawFace, bDrawBlob;
    float pointOpacity, pointSize, lineWidth;
    float anaglyphSize;
    
    ofVec3f   viewPos;
    ofLight   light1, light2, light3, light4;
    
    /* > Flock stuff < */
    void updateFlock();
    ofSpherePrimitive bounds;
    MyFlock   flock1, flock2;
    ofVec3f   swarmOrigin;
    int       numBoids;
    int       type;
    
    bool bNewFlockValues;
    float separate, distSeparation;
    float align, distAlign;
    float cohesion, distCohesion;
    float attraction, attractionDev;
    float maxSpeed;
		
};
