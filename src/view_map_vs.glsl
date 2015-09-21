#version 130

uniform mat4 transform;
in  vec3 pos;
in  vec4 color;
out vec4 color_terp;

void main() {
  gl_Position = vec4(pos, 1.0)*transform;
  color_terp = color;
}
