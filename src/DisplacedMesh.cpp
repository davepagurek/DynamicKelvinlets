#include "DisplacedMesh.h"

DisplacedMesh::DisplacedMesh(ofMesh mesh, Material material):
  mesh(mesh),
  material(material),
  currentTime(0),
  originalPositions(mesh.getVertices())
{}

void DisplacedMesh::setup() {
  shader.load("shaders/displacement-push");
}

void DisplacedMesh::update(float elapsedTime) {
  currentTime += elapsedTime;

  // TODO remove kelvinlets with no influence

  /*mesh.getVertices() = originalPositions;

  // Take the superposition of offsets from all applied Kelvinlets
  for (auto& kelvinlet_ts: kelvinlets) {
    const vector<glm::vec3>& offsets = kelvinlet_ts.displacements(material, currentTime);
    for (size_t i = 0; i < mesh.getNumVertices(); i++) {
      mesh.setVertex(i, mesh.getVertex(i) + offsets[i]);
    }
  }*/
}

void DisplacedMesh::shaderStart() const {
  shader.begin();

  shader.setUniform1f("alpha", material.alpha);
  shader.setUniform1f("beta", material.beta);
  shader.setUniform1i("numKelvinlets", kelvinlets.size());
  for (int i = 0; i < kelvinlets.size(); i++) {
    shader.setUniform3fv((string("kelvinletCenters[") + to_string(i) + "]").c_str(), glm::value_ptr(kelvinlets[i].kelvinlet->center));
    shader.setUniform3fv((string("kelvinletForces[") + to_string(i) + "]").c_str(), glm::value_ptr(kelvinlets[i].kelvinlet->force));
    shader.setUniform1f((string("kelvinletTimes[") + to_string(i) + "]").c_str(), currentTime - kelvinlets[i].t0);
    shader.setUniform1f((string("kelvinletScales[") + to_string(i) + "]").c_str(), kelvinlets[i].kelvinlet->scale);
  }
}

void DisplacedMesh::shaderEnd() const {
  shader.end();
}

void DisplacedMesh::draw() const {
  shaderStart();
  mesh.draw();
  shaderEnd();
}

void DisplacedMesh::drawWireframe() const {
  shaderStart();
  mesh.drawWireframe();
  shaderEnd();
}

const vector<glm::vec3>& DisplacedMesh::TimeShiftedKelvinlet::displacements(Material material, float t) const {
  static vector<glm::vec3> result;
  result.clear();

  // Apply displacements to every point
  for (auto& point : initialLocations) {
    result.push_back(kelvinlet->displacementRK4(point, material, t - t0));
  }

  return result;
}
