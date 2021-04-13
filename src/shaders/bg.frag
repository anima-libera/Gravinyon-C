
#version 430 core
layout(location = 0) uniform float u_time;
out vec4 out_color;

#define TAU 6.28318530717

float noise(vec4 v)
{
	return fract(sin(dot(v, vec4(12.9898, 78.233, 43.045, 30.7226))) * 43758.5453);
}

float interpolate(float ratio, float inf, float sup)
{
	//float smoothratio = smoothstep(0.0, 1.0, ratio);
	float smoothratio = (1.0 - cos(ratio * TAU / 2.0)) / 2.0;
	return smoothratio * inf + (1.0 - smoothratio) * sup;
}

float tile_noise(float tile_size, int time_tile_size, float channel)
{
	vec2 coords = vec2(gl_FragCoord.xy) + vec2(u_time * 2.0, 0.0);
	float time_coord = u_time;
	vec2 tile_coords = coords - vec2(mod(coords.x, tile_size), mod(coords.y, tile_size));
	int time_tile_coord = int(time_coord - mod(time_coord, time_tile_size));

	vec2 coords_00 = tile_coords + vec2(0.0, 0.0) * tile_size;
	vec2 coords_10 = tile_coords + vec2(1.0, 0.0) * tile_size;
	vec2 coords_01 = tile_coords + vec2(0.0, 1.0) * tile_size;
	vec2 coords_11 = tile_coords + vec2(1.0, 1.0) * tile_size;
	float time_coords_0 = time_tile_coord + 0 * time_tile_size;
	float time_coords_1 = time_tile_coord + 1 * time_tile_size;

	float noise_000 = noise(vec4(coords_00, time_coords_0, channel));
	float noise_100 = noise(vec4(coords_10, time_coords_0, channel));
	float noise_010 = noise(vec4(coords_01, time_coords_0, channel));
	float noise_110 = noise(vec4(coords_11, time_coords_0, channel));
	float noise_001 = noise(vec4(coords_00, time_coords_1, channel));
	float noise_101 = noise(vec4(coords_10, time_coords_1, channel));
	float noise_011 = noise(vec4(coords_01, time_coords_1, channel));
	float noise_111 = noise(vec4(coords_11, time_coords_1, channel));

	vec2 intile_coords = coords - coords_00;
	float leftness = 1.0 - (float(intile_coords.x) / float(tile_size));
	float topness = 1.0 - (float(intile_coords.y) / float(tile_size));
	float intile_time_coord = time_coord - time_coords_0;
	float earlyness = 1.0 - (float(intile_time_coord) / float(time_tile_size));

	float early_noise_top = interpolate(leftness, noise_000, noise_100);
	float early_noise_bot = interpolate(leftness, noise_010, noise_110);
	float early_noise = interpolate(topness, early_noise_top, early_noise_bot);
	float late_noise_top = interpolate(leftness, noise_001, noise_101);
	float late_noise_bot = interpolate(leftness, noise_011, noise_111);
	float late_noise = interpolate(topness, late_noise_top, late_noise_bot);
	float noise = interpolate(earlyness, early_noise, late_noise);

	return noise;
}

float fractal_noise(float channel)
{
	float noise_sum = 0.0;
	float importance_sum = 0.0;
	float current_importance = 100.0;
	float current_size = 400.0;
	float current_time_size = 800.0;
	for (int i = 0; i < 12; i++)
	{
		noise_sum += tile_noise(current_size, int(current_time_size), channel) * current_importance;
		importance_sum += current_importance;
		current_importance /= 2.0;
		current_size /= 2.0;
		current_time_size /= 1.7;
	}
	float noise = noise_sum / importance_sum;

	return noise;
}

float expand(float x, float range_inf, float range_sup)
{
	return (x - range_inf) / (range_sup - range_inf);
}

void main()
{
	vec3 color;
	
	float noise = fractal_noise(0.0);

	if (noise < 0.3)
	{
		color = vec3(0.0, 0.0, 0.0);
	}
	else if (noise < 0.6)
	{
		color = vec3(0.0, 0.0, 1.0) * expand(noise, 0.3, 0.6);
	}
	else if (noise < 0.93)
	{
		color = vec3(0.0, 0.0, 1.0) +
			vec3(1.0, 0.0, 0.0) * expand(noise, 0.6, 0.93);
	}
	else
	{
		color = vec3(1.0, 0.0, 1.0);
	}

	out_color = vec4(color, 1.0);
}
