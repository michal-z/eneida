#pragma once

#define k_pi 3.1415926f
#define k_pi_div_2 (0.5f * k_pi)

typedef struct Vec3 {
	union {
		struct {
			float x, y, z;
		};
		float m[3];
	};
} Vec3;

typedef struct Vec4 {
	union {
		struct {
			float x, y, z, w;
		};
		float m[4];
	};
} Vec4;

typedef struct Mat4 {
	union {
		struct {
			Vec4 c0, c1, c2, c3;
		};
		float m[4][4];
	};
} Mat4;

static inline Vec3 *vec3_set(Vec3 *out, float x, float y, float z)
{
	out->x = x;
	out->y = y;
	out->z = z;
	return out;
}

static inline Vec3 *vec3_mul(Vec3 *out, const Vec3 *v0, const Vec3 *v1)
{
	out->x = v0->x * v1->x;
	out->y = v0->y * v1->y;
	out->z = v0->z * v1->z;
	return out;
}

static inline Mat4 mat4_perspective(float fovy, float aspect, float n, float f)
{
    assert(0);
	float k = fovy;//tanf(k_pi_div_2 - 0.5f * fovy);
	float ri = 1.0f / (n - f);
	Mat4 m = {
		k / aspect, 0.0f, 0.0f, 0.0f,
		0.0f, k, 0.0f, 0.0f,
		0.0f, 0.0f, (n + f) * ri, -1.0f,
		0.0f, 0.0f, n * f * ri * 2.0f, 0.0f
	};
	return m;
}
/*
static mat4 LookAt(const vec3 &eye, const vec3 &at, const vec3 &up) {
	const vec3 az = normalize(eye - at);
	const vec3 ax = normalize(cross(up, az));
	const vec3 ay = cross(az, ax);
	return mat4(ax[0], ay[0], az[0], 0.0f,
		ax[1], ay[1], az[1], 0.0f,
		ax[2], ay[2], az[2], 0.0f,
		-dot(ax, eye), -dot(ay, eye), -dot(az, eye), 1.0f);
}
*/
