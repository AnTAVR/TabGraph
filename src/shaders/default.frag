#version 450 core
#define M_PI 3.1415926535897932384626433832795

uniform vec3		in_CamPos;

uniform vec2		in_UVMax;
uniform vec2		in_UVMin;

uniform vec3		in_Albedo;
uniform vec3		in_Emitting;
uniform float		in_Roughness;
uniform float		in_Metallic;
uniform float		in_Refraction;
uniform float		in_Alpha;
uniform float		in_Parallax;

uniform bool		in_Use_Texture_Albedo;
uniform bool		in_Use_Texture_Roughness;
uniform bool		in_Use_Texture_Metallic;
uniform bool		in_Use_Texture_Normal;
uniform bool		in_Use_Texture_Height;
uniform sampler2D	in_Texture_Albedo;
uniform sampler2D	in_Texture_Roughness;
uniform sampler2D	in_Texture_Metallic;
uniform sampler2D	in_Texture_Normal;
uniform sampler2D	in_Texture_Height;

uniform samplerCube	in_Texture_Env;
uniform samplerCube	in_Texture_Env_Spec;

in vec3			frag_WorldNormal;
in vec3			frag_WorldPosition;
in vec2			frag_Texcoord;
in vec3			frag_Tangent;
in vec3			frag_Bitangent;

layout(location = 0) out vec4		out_Color;

float chiGGX(float v)
{
    return v > 0 ? 1 : 0;
}

float	GGX_Geometry(in float NdV, in float roughness)
{
	float NdV2 = 2 * NdV;
	float r2 = roughness * roughness;
	float denom = NdV + sqrt(r2 + (1 - r2) * (NdV * NdV));
	return (NdV / denom);
}

float	GGX_Distribution(in float NdH, in float roughness)
{
    float r2 = roughness * roughness;
    float NoH2 = NdH * NdH;
    float den = NoH2 * r2 + (1 - NoH2);
    return (chiGGX(NdH) * r2) / ( M_PI * den * den );
}

float Cooktorrance_Specular(in float NdL, in float NdV, in float NdH, in float HdV, in float roughness)
{
	float	D = GGX_Distribution(NdH, roughness);
	float	G = GGX_Geometry(NdV, roughness);
	return (max(0, D * G));
}

float	Oren_Nayar_Diffuse(in float LdV, in float NdL, in float NdV, in float roughness)
{
	float	lde;
	float	r2;

	lde = LdV - NdL * NdV;
	r2 = roughness * roughness;
	return (max(
		max(0.0, NdL) * ((1.0 + r2 * (1 / (r2 + 0.13) + 0.5 / (r2 + 0.33))) +
		(0.45 * r2 / (r2 + 0.09)) * lde / mix(1.0, max(NdL, NdV),
		step(0.0, lde))) / M_PI, 0));
}

vec3 Fresnel_F0(in float ior, in float metallic, in vec3 albedo)
{
	vec3 F0 = vec3(abs((1.0 - ior) / (1.0 + ior)));
	F0 = F0 * F0;
	F0 = mix(F0, albedo, metallic);
	return (F0);
}

vec3 Fresnel_Schlick(in float HdV, in vec3 F0)
{
  return F0 + (1-F0) * pow( 1 - HdV, 5);
}

/*vec2	sample_height_map(in vec3 tbnV, in vec2 vt)
{
	//vec3	vdir;
	float	d[3];
	vec2	c[2];
	int		tries;

	//vdir = normalize(tbn * V);
	d[1] = mix(64, 256, abs(tbnV.z));
	tries = int(d[1] * 2);
	d[0] = 1.0 / d[1];
	c[1] = in_Parallax * tbnV.xy / tbnV.z / d[1];
	d[1] = 0.0;
	c[0] = vt;
	d[2] = 1 - texture(in_Texture_Height, c[0]).x;
	while (tries > 0 && d[1] < d[2])
	{
		c[0] = c[0] - c[1];
		d[2] = 1 - texture(in_Texture_Height, c[0]).x;
		d[1] += d[0];
		tries--;
	}
	c[1] = c[0] + c[1];
	d[2] = d[2] - d[1];
	d[2] = d[2] /
	(d[2] - 1 - texture(in_Texture_Height, c[1]).x - d[1] + d[0]);
	return (c[1] * d[2]) + (c[0] * (1.0 - d[2]));
}*/

vec2 Parallax_Mapping(in vec3 tbnV, in vec2 T, out float parallaxHeight)
{
	// determine optimal number of layers
	const float minLayers = 64;
	const float maxLayers = 128;
	float numLayers = mix(maxLayers, minLayers, abs(tbnV.z));
	int	tries = int(numLayers);
	//vec2 texStep = tbnV.xy * in_Parallax / 10.f / numLayers;

	// height of each layer
	float layerHeight = 1.0 / numLayers;
	// current depth of the layer
	float curLayerHeight = 0;
	// shift of texture coordinates for each layer
	vec2 dtex = in_Parallax * tbnV.xy / tbnV.z / numLayers;

	// current texture coordinates
	vec2 currentTextureCoords = T;

	// depth from heightmap
	float heightFromTexture = 1 - texture(in_Texture_Height, currentTextureCoords).r;

	// while point is above the surface
	while(tries > 0 && heightFromTexture > curLayerHeight) 
	{
		tries--;
		// to the next layer
		curLayerHeight += layerHeight; 
		// shift of texture coordinates
		currentTextureCoords -= dtex;
		// new depth from heightmap
		heightFromTexture = 1 - texture(in_Texture_Height, currentTextureCoords).r;
	}

	///////////////////////////////////////////////////////////

	// previous texture coordinates
	vec2 prevTCoords = currentTextureCoords + dtex;

	// heights for linear interpolation
	float nextH	= heightFromTexture - curLayerHeight;
	float prevH	= 1 - texture(in_Texture_Height, prevTCoords).r
	                       - curLayerHeight + layerHeight;

	// proportions for linear interpolation
	float weight = nextH / (nextH - prevH);

	// interpolation of texture coordinates
	vec2 finalTexCoords = prevTCoords * weight + currentTextureCoords * (1.0-weight);

	// interpolation of depth values
	parallaxHeight = (curLayerHeight + prevH * weight + nextH * (1.0 - weight));
	parallaxHeight *= in_Parallax;
	// return result
	return finalTexCoords;
}

vec2 ParallaxMapping(vec3 tbnV, vec2 texCoords)
{ 
	// number of depth layers
	const float minLayers = 64.0;
	const float maxLayers = 128.0;
	float numLayers = mix(maxLayers, minLayers, abs(tbnV.z)); 
	int tries = int(numLayers);
	// calculate the size of each layer
	float layerDepth = 1.0 / numLayers;
	// depth of current layer
	float currentLayerDepth = 0.0;
	// the amount to shift the texture coordinates per layer (from vector P)
	vec2 P = tbnV.xy * in_Parallax; 
	vec2 deltaTexCoords = in_Parallax * tbnV.xy / tbnV.z / numLayers;//P / numLayers;

	vec2  currentTexCoords     = texCoords;
	float currentDepthMapValue = 1 - texture(in_Texture_Height, currentTexCoords).r;
	  
	while(tries > 0 && currentLayerDepth < currentDepthMapValue)
	{
		// shift texture coordinates along direction of P
		currentTexCoords -= deltaTexCoords;
		// get depthmap value at current texture coordinates
		currentDepthMapValue = 1 - texture(in_Texture_Height, currentTexCoords).r;  
		// get depth of next layer
		currentLayerDepth += layerDepth;
		tries--;
	}
	vec2 prevTexCoords = currentTexCoords + deltaTexCoords;

	// get depth after and before collision for linear interpolation
	float afterDepth  = currentDepthMapValue - currentLayerDepth;
	float beforeDepth = 1 - texture(in_Texture_Height, prevTexCoords).r - currentLayerDepth + layerDepth;

	// interpolation of texture coordinates
	float weight = afterDepth / (afterDepth - beforeDepth);
	vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);
	return finalTexCoords;  
}

mat3x3	tbn_matrix(in vec3 position, in vec3 normal, in vec2 texcoord)
{
	vec3 Q1 = dFdxCoarse(position);
	vec3 Q2 = dFdyCoarse(position);
	vec2 st1 = dFdxCoarse(texcoord);
	vec2 st2 = dFdyCoarse(texcoord);
	vec3 T = normalize(Q1*st2.t - Q2*st1.t);
	vec3 B = normalize(-Q1*st2.s + Q2*st1.s);
	return(transpose(mat3(T, B, normal)));
}

vec3	light_Pos = vec3(-3, 3, 3);
vec3	light_Color = vec3(1, 1, 1);
float	light_Power = 1;

void main()
{
	vec3	worldPosition = frag_WorldPosition;
	vec3	worldNormal = frag_WorldNormal;
	vec3	albedo = in_Albedo;
	vec2	vt = frag_Texcoord;
	float	alpha = in_Alpha;
	float	roughness = in_Roughness;
	float	metallic = in_Metallic;
	mat3x3	tbn = tbn_matrix(worldPosition, worldNormal, frag_Texcoord);

	float ph;
	if (in_Use_Texture_Height)
	{
		//vt = ParallaxMapping(tbn * normalize(in_CamPos - worldPosition), vt);
		//vt = sample_height_map(tbn * normalize(in_CamPos - worldPosition), vt);
		vt = Parallax_Mapping(tbn * normalize(in_CamPos - worldPosition), vt, ph);
		if(vt.x > in_UVMax.x || vt.y > in_UVMax.y || vt.x < in_UVMin.x || vt.y < in_UVMin.y)
			discard;
		worldPosition = worldPosition - (worldNormal * ph);
	}
	vec3	L = normalize(light_Pos - worldPosition);
	vec3	V = normalize(in_CamPos - worldPosition);
	vec3	H = normalize(L + V);
	if (in_Use_Texture_Albedo)
	{
		albedo = texture(in_Texture_Albedo, vt).rgb;
		alpha = texture(in_Texture_Albedo, vt).a;
	}
	if (alpha == 0.f)
		discard;
	if (in_Use_Texture_Normal)
		worldNormal = normalize((texture(in_Texture_Normal, vt).xyz * 2 - 1) * tbn);
	if (in_Use_Texture_Roughness)
		roughness = texture(in_Texture_Roughness, vt).x;
	if (in_Use_Texture_Metallic)
		metallic = texture(in_Texture_Metallic, vt).x;

	float	light_Attenuation = light_Power * 1.f / (1 + distance(light_Pos, worldPosition));
	light_Color *= light_Attenuation;

	float NdL = max(0.0, dot(worldNormal, L));
	float NdV = max(0.001, dot(worldNormal, V));
	float NdH = max(0.001, dot(worldNormal, H));
	float HdV = max(0.001, dot(H, V));
	float LdV = max(0.001, dot(L, V));

	vec3	F0 = Fresnel_F0(in_Refraction, metallic, albedo);
	vec3	fresnel = Fresnel_Schlick(NdH, F0);
	vec3	kd = mix(vec3(1.0) - fresnel, vec3(0.04), metallic);

	vec3	specular = fresnel * Cooktorrance_Specular(NdL, NdV, NdH, HdV, roughness);
	vec3	diffuse = kd * albedo * Oren_Nayar_Diffuse(LdV, NdL, NdV, roughness);
	

	vec3	refl = reflect(V, worldNormal);
	fresnel = Fresnel_Schlick(NdV, F0);
	vec3	env_diffuse = textureLod(in_Texture_Env, -worldNormal, 10.0).rgb * albedo;// * kd;
	vec3	env_reflection = textureLod(in_Texture_Env, refl, roughness * 10.f).rgb * fresnel;
	vec3	env_specular = textureLod(in_Texture_Env_Spec, refl, roughness * 10.f).rgb * F0;

	//vec3	reflection = mix(albedo, env_reflection, max(0.15, metallic));
	//out_Color = vec4(textureLod(in_Texture_Env, refl, 1).xyz, 1);
	//float	brightness = (max(0, in_Emitting.r - 1) + max(0, in_Emitting.g - 1) + max(0, in_Emitting.b - 1));
	out_Color = vec4(in_Emitting + env_reflection + env_diffuse + env_specular + light_Color * specular + light_Color * diffuse, alpha);
}