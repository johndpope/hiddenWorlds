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

class MyFlock : public ofBaseApp {
public:
    void setup(int numBoids_, ofSpherePrimitive bounds_, float dev_, float dt_);
    void update();
    void draw(bool bDrawVertices, bool bDrawWire, bool bDrawFaces, bool bDrawBlob);
    void setColor(int r, int g, int b, float shiny);
    
    void setAttractor(ofVec3f loc, float force, float sensorDist);
    
    int getNumVertices();
    
    Flock3d   flock;
    ofVboMesh flockMesh;
    ofMaterial mat;
    int numBoids;
    ofColor colour;
    float shininess;
    
    
};