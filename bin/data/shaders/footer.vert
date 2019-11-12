void main() {
  vec3 offset = vec3(0.0, 0.0, 0.0);
  
  for (int i = 0; i < MAX_KELVINLETS; i++) {
    if (i >= numKelvinlets) break;
    
    if (kelvinletTypes[i] == IMPULSE) {
      offset += Impulse_displacementRK4(i);
    } else if (kelvinletTypes[i] == PUSH) {
      offset += Push_displacementRK4(i);
    }
  }
  
  gl_Position = modelViewProjectionMatrix * (position + vec4(offset, 0));
}
