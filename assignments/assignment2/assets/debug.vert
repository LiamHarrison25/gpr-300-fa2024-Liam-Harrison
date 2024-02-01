#version 450

layout(location = 0) in vec3 vPos;
out vec3 uv;

void main()
{
	uv = vPos;
	gl_Position = vec4(vPos.y * 2.0, -1.0, 0.5, 1.0);
}
