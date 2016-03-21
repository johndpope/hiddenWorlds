/*
 >>> --- Hidden Worlds --- <<<
 openframeworks app for Creative Coding class
 Carlos Valente - Goldsmiths March 2016
 
 A magic flaslight reveals hidden creatures,
 competing swarm systems float in middair invisible from
 the unaware spectator
 
 Wrapping all boids functions for ease of use
 */

#include "myFlock.h"

void MyFlock::setup(int numBoids_, ofSpherePrimitive bounds_, float dev_, float dt_ ) {
    
    numBoids = numBoids_;
    float rad = bounds_.getRadius();
    ofVec3f center = bounds_.getPosition();
    flock.setup(numBoids, center, dev_);
    
    
    
    
//    flock.setBounds(min, min, min, 1000, 1000, 1000); //temporary for box behaviour
//    flock.setBounds(0,0,-ofGetWidth(),ofGetWidth(), ofGetHeight(),0);
    flock.setBounds(center, rad);
    flock.setDt(dt_);
    
    
    /* POPULATE MESH */
    ofMesh tempMesh;
    tempMesh.clear();
    tempMesh.setMode(OF_PRIMITIVE_TRIANGLES);
    
    for (int i = 0; i < numBoids; i++){
        tempMesh.addVertex(flock.boids[i]->getLoc());
        tempMesh.addIndex(i);
    }
    flockMesh.setMode(OF_PRIMITIVE_TRIANGLES);
    
    flockMesh.clear();
    flockMesh = tempMesh;

    
}
void MyFlock::update() {
    
    flock.update();
    for (int i = 0; i < numBoids; i++){
        ofVec3f point = flock.boids[i] -> getLoc();
        flockMesh.setVertex(i, point);
    }
    

}
void MyFlock::draw(bool bDrawVertices = true, bool bDrawWire = false, bool bDrawFaces = false, bool bDrawBlob = false) {

    if (bDrawVertices) flockMesh.drawVertices();
    if (bDrawWire)     flockMesh.drawWireframe();
    if (bDrawFaces)    flockMesh.drawFaces();
    
    if (bDrawBlob) {
        ofPolyline blob;
        blob.clear();
        blob.addVertices(flock.getPoints());
        blob.setClosed(false);
        ofPolyline smooth;
        blob = blob.getResampledByCount(200).getSmoothed(8);
        blob.draw();
    }

}


