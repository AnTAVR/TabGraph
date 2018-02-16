#version 410 core
#define	KERNEL_SIZE 9

uniform sampler2D	in_Texture_Color;
uniform sampler2D	in_Texture_Normal;
uniform sampler2D	in_Texture_Position;
uniform sampler2D	in_Texture_Depth;
uniform samplerCube	in_Texture_Env;

in vec2				frag_UV;
in vec3				frag_Cube_UV;

out vec4			out_Color;

const float			gaussian_kernel[] = float[KERNEL_SIZE](
	0.091637, 0.105358, 0.1164, 0.123573, 0.126061, 0.123573, 0.1164, 0.105358, 0.091637
);

vec3	texture_gaussian(sampler2D tex, vec2 tex_Coord, float width)
{
	vec2	texture_Size = textureSize(tex, 0);
	vec3	color = vec3(0);
	for (int x = 0; x < KERNEL_SIZE; x++)
	{
		for (int y = 0; y < KERNEL_SIZE; y++)
		{
			vec2	offset = vec2(x - KERNEL_SIZE / 2.f, y - KERNEL_SIZE / 2.f) / texture_Size * width;
			vec2	sampleUV = tex_Coord + offset;
			float	weight = gaussian_kernel[x] * gaussian_kernel[y];
			color += texture(tex, sampleUV).xyz * weight;
		}
	}
	return (color);
}

void main()
{
	vec4	color = texture(in_Texture_Color, frag_UV);
	vec3	normal = texture(in_Texture_Normal, frag_UV).xyz;
	vec3	position = texture(in_Texture_Position, frag_UV).xyz;
	float	depth = texture(in_Texture_Depth, frag_UV).r;
	float	sampledist = 2.5f;
	vec3	finalColor = vec3(0);
	vec3	bloom = vec3(0);
	vec2	texture_Size = 1.f / textureSize(in_Texture_Color, 0);
	float	occlusion = 0.f;
	for (int i = 0; i < KERNEL_SIZE; i++) 
	{
		for (int j = 0; j < KERNEL_SIZE; j++) 
		{
			vec2	index = vec2(float(i - KERNEL_SIZE / 2.f), float(j - KERNEL_SIZE / 2.f));
			vec2	sampleUV = frag_UV + index * sampledist * texture_Size;
			vec3	sampleColor = textureLod(in_Texture_Color, sampleUV, 2.5f).rgb;
			float	weight = gaussian_kernel[i] * gaussian_kernel[j];
			bloom += max(vec3(0), sampleColor - 1) * weight;
			sampleUV = frag_UV + index * 5.f * texture_Size;
			float	sampleDepth = texture(in_Texture_Depth, sampleUV).r;
			if (depth == 1 || sampleDepth == 1)
				continue;
			vec3	samplePosition = texture(in_Texture_Position, sampleUV).xyz;
			vec3	V = position - samplePosition;
			float	D = length(V);
			if (D < 0.001f)
				continue;
			D /= 5.f;
			float	bias = 0.001 + sampleDepth * 0.5f;
			float	factor = dot(normal, normalize(V));
			if (isnan(factor))
				continue;
			float	angle = max(0, factor - bias);
			occlusion += (angle * (1.f / (1.f + D))) * weight;
		}
	}
	finalColor = bloom + color.rgb * color.a - occlusion;
	out_Color = texture(in_Texture_Env, frag_Cube_UV) * (1 - color.a);
	out_Color += vec4(finalColor, 1);
	//out_Color = vec4(bloom, 1);
}
