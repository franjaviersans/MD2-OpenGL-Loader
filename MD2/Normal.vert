#version 450

uniform mat4 modelView;
uniform mat4 Projection;

layout(location = 0) in vec4 glVertex;
layout(location = 1) in vec3 glNormal;
layout(location = 2) in vec2 glTexCoord;

out vec2 Tex;

void main()
{
	Tex = glTexCoord;
	gl_Position = (Projection * modelView * glVertex);
}

