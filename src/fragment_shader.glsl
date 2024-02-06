#version 330 core
out vec4 FragColor;

uniform vec3 camera_position;
uniform vec2 camera_direction;
uniform vec2 resolution;
uniform float time;

mat2 rotate(float angle) {
    float c = cos(angle);
    float s = sin(angle);
    return mat2(c, -s, s, c);
}

float plane(float h, vec3 p) {
	return p.y - h;
}

float sphere(float r, vec3 p) {
	return length(p) - r;
}

float get_dist(vec3 p) {

	float sd = sphere(1.0, p - vec3(0.0, 1.0, 6.0));
	float pd = plane(0.0, p);
	float ds = min(sd, pd);
	return ds;

}
	
float raymarch(vec3 ro, vec3 rd) {
	
	float dO = 0.0;
	for(int i=0;i<100;i++) {
		vec3 p = ro + dO * rd;
		float ds = get_dist(p);
		dO += ds;
		if(dO > 100.0 || ds < 0.01) {
			break;
		}
	}
	return dO;
}

void main() {

	vec2 uv = 2.0 * gl_FragCoord.xy / resolution - 1.0;

	vec3 ro = camera_position;

	vec3 rd = normalize(vec3(uv, 1.0));
	rd.xz *= rotate(camera_direction.x);

	float dO = raymarch(ro, rd);

	vec3 col = vec3(1.0 - dO / 10.0); 

	FragColor = vec4(col, 1.0);
}
