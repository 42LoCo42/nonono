#version 450

layout(location = 0) out vec3 fragColor;

void main() {
	const vec2 positions[] = vec2[](
		vec2(0.0, -0.5), // center top
		vec2(0.5, 0.5),  // left down
		vec2(-0.5, 0.5)  // right down
	);

	const vec3 colors[] = vec3[](
		vec3(1.0, 0.0, 0.0), // red
		vec3(0.0, 1.0, 0.0), // green
		vec3(0.0, 0.0, 1.0)  // blue
	);

	gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
	fragColor   = colors[gl_VertexIndex];
}
