/*
 >>> --- Hidden Worlds --- <<<
 openframeworks app for Creative Coding class
 Carlos Valente - Goldsmiths March 2016
 
 A magic flaslight reveals hidden creatures,
 competing swarm systems float in middair invisible from
 the unaware spectator
 
 Wrapping all boids functions for ease of use, hidding
 long stuff
 
 Flock is made of several parts
  - VBO (for point render with shader)
  - VBOmesh (for face and wireframe render)
  - points array
*/

#include "myFlock.h"

void MyFlock::setup(int numBoids_, ofSpherePrimitive bounds_, float dev_, float dt_, float pointSize_) {
    
    /* Initialize flock */
    numBoids = numBoids_;
    pointSize = pointSize_;
    float rad = bounds_.getRadius();
    ofVec3f center = bounds_.getPosition();
    flock.setup(numBoids, center, dev_);
    flock.setBounds(center, rad);
    flock.setDt(dt_);
    
    /* Add default values */
    colour = ofColor::white;
    shininess = 0.010f;
    lineWidth = .1f;
    
    /* Set Material for vboMesh */
    mat.setDiffuseColor(colour);
    mat.setAmbientColor(colour);
    mat.setShininess(shininess);
    
    /* POPULATE MESH */
    ofMesh tempMesh;
    tempMesh.clear();
    tempMesh.setupIndicesAuto(); // add index automatically
    tempMesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
    
    for (int i = 0; i < numBoids; i++){
        ofVec2f pos = flock.boids[i] -> getLoc();
        tempMesh.addVertex(pos);
        tempMesh.addTexCoord(pos);
        
        float size = ofRandom(pointSize / 2, pointSize);
        tempMesh.addNormal(ofVec3f(size));
        points.push_back(pos);
        sizes.push_back(ofVec3f(size));
    }
    
    flockMesh.setMode(OF_PRIMITIVE_TRIANGLES);
    flockMesh.setUsage(GL_STREAM_DRAW);
    
    flockMesh.clear();
    flockMesh = tempMesh;
    
    int total = (int)points.size();
    mesh.setVertexData(&points[0], total, GL_STREAM_DRAW);
    mesh.setNormalData(&sizes[0], total, GL_STREAM_DRAW);
    
    /* load the texture */
    if (CONSTANTS.bRenderSprite) {
        ofDisableArbTex();
        ofLoadImage(texture, CONSTANTS.file);
        shader.load("shaders/shader");  // shader from POINTS AS TEXTURES example
    }

}

void MyFlock::update() {
    
    /* Initialize aux */
    int total = (int)points.size();
    
    /* Update flock */
    flock.update();
    
    /* Update vboMesh and points */
    for (int i = 0; i < total; i++){
        ofVec3f point = flock.boids[i] -> getLoc();
        flockMesh.setVertex(i, point);
        points[i] = point;
    }
    
    /* Update vbo (for shader) */
    mesh.setVertexData(&points[0], total, GL_STREAM_DRAW);
    mesh.setNormalData(&sizes[0], total, GL_STREAM_DRAW);
    mesh.setVertexData(&points[0], total, GL_STREAM_DRAW);
    
}

void MyFlock::draw() {
    
    ofPushStyle();
    
        ofSetColor(colour.r, colour.g, colour.b, colour.a / 4);
        glDepthMask(GL_FALSE);
        ofEnableBlendMode(OF_BLENDMODE_ADD);
        ofEnablePointSprites();
    
        shader.begin();
            texture.bind();
                mesh.draw(GL_POINTS, 0, flockMesh.getNumVertices());
            texture.unbind();
        shader.end();
    
        ofDisablePointSprites();
        ofDisableBlendMode();
    
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




