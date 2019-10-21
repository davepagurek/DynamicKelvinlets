#include "DisplacedMesh.h"

DisplacedMesh::DisplacedMesh(ofMesh mesh, Material material):
  mesh(mesh),
  material(material),
  currentTime(0),
  originalPositions(mesh.getVertices())
{}

void DisplacedMesh::update(float elapsedTime) {
  currentTime += elapsedTime;

  // TODO remove kelvinlets with no influence
  
  mesh.getVertices() = originalPositions;
  
  // Take the superposition of offsets from all applied Kelvinlets
  for (auto& kelvinlet_ts: kelvinlets) {
    vector<glm::vec3> offsets = kelvinlet_ts.displacements(material, currentTime);
    for (size_t i = 0; i < mesh.getNumVertices(); i++) {
      mesh.setVertex(i, mesh.getVertex(i) + offsets[i]);
    }
  }
}

void DisplacedMesh::addKelvinlet(const ImpulseKelvinlet kelvinlet) {
  kelvinlets.push_back(DisplacedMesh::TimeShiftedKelvinlet{
    .kelvinlet=kelvinlet,
    .t0=currentTime,
    .initialLocations=mesh.getVertices()
  });
}

void DisplacedMesh::draw() const {
  mesh.draw();
}

void DisplacedMesh::drawWireframe() const {
  mesh.drawWireframe();
}

vector<glm::vec3> DisplacedMesh::TimeShiftedKelvinlet::displacements(Material material, float t) const {
  vector<glm::vec3> result;
  // Apply displacements to every point
  for (auto& point : initialLocations) {
    result.push_back(kelvinlet.displacementRK4(point, material, t - t0));
  }
  
  return result;
}
