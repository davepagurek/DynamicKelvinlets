#include "AnimatedMesh.h"
#include "constants.h"

AnimatedMesh::AnimatedMesh(const ofxAssimpModelLoader &loaderMesh,
                           const float mass, const KelvinletGenerator &callback,
                           AnimatedMesh::LoopType loopType)
    : smoothLoop(loopType == AnimatedMesh::LoopType::CONTINUOUS),
      original(loaderMesh), callback(callback) {
  mesh.addVertices(original.getCurrentAnimatedMesh(0).getVertices());
  mesh.addIndices(original.getCurrentAnimatedMesh(0).getIndices());
  setMass(mass);
  original.playAllAnimations();
  original.setPausedForAllAnimations(true);
  original.update();
}
void AnimatedMesh::update(float t) {
  float oldPos = original.getAnimation(0).getPosition();
  auto& animation = original.getAnimation(0);
  animation.setPosition(fmod(t / animation.getDurationInSeconds(), 1.f));
  original.update();
  float newPos = original.getAnimation(0).getPosition();
  
  // If the animation just looped without continuity, don't count old frames for accelerations
  if (newPos < oldPos && !smoothLoop) {
    numFrames = 0;
  }
  
  if (currentIndex == -1) {
    for (int i = vertices.size()-1; i > 0; --i) {
      times[i] = t - (vertices.size() - i) * 0.01;
      vertices[i] = getOriginalVertices();
    }
  }
  currentIndex = (currentIndex + 1) % vertices.size();
  times[currentIndex] = t;
  vertices[currentIndex] = getOriginalVertices();
  if (numFrames < vertices.size()) numFrames++;
  
  // Don't call the acceleration callback if there isn't enough data to calculate accelerations
  if (numFrames == 3) {
  
    int prevIndex = (currentIndex - 1 + vertices.size()) % vertices.size();
    int twicePrevIndex = (currentIndex - 2 + vertices.size()) % vertices.size();
    
    vector<glm::vec3> accelerations(vertices[currentIndex].size());
    for (int i = 0; i < vertices[currentIndex].size(); ++i) {
      auto& current = vertices[currentIndex][i];
      auto& prev = vertices[prevIndex][i];
      auto& twicePrev = vertices[twicePrevIndex][i];
      
      auto currentVelocity = (current - prev)/(times[currentIndex] - times[prevIndex]);
      auto prevVelocity = (prev - twicePrev)/(times[prevIndex] - times[twicePrevIndex]);
      
      accelerations[i] = (currentVelocity - prevVelocity)/(times[currentIndex] - times[prevIndex]);
    }
  
    callback(vertices[currentIndex], accelerations);
  }
}
void AnimatedMesh::draw() {
  if (USE_SHADER) {
    original.drawFaces();
  } else {
    mesh.draw();
  }
}
void AnimatedMesh::drawWireframe() {
  if (USE_SHADER) {
    original.drawWireframe();
  } else {
    mesh.drawWireframe();
  }
}
vector<glm::vec3>& AnimatedMesh::getVertices() {
  return mesh.getVertices();
}
vector<glm::vec3> AnimatedMesh::getOriginalVertices() {
  return original.getCurrentAnimatedMesh(0).getVertices();
}


AnimatedMesh::TransformFreeScene::TransformFreeScene(const ofxAssimpModelLoader& other): ofxAssimpModelLoader(other) {}
void AnimatedMesh::TransformFreeScene::update() {
  ofxAssimpModelLoader::update();
  modelMeshes[0].matrix.makeIdentityMatrix();
  modelMatrix.makeIdentityMatrix();
}
