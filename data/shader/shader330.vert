#version 330 core

in vec2 texcoord;
in vec2 position;
in vec4 diffuse;

out vec2 texcoord_var;
out vec4 diffuse_var;

uniform mat3 modelviewprojection;

void main(void)
{
  texcoord_var = texcoord;
  diffuse_var = diffuse;
  gl_Position = vec4(vec3(position, 1) * modelviewprojection, 1.0);
}

/* EOF */
