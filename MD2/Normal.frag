#version 450

layout(binding = 0) uniform sampler2D Image;


in vec2 Tex;

layout(location = 0) out vec4 FragColor;

void main()
{
	//FragColor = vec4(Tex.xy,0.0f, 1.0f);
	FragColor = vec4(texture(Image, Tex.xy).rgb,1.0f);
	//FragColor = vec4(1.0f,0.0f,1.0f,1.0f);
}
