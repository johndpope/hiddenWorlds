/*
 >>> --- Hidden Worlds --- <<<
 openframeworks app for Creative Coding class
 Carlos Valente - Goldsmiths March 2016
 
 A magic flaslight reveals hidden creatures,
 competing swarm systems float in middair invisible from
 the unaware spectator
 
 Wrapping all boids functions for ease of use, hidding
 long stuff
*/

#include "myFlock.h"

void MyFlock::setup(int numBoids_, ofSpherePrimitive bounds_, float dev_, float dt_ ) {
    
    numBoids = numBoids_;
    float rad = bounds_.getRadius();
    ofVec3f center = bounds_.getPosition();
    flock.setup(numBoids, center, dev_);
    colour = ofColor::white;
    shininess = 0.010f;
    
    pointSize = 3.0f;
    lineWidth = .1f;
    
    mat.setDiffuseColor(colour);
    mat.setAmbientColor(colour);
    mat.setShininess(shininess);
    
    flock.setBounds(center, rad);
    flock.setDt(dt_);
    
    /* POPULATE MESH */
    ofMesh tempMesh;
    tempMesh.clear();
    tempMesh.setupIndicesAuto(); // add index automatically
    tempMesh.setMode(OF_PRIMITIVE_TRIANGLES);
    
    for (int i = 0; i < numBoids; i++){
        ofVec2f pos = flock.boids[i]->getLoc();
        tempMesh.addVertex(pos);
        tempMesh.addTexCoord(pos);
    }
    
    flockMesh.setMode(OF_PRIMITIVE_TRIANGLES);
    flockMesh.setUsage(GL_STREAM_DRAW);
    
    flockMesh.clear();
    flockMesh = tempMesh;
    
    // load the texure
    if (CONSTANTS.bRenderSprite) {
        ofEnablePointSprites();         // will let me render a point with a sprite
        ofDisableArbTex();
        ofLoadImage(texture, CONSTANTS.file);
    }
    // note that we disable arb tex, meaning we will use normalized texture coordinates,
    // where a texture's x and y coordinate are each expressed as a normalized float.
    // this makes things slightly easier in the shader.
    

}

void MyFlock::update() {
    
    flock.update();
    for (int i = 0; i < numBoids; i++){
        ofVec3f point = flock.boids[i] -> getLoc();
        flockMesh.setVertex(i, point);
    }
    
}

void MyFlock::draw() {
    
    
    ofPushStyle();
    
        /* Draw Vertices */
        glPointSize(pointSize);
        ofSetColor(colour);
    
        if (CONSTANTS.bRenderSprite) texture.bind();
        mat.begin();
    
        flockMesh.drawVertices();
    
        mat.end();
        if (CONSTANTS.bRenderSprite) texture.unbind();
    
    ofPopStyle();
}
    


void MyFlock::draw(bool bDrawWire, bool bDrawFaces, bool bDrawBlob) {
    
    float alpha = 255;
    ofPushStyle();
    
        mat.begin();
    
        /* Draw Lines */
        glLineWidth(lineWidth);
        ofSetColor(colour, alpha);
    
        if (bDrawWire) flockMesh.drawWireframe();

        /* Draw Faces */
        ofSetColor(colour, alpha);
        if (bDrawFaces) flockMesh.drawFaces();
    
        mat.end();
    
        /* Draw Blob */
        ofSetColor(colour, alpha);
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
    mat.setShininess(shiny);
}


void MyFlock::setRenderColour(ofColor colVert, ofColor colWire, ofColor colFaces, ofColor colBlob) {
    
   // NOTHING YET
}


void MyFlock::setAttractor(ofVec3f loc, float force, float sensorDist){
    
    flock.addAttractionPoint(loc, force, sensorDist);
    
}

void MyFlock::setDrawValues(float pointSize_, float lineWidth_){
    
    pointSize = pointSize_;
    lineWidth = lineWidth_;
    
}


int MyFlock::getNumVertices(){
    
    return flockMesh.getNumVertices();
    
}

void MyFlock::setAlign(float align){
    flock.setAlign(align);
}
void MyFlock::setAlignDist(float dist){
    flock.setDistAlign(dist);
}

void MyFlock::setAttraction(float att){
    flock.setAttraction(att);
}

void MyFlock::setAttractionDev(float dev){
    flock.setAttractionDev(dev);
}


void MyFlock::setCohesion(float coh){
    flock.setCohesion(coh);
    
}
void MyFlock::setCohesionDist(float dist){
    flock.setDistCohesion(dist);
    
}

void MyFlock::setSeparation(float sep){
    flock.setSeparate(sep);
    
}
void MyFlock::setSeparationDist(float dist){
    flock.setDistSeparation(dist);
}

void MyFlock::setDt(float dt){
    flock.setDt(dt);
    
}
void MyFlock::setMaxForce(float force){
    flock.setMaxForce(force);
    
}
void MyFlock::setMaxSpeed(float speed){
    flock.setMaxSpeed(speed);
    
}
void MyFlock::setMaxTurn(float turn){
    flock.setMaxTurn(turn);
    
}




