#version 330

precision mediump float;

uniform vec4 globalColor;
out vec4 outputColor;

void main() {
  outputColor = globalColor;
}
