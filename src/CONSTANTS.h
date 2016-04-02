/*
 >>> --- Hidden Worlds --- <<<
 openframeworks app for Creative Coding class
 Carlos Valente - Goldsmiths March 2016
 
 A magic flaslight reveals hidden creatures,
 competing swarm systems float in middair invisible from
 the unaware spectator
 
 Holds constants to be shared across classes
 */


#pragma once

class CONSTANTS : public ofBaseApp {
public:
    void setup() {
    /* Can only be called after main is initialized */
        WIDTH   = ofGetWidth();
        HEIGHT  = ofGetHeight();
        CENTERX = WIDTH / 2;
        CENTERY = HEIGHT / 2;
        ORIGIN  = ofVec3f(0.0f, 0.0f, 0.0f);
        
    }

    int WIDTH;              // holds frame WIDTH
    int HEIGHT;             // holds frame HEIGHT
    float CENTERX, CENTERY; // holds image CENTERS
    ofVec3f ORIGIN;         // holds common origins
    
    bool bRenderSprite = true;
    string file = "dot.png";    // file path for particle texture
    
};
