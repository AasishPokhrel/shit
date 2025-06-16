#version 100

precision mediump float;

uniform sampler2D diffuse_texture;
uniform sampler2D displacement_texture;
uniform sampler2D framebuffer_texture;
uniform mat3 fragcoord2uv;
uniform float backbuffer;
uniform float game_time;
uniform vec2 animate;
uniform vec2 displacement_animate;

varying vec2 texcoord_var;
varying vec4 diffuse_var;

void main(void)
{
  if (backbuffer == 0.0)
  {
    vec4 color = diffuse_var * texture2D(diffuse_texture, texcoord_var.st + (animate * game_time));
    gl_FragColor = color;
  }
  else
  {
    vec4 pixel = texture2D(displacement_texture, texcoord_var.st + (displacement_animate * game_time));
    vec2 displacement = (pixel.rg - vec2(0.5, 0.5)) * 255.0;
    float alpha = pixel.a;

    vec2 uv = (fragcoord2uv * (gl_FragCoord.xyw + vec3(displacement.xy * alpha, 0))).xy;
    uv = vec2(uv.x, 1.0 - uv.y);
    vec4 back_color = texture2D(framebuffer_texture, uv);

    vec4 color =  diffuse_var * texture2D(diffuse_texture, texcoord_var.st + (animate * game_time));
    gl_FragColor = vec4(mix(color.rgb, back_color.rgb, alpha), color.a);
  }
}

/* EOF */
