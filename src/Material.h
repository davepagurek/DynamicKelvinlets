#pragma once

struct Material {
  Material(float stiffness, float compressibility);
  
  float beta;
  float alpha;
};
