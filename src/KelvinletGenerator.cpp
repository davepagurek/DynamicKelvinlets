#define _USE_MATH_DEFINES
#include "KelvinletGenerator.h"
#include "ImpulseKelvinlet.h"

KelvinletGenerator kelvinletGenerator(const shared_ptr<DisplacedMesh>& displacedMesh) {
  // Tunable parameter: scale how much regularization is added based on the difference
  // between Kelvinlet centers
  constexpr float stretchAmount = 0.5;
  
  return [&](const vector<glm::vec3>& vertices, const vector<glm::vec3>& accelerations) {
    // assume that the mass of body is distributed equally among mesh vertices
    float mass = displacedMesh->getMesh()->getMass();
    float massPerVert = mass / vertices.size();

    // Come up with Kelvinlets given the acceleration of each vertex
    list<ImpulseKelvinlet> potentialKelvinlets;
    for (size_t i = 0; i < accelerations.size(); ++i) {
      // TODO rethink this threshold, the Kelvinlet force magnitude, the regularization, etc
      if (glm::length2(accelerations[i]) > 4000) {
        glm::vec3 force = massPerVert * accelerations[i];
        potentialKelvinlets.push_back(ImpulseKelvinlet(force, vertices[i], 0.8));
      }
    }

    // Group similar Kelvinlets together
    bool changed;
    do {
      changed = false;

      for (auto a = potentialKelvinlets.begin(); a != potentialKelvinlets.end(); ++a) {
        for (auto b = next(a); b != potentialKelvinlets.end();) {
          if (glm::distance(a->center, b->center) < (a->approxRadius(0.001) + b->approxRadius(0.001)) &&
              glm::dot(glm::normalize(a->force), glm::normalize(b->force)) > 0.5) {
            // Combine kelvinlets
            float magA = glm::length(a->force);
            float magB = glm::length(b->force);
            float volumeA = magA;
            float volumeB = magB;
            float newScale = (a->scale*volumeA + b->scale*volumeB + stretchAmount*min(volumeA, volumeB)*glm::distance(a->center, b->center))/(volumeA + volumeB);
            float newMag = magA + magB; // Preserves total volume
            a->scale = newScale;
            a->force = glm::normalize(volumeA*a->force + volumeB*b->force) * newScale;
            a->center = (volumeA*a->center + volumeB*b->center)/(volumeA + volumeB);
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
