#pragma once

#include "ofMain.h"
#include "Material.h"

struct Kelvinlet {
  glm::vec3 force;
  glm::vec3 center;
  float scale;
  float approxRadius(float maxInfluence = 0.01) const {
    // The distance from the center at which the force distribution goes down to maxInfluence
    float det = pow(glm::length(force) * 15 * pow(scale, 4) / (8 * M_PI), 2.0/7.0) - scale*scale;
    if (det <= 0) return 0;
    return 0.5 * sqrt(det) / maxInfluence;
  }
  virtual int type() const = 0;
  virtual glm::vec3 displacement(glm::vec3 position, Material material, float t) const = 0;
  Kelvinlet(glm::vec3 Force, glm::vec3 Center, float Scale)
      : force(Force), center(Center), scale(Scale) {}
  glm::vec3 displacementRK4(glm::vec3 position, Material material, float t) const;
};
