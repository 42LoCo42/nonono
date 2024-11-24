vec3 palette(float t) {
	// from http://dev.thi.ng/gradients
	vec3 a = vec3(0.5, 0.5, 0.5);
	vec3 b = vec3(0.5, 0.5, 0.5);
	vec3 c = vec3(1.0, 1.0, 1.0);
	vec3 d = vec3(0.263, 0.416, 0.557);

	return a + b * cos(6.28318 * (c * t + d));
}

void mainImage(out vec4 fragColor, in vec2 fragCoord) {
	// resolution-independent coordinate transformation
	vec2 uv = (fragCoord * 2.0 - iResolution.xy) / iResolution.y;
	vec2 uv0 = uv;

	vec3 finalColor = vec3(0.0);
	for(float i = 0.0; i < 4.0; i++) {
		uv = fract(uv * 1.5) - 0.5;    // space repetition

		float d = length(uv);          // black to white gradient at local origin
		d *= exp(-length(uv0));        // inverse exponential backoff from global origin
		d = sin(d*8.0 + iTime)/8.0;    // animated repetitions
		d = abs(d);                    // ring gradient
		d = 0.01 / d;                  // inverse/neon colors
		d = pow(d, 2.0);               // boost contrast

		vec3 col = palette( // color depends on
			length(uv0) +   // distance from global origin
			iTime * 0.4 +   // time
			i     * 0.4     // fractal loop index
		);

		finalColor += col * d;
	}

	fragColor = vec4(finalColor, 1.0);
}
