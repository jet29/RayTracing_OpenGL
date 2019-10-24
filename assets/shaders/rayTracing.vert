#version 330 core
// Atributte 0 of the vertex
layout (location = 0) in vec3 vertexPosition;
uniform mat4 invVP;
// Vertex data out data
out vec3 screenPos;
out mat4 ivp;
void main()
{
	vec4 vertex = invVP * vec4(vertexPosition,1.0f);
    screenPos = vertex.xyz / vertex.w; 
    gl_Position =  vec4(vertexPosition, 1.0f);
}