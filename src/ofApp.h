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
        void drawASCII();
    
		void keyPressed(int key);
		void keyReleased(int key);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
        void exit();
    
    /* > Useful stuff < */
    CONSTANTS CONSTANTS;
    vector <ofColor> palette;
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
    ofSoundStream   soundStream;
    float           smoothedVol, scaledVol;
    vector <float>  left;
    vector <float>  right;
    bool            bDoListen;
    
    /* > FBOs < */
    ofFbo scene, sceneDetail, style;
    float fadeAmnt;
    
    /* > 3D appearence stuff < */
    ofEasyCam   cam;
    ofVec3f     camDist;
    ofShader    shader;
    ofTexture   texture;
    bool        bDrawWire, bDrawFace, bDrawBlob;
    float       pointOpacity, pointSize, lineWidth;
    float       anaglyph, anaglyphSize;
    
    ofVec3f viewPos;
    vector <ofLight> lights;
    
    /* > ASCII < */
    string asciiCharacters;
    ofTrueTypeFont  font;
    
    /* > Flock stuff < */
    void updateFlock();
    vector <MyFlock>    flocks;
    ofSpherePrimitive   bounds;
    ofVec3f             swarmOrigin;
    int                 numBoids;
    int                 type;
    
    bool    bNewFlockValues;
    float   separate, distSeparation;
    float   align, distAlign;
    float   cohesion, distCohesion;
    float   attraction, attractionDev;
    float   maxSpeed;
		
};
