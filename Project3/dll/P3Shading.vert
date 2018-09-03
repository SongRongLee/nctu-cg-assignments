#version 150 compatibility

out Vertex{
	vec3 vertexEyeSpace;
	vec3 normal;
	vec2 _texcoord;
}vertex;

void main(){
    vertex.vertexEyeSpace = vec3(gl_ModelViewMatrix * gl_Vertex);
    vertex.normal = gl_Normal;
    gl_Position = gl_Vertex;
	vertex._texcoord = gl_MultiTexCoord0.xy;
}
