R""(
#define M_PI	3.1415926535897932384626433832795
#define PI		M_PI
#define EPSILON	0.0001

precision lowp float;
precision lowp int;
precision lowp sampler2D;
precision lowp samplerCube;

struct t_Environment {
	samplerCube	Diffuse;
	samplerCube	Irradiance;
};

#ifdef LIGHTSHADER
struct t_BackTextures {
	sampler2D	Color;
	sampler2D	Emitting;
	sampler2D	Normal;
};
#endif //LIGHTSHADER

struct t_Textures {
	sampler2D		CDiff;
	sampler2D		Emitting;
	sampler2D		F0;
	sampler2D		Ior;
	sampler2D		AO;
	sampler2D		Normal;
	sampler2D		Depth;
	sampler2D		BRDF;
	t_Environment	Environment;
#ifdef LIGHTSHADER
	t_BackTextures	Back;
#endif //LIGHTSHADER
};

struct t_BRDF {
	vec3	CDiff;
	vec3	F0;
	float	Alpha;
};

struct t_Frag {
	float		Depth;
	vec2		UV;
	vec3		CubeUV;
	vec3		Position;
	vec3		Normal;
	vec3		Emitting;
	float		Ior;
	float		Opacity;
	float		AO;
	//t_Material	Material;
	t_BRDF		BRDF;
};

struct t_CameraMatrix {
	mat4	View;
	mat4	Projection;
};

struct t_Camera {
	vec3			Position;
	t_CameraMatrix	Matrix;
	t_CameraMatrix	InvMatrix;
};

#ifdef LIGHTSHADER
struct t_Out {
	vec4		Color;
	vec3		Emitting;
};

layout(location = 0) out vec4	out_Color;
layout(location = 1) out vec3	out_Emitting;
t_Out	Out;
#endif //LIGHTSHADER

uniform t_Textures		Texture;
uniform t_Camera		Camera;
uniform vec3			Resolution;
uniform float			Time;
uniform uint			FrameNumber;

in vec2				frag_UV;
in vec3				frag_Cube_UV;

#ifdef POSTSHADER
struct t_Out {
	t_Material	Material;
	float		Depth;
	vec3		Normal;
};

layout(location = 0) out vec4	out_CDiff; //BRDF CDiff, Transparency
layout(location = 1) out vec3	out_Emitting;
layout(location = 2) out vec4	out_F0; //BRDF F0, BRDF Alpha
layout(location = 3) out float	out_Ior;
layout(location = 4) out float	out_AO;
layout(location = 5) out vec2	out_Normal;
#endif //POSTSHADER

t_Frag	Frag;

mat3x3	tbn_matrix()
{
	vec3 Q1 = dFdx(Frag.Position);
	vec3 Q2 = dFdy(Frag.Position);
	vec2 st1 = dFdx(Frag.UV);
	vec2 st2 = dFdy(Frag.UV);
	vec3 T = normalize(Q1*st2.t - Q2*st1.t);
	vec3 B = normalize(-Q1*st2.s + Q2*st1.s);
	return(transpose(mat3(T, B, Frag.Normal)));
}

//Generate Pseudo random numbers using TEA (Tiny Encription Algorithm)
uvec2 ScrambleTEA(uvec2 v, uint IterationCount)
{
	// Start with some random data (numbers can be arbitrary but those have been used by others and seem to work well)
	uint k[4] ={ 0xA341316Cu , 0xC8013EA4u , 0xAD90777Du , 0x7E95761Eu };
	
	uint y = v[0];
	uint z = v[1];
	uint sum = 0;
	
	for(uint i = 0; i < IterationCount; ++i)
	{
		sum += 0x9e3779b9;
		y += (z << 4u) + k[0] ^ z + sum ^ (z >> 5u) + k[1];
		z += (y << 4u) + k[2] ^ y + sum ^ (y >> 5u) + k[3];
	}

	return uvec2(y, z);
}

uvec2 ScrambleTEA(uvec2 v)
{
	return ScrambleTEA(v, 3);
}

float	random(in vec2 seed, in float freq)
{
	float dt = dot(floor(seed * freq), vec2(53.1215, 21.1352));
	return fract(sin(dt) * 2105.2354);
}

float	random(in vec3 seed, in float freq)
{
	float dt = dot(floor(seed * freq), vec3(53.1215, 21.1352, 9.1322));
	return fract(sin(dt) * 2105.2354);
}

float	random(in vec2 seed)
{
	return random(seed, 4096);
}

float	random(in vec3 seed)
{
	return random(seed, 4096);
}

float	randomAngle(in vec2 seed)
{
	return random(seed) * 6.283285;
}

float	randomAngle(in vec3 seed)
{
	return random(seed) * 6.283285;
}

#ifndef textureQueryLevels
float compMax(vec3 v) { return max(max(v.x, v.y), v.z); }
float compMax(vec2 v) { return max(v.x, v.y); }
#define textureQueryLevels(tex) int(log2(compMax(textureSize(tex, 0))))
#endif

#define sampleLod(tex, uv, lod) textureLod(tex, uv, lod * textureQueryLevels(tex))

vec4	texelFetchLod(in sampler2D tex, in vec2 uv, in float mipLevel)
{
	ivec2	Resolution = textureSize(tex, 0);
	return mix(texelFetch(tex, ivec2(Resolution * uv), int(floor(mipLevel))), texelFetch(tex, ivec2(Resolution * uv), int(ceil(mipLevel))), fract(mipLevel));
}

vec4	texelFetchLod(in sampler2D tex, in vec2 uv, in int mipLevel)
{
	return texelFetch(tex, ivec2(textureSize(tex, 0) * uv), mipLevel);
}

#define map(value, low1, high1, low2, high2) (low2 + (value - low1) * (high2 - low2) / (high1 - low1))

float	smootherstep(float edge0, float edge1, float x) {
	x = clamp((x - edge0) / (edge1 - edge0), 0.0, 1.0);
	return x * x * x * (x * (x * 6 - 15) + 10);
}

bool	isZero(in float v)
{
	return (abs(v) < EPSILON);
}

bool	isZero(in vec2 v)
{
	return all(equal(v, vec2(0)));
}

#define lequal(a, b) all(lessThanEqual(a, b))

float Luminance(vec3 LinearColor)
{
	return dot(LinearColor, vec3(0.299, 0.587, 0.114));
}

vec4	WorldToView(in vec3 position)
{
	return Camera.Matrix.View * vec4(position, 1);
}

vec4	ViewToClip(in vec4 position)
{
	return Camera.Matrix.Projection * position;
}

vec4	ClipToScreen(in vec4 position)
{
	position.xyz /= position.w;
	position.xyz = position.xyz * 0.5 + 0.5;
	return position;
}

/** Returns the World position and the scene depth in world units */
vec4	WorldToScreen(in vec3 position)
{
	return ClipToScreen(ViewToClip(WorldToView(position)));
}

vec4	WorldToClip(in vec3 position)
{
	return ViewToClip(WorldToView(position));
}

vec3 TangentToWorld(in vec3 vec)
{
	return normalize(vec * tbn_matrix());
}

vec3	TangentToWorld(in vec2 vec)
{
	return TangentToWorld(vec3(vec, 1));
}

vec3	ScreenToWorld(in vec2 UV, in float depth)
{
	vec4	projectedCoord = vec4(UV * 2.0 - 1.0, depth * 2.0 - 1.0, 1.0);
	projectedCoord = Camera.InvMatrix.View * Camera.InvMatrix.Projection * projectedCoord;
	return (projectedCoord.xyz / projectedCoord.w);
}

float SceneDepth(vec2 UV, float depth)
{
	vec4	projectedCoord = vec4(UV * 2.0 - 1.0, depth * 2.0 - 1.0, 1.0);
	projectedCoord = Camera.InvMatrix.View * Camera.InvMatrix.Projection * projectedCoord;
	return projectedCoord.w;
}

float SceneDepth()
{
	return SceneDepth(Frag.UV, Frag.Depth);
}

float	Depth(in vec2 UV)
{
	return texture(Texture.Depth, UV, 0).r;
}

vec3	WorldPosition(in vec2 UV)
{
	return ScreenToWorld(UV, Depth(UV));
}

vec3	WorldPosition()
{
	return ScreenToWorld(Frag.UV, Frag.Depth);
}

void sincos(const in float x, out float sinX, out float cosX)
{
	sinX = sin(x);
	cosX = cos(x);
}

vec2 encodeNormal(const in vec3 n)
{
	vec2 xy = normalize(n.xy);
	return vec2(atan(xy.y, xy.x) / M_PI, n.z) * 0.5 + 0.5;
	/*vec3 n = normal * 0.5 + 0.5;
	float p = sqrt(n.z*8+8);
    return vec2(n.xy/p + 0.5);*/
	/*vec3 n = normalize(normal) * 0.5 + 0.5;
	float phi = atan(n.y, n.x);
	float theta = acos(n.z);
	vec2 encoded = vec2(phi, theta) / M_PI;
	return vec2(encoded.x * 0.5 + 0.5, encoded.y);*/
}

vec3 decodeNormal(const in vec2 enc)
{
	vec2 ang = enc * 2 - 1;
    vec2 scth;
    sincos(ang.x * M_PI, scth.x, scth.y);
    vec2 scphi = vec2(sqrt(1.0 - ang.y*ang.y), ang.y);
	vec2 xy = vec2(scth.y * scphi.x, scth.x * scphi.x);
    return normalize(vec3(xy.x, xy.y, scphi.y));
	/*vec2 fenc = enc*4-2;
    float f = dot(fenc,fenc);
    float g = sqrt(1-f/4);
    vec3 n;
    n.xy = fenc*g;
    n.z = 1-f/2;
    return normalize(n * 2 - 1);*/
	/*vec2 enc = vec2(encoded.x * 2 - 1, encoded.y) * M_PI;
	float sinPhi = sin(enc.x);
	float cosPhi = cos(enc.x);
	float sinTheta = sin(enc.y);
	float cosTheta = cos(enc.y);
	return vec3(
		sinTheta * cosPhi,
		sinTheta * sinPhi,
		cosTheta
	) * 2 - 1;*/
}

void	FillFrag()
{
	Frag.UV = frag_UV;
	Frag.CubeUV = frag_Cube_UV;
	Frag.Depth = gl_FragDepth = texture(Texture.Depth, frag_UV).r;
	Frag.Position = WorldPosition();
	Frag.Normal = decodeNormal(texture(Texture.Normal, frag_UV).xy);
	Frag.Ior = texture(Texture.Ior, frag_UV).x;
	vec4	CDiff_sample = texture(Texture.CDiff, frag_UV);
	vec4	F0_sample = texture(Texture.F0, frag_UV);
	Frag.BRDF.CDiff = CDiff_sample.rgb;
	Frag.BRDF.F0 = F0_sample.xyz;
	Frag.BRDF.Alpha = F0_sample.a;
	Frag.Opacity = CDiff_sample.a;
	Frag.Emitting = texture(Texture.Emitting, frag_UV).rgb;
	Frag.AO = texture(Texture.AO, frag_UV).r;
#ifdef LIGHTSHADER
	Out.Color = texture(Texture.Back.Color, frag_UV);
	Out.Emitting = texture(Texture.Back.Emitting, frag_UV).rgb;
#endif
}

#ifdef POSTSHADER
void	FillOut(in vec3 OriginalPosition)
{
	gl_FragDepth = Frag.Depth;
	bvec3	positionsEqual = notEqual(Frag.Position, OriginalPosition);
	if (positionsEqual.x || positionsEqual.y || positionsEqual.z)
	{
		vec4	NDC = Camera.Matrix.Projection * Camera.Matrix.View * vec4(Frag.Position, 1.0);
		gl_FragDepth = NDC.z / NDC.w * 0.5 + 0.5;
	}
	out_CDiff = vec4(Frag.BRDF.CDiff, Frag.Opacity);
	out_F0.rgb = Frag.BRDF.F0;
	out_F0.a = Frag.BRDF.Alpha;
	out_Emitting = Frag.Emitting;
	out_Ior = Frag.Ior;
	out_AO = Frag.AO;
	out_Normal = encodeNormal(normalize(Frag.Normal));
}
#endif

#ifdef LIGHTSHADER
void	FillOut(in vec3 OriginalPosition)
{
	if (any(notEqual(Frag.Position, OriginalPosition)))
	{
		vec4	NDC = Camera.Matrix.Projection * Camera.Matrix.View * vec4(Frag.Position, 1.0);
		gl_FragDepth = NDC.z / NDC.w * 0.5 + 0.5;
	}
	out_Color = Out.Color;
	out_Emitting = Out.Emitting;
}
#endif

vec2	BRDF(in float NdV, in float Roughness)
{
	return (texture(Texture.BRDF, vec2(NdV, Frag.BRDF.Alpha)).xy);
}

void	ApplyTechnique();

void main()
{
	FillFrag();
	vec3	OriginalPosition = Frag.Position;
	ApplyTechnique();
	FillOut(OriginalPosition);
}

)""