#include "KelvinletGenerator.h"
#include "ImpulseKelvinlet.h"

KelvinletGenerator kelvinletGenerator(const shared_ptr<DisplacedMesh>& displacedMesh) {
  return [&](const vector<glm::vec3>& vertices, const vector<glm::vec3>& accelerations) {
    // Need at least 3 frames to have enough data to calculate accelerations
    if (ofGetFrameNum() < 3)
      return;

    // Come up with Kelvinlets given the acceleration of each vertex
    list<ImpulseKelvinlet> potentialKelvinlets;
    for (size_t i = 0; i < accelerations.size(); ++i) {
      // TODO rethink this threshold, the Kelvinlet force magnitude, the regularization, etc
      if (glm::length2(accelerations[i]) > 4000) {
        glm::vec3 force = 0.0005 * accelerations[i];
        potentialKelvinlets.push_back(ImpulseKelvinlet(force, vertices[i], 0.2));
      }
    }

    // Group similar Kelvinlets together
    bool changed;
    do {
      changed = false;

      for (auto a = potentialKelvinlets.begin(); a != potentialKelvinlets.end(); ++a) {
        for (auto b = next(a); b != potentialKelvinlets.end();) {
          if (glm::distance(a->center, b->center) / (a->scale + b->scale) < 1) {
            // Combine kelvinlets
            // TODO do this in a less arbitrary way
            float weight = a->scale / (a->scale + b->scale);
            a->scale = max(a->scale, glm::distance(a->center, b->center) + b->scale);
            a->force = a->force + b->force / max(1.f, glm::distance(a->center, b->center));
            a->center = weight * a->center + (1 - weight) * b->center;
            b = potentialKelvinlets.erase(b);
          } else {
            ++b;
          }
        }
      }
    } while (changed);

    // Apply the merged Kelvinlets
    for (const auto& k : potentialKelvinlets)
      displacedMesh->addKelvinlet(k);
  };
}
