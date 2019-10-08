#include "Material.h"
#include "ofMain.h"

// Stiffness is μ in Eq. 1
// Compressibility is v in Eq. 1
Material::Material(float stiffness, float compressibility):
  beta(sqrt(stiffness)),
  alpha(beta * sqrt(1.0 + 1.0/(1.0 - 2.0*compressibility)))
{}
