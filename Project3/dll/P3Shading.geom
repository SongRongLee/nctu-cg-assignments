#version 150 compatibility

uniform int Level;
uniform float Radius;
uniform vec4 Centroid;

layout(triangles) in;
layout(triangle_strip, max_vertices=85) out;

in Vertex{
	vec3 vertexEyeSpace;
	vec3 normal;
	vec2 _texcoord;
}vertex[3];

out vec3 vertexEyeSpace;
out vec3 normal;
out vec2 _texcoord;

vec3 V0, V01, V02;
vec3 N0, N01, N02;
vec2 t0, t01, t02;
void GenVertex(float s, float t)
{
	//interpolations
	vec3 v = V0 + s*V01 + t*V02;
	vec3 N = N0 + s*N01 + t*N02;
	vec2 T = t0 + s*t01 + t*t02;
	v = normalize(v-Centroid.xyz);
	vec3 tmppos = Radius*v + Centroid.xyz;
	vec4 actual_position = gl_ModelViewMatrix * (vec4(tmppos, 1));
	
	//set outputs
	normal = normalize(gl_NormalMatrix * v);
	gl_Position = gl_ProjectionMatrix * actual_position;
	vertexEyeSpace = actual_position.xyz;
	_texcoord = T;
	EmitVertex();
}

void main(){
	//init st coord.
	V01 = (gl_in[1].gl_Position - gl_in[0].gl_Position).xyz;
	V02 = (gl_in[2].gl_Position - gl_in[0].gl_Position).xyz;
	V0 = gl_in[0].gl_Position.xyz;
	
	t01 = (vertex[1]._texcoord - vertex[0]._texcoord).xy;
	t02 = (vertex[2]._texcoord - vertex[0]._texcoord).xy;
	t0 = vertex[0]._texcoord.xy;
	
	N01 = (vertex[1].normal - vertex[0].normal).xyz;
	N02 = (vertex[2].normal - vertex[0].normal).xyz;
	N0 = vertex[0].normal.xyz;
	
	int num_layers = 1 << Level;
	float dt = 1/float(num_layers);
	float t_top = 1;
	for(int it = 0; it < num_layers; it++)
	{
		float t_bot = t_top - dt;
		float smax_top = 1 - t_top;
		float smax_bot = 1 - t_bot;
		float ds_top = smax_top / float(it);
		float ds_bot = smax_bot / float(it + 1);
		float s_top = 0;
		float s_bot = 0;
		for(int is = 0; is < it + 1; is++)
		{
			GenVertex(s_bot, t_bot);
			GenVertex(s_top, t_top);
			s_top += ds_top;
			s_bot += ds_bot;
		}
		GenVertex(s_bot, t_bot);
		EndPrimitive();
		t_top = t_bot;
		t_bot -= dt;
	}
}
