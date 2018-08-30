#pragma once

#include <cmath>
#include <string.h>
#include "game.hpp"


namespace SMOBA
{

	// this is pretty much all stolen from Evan Todd

#define PI 3.1415926535897f
#define HALF_PI (3.1415926535897f * 0.5f)

#define toRadians(t) (t*(PI/180))

	template<typename T> inline T f_max(T a, T b) { return  a > b ? a : b; }
	template<typename T> inline T f_min(T a, T b) { return  a < b ? a : b; }



	struct vec2
	{
		r32 x;
		r32 y;

		static const vec2 zero;

		vec2() : x(0), y(0) {}

		vec2(r32 x, r32 y) : x(x), y(y) {}

		inline explicit vec2(r32 scalar) : x(scalar), y(scalar) {}

		inline explicit vec2(const r32 coordinate[2]) : x(coordinate[0]), y(coordinate[1]) {}

		inline explicit vec2(const i32 coordinate[2]) {
			x = (r32)coordinate[0];
			y = (r32)coordinate[1];
		}

		inline explicit vec2(r32* const r) : x(r[0]), y(r[1]) {}

		inline r32 operator[] (i32 i) const {
			return *(&x + 1);
		}

		inline r32& operator[] (i32 i) {
			return *(&x + i);
		}

		inline b8 operator== (const vec2& rvector) const {
			return (x == rvector.x) && (y == rvector.y);
		}

		inline b8 operator!= (const vec2& rvector) const {
			return (x != rvector.x) || (y != rvector.y);
		}

		inline vec2 operator+ (const vec2& rvector) const {
			return vec2(x + rvector.x, y + rvector.y);
		}

		inline vec2 operator- (const vec2& rvector) const {
			return vec2(x - rvector.x, y - rvector.y);
		}

		inline vec2 operator* (const vec2& rvector) const {
			return vec2(x * rvector.x, y * rvector.y);
		}

		inline vec2 operator* (r32 scalar) const {
			return vec2(x * scalar, y * scalar);
		}

		inline vec2 operator/ (r32 scalar) const {
			r32 nscalar = 1.0f / scalar;
			return vec2(x * nscalar, y * nscalar);
		}

		inline vec2 operator/ (const vec2& rvector) const {
			return vec2(x / rvector.x, y / rvector.y);
		}

		inline vec2 operator+ () const {
			return *this;
		}

		inline vec2 operator- () const {
			return vec2(-x, -y);
		}

		inline friend vec2 operator* (r32 scalar, const vec2& rvector) {
			return vec2(scalar * rvector.x, scalar * rvector.y);
		}

		inline friend vec2 operator/ (r32 scalar, const vec2& rvector) {
			return vec2(scalar / rvector.x, scalar / rvector.y);
		}

		inline vec2& operator+= (const vec2& rvector) {
			this->x += rvector.x;
			this->y += rvector.y;

			return *this;
		}

		inline vec2& operator+= (r32 scalar) {
			x += scalar;
			y += scalar;

			return *this;
		}

		inline vec2& operator-= (const vec2& rvector) {
			x -= rvector.x;
			y -= rvector.y;

			return *this;
		}

		inline vec2& operator-= (r32 scalar) {
			x -= scalar;
			y -= scalar;

			return *this;
		}

		inline vec2& operator*= (const vec2& rvector) {
			x *= rvector.x;
			y *= rvector.y;

			return *this;
		}

		inline vec2& operator*= (r32 scalar) {
			x *= scalar;
			y *= scalar;

			return *this;
		}

		inline vec2& operator/= (const vec2& rvector) {
			x /= rvector.x;
			y /= rvector.y;

			return *this;
		}

		inline vec2& operator/= (r32 scalar) {

			r32 nscalar = 1.0f / scalar;

			x *= nscalar;
			y *= nscalar;

			return *this;
		}

		inline r32 length() const {
			return sqrt(x*x + y*y);
		}

		inline r32 length_squared() const {
			return x*x + y*y;
		}

		inline r32 dot(const vec2& other) const {
			return x * other.x + y * other.y;
		}

		inline r32 normalize() {
			r32 length = sqrt(x*x + y*y);

			r32 scaleLen = 1.0f / length;
			x *= scaleLen;
			y *= scaleLen;

			return length;
		}

		inline static vec2 normalize(const vec2& vec) {
			r32 length = sqrt(vec.x*vec.x + vec.y*vec.y);
			return vec2(vec.x / length, vec.y / length);
		}

		inline vec2 perpendicular(void) const {
			return vec2(-y, x);
		}

		inline r32 cross(const vec2& rvector) const {
			return x * rvector.y - y * rvector.x;
		}

		inline vec2 reflect(const vec2& normal) const {
			return *this - (2.0f * this->dot(normal) * normal);
		}

		static inline vec2 lerp(r32 x, const vec2& a, const vec2& b) {
			return (a * (1.0f - x)) + (b * x);
		}
	};

	struct rect
	{
		vec2 pos;
		vec2 size;

		inline rect() : pos(), size() {}
		inline rect(const vec2& p, const vec2& s) : pos(p), size(s) {}
		inline rect(const rect& r) : pos(r.pos), size(r.size) {}

		rect outset(r32) const;
		rect pad(const rect& other) const;
		b8 contains(const vec2&) const;

	};

	struct iRect
	{
		union
		{
			i32 x;
			i32 left;
		};
		union
		{
			i32 y;
			i32 top;
		};

		i32 w, h;

		static const iRect zero;

		iRect() : x(0), y(0), w(0), h(0) {}

		iRect(i32 x, i32 y, i32 w, i32 h) : x(x), y(y), w(w), h(h) {}

		inline b8 operator== (const iRect& rRect) const {
			return (x == rRect.x) && (y == rRect.y) && (w == rRect.w) && (h == rRect.h);
		}

		inline b8 operator!= (const iRect& rRect) const {
			return (x != rRect.x) || (y != rRect.y) || (w != rRect.w) || (h != rRect.h);
		}

		b8 contains(const vec2& point) const;

		b8 intersects(const iRect& other) const;
	};

	struct vec3
	{
		r32 x;
		r32 y;
		r32 z;

		static const vec3 zero;

		vec3() : x(0), y(0), z(0) {}

		vec3(r32 x, r32 y, r32 z) : x(x), y(y), z(z) {}

		inline explicit vec3(r32 scalar) : x(scalar), y(scalar), z(scalar) {}

		inline explicit vec3(const r32 coordinate[3]) : x(coordinate[0]), y(coordinate[1]), z(coordinate[2]) {}

		inline explicit vec3(const i32 coordinate[3]) {
			x = (r32)coordinate[0];
			y = (r32)coordinate[1];
			z = (r32)coordinate[2];
		}

		inline explicit vec3(r32* const r) : x(r[0]), y(r[1]), z(r[2]) {}

		inline r32 operator[] (i32 i) const {
			return *(&x + 1);
		}

		inline r32& operator[] (i32 i) {
			return *(&x + i);
		}

		inline b8 operator== (const vec3& rvector) const {
			return (x == rvector.x) && (y == rvector.y) && (z == rvector.z);
		}

		inline b8 operator!= (const vec3& rvector) const {
			return (x != rvector.x) || (y != rvector.y) || (z != rvector.z);
		}

		inline vec3 operator+ (const vec3& rvector) const {
			return vec3(x + rvector.x, y + rvector.y, z + rvector.z);
		}

		inline vec3 operator- (const vec3& rvector) const {
			return vec3(x - rvector.x, y - rvector.y, z - rvector.z);
		}

		inline vec3 operator* (const vec3& rvector) const {
			return vec3(x * rvector.x, y * rvector.y, z * rvector.z);
		}

		inline vec3 operator* (r32 scalar) const {
			return vec3(x * scalar, y * scalar, z * scalar);
		}

		inline vec3 operator/ (r32 scalar) const {
			r32 nscalar = 1.0f / scalar;
			return vec3(x * nscalar, y * nscalar, z * nscalar);
		}

		inline vec3 operator/ (const vec3& rvector) const {
			return vec3(x / rvector.x, y / rvector.y, z / rvector.z);
		}

		inline vec3 operator+ () const {
			return *this;
		}

		inline vec3 operator- () const {
			return vec3(-x, -y, -z);
		}

		inline friend vec3 operator* (r32 scalar, const vec3& rvector) {
			return vec3(scalar * rvector.x, scalar * rvector.y, scalar * rvector.z);
		}

		inline friend vec3 operator/ (r32 scalar, const vec3& rvector) {
			return vec3(scalar / rvector.x, scalar / rvector.y, scalar / rvector.z);
		}

		inline vec3& operator+= (const vec3& rvector) {
			x += rvector.x;
			y += rvector.y;
			z += rvector.z;

			return *this;
		}

		inline vec3& operator+= (r32 scalar) {
			x += scalar;
			y += scalar;
			z += scalar;

			return *this;
		}

		inline vec3& operator-= (const vec3& rvector) {
			x -= rvector.x;
			y -= rvector.y;
			z -= rvector.z;

			return *this;
		}

		inline vec3& operator-= (r32 scalar) {
			x -= scalar;
			y -= scalar;
			z -= scalar;

			return *this;
		}

		inline vec3& operator*= (const vec3& rvector) {
			x *= rvector.x;
			y *= rvector.y;
			z *= rvector.z;

			return *this;
		}

		inline vec3& operator*= (r32 scalar) {
			x *= scalar;
			y *= scalar;
			z *= scalar;

			return *this;
		}

		inline vec3& operator/= (const vec3& rvector) {
			x /= rvector.x;
			y /= rvector.y;
			z /= rvector.z;

			return *this;
		}

		inline vec3& operator/= (r32 scalar) {

			r32 nscalar = 1.0f / scalar;

			x *= nscalar;
			y *= nscalar;
			z *= nscalar;

			return *this;
		}

		inline r32 length() const {
			return sqrt(x*x + y*y + z*z);
		}

		inline r32 length_squared() const {
			return x*x + y*y + z*z;
		}

		inline r32 dot(const vec3& other) const {
			return x * other.x + y * other.y + z * other.z;
		}

		inline r32 normalize() {
			r32 length = sqrt(x*x + y*y + z*z);

			r32 scaleLen = 1.0f / length;
			x *= scaleLen;
			y *= scaleLen;
			z *= scaleLen;

			return length;
		}

		inline static vec3 normalize(const vec3& vec) {
			r32 length = sqrt(vec.x*vec.x + vec.y*vec.y + vec.z*vec.z);
			return vec3(vec.x / length, vec.y / length, vec.z / length);
		}

		inline vec3 cross(const vec3& rvector) const {
			return vec3(y * rvector.z - z * rvector.y, z * rvector.x - x * rvector.z, x * rvector.y - y * rvector.x);
		}

		inline vec3 reflect(const vec3& normal) const {
			return *this - (2.0f * this->dot(normal) * normal);
		}

		static inline vec3 lerp(r32 x, const vec3& a, const vec3& b) {
			return (a * (1.0f - x)) + (b * x);
		}


	};

	struct vec4
	{
		r32 x;
		r32 y;
		r32 z;
		r32 w;

		static const vec4 zero;

		vec4() : x(0), y(0), z(0), w(0) {}

		vec4(r32 x, r32 y, r32 z, r32 w) : x(x), y(y), z(z), w(w) {}

		inline explicit vec4(r32 scalar) : x(scalar), y(scalar), z(scalar), w(scalar) {}

		inline explicit vec4(const r32 coordinate[4]) : x(coordinate[0]), y(coordinate[1]), z(coordinate[2]), w(coordinate[3]) {}

		inline explicit vec4(const i32 coordinate[4]) {
			x = (r32)coordinate[0];
			y = (r32)coordinate[1];
			z = (r32)coordinate[2];
			w = (r32)coordinate[3];
		}

		inline explicit vec4(r32* const r) : x(r[0]), y(r[1]), z(r[2]), w(r[3]) {}

		inline r32 operator[] (i32 i) const {
			return *(&x + 1);
		}

		inline r32& operator[] (i32 i) {
			return *(&x + i);
		}

		inline b8 operator== (const vec4& rvector) const {
			return (x == rvector.x) && (y == rvector.y) && (z == rvector.z) && (w == rvector.w);
		}

		inline b8 operator!= (const vec4& rvector) const {
			return (x != rvector.x) || (y != rvector.y) || (z != rvector.z) || (w != rvector.w);
		}

		inline vec4 operator+ (const vec4& rvector) const {
			return vec4(x + rvector.x, y + rvector.y, z + rvector.z, w + rvector.w);
		}

		inline vec4 operator- (const vec4& rvector) const {
			return vec4(x - rvector.x, y - rvector.y, z - rvector.z, w - rvector.w);
		}

		inline vec4 operator* (const vec4& rvector) const {
			return vec4(x * rvector.x, y * rvector.y, z * rvector.z, w * rvector.w);
		}

		inline vec4 operator* (r32 scalar) const {
			return vec4(x * scalar, y * scalar, z * scalar, w * scalar);
		}

		inline vec4 operator/ (r32 scalar) const {
			r32 nscalar = 1.0f / scalar;
			return vec4(x * nscalar, y * nscalar, z * nscalar, w * nscalar);
		}

		inline vec4 operator/ (const vec4& rvector) const {
			return vec4(x / rvector.x, y / rvector.y, z / rvector.z, w / rvector.w);
		}

		inline vec4 operator+ () const {
			return *this;
		}

		inline vec4 operator- () const {
			return vec4(-x, -y, -z, -w);
		}

		inline friend vec4 operator* (r32 scalar, const vec4& rvector) {
			return vec4(scalar * rvector.x, scalar * rvector.y, scalar * rvector.z, scalar * rvector.w);
		}

		inline friend vec4 operator/ (r32 scalar, const vec4& rvector) {
			return vec4(scalar / rvector.x, scalar / rvector.y, scalar / rvector.z, scalar / rvector.w);
		}

		inline vec4& operator+= (const vec4& rvector) {
			x += rvector.x;
			y += rvector.y;
			z += rvector.z;
			w += rvector.w;

			return *this;
		}

		inline vec4& operator+= (r32 scalar) {
			x += scalar;
			y += scalar;
			z += scalar;
			w += scalar;

			return *this;
		}

		inline vec4& operator-= (const vec4& rvector) {
			x -= rvector.x;
			y -= rvector.y;
			z -= rvector.z;
			w -= rvector.w;

			return *this;
		}

		inline vec4& operator-= (r32 scalar) {
			x -= scalar;
			y -= scalar;
			z -= scalar;
			w -= scalar;

			return *this;
		}

		inline vec4& operator*= (const vec4& rvector) {
			x *= rvector.x;
			y *= rvector.y;
			z *= rvector.z;
			w *= rvector.w;

			return *this;
		}

		inline vec4& operator*= (r32 scalar) {
			x *= scalar;
			y *= scalar;
			z *= scalar;
			w *= scalar;

			return *this;
		}

		inline vec4& operator/= (const vec4& rvector) {
			x /= rvector.x;
			y /= rvector.y;
			z /= rvector.z;
			w /= rvector.w;

			return *this;
		}

		inline vec4& operator/= (r32 scalar) {

			r32 nscalar = 1.0f / scalar;

			x *= nscalar;
			y *= nscalar;
			z *= nscalar;
			w *= nscalar;

			return *this;
		}

		inline r32 dot(const vec4& other) const {
			return x * other.x + y * other.y + z * other.z + w * other.w;
		}

		static inline vec4 lerp(r32 x, const vec4& a, const vec4& b) {
			return (a * (1.0f - x)) + (b * x);
		}


	};

	struct plane
	{
		vec3 normal;
		r32 d;

		plane();
		plane(const plane&);
		plane(const vec3&, r32);
		plane(r32, r32, r32, r32);
		plane(const vec3&, const vec3&);
		plane(const vec3&, const vec3&, const vec3&);

		r32 distance(const vec3&) const;

		void redefine(const vec3&, const vec3&, const vec3&);

		void redefine(const vec3&, const vec3&);

		vec3 project(const vec3&) const;

		r32 normalize(void);

		b8 operator== (const plane& rplane) const {
			return d == rplane.d && normal == rplane.normal;
		}

		b8 operator!= (const plane& rplane) const {
			return d != rplane.d || normal != rplane.normal;
		}
	};

	struct mat3
	{
		// indexed by [row][col]
		r32 m[3][3];

		static const mat3 zero;
		static const mat3 identity;

		inline mat3() {}

		inline explicit mat3(const r32 arr[3][3]) {
			memcpy(m, arr, 9 * sizeof(r32));
		}

		inline mat3(const mat3& other) {
			memcpy(m, other.m, 9 * sizeof(r32));
		}

		mat3(r32 entry00, r32 entry01, r32 entry02,
			r32 entry10, r32 entry11, r32 entry12,
			r32 entry20, r32 entry21, r32 entry22) {

			m[0][0] = entry00;
			m[0][1] = entry01;
			m[0][2] = entry02;
			m[1][0] = entry10;
			m[1][1] = entry11;
			m[1][2] = entry12;
			m[2][0] = entry20;
			m[2][1] = entry21;
			m[2][2] = entry22;
		}

		inline const r32* operator[] (i32 row) const {
			return m[row];
		}

		inline r32* operator[] (i32 row) {
			return m[row];
		}

		vec3 get_column(i32 iCol) const;
		void set_column(i32 iCol, const vec3& vec);
		void from_axes(const vec3& xAxis, const vec3& yAxis, const vec3& zAxis);

		inline mat3& operator= (const mat3& other) {
			memcpy(m, other.m, 9 * sizeof(r32));
			return *this;
		}

		b8 operator== (const mat3&) const;

		inline b8 operator!= (const mat3& other) const {
			return !operator==(other);
		}

		mat3 operator+ (const mat3&) const;

		mat3 operator- (const mat3&) const;

		mat3 operator* (const mat3&) const;

		mat3 operator- () const;

		vec3 operator* (const vec3&) const;

		friend vec3 operator* (const vec3&, const mat3&);

		mat3 operator* (r32) const;

		friend mat3 operator* (r32, const mat3&);

		mat3 transpose() const;
		b8 inverse(mat3*, r32 = 1e-06f) const;
		mat3 inverse(r32 = 1e-06f) const;
		r32 determinant() const;

		void orthonormalize();
		void qdu_decomposition(mat3*, vec3*, vec3*) const;
	};

	struct quat
	{
		r32 w, x, y, z;

		static const quat zero;
		static const quat identity;
		static const r32 epsilon;

		inline quat() : w(1), x(0), y(0), z(0) {}

		inline quat(r32 w, r32 x, r32 y, r32 z) : w(w), x(x), y(y), z(z) {}

		inline quat(const mat3& rot) {
			this->from_rotation_matrix(rot);
		}

		inline quat(r32 angle, const vec3& axis) {
			this->from_angle_axis(angle, axis);
		}

		inline quat(const vec3& xaxis, const vec3& yaxis, const vec3& zaxis) {
			this->from_axes(xaxis, yaxis, zaxis);
		}

		inline quat(const vec3 axes[3]) {
			this->from_axes(axes);
		}

		inline quat(r32* valptr) {
			memcpy(&w, valptr, sizeof(r32) * 4);
		}

		inline r32 operator[] (i32 i) const {
			return *(&w + i);
		}

		inline r32& operator[] (i32 i) {
			return *(&w + i);
		}

		void from_rotation_matrix(const mat3&);
		void to_rotation_matrix(mat3*) const;
		void from_angle_axis(r32, const vec3&);
		void to_angle_axis(r32*, vec3*) const;
		void from_axes(const vec3[]);
		void from_axes(const vec3&, const vec3&, const vec3&);
		void to_axes(vec3*) const;
		void to_axes(vec3*, vec3*, vec3*) const;

		vec3 x_axis() const;

		vec3 y_axis() const;

		vec3 z_axis() const;

		quat operator+ (const quat& rquat) const;
		quat operator- (const quat& rquat) const;
		quat operator* (const quat& rquat) const;
		quat operator* (r32 scalar) const;
		quat& operator*= (const quat&);
		friend quat operator* (r32 scalar, const quat& rquat);
		quat operator- () const;

		inline b8 operator== (const quat& rquat) const {
			return (rquat.x == x) && (rquat.y == y) && (rquat.z == z) && (rquat.w == w);
		}

		inline b8 operator!= (const quat& rquat) const {
			return !operator==(rquat);
		}

		r32 dot(const quat& rquat) const;
		r32 length() const;
		r32 normalize(void);
		static quat normalize(const quat& q);
		quat inverse() const; // apply to non-zero quat
		quat unit_inverse() const; // apply to unit-length quat
		quat exp() const;
		quat log() const;

		// rotation of a vector by a quat
		vec3 operator* (const vec3& rvector) const;

		static quat euler(r32 pitch, r32 yaw, r32 roll);

		static r32 angle(const quat& a, const quat& b);

		static quat look(const vec3& dir);

		static quat slerp(r32 T, const quat& aquat, const quat& bquat);

		static quat slerp_extra_spins(r32 T, const quat& aquat, const quat& bquat, i32 extraSpins);

		// setup for spherical quadratic interpoliation
		static void intermediate(const quat& quat0, const quat& quat1, const quat& quat2, quat& outa, quat& outb);

		// spherical quadratic interpolation
		static quat squad(r32 T, const quat& aquat, const quat& outa, const quat& outb, const quat& bquat);

		static quat nlerp(r32 T, const quat& aquat, const quat& bquat, b8 shortestPath = false);

	};

	struct mat4
	{
		//indexed by [row][col]
		union {
			r32 m[4][4];
			r32 _m[16];
		};

		static const mat4 zero;
		static const mat4 identity;

		inline mat4() {}

		inline mat4(r32 m00, r32 m01, r32 m02, r32 m03,
			r32 m10, r32 m11, r32 m12, r32 m13,
			r32 m20, r32 m21, r32 m22, r32 m23,
			r32 m30, r32 m31, r32 m32, r32 m33) {
			m[0][0] = m00;
			m[0][1] = m01;
			m[0][2] = m02;
			m[0][3] = m03;
			m[1][0] = m10;
			m[1][1] = m11;
			m[1][2] = m12;
			m[1][3] = m13;
			m[2][0] = m20;
			m[2][1] = m21;
			m[2][2] = m22;
			m[2][3] = m23;
			m[3][0] = m30;
			m[3][1] = m31;
			m[3][2] = m32;
			m[3][3] = m33;
		}

		inline mat4(const mat3& m3x3) {
			operator=(identity);
			operator=(m3x3);
		}

		inline mat4(const quat& rot) {
			mat3 m3x3;
			rot.to_rotation_matrix(&m3x3);
			operator=(identity);
			operator=(m3x3);
		}

		inline r32* operator[] (i32 column) {
			return m[column];
		}

		inline const r32* operator[] (i32 column) const {
			return m[column];
		}

		inline mat4 concatenate(const mat4 &m2) const {
			mat4 r;
			r.m[0][0] = m[0][0] * m2[0][0] + m[0][1] * m2.m[1][0] + m[0][2] * m2.m[2][0] + m[0][3] * m2.m[3][0];
			r.m[0][1] = m[0][0] * m2[0][1] + m[0][1] * m2.m[1][1] + m[0][2] * m2.m[2][1] + m[0][3] * m2.m[3][1];
			r.m[0][2] = m[0][0] * m2[0][2] + m[0][1] * m2.m[1][2] + m[0][2] * m2.m[2][2] + m[0][3] * m2.m[3][2];
			r.m[0][3] = m[0][0] * m2[0][3] + m[0][1] * m2.m[1][3] + m[0][2] * m2.m[2][3] + m[0][3] * m2.m[3][3];

			r.m[1][0] = m[1][0] * m2[0][0] + m[1][1] * m2.m[1][0] + m[1][2] * m2.m[2][0] + m[1][3] * m2.m[3][0];
			r.m[1][1] = m[1][0] * m2[0][1] + m[1][1] * m2.m[1][1] + m[1][2] * m2.m[2][1] + m[1][3] * m2.m[3][1];
			r.m[1][2] = m[1][0] * m2[0][2] + m[1][1] * m2.m[1][2] + m[1][2] * m2.m[2][2] + m[1][3] * m2.m[3][2];
			r.m[1][3] = m[1][0] * m2[0][3] + m[1][1] * m2.m[1][3] + m[1][2] * m2.m[2][3] + m[1][3] * m2.m[3][3];

			r.m[2][0] = m[2][0] * m2[0][0] + m[2][1] * m2.m[1][0] + m[2][2] * m2.m[2][0] + m[2][3] * m2.m[3][0];
			r.m[2][1] = m[2][0] * m2[0][1] + m[2][1] * m2.m[1][1] + m[2][2] * m2.m[2][1] + m[2][3] * m2.m[3][1];
			r.m[2][2] = m[2][0] * m2[0][2] + m[2][1] * m2.m[1][2] + m[2][2] * m2.m[2][2] + m[2][3] * m2.m[3][2];
			r.m[2][3] = m[2][0] * m2[0][3] + m[2][1] * m2.m[1][3] + m[2][2] * m2.m[2][3] + m[2][3] * m2.m[3][3];

			r.m[3][0] = m[3][0] * m2[0][0] + m[3][1] * m2.m[1][0] + m[3][2] * m2.m[2][0] + m[3][3] * m2.m[3][0];
			r.m[3][1] = m[3][0] * m2[0][1] + m[3][1] * m2.m[1][1] + m[3][2] * m2.m[2][1] + m[3][3] * m2.m[3][1];
			r.m[3][2] = m[3][0] * m2[0][2] + m[3][1] * m2.m[1][2] + m[3][2] * m2.m[2][2] + m[3][3] * m2.m[3][2];
			r.m[3][3] = m[3][0] * m2[0][3] + m[3][1] * m2.m[1][3] + m[3][2] * m2.m[2][3] + m[3][3] * m2.m[3][3];

			return r;
		}

		inline mat4 operator * (const mat4 &m2) const {
			return concatenate(m2);
		}

		inline vec4 operator * (const vec4& v) const {
			return vec4(
				m[0][0] * v.x + m[1][0] * v.y + m[2][0] * v.z + m[3][0] * v.w,
				m[0][1] * v.x + m[1][1] * v.y + m[2][1] * v.z + m[3][1] * v.w,
				m[0][2] * v.x + m[1][2] * v.y + m[2][2] * v.z + m[3][2] * v.w,
				m[0][3] * v.x + m[1][3] * v.y + m[2][3] * v.z + m[3][3] * v.w
			);
		}

		inline plane operator* (const plane& p) const {
			plane ret;
			mat4 invTrans = inverse().transpose();
			vec4 v4(p.normal.x, p.normal.y, p.normal.z, p.d);
			v4 = invTrans * v4;
			ret.normal.x = v4.x;
			ret.normal.y = v4.y;
			ret.normal.z = v4.z;
			ret.d = v4.w / ret.normal.normalize();

			return ret;
		}

		inline mat4 operator+ (const mat4 &m2) const {
			mat4 r;

			r.m[0][0] = m[0][0] + m2.m[0][0];
			r.m[0][1] = m[0][1] + m2.m[0][1];
			r.m[0][2] = m[0][2] + m2.m[0][2];
			r.m[0][3] = m[0][3] + m2.m[0][3];

			r.m[1][0] = m[1][0] + m2.m[1][0];
			r.m[1][1] = m[1][1] + m2.m[1][1];
			r.m[1][2] = m[1][2] + m2.m[1][2];
			r.m[1][3] = m[1][3] + m2.m[1][3];

			r.m[2][0] = m[2][0] + m2.m[2][0];
			r.m[2][1] = m[2][1] + m2.m[2][1];
			r.m[2][2] = m[2][2] + m2.m[2][2];
			r.m[2][3] = m[2][3] + m2.m[2][3];

			r.m[3][0] = m[3][0] + m2.m[3][0];
			r.m[3][1] = m[3][1] + m2.m[3][1];
			r.m[3][2] = m[3][2] + m2.m[3][2];
			r.m[3][3] = m[3][3] + m2.m[3][3];

			return r;
		}

		inline mat4 operator- (const mat4 &m2) const {
			mat4 r;

			r.m[0][0] = m[0][0] - m2.m[0][0];
			r.m[0][1] = m[0][1] - m2.m[0][1];
			r.m[0][2] = m[0][2] - m2.m[0][2];
			r.m[0][3] = m[0][3] - m2.m[0][3];

			r.m[1][0] = m[1][0] - m2.m[1][0];
			r.m[1][1] = m[1][1] - m2.m[1][1];
			r.m[1][2] = m[1][2] - m2.m[1][2];
			r.m[1][3] = m[1][3] - m2.m[1][3];

			r.m[2][0] = m[2][0] - m2.m[2][0];
			r.m[2][1] = m[2][1] - m2.m[2][1];
			r.m[2][2] = m[2][2] - m2.m[2][2];
			r.m[2][3] = m[2][3] - m2.m[2][3];

			r.m[3][0] = m[3][0] - m2.m[3][0];
			r.m[3][1] = m[3][1] - m2.m[3][1];
			r.m[3][2] = m[3][2] - m2.m[3][2];
			r.m[3][3] = m[3][3] - m2.m[3][3];

			return r;
		}

		inline b8 operator == (const mat4& m2) const {
			if (
				m[0][0] != m2.m[0][0] || m[0][1] != m2.m[0][1] || m[0][2] != m2.m[0][2] || m[0][3] != m2.m[0][3] ||
				m[1][0] != m2.m[1][0] || m[1][1] != m2.m[1][1] || m[1][2] != m2.m[1][2] || m[1][3] != m2.m[0][3] ||
				m[2][0] != m2.m[2][0] || m[2][1] != m2.m[2][1] || m[2][2] != m2.m[2][2] || m[1][3] != m2.m[0][3] ||
				m[3][0] != m2.m[3][0] || m[3][1] != m2.m[3][1] || m[3][2] != m2.m[3][2] || m[1][3] != m2.m[0][3])
				return false;
			return true;
		}

		inline b8 operator != (const mat4& m2) const {
			if (
				m[0][0] != m2.m[0][0] || m[0][1] != m2.m[0][1] || m[0][2] != m2.m[0][2] || m[0][3] != m2.m[0][3] ||
				m[1][0] != m2.m[1][0] || m[1][1] != m2.m[1][1] || m[1][2] != m2.m[1][2] || m[1][3] != m2.m[0][3] ||
				m[2][0] != m2.m[2][0] || m[2][1] != m2.m[2][1] || m[2][2] != m2.m[2][2] || m[1][3] != m2.m[0][3] ||
				m[3][0] != m2.m[3][0] || m[3][1] != m2.m[3][1] || m[3][2] != m2.m[3][2] || m[1][3] != m2.m[0][3])
				return true;
			return false;
		}

		inline void operator= (const mat3& mat3) {
			m[0][0] = mat3.m[0][0]; m[0][1] = mat3.m[0][1]; m[0][2] = mat3.m[0][2];
			m[1][0] = mat3.m[1][0]; m[1][1] = mat3.m[1][1]; m[1][2] = mat3.m[1][2];
			m[2][0] = mat3.m[2][0]; m[2][1] = mat3.m[2][1]; m[2][2] = mat3.m[2][2];
		}

		inline mat4 transpose(void) const {
			return mat4(m[0][0], m[0][1], m[0][2], m[0][3],
				m[1][0], m[1][1], m[1][2], m[1][3],
				m[2][0], m[2][1], m[2][2], m[2][3],
				m[3][0], m[3][1], m[3][2], m[3][3]);
		}

		inline void translation(const vec3& v) {
			m[3][0] = v.x;
			m[3][1] = v.y;
			m[3][2] = v.z;
		}

		inline void translate(const vec3& v) {
			m[3][0] += v.x;
			m[3][1] += v.y;
			m[3][2] += v.z;
		}

		inline void rotation(const quat& q) {
			mat3 r;
			q.to_rotation_matrix(&r);
			operator=(r);
		}

		inline void rotation(const mat3& r) {
			operator=(r);
		}

		inline void rotate(const quat& q) {
			mat3 rot3x3;
			q.to_rotation_matrix(&rot3x3);

			m[0][0] *= rot3x3[0][0]; m[1][0] *= rot3x3[1][0]; m[2][0] *= rot3x3[2][0];
			m[0][1] *= rot3x3[0][1]; m[1][1] *= rot3x3[1][1]; m[2][1] *= rot3x3[2][1];
			m[0][2] *= rot3x3[0][2]; m[1][2] *= rot3x3[1][2]; m[2][2] *= rot3x3[2][2];
		}

		inline vec3 translation() const {
			return vec3(m[3][0], m[3][1], m[3][2]);
		}

		inline void make_translate(const vec3& v) {
			m[0][0] = 1.0; m[1][0] = 0.0; m[2][0] = 0.0; m[3][0] = v.x;
			m[0][1] = 0.0; m[1][1] = 1.0; m[2][1] = 0.0; m[3][1] = v.y;
			m[0][2] = 0.0; m[1][2] = 0.0; m[2][2] = 1.0; m[3][2] = v.z;
			m[0][3] = 0.0; m[1][3] = 0.0; m[2][3] = 0.0; m[3][3] = 1.0;
		}

		inline void make_translate(r32 tx, r32 ty, r32 tz) {
			m[0][0] = 1.0; m[1][0] = 0.0; m[2][0] = 0.0; m[3][0] = tx;
			m[0][1] = 0.0; m[1][1] = 1.0; m[2][1] = 0.0; m[3][1] = ty;
			m[0][2] = 0.0; m[1][2] = 0.0; m[2][2] = 1.0; m[3][2] = tz;
			m[0][3] = 0.0; m[1][3] = 0.0; m[2][3] = 0.0; m[3][3] = 1.0;
		}

		inline static mat4 make_translation(const vec3& v) {
			mat4 r;

			r.m[0][0] = 1.0; r.m[1][0] = 0.0; r.m[2][0] = 0.0; r.m[3][0] = v.x;
			r.m[0][1] = 0.0; r.m[1][1] = 1.0; r.m[2][1] = 0.0; r.m[3][1] = v.y;
			r.m[0][2] = 0.0; r.m[1][2] = 0.0; r.m[2][2] = 1.0; r.m[3][2] = v.z;
			r.m[0][3] = 0.0; r.m[1][3] = 0.0; r.m[2][3] = 0.0; r.m[3][3] = 1.0;

			return r;
		}

		inline static mat4 make_translation(r32 tx, r32 ty, r32 tz) {
			mat4 r;

			r.m[0][0] = 1.0; r.m[1][0] = 0.0; r.m[2][0] = 0.0; r.m[3][0] = tx;
			r.m[0][1] = 0.0; r.m[1][1] = 1.0; r.m[2][1] = 0.0; r.m[3][1] = ty;
			r.m[0][2] = 0.0; r.m[1][2] = 0.0; r.m[2][2] = 1.0; r.m[3][2] = tz;
			r.m[0][3] = 0.0; r.m[1][3] = 0.0; r.m[2][3] = 0.0; r.m[3][3] = 1.0;

			return r;
		}

		inline void scale(const vec3& v) {
			m[0][0] *= v.x; m[1][0] *= v.y; m[2][0] *= v.z;
			m[0][1] *= v.x; m[1][1] *= v.y; m[2][1] *= v.z;
			m[0][2] *= v.x; m[1][2] *= v.y; m[2][2] *= v.z;
		}

		inline static mat4 make_scale(const vec3& v) {
			mat4 r;

			r.m[0][0] = v.x; r.m[1][0] = 0.0; r.m[2][0] = 0.0; r.m[3][0] = 0.0;
			r.m[0][1] = 0.0; r.m[1][1] = v.y; r.m[2][1] = 0.0; r.m[3][1] = 0.0;
			r.m[0][2] = 0.0; r.m[1][2] = 0.0; r.m[2][2] = v.z; r.m[3][2] = 0.0;
			r.m[0][3] = 0.0; r.m[1][3] = 0.0; r.m[2][3] = 0.0; r.m[3][3] = 1.0;

			return r;
		}

		inline static mat4 make_scale(r32 sx, r32 sy, r32 sz) {
			mat4 r;

			r.m[0][0] = sx; r.m[1][0] = 0.0; r.m[2][0] = 0.0; r.m[3][0] = 0.0;
			r.m[0][1] = 0.0; r.m[1][1] = sy; r.m[2][1] = 0.0; r.m[3][1] = 0.0;
			r.m[0][2] = 0.0; r.m[1][2] = 0.0; r.m[2][2] = sz; r.m[3][2] = 0.0;
			r.m[0][3] = 0.0; r.m[1][3] = 0.0; r.m[2][3] = 0.0; r.m[3][3] = 1.0;

			return r;
		}

		inline void extract_mat3(mat3& m3x3) const {
			m3x3.m[0][0] = m[0][0];
			m3x3.m[0][1] = m[0][1];
			m3x3.m[0][2] = m[0][2];
			m3x3.m[1][0] = m[1][0];
			m3x3.m[1][1] = m[1][1];
			m3x3.m[1][2] = m[1][2];
			m3x3.m[2][0] = m[2][0];
			m3x3.m[2][1] = m[2][1];
			m3x3.m[2][2] = m[2][2];
		}

		inline quat extract_quat() const {
			mat3 m3x3;
			extract_mat3(m3x3);
			return quat(m3x3);
		}

		inline mat4 operator*(r32 scalar) const {
			return mat4(
				scalar*m[0][0], scalar*m[0][1], scalar*m[0][2], scalar*m[0][3],
				scalar*m[1][0], scalar*m[1][1], scalar*m[1][2], scalar*m[1][3],
				scalar*m[2][0], scalar*m[2][1], scalar*m[2][2], scalar*m[2][3],
				scalar*m[3][0], scalar*m[3][1], scalar*m[3][2], scalar*m[3][3]);
		}

		mat4 adjoint() const;
		r32 determinant() const;
		mat4 inverse() const;

		static mat4 perspective(r32 fov, r32 aspect, r32 near, r32 far);
		static mat4 orthographic(r32 fov, r32 aspect, r32 near, r32 far);
		static mat4 look(const vec3& eye, const vec3& scale, const vec3& up);

		void make_transform(const vec3& position, const vec3& scale, const quat& orientation);
		void make_inverse_transform(const vec3& position, const vec3& scale, const quat& orientation);

		void decomposition(vec3*, vec3*, quat*) const;

		inline b8 is_affine(void) const {
			return m[3][0] == 0 && m[3][1] == 0 && m[3][2] == 0 && m[3][3] == 1;
		}

		mat4 inverse_affine(void) const;

		inline mat4 concatenate_affine(const mat4 &m2) const {
			return mat4(
				m[0][0] * m2.m[0][0] + m[0][1] * m2.m[1][0] + m[0][2] * m2.m[2][0],
				m[0][0] * m2.m[0][1] + m[0][1] * m2.m[1][1] + m[0][2] * m2.m[2][1],
				m[0][0] * m2.m[0][2] + m[0][1] * m2.m[1][2] + m[0][2] * m2.m[2][2],
				m[0][0] * m2.m[0][3] + m[0][1] * m2.m[1][3] + m[0][2] * m2.m[2][3] + m[0][3],

				m[1][0] * m2.m[0][0] + m[1][1] * m2.m[1][0] + m[1][2] * m2.m[2][0],
				m[1][0] * m2.m[0][1] + m[1][1] * m2.m[1][1] + m[1][2] * m2.m[2][1],
				m[1][0] * m2.m[0][2] + m[1][1] * m2.m[1][2] + m[1][2] * m2.m[2][2],
				m[1][0] * m2.m[0][3] + m[1][1] * m2.m[1][3] + m[1][2] * m2.m[2][3] + m[1][3],

				m[2][0] * m2.m[0][0] + m[2][1] * m2.m[1][0] + m[2][2] * m2.m[2][0],
				m[2][0] * m2.m[0][1] + m[2][1] * m2.m[1][1] + m[2][2] * m2.m[2][1],
				m[2][0] * m2.m[0][2] + m[2][1] * m2.m[1][2] + m[2][2] * m2.m[2][2],
				m[2][0] * m2.m[0][3] + m[2][1] * m2.m[1][3] + m[2][2] * m2.m[2][3] + m[2][3],

				0, 0, 0, 1);
		}

		inline vec3 transform_affine(const vec3& v) const {
			return vec3(
				m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3],
				m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3],
				m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3]);
		}

		inline vec4 transform_affine(const vec4& v) const {
			return vec4(
				m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3] * v.w,
				m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3] * v.w,
				m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3] * v.w,
				v.w);
		}
	};

	inline vec4 operator* (const vec4& v, const mat4& mat) {
		return vec4(
			v.x*mat[0][0] + v.y*mat[1][0] + v.z*mat[2][0] + v.w*mat[3][0],
			v.x*mat[0][1] + v.y*mat[1][1] + v.z*mat[2][1] + v.w*mat[3][1],
			v.x*mat[0][2] + v.y*mat[1][2] + v.z*mat[2][2] + v.w*mat[3][2],
			v.x*mat[0][3] + v.y*mat[1][3] + v.z*mat[2][3] + v.w*mat[3][3]
		);
	}

	namespace sMath
	{
		vec3 triangle_closest_point(const vec3&, const vec3&, const vec3&, const vec3&);

		inline r32 clampf(r32 t, r32 a, r32 b) {
			return f_min(b, f_max(a, t));
		}

		inline r32 lerpf(r32 t, r32 a, r32 b) {
			return a + (b - a) * t;
		}

		inline r64 lerpd(r64 t, r64 a, r64 b) {
			return a + (b - a) * t;
		}

		inline r32 closest_angle(r32 x, r32 y) {
			r32 result = x;
			while (result > y + PI)
				result -= PI * 2.0f;
			while (result < y - PI)
				result += PI * 2.0f;
			return result;
		}

		inline r32 angle_range(r32 x) {
			return closest_angle(x, 0.0f);
		}

		inline r32 angle_to(r32 x, r32 y) {
			return y - closest_angle(x, y);
		}

		inline r32 rotate_toward(r32 angle, r32 target, r32 delta) {
			r32 closest_target = closest_angle(target, angle);
			if (angle < closest_target)
				return angle_range(f_min(angle + delta, closest_target));
			else
				return angle_range(f_max(angle - delta, closest_target));
		}

		enum class RaySphereIntersection : i8 {
			FrontFace,
			BackFace,
			count
		};

		b8 ray_sphere_intersect(const vec3&, const vec3&, const vec3&, vec3* = 0, RaySphereIntersection = RaySphereIntersection::FrontFace);

		vec3 desaturate(const vec3&);
		vec4 desaturate(const vec4&);
	}

}
