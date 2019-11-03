#ifndef IMPULSEKELVINLET_H
#define IMPULSEKELVINLET_H

#include <Kelvinlet.h>

class ImpulseKelvinlet: public Kelvinlet
{
public:
  ImpulseKelvinlet(glm::vec3 Force, glm::vec3 Center, float Scale)
      : Kelvinlet(Force, Center, Scale) {}
  virtual int type() const override { return 0; }
  glm::vec3 displacement(glm::vec3 position, Material material, float t) const;
};

#endif // IMPULSEKELVINLET_H
