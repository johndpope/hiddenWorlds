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
    colour = ofColor::white;
    shininess = 1.0f;
    
    mat.setDiffuseColor(colour);
    mat.setAmbientColor(colour);
//    mat.setShininess(shininess);
    
    flock.setBounds(center, rad);
    flock.setDt(dt_);
    
    /* POPULATE MESH */
    ofMesh tempMesh;
    tempMesh.clear();
    tempMesh.setupIndicesAuto(); // add index automatically
    tempMesh.setMode(OF_PRIMITIVE_TRIANGLES);
    
    for (int i = 0; i < numBoids; i++){
        tempMesh.addVertex(flock.boids[i]->getLoc());
    }
    flockMesh.setMode(OF_PRIMITIVE_TRIANGLES);
    flockMesh.setUsage(GL_STREAM_DRAW);
    
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
    
    ofPushStyle();
    ofSetColor(colour);
    mat.begin();
    if (bDrawVertices) flockMesh.drawVertices();
    ofSetColor(ofColor::white);
    if (bDrawWire)     flockMesh.drawWireframe();
    ofSetColor(colour);
    if (bDrawFaces)    flockMesh.drawFaces();
    mat.end();
    
    if (bDrawBlob) {
        ofPolyline blob;
        blob.clear();
        blob.addVertices(flock.getPoints());
        blob.setClosed(false);
        ofPolyline smooth;
        blob = blob.getResampledByCount(200).getSmoothed(8);
        blob.draw();
    }
    ofPopStyle();

}



void MyFlock::setColor(int r, int g, int b, float shiny) {
    colour = ofColor(r, g, b);
    mat.setDiffuseColor(colour);
    mat.setAmbientColor(colour);
//    mat.setShininess(shiny);
}


void MyFlock::setAttractor(ofVec3f loc, float force, float sensorDist){
    
    flock.addAttractionPoint(loc, force, sensorDist);
    
}



int MyFlock::getNumVertices(){
    
    return flockMesh.getNumVertices();
    
}



