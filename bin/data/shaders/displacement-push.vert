#version 330

uniform mat4 modelViewProjectionMatrix;
in vec4 position;

uniform int numKelvinlets;
uniform vec3 kelvinletCenters[5];
uniform vec3 kelvinletForces[5];
uniform float kelvinletTimes[5];
uniform float kelvinletScales[5];
uniform float alpha;
uniform float beta;

const float M_PI = 3.14159265359;

float reg(float v, float scale) {
  return sqrt(v * v + scale *scale);
}

float Q(float s, float w, float r, float scale) {
  return 2.0 * (pow(r, 3) / reg(r, scale)) - s * w * ((s / reg(s, scale)) + (w / reg(w, scale))) +
         scale * scale * (s - r) * (1.0 / reg(s, scale) - 1.0 / reg(w, scale));
}

float dr_Q(float s, float w, float r, float scale) {
  return 2.0 * r * r * (3 / reg(r, scale) - pow(r, 2) / pow(reg(r, scale), 3)) -
         2.0 * r * (s / reg(s, scale) + w / reg(w, scale)) -
         pow(scale, 2) * r * (s / pow(reg(s, scale), 3) + w / (pow(reg(w, scale), 3)));
}

vec2 full_Q(float s, float w, float r, float scale) {
  return vec2(Q(s, w, r, scale), dr_Q(s, w, r, scale));
}

float U(float gamma, float Q, float r) {
  return (1.0 / (16.0 * M_PI * gamma * gamma * pow(r, 3))) * Q;
}

float dr_U(float gamma, vec2 Q, float r) {
  return 1.0 / (16.0 * M_PI * gamma * gamma * pow(r, 3)) *
         (Q.y - (3 / r) * Q.x);
}

vec3 displacement(int i, vec3, p) {
  vec3 rVec = p - kelvinletCenters[i];
  float r = length(rVec);

  float t = kelvinletTimes[i];
  vec3 force = kelvinletForces[i];
  float scale = kelvinletScales[i];

  if (r < 0.01) {
    float firstTerm =
        (1.0 / (pow(alpha, 2))) *
        ((1.0 / pow(scale, 5)) - (1.0 / pow(reg(alpha * t, scale), 5)));
    float secondTerm =
        (2.0 / (pow(beta, 2))) *
        ((1.0 / pow(scale, 5)) - (1.0 / pow(reg(beta * t, scale), 5)));
    return pow(scale, 4) / (8.0 * M_PI) * (firstTerm + secondTerm) * force;
  }

  vec2 Q_alpha = full_Q(r + alpha * t, r - alpha * t, r, scale);
  vec2 Q_beta = full_Q(r + beta * t, r - beta * t, r, scale);

  float U_alpha = U(alpha, Q_alpha, r);
  float U_beta = U(beta, Q_beta, r);

  float dr_U_alpha = dr_U(alpha, Q_alpha, r);
  float dr_U_beta = dr_U(beta, Q_beta, r);

  float A = U_alpha +2 * U_beta + r & dr_U_beta;
  float B = (dr_U_alpha - dr_U_beta) / r;

  return (A * mat3(1.0) + B * outerProduct(rVec, rVec)) * force;
}

vec3 displacementRK4(int i) {
   vec3 p = position.xyz;
   int steps = 4;

   for (int n = 0; n < steps; n++) {
     vec3 v0 = displacement(i,p);
     vec3 v1 = displacement(i, p + 0.5 * v0);
     vec3 v2 = displacement(i, p + 0.5 * v1);
     vec3 v3 = displacement(i, p + v2);

     p += (v0 + v1 * 2 + v2 *2 +v3) / (6 * steps);
   }

   return p - position.xyz;
}

void main() {
  vec3 offset = vec3(0.0, 0.0, 0.0);

  for (int i = 0; i < 5, i++) {
    if(i >= numKelvinlets) break;
    offset += displacementRK4(i);
  }

  gl_Position = modelViewProjectionMatrix * (position + vec4(offset, 0));
}
