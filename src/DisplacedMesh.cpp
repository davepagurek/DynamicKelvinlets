#include "DisplacedMesh.h"
#include "constants.h"

string slurp(string path) {
  return ofFile(path).readToBuffer().getText();
}

void DisplacedMesh::setup() {
  if (USE_SHADER) {
    ofShaderSettings settings;
    
    string vertexShaderSource =
      slurp("shaders/header.vert") +
      slurp("shaders/displacement-impulse.vert") +
      slurp("shaders/displacement-push.vert") +
      slurp("shaders/footer.vert");
    string fragmentShaderSource = slurp("shaders/displacement.frag");
    
    settings.shaderSources[GL_VERTEX_SHADER] = vertexShaderSource;
    settings.shaderSources[GL_FRAGMENT_SHADER] = fragmentShaderSource;
    settings.intDefines["MAX_KELVINLETS"] = MAX_KELVINLETS;
    shader.setup(settings);
  }
  
  forceMesh = ofMesh::cone(0.2, 0.2, 3, 2);
}

void DisplacedMesh::update(float elapsedTime) {
  currentTime += elapsedTime;
  mesh->update(currentTime);

  // Remove old Kelvinlets
  for (auto it = kelvinlets.begin(); it != kelvinlets.end();) {
    if (currentTime - it->t0 > 2 && it->kelvinlet->type() == 0) {
      it = kelvinlets.erase(it);
    } else {
      ++it;
    }
  }
  cout << kelvinlets.size() << endl;

  if (!USE_SHADER) {
    mesh->getVertices() = mesh->getOriginalVertices();
    // Take the superposition of offsets from all applied Kelvinlets
    for (auto& kelvinlet_ts: kelvinlets) {
      const vector<glm::vec3>& offsets = kelvinlet_ts.displacements(material, currentTime, mesh->getOriginalVertices());
      for (size_t i = 0; i < mesh->getOriginalVertices().size(); i++) {
        mesh->getVertices()[i] += offsets[i];
      }
    }
  }
}

void DisplacedMesh::shaderStart() const {
  shader.begin();

  shader.setUniform1f("alpha", material.alpha);
  shader.setUniform1f("beta", material.beta);
  shader.setUniform1i("numKelvinlets", kelvinlets.size());
  
  static vector<int> types;
  static vector<float> centers;
  static vector<float> forces;
  static vector<float> times;
  static vector<float> scales;
  
  types.clear();
  centers.clear();
  forces.clear();
  times.clear();
  scales.clear();
  
  int i = 0;
  for (auto& k : kelvinlets) {
    types.push_back(k.kelvinlet->type());
    for (size_t i : {0, 1, 2}) centers.push_back(k.kelvinlet->center[i]);
    for (size_t i : {0, 1, 2}) forces.push_back(k.kelvinlet->force[i]);
    times.push_back(currentTime - k.t0);
    scales.push_back(k.kelvinlet->scale);
    
    ++i;
    if (i >= MAX_KELVINLETS) break;
  }
  
  shader.setUniform1iv("kelvinletTypes", types.data(), kelvinlets.size());
  shader.setUniform3fv("kelvinletCenters", centers.data(), kelvinlets.size());
  shader.setUniform3fv("kelvinletForces", forces.data(), kelvinlets.size());
  shader.setUniform1fv("kelvinletTimes", times.data(), kelvinlets.size());
  shader.setUniform1fv("kelvinletScales", scales.data(), kelvinlets.size());
}

void DisplacedMesh::shaderEnd() const {
  shader.end();
}

void DisplacedMesh::draw() {
  if (USE_SHADER) shaderStart();
  mesh->draw();
  if (USE_SHADER) shaderEnd();
//  cout << ofGetFrameRate() << endl;
}

void DisplacedMesh::drawWireframe() {
  if (USE_SHADER) shaderStart();
  mesh->drawWireframe();
  if (USE_SHADER) shaderEnd();
}

void DisplacedMesh::drawForces() {
  for (const auto& k : kelvinlets) {
    ofPushMatrix();
    ofTranslate(k.kelvinlet->center);
    ofMultMatrix(glm::toMat4(glm::rotation({0, 1, 0}, glm::normalize(k.kelvinlet->force))));
    ofScale(glm::length(k.kelvinlet->force));
    forceMesh.draw();
    ofPopMatrix();
  }
}

glm::vec3 DisplacedMesh::TimeShiftedKelvinlet::displacement(Material material, float t, const glm::vec3& position) const {
  return kelvinlet->displacementRK4(position, material, t - t0);
}

const vector<glm::vec3>& DisplacedMesh::TimeShiftedKelvinlet::displacements(Material material, float t, const vector<glm::vec3>& vertices) const {
  static vector<glm::vec3> result;
  result.clear();

  // Apply displacements to every point
  for (auto& point : vertices) {
    result.push_back(displacement(material, t, point));
  }

  return result;
}
