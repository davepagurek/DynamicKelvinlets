#include "KelvinletGenerator.h"
#include "ImpulseKelvinlet.h"

KelvinletGenerator kelvinletGenerator(const shared_ptr<DisplacedMesh>& displacedMesh) {
  return [&](const vector<glm::vec3>& vertices, const vector<glm::vec3>& accelerations) {
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
          if (glm::distance(a->center, b->center) < (a->approxRadius(0.1) + b->approxRadius(0.1)) &&
              glm::dot(glm::normalize(a->force), glm::normalize(b->force)) > 0.5) {
            // Combine kelvinlets
            float magA = glm::length(a->force);
            float magB = glm::length(b->force);
            float newScale = (a->scale*magA + b->scale*magB + min(magA, magB)*glm::distance(a->center, b->center))/(magA + magB);
            float newMag = newScale*newScale * (magA/(a->scale*a->scale) + magB/(b->scale*b->scale));
            a->scale = newScale;
            a->force = glm::normalize(magA*a->force + magB*b->force) * newScale;
            a->center = (magA*a->center + magB*b->center)/(magA + magB);
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
