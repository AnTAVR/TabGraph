#version 430
#pragma optionNV (unroll all)
#define	KERNEL_SIZE 64
#define M_PI 3.1415926535897932384626433832795

precision lowp float;
precision lowp int;
precision lowp sampler2D;
precision lowp samplerCube;

struct t_Environment {
	samplerCube	Diffuse;
	samplerCube	Irradiance;
};

uniform sampler2D	in_Texture_Albedo;
uniform sampler2D	in_Texture_Fresnel;
uniform sampler2D	in_Texture_Emitting;
uniform sampler2D	in_Texture_Material_Values;
uniform sampler2D	in_Texture_BRDF;
uniform sampler2D	in_Texture_Normal;
uniform sampler2D	in_Texture_Position;
uniform sampler2D	in_Texture_Depth;
uniform sampler2D	in_Back_Color;
uniform sampler2D	in_Back_Bright;

uniform vec3		in_CamPos;
uniform mat4		in_ViewMatrix;

uniform t_Environment	Environment;

in vec2				frag_UV;
in vec3				frag_Cube_UV;

layout(location = 0) out vec4	out_Color;
layout(location = 1) out vec4	out_Emitting;

vec4	sampleLod(samplerCube texture, vec3 uv, float value)
{
	value = clamp(value, 0, 1);
	float factor = textureQueryLevels(texture);//floor(log2(float(textureSize(texture, 0).x)));
	return textureLod(texture, uv, value * factor);
}

vec4	sampleLod(sampler2D texture, vec2 uv, float value)
{
	value = clamp(value, 0, 1);
	float factor = textureQueryLevels(texture);//floor(log2(float(textureSize(texture, 0).x)));
	return textureLod(texture, uv, value * factor);
}

/* float	Env_Specular(in float NdV, in float roughness)
{
	float	alpha = roughness * roughness;
	float	den = (alpha - 1) + 1;
	float	D = (alpha / (M_PI * den * den));
	float	alpha2 = alpha * alpha;
	float	G = (2 * NdV) / (NdV + sqrt(alpha2 + (1 - alpha2) * (NdV * NdV)));
	return (max(D * G, 0));
}
 */

float	Env_Specular(in float NdV, in float roughness)
{
	float a2 = roughness * roughness;
	float NdVa2 = NdV * a2;
	float G_V = NdV + sqrt((NdV - NdVa2) * NdV + a2 );
	float G_L = -NdV + sqrt((-NdV + NdVa2) * -NdV + a2 );
	return (1 / (G_V * G_L));
}

float	cmix(float min, float max, float percent)
{
	if (percent < 0 || percent > 1)
		return (mix(min, max, 1 - mod(percent, 1)));
	return (mix(min, max, percent));
}

vec2	cmix(vec2 min, vec2 max, vec2 percent)
{
	return (vec2(cmix(min.x, max.x, percent.x), cmix(min.y, max.y, percent.y)));
}

float	warpUV(float min, float max, float percent)
{
	if (percent < 0 || percent > 1)
		return (smoothstep(min, max, 1 - mod(percent, 1)));
	return (percent);
}

vec2	warpUV(vec2 min, vec2 max, vec2 percent)
{
	return (vec2(warpUV(min.x, max.x, percent.x), warpUV(min.y, max.y, percent.y)));
}

void main()
{
	out_Color.a = 1;
	out_Emitting.a = 1;
	float	Depth = gl_FragDepth = texture(in_Texture_Depth, frag_UV).r;
	vec3	EnvDiffuse = texture(Environment.Diffuse, frag_Cube_UV).rgb;
	vec4	Albedo = texture(in_Texture_Albedo, frag_UV);
	vec3	Fresnel = texture(in_Texture_Fresnel, frag_UV).rgb;
	vec4	Emitting = texture(in_Texture_Emitting, frag_UV);
	vec4	Material_Values = texture(in_Texture_Material_Values, frag_UV);
	vec4	BRDF = texture(in_Texture_BRDF, frag_UV);
	vec3	Normal = normalize(texture(in_Texture_Normal, frag_UV).xyz);
	vec3	Position = texture(in_Texture_Position, frag_UV).xyz;
	float	Roughness = Material_Values.x;
	float	Metallic = Material_Values.y;
	float	AO = Material_Values.z;
	float	Ior = BRDF.z;

	vec3	V = normalize(in_CamPos - Position);
	float	NdV = dot(Normal, V);
	if (NdV < 0) {
		Normal = -Normal;
		NdV = -NdV;
	}
	AO = clamp(1 - AO, 0, 1);

	vec3	diffuse = AO * (sampleLod(Environment.Diffuse, -Normal, Roughness + 0.9).rgb
			+ sampleLod(Environment.Irradiance, -Normal, Roughness).rgb);
	vec3	R = reflect(V, Normal);
	vec3	reflection = sampleLod(Environment.Diffuse, R, Roughness * 1.5f).rgb * Fresnel;
	vec3	specular = sampleLod(Environment.Irradiance, R, Roughness).rgb;
	vec3	reflection_spec = pow(sampleLod(Environment.Diffuse, R, Roughness + 0.1).rgb, vec3(2.2));

	
	vec2	refract_UV = frag_UV;
	if (Ior > 1)
	{
		vec2	refractFactor = vec2(1 - Depth) * vec2(0.25f) + (Fresnel.x + Fresnel.y + Fresnel.z) / 3.f * 0.0125f;
		vec2	refractDir = (mat3(in_ViewMatrix) * normalize(refract(V, Normal, 1.0 / Ior))).xy;
		refract_UV = frag_UV + refractDir * refractFactor;
		refract_UV = warpUV(vec2(0), vec2(1), refract_UV);
	}
	vec3	Back_Color = sampleLod(in_Back_Color, refract_UV, Roughness).rgb;
	vec3	Back_Bright = sampleLod(in_Back_Bright, refract_UV, Roughness).rgb;

	if (Albedo.a == 0) {
		out_Color.rgb = Back_Color;
		out_Emitting.rgb = Back_Bright;
		return ;
	}

	float	brightness = 0;
	brightness = dot(reflection_spec, vec3(0.299, 0.587, 0.114));
	reflection_spec *= brightness * min(Fresnel + 1, Fresnel * Env_Specular(NdV, Roughness));
	specular *= Fresnel * BRDF.x + mix(vec3(1), Fresnel, Metallic) * BRDF.y;
	specular += reflection_spec;
	diffuse *= Albedo.rgb * (1 - Metallic);
	Albedo.a += dot(specular, specular);
	Albedo.a = min(1, Albedo.a);

	Back_Color *= Albedo.rgb;
	Back_Bright *= Albedo.rgb;

	out_Color.rgb = specular + diffuse + reflection;
	out_Color.rgb = mix(Back_Color, out_Color.rgb, Albedo.a);
	brightness = dot(pow(out_Color.rgb, vec3(2.2)), vec3(0.299, 0.587, 0.114));
	out_Emitting.rgb = max(vec3(0), out_Color.rgb - 0.6) * min(1, brightness) + Emitting.rgb;
	out_Emitting.rgb = mix(Back_Bright, out_Emitting.rgb, Albedo.a);
}
