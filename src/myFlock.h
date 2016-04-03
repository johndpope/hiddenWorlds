/*
 >>> --- Hidden Worlds --- <<<
 openframeworks app for Creative Coding class
 Carlos Valente - Goldsmiths March 2016
 
 A magic flaslight reveals hidden creatures,
 competing swarm systems float in middair invisible from
 the unaware spectator
 
 Wrapping all boids functions for ease of use
 */

#pragma once

#include "ofMain.h"
#include "Flock3d.h"   // my modification of ofxBoids
#include "CONSTANTS.h"


class MyFlock : public ofBaseApp {
public:
    
    void setup(int numBoids_, ofSpherePrimitive bounds_, float dev_, float dt_);
    void update();
    void draw(); // draw Points
    void draw(bool bDrawWire, bool bDrawFaces, bool bDrawBlob); // draw other
    void setColor(int r, int g, int b, float shiny);
    void setRenderColour(ofColor colVert, ofColor colWire, ofColor colFaces, ofColor colBlob);
    
    int getNumVertices();
    
    
    /* ALL FUNCTIONS TO SET VALUES - maybe a bad way of doing this */
 
    void setAttractor(ofVec3f loc, float force, float sensorDist);
    void setDrawValues(float pointSize_, float lineWidth_);
    
    void setAlign(float align);
    void setAlignDist(float dist);
    
    void setAttraction(float att);
    void setAttractionDev(float dev);
    
    void setCohesion(float coh);
    void setCohesionDist(float dist);
    
    void setSeparation(float sep);
    void setSeparationDist(float dist);
    
    void setDt(float dt);
    void setMaxForce(float force);
    void setMaxSpeed(float speed);
    void setMaxTurn(float turn);
    
    CONSTANTS CONSTANTS;
    
    Flock3d flock;
    int numBoids;
    
    ofVboMesh flockMesh;
    ofMaterial mat;
    ofTexture texture;
    ofShader shader;

    ofColor colour;
    float shininess;
    float pointSize, lineWidth;
       
};