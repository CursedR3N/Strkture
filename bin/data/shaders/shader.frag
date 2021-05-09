#version 410 core
uniform float time;
uniform vec2 resolution;
uniform float fft[8];
uniform float fftSmoothed[8];
uniform float fftInc[8];
out vec4 outputColor;

float rand(vec2 i){
	return fract(sin(dot(i, vec2(12.9898,78.233)))*43758.5453123);
}

float noise(vec2 p){
	vec2 f = smoothstep(0,1,fract(p));
	vec2 i = floor(p);

	float a = rand(i);
	float b = rand(i+vec2(1,0));
	float c = rand(i+vec2(0,1));
	float d = rand(i+vec2(1,1));

	return mix(mix(a, b, f.x), mix(c, d, f.x), f.y);
}

void main(void) {
	float pi = acos(-1);
	vec2 tc = gl_FragCoord.xy/vec2(resolution.xy);
	vec2 uv = gl_FragCoord.xy/vec2(resolution.y);
	vec2 uvc = uv-vec2(resolution.x/resolution.y,1)/2;
	vec2 fuv = vec2(length(uvc)/1.25, atan(uvc.x, uvc.y)/pi);
	fuv.y -= time/100;
	//outputColor = vec4(vec3(step(uv.y, fftSmoothed[int(floor(tc.x*9))])), 1);
	outputColor = vec4(mix(0, 1, pow(1-length(uvc),2)*max(0.75,1.5*fftSmoothed[1]))*vec3(step(fract(fuv.x*10-(time/24+fftInc[1]/15)+2*noise((time/16+fftInc[1]/10)+sin(3*fuv*pi*2))), 0.1)),1);
}
