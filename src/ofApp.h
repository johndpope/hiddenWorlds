#pragma once

#include "ofMain.h"

#include "ofxOsc.h"    // standard OSC
#define PORT 12345

#include "myFlock.h"   // my wrapper for boids code

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
    
        /* Application specific methods */
        void report();     // builds report string
        void drawScene();  // draw call for FBOs

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
    
    /* Useful stuff */
    float WIDTH, HEIGHT, CENTERX, CENTERY;
    bool bReport, bCamFromOSC;
    float positionEase;
    int detailAmount;
    int worldBound;
    
    /* OSC Stuff */
    ofxOscReceiver osc;
    
    /* 3D pretty stuff */
    ofEasyCam cam;
    
    ofLight   flashLight;    // light is parented to camera
    ofVec3f   flashLightPos;
    
    /* FBOs */
    ofFbo scene, sceneDetail;
    
    /* Flock stuff */
    MyFlock   flock;
    ofVec3f   swarmOrigin;
    int       numBoids;
    int       type;
    
    float     regionRadius;
    
		
};
