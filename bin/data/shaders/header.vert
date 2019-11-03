#version 330

uniform mat4 modelViewProjectionMatrix;
in vec4 position;

uniform int numKelvinlets;
uniform vec3 kelvinletCenters[5];
uniform vec3 kelvinletForces[5];
uniform float kelvinletTimes[5];
uniform float kelvinletScales[5];
uniform int kelvinletTypes[5];
uniform float alpha;
uniform float beta;

const float M_PI = 3.14159265359;

const int IMPULSE = 0;
const int PUSH = 1;

float reg(float v, float scale) {
  return sqrt(v*v + scale*scale);
}
