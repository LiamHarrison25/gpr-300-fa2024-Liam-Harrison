#version 450

layout(location = 0) in vec2 vPos;
out vec2 uv;


void main()
{
	uv = vPos;
	gl_Position = vec4(vPos * 2.0 -1.0, 0.5, 1.0);
}
