#version 100

attribute vec2 texcoord;
attribute vec2 position;
attribute vec4 diffuse;

varying mediump vec2 texcoord_var;
varying lowp vec4 diffuse_var;

uniform mat3 modelviewprojection;

void main(void)
{
  texcoord_var = texcoord;
  diffuse_var = diffuse;
  gl_Position = vec4(vec3(position, 1) * modelviewprojection, 1.0);
}

/* EOF */
