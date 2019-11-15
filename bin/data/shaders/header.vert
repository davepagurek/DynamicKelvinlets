#version 330
#define MAX_KELVINLETS 5

uniform mat4 modelViewProjectionMatrix;
in vec4 position;

uniform int numKelvinlets;
uniform vec3 kelvinletCenters[MAX_KELVINLETS];
uniform vec3 kelvinletForces[MAX_KELVINLETS];
uniform float kelvinletTimes[MAX_KELVINLETS];
uniform float kelvinletScales[MAX_KELVINLETS];
uniform int kelvinletTypes[MAX_KELVINLETS];
uniform float alpha;
uniform float beta;

const float M_PI = 3.14159265359;

const int IMPULSE = 0;
const int PUSH = 1;

float reg(float v, float scale) {
  return sqrt(v*v + scale*scale);
}
