
#include "sMath.hpp"
//#include "sAssert.hpp"

namespace SMOBA
{

	namespace sMath
	{
		vec3 triangle_closest_point(const vec3& u, const vec3& v, const vec3& w, const vec3& p) {
			vec3 edge0 = v - u;
			vec3 edge1 = w - u;
			vec3 v0 = u - p;

			r32 a = edge0.dot(edge0);
			r32 b = edge0.dot(edge1);
			r32 c = edge1.dot(edge1);
			r32 d = edge0.dot(v0);
			r32 e = edge1.dot(v0);

			r32 det = a*c - b*b;
			r32 s = b*e - c*d;
			r32 t = b*d;

			if (s + t < det) {
				if (s < 0.f) {
					if (t < 0.f) {
						if (d < 0.f) {
							s = clampf(-d / a, 0.f, 1.f);
							t = 0.f;
						}
						else {
							s = 0.f;
							t = clampf(-e / c, 0.f, 1.f);
						}
					}
					else {
						s = 0.f;
						t = clampf(-e / c, 0.f, 1.f);
					}
				}
				else if (t < 0.f) {
					s = clampf(-d / a, 0.f, 1.f);
					t = 0.f;
				}
				else {
					r32 invDet = 1.f / det;
					s *= invDet;
					t *= invDet;
				}
			}
			else {
				if (s < 0.f) {
					r32 tmp0 = b + d;
					r32 tmp1 = c + e;
					if (tmp1 > tmp0) {
						r32 numer = tmp1 - tmp0;
						r32 denom = a - 2 * b + c;
						s = clampf(numer / denom, 0.f, 1.f);
						t = 1 - s;
					}
					else {
						t = clampf(-e / c, 0.f, 1.f);
						s = 0.f;
					}
				}
				else if (t < 0.f) {
					if (a + d > b + e) {
						r32 numer = c + e - b - d;
						r32 denom = a - 2 * b + c;
						s = clampf(numer / denom, 0.f, 1.f);
						t = 1 - s;
					}
					else {
						s = clampf(-e / c, 0.f, 1.f);
						t = 0.f;
					}
				}
				else {
					r32 numer = c + e - b - d;
					r32 denom = a - 2 * b + c;
					s = clampf(numer / denom, 0.f, 1.f);
					t = 1.f - s;
				}
			}
			return u + s * edge0 + t * edge1;
		}

		b8 ray_sphere_intersect(const vec3& ray_start, const vec3& ray_end, const vec3& sphere_pos, r32 sphere_radius, vec3* intersection, RaySphereIntersection type) {
			vec3 ray = ray_end - ray_start;
			vec3 sphere_to_ray_start = ray_start - sphere_pos;
			r32 ray_start_distance_sq = sphere_to_ray_start.length_squared();
			r32 sphere_radius_sq = sphere_radius * sphere_radius;
			r32 dot = sphere_to_ray_start.dot(ray);

			if (type == RaySphereIntersection::FrontFace && ray_start_distance_sq < sphere_radius_sq) {
				// we're starting out inside the sphere; raycast hit will be behind ray_start
				ray *= -1.0f;
				dot = sphere_to_ray_start.dot(ray);
				type = RaySphereIntersection::BackFace;
			}

			r32 a = ray.length_squared();
			r32 b = 2.0f * dot;
			r32 c = ray_start_distance_sq - (sphere_radius * sphere_radius);

			r32 delta = (b * b) - 4.0f * a * c;

			if (delta > 0.0f) {
				r32 distance;
				if (type == RaySphereIntersection::BackFace)
					distance = (-b + sqrtf(delta)) / (2.0f * a);
				else
					distance = (-b - sqrtf(delta)) / (2.0f * a);

				if (distance >= 0.0f && distance < 1.0f) {
					if (intersection)
						*intersection = ray_start + ray * distance;
					return true;
				}
			}
			return false;
		}

		const r32 INTENSITY_COEFFICIENT = 0.2f;

		vec3 desaturate(const vec3& c) {
			r32 intensity = c.x * INTENSITY_COEFFICIENT + c.y * INTENSITY_COEFFICIENT + c.z * INTENSITY_COEFFICIENT;
			return vec3(intensity);
		}

		vec4 desaturate(const vec4& c) {
			r32 intensity = c.x * INTENSITY_COEFFICIENT + c.y * INTENSITY_COEFFICIENT + c.z * INTENSITY_COEFFICIENT;
			return vec4(intensity, intensity, intensity, c.w);
		}

	}

	const vec2 vec2::zero(0, 0);
	const vec3 vec3::zero(0, 0, 0);
	const vec4 vec4::zero(0, 0, 0, 0);

	rect rect::outset(r32 padding) const {
		vec2 p(padding, padding);
		return {
			pos - p,
			size + p * 2.0f,
		};
	}

	rect rect::pad(const rect& p) const {
		return {
			pos - p.pos,
			size + p.pos + p.size,
		};
	}

	b8 rect::contains(const vec2& p) const {
		return p.x > pos.x
			&& p.y > pos.y
			&& p.x < pos.x + size.x
			&& p.y < pos.y + size.y;
	}

	const iRect iRect::zero(0, 0, 0, 0);

	b8 iRect::contains(const vec2& point) const {
		return point.x > x
			&& point.y > y
			&& point.x < x + w
			&& point.y < y + h;
	}

	b8 iRect::intersects(const iRect& other) const {
		if (contains(vec2(other.x, other.y))) return true;
		if (contains(vec2(other.x + w, other.y))) return true;
		if (contains(vec2(other.x + w, other.y+h))) return true;
		if (contains(vec2(other.x, other.y+h))) return true;
		return false;
	}

	const quat quat::zero(0, 0, 0, 0);
	const quat quat::identity(1, 0, 0, 0);
	const r32 quat::epsilon = 1e-03f;

	const mat3 mat3::zero(
		0, 0, 0,
		0, 0, 0,
		0, 0, 0);

	const mat3 mat3::identity(
		1, 0, 0,
		0, 1, 0,
		0, 0, 1);

	const mat4 mat4::zero(
		0, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0);

	const mat4 mat4::identity(
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1);

	plane::plane() : normal(vec3::zero), d() {}

	plane::plane(const plane& rhs) : normal(rhs.normal), d(rhs.d) {}

	plane::plane(const vec3& nNormal, r32 d) : normal(nNormal), d(d) {}

	plane::plane(r32 a, r32  b, r32 c, r32 d) : normal(a, b, c), d(d) {}

	plane::plane(const vec3& nNormal, const vec3& nPoint) {
		redefine(nNormal, nPoint);
	}

	plane::plane(const vec3& nPoint0, const vec3& nPoint1, const vec3& nPoint2) {
		redefine(nPoint0, nPoint1, nPoint2);
	}

	r32 plane::distance(const vec3& nPoint) const {
		return normal.dot(nPoint) + d;
	}

	void plane::redefine(const vec3& nPoint0, const vec3& nPoint1, const vec3& nPoint2) {
		vec3 nEdge1 = nPoint1 - nPoint0;
		vec3 nEdge2 = nPoint2 - nPoint0;
		normal = nEdge1.cross(nEdge2);
		normal.normalize();
		d = -(normal.dot(nPoint0));
	}

	void plane::redefine(const vec3& nNormal, const vec3& nPoint) {
		normal = nNormal;
		d = -(nNormal.dot(nPoint));
	}

	vec3 plane::project(const vec3& p) const {
		// We know plane normal is unit length, so use simple method
		mat3 xform;
		xform[0][0] = 1.0f - normal.x * normal.x;
		xform[0][1] = -normal.x * normal.y;
		xform[0][2] = -normal.x * normal.z;
		xform[1][0] = -normal.y * normal.x;
		xform[1][1] = 1.0f - normal.y * normal.y;
		xform[1][2] = -normal.y * normal.z;
		xform[2][0] = -normal.z * normal.x;
		xform[2][1] = -normal.z * normal.y;
		xform[2][2] = 1.0f - normal.z * normal.z;
		return xform * p;
	}

	r32 plane::normalize(void) {
		r32 fLength = normal.length();

		// Will also work for zero-sized vectors, but will change nothing
		// We're not using epsilons because we don't need to.
		//Read http://www.ogre3d.org/forums/viewtopic.php?f=4&t=61259

		if (fLength > r32(0.0f)) {
			r32 fInvLength = 1.0f / fLength;
			normal *= fInvLength;
			d *= fInvLength;
		}
		return fLength;
	}

	vec3 mat3::get_column(i32 iCol) const {
		return vec3(m[0][iCol], m[1][iCol], m[2][iCol]);
	}

	void mat3::set_column(i32 iCol, const vec3& vec) {
		m[0][iCol] = vec.x;
		m[1][iCol] = vec.y;
		m[2][iCol] = vec.z;
	}

	void mat3::from_axes(const vec3& xAxis, const vec3& yAxis, const vec3& zAxis) {
		set_column(0, xAxis);
		set_column(1, yAxis);
		set_column(2, zAxis);
	}

	b8 mat3::operator== (const mat3& nMatrix) const {
		for (i32 iRow = 0; iRow < 3; iRow++) {
			for (i32 iCol = 0; iCol < 3; iCol++) {
				if (m[iRow][iCol] != nMatrix.m[iRow][iCol])
					return false;
			}
		}
		return true;
	}

	mat3 mat3::operator+ (const mat3& rMatrix) const {
		mat3 kSum;
		for (i32 iRow = 0; iRow < 3; iRow++) {
			for (i32 iCol = 0; iCol < 3; iCol++)
				kSum.m[iRow][iCol] = m[iRow][iCol] + rMatrix.m[iRow][iCol];
		}
		return kSum;
	}

	mat3 mat3::operator- (const mat3& rMatrix) const {
		mat3 kDiff;
		for (i32 iRow = 0; iRow < 3; iRow++) {
			for (i32 iCol = 0; iCol < 3; iCol++)
				kDiff.m[iRow][iCol] = m[iRow][iCol] - rMatrix.m[iRow][iCol];
		}
		return kDiff;
	}

	mat3 mat3::operator* (const mat3& rMatrix) const {
		mat3 kProd;
		for (i32 iRow = 0; iRow < 3; iRow++) {
			for (i32 iCol = 0; iCol < 3; iCol++) {
				kProd.m[iRow][iCol] =
					m[iRow][0] * rMatrix.m[0][iCol] +
					m[iRow][1] * rMatrix.m[1][iCol] +
					m[iRow][2] * rMatrix.m[2][iCol];
			}
		}
		return kProd;
	}

	vec3 mat3::operator* (const vec3& rPoint) const {
		vec3 kProd;
		for (i32 iRow = 0; iRow < 3; iRow++) {
			kProd[iRow] =
				m[iRow][0] * rPoint[0] +
				m[iRow][1] * rPoint[1] +
				m[iRow][2] * rPoint[2];
		}
		return kProd;
	}

	vec3 operator* (const vec3& rPoint, const mat3& rMatrix) {
		vec3 kProd;
		for (i32 iRow = 0; iRow < 3; iRow++) {
			kProd[iRow] =
				rPoint[0] * rMatrix.m[0][iRow] +
				rPoint[1] * rMatrix.m[1][iRow] +
				rPoint[2] * rMatrix.m[2][iRow];
		}
		return kProd;
	}

	mat3 mat3::operator- () const {
		mat3 kNeg;
		for (i32 iRow = 0; iRow < 3; iRow++) {
			for (i32 iCol = 0; iCol < 3; iCol++) {
				kNeg[iRow][iCol] = -m[iRow][iCol];
			}
		}
		return kNeg;
	}

	mat3 mat3::operator* (r32 scalar) const {
		mat3 kProd;
		for (i32 iRow = 0; iRow < 3; iRow++) {
			for (i32 iCol = 0; iCol < 3; iCol++) {
				kProd.m[iRow][iCol] = scalar*m[iRow][iCol];
			}
		}
		return kProd;
	}

	mat3 operator* (r32 scalar, const mat3& rMatrix) {
		mat3 kProd;
		for (i32 iRow = 0; iRow < 3; iRow++) {
			for (i32 iCol = 0; iCol < 3; iCol++) {
				kProd.m[iRow][iCol] = scalar*rMatrix.m[iRow][iCol];
			}
		}
		return kProd;
	}

	mat3 mat3::transpose() const {
		mat3 kTranspose;
		for (i32 iRow = 0; iRow < 3; iRow++) {
			for (i32 iCol = 0; iCol < 3; iCol++)
				kTranspose[iRow][iCol] = m[iCol][iRow];
		}
		return kTranspose;
	}

	b8 mat3::inverse(mat3* out, r32 tolerance) const {
		// Invert a 3x3 using cofactors. This is about 8 times faster than
		// the Numerical Recipes code which uses Gaussian elimination.
		(*out)[0][0] = m[1][1] * m[2][2] - m[1][2] * m[2][1];
		(*out)[0][1] = m[0][2] * m[2][1] - m[0][1] * m[2][2];
		(*out)[0][2] = m[0][1] * m[1][2] - m[0][2] * m[1][1];
		(*out)[1][0] = m[1][2] * m[2][0] - m[1][0] * m[2][2];
		(*out)[1][1] = m[0][0] * m[2][2] - m[0][2] * m[2][0];
		(*out)[1][2] = m[0][2] * m[1][0] - m[0][0] * m[1][2];
		(*out)[2][0] = m[1][0] * m[2][1] - m[1][1] * m[2][0];
		(*out)[2][1] = m[0][1] * m[2][0] - m[0][0] * m[2][1];
		(*out)[2][2] = m[0][0] * m[1][1] - m[0][1] * m[1][0];

		r32 det = m[0][0] * (*out)[0][0]
			+ m[0][1] * (*out)[1][0]
			+ m[0][2] * (*out)[2][0];

		if (fabsf(det) <= tolerance)
			return false;

		r32 det_inv = 1.0f / det;
		for (i32 iRow = 0; iRow < 3; iRow++)
		{
			for (i32 iCol = 0; iCol < 3; iCol++)
				(*out)[iRow][iCol] *= det_inv;
		}

		return true;
	}

	mat3 mat3::inverse(r32 tolerance) const {
		mat3 out = mat3::zero;
		inverse(&out, tolerance);
		return out;
	}

	r32 mat3::determinant() const {
		r32 cofactor00 = m[1][1] * m[2][2] - m[1][2] * m[2][1];
		r32 cofactor10 = m[1][2] * m[2][0] - m[1][0] * m[2][2];
		r32 cofactor20 = m[1][0] * m[2][1] - m[1][1] * m[2][0];

		r32 det =
			m[0][0] * cofactor00 +
			m[0][1] * cofactor10 +
			m[0][2] * cofactor20;

		return det;
	}

	void mat3::orthonormalize() {
		// Algorithm uses Gram-Schmidt orthogonalization.  If 'this' matrix is
		// M = [m0|m1|m2], then orthonormal output matrix is Q = [q0|q1|q2],
		//
		//   q0 = m0/|m0|
		//   q1 = (m1-(q0*m1)q0)/|m1-(q0*m1)q0|
		//   q2 = (m2-(q0*m2)q0-(q1*m2)q1)/|m2-(q0*m2)q0-(q1*m2)q1|
		//
		// where |V| indicates length of vector V and A*B indicates dot
		// product of vectors A and B.

		// compute q0
		r32 fInvLength = 1.0f / sqrtf(m[0][0] * m[0][0]
			+ m[1][0] * m[1][0] +
			m[2][0] * m[2][0]);

		m[0][0] *= fInvLength;
		m[1][0] *= fInvLength;
		m[2][0] *= fInvLength;

		// compute q1
		r32 fDot0 =
			m[0][0] * m[0][1] +
			m[1][0] * m[1][1] +
			m[2][0] * m[2][1];

		m[0][1] -= fDot0*m[0][0];
		m[1][1] -= fDot0*m[1][0];
		m[2][1] -= fDot0*m[2][0];

		fInvLength = 1.0f / sqrtf(m[0][1] * m[0][1] +
			m[1][1] * m[1][1] +
			m[2][1] * m[2][1]);

		m[0][1] *= fInvLength;
		m[1][1] *= fInvLength;
		m[2][1] *= fInvLength;

		// compute q2
		r32 fDot1 =
			m[0][1] * m[0][2] +
			m[1][1] * m[1][2] +
			m[2][1] * m[2][2];

		fDot0 =
			m[0][0] * m[0][2] +
			m[1][0] * m[1][2] +
			m[2][0] * m[2][2];

		m[0][2] -= fDot0*m[0][0] + fDot1*m[0][1];
		m[1][2] -= fDot0*m[1][0] + fDot1*m[1][1];
		m[2][2] -= fDot0*m[2][0] + fDot1*m[2][1];

		fInvLength = 1.0f / sqrtf(m[0][2] * m[0][2] +
			m[1][2] * m[1][2] +
			m[2][2] * m[2][2]);

		m[0][2] *= fInvLength;
		m[1][2] *= fInvLength;
		m[2][2] *= fInvLength;
	}

	void mat3::qdu_decomposition(mat3* pq, vec3* pd, vec3* pu) const
	{
		// factor M = QR = QDU where Q is orthogonal, D is diagonal,
		// and U is upper triangular with ones on its diagonal.  Algorithm uses
		// Gram-Schmidt orthogonalization (the QR algorithm).
		//
		// If M = [ m0 | m1 | m2 ] and Q = [ q0 | q1 | q2 ], then
		//
		//   q0 = m0/|m0|
		//   q1 = (m1-(q0*m1)q0)/|m1-(q0*m1)q0|
		//   q2 = (m2-(q0*m2)q0-(q1*m2)q1)/|m2-(q0*m2)q0-(q1*m2)q1|
		//
		// where |V| indicates length of vector V and A*B indicates dot
		// product of vectors A and B.  The matrix R has entries
		//
		//   r00 = q0*m0  r01 = q0*m1  r02 = q0*m2
		//   r10 = 0      r11 = q1*m1  r12 = q1*m2
		//   r20 = 0      r21 = 0      r22 = q2*m2
		//
		// so D = diag(r00,r11,r22) and U has entries u01 = r01/r00,
		// u02 = r02/r00, and u12 = r12/r11.

		// Q = rotation
		// D = scaling
		// U = shear

		// D stores the three diagonal entries r00, r11, r22
		// U stores the entries U[0] = u01, U[1] = u02, U[2] = u12

		mat3& q = *pq;
		vec3& d = *pd;
		vec3& u = *pu;

		// build orthogonal matrix Q
		r32 fInvLength = 1.0f / sqrtf(m[0][0] * m[0][0] + m[1][0] * m[1][0] + m[2][0] * m[2][0]);

		q[0][0] = m[0][0] * fInvLength;
		q[1][0] = m[1][0] * fInvLength;
		q[2][0] = m[2][0] * fInvLength;

		r32 fDot = q[0][0] * m[0][1] + q[1][0] * m[1][1] +
			q[2][0] * m[2][1];
		q[0][1] = m[0][1] - fDot*q[0][0];
		q[1][1] = m[1][1] - fDot*q[1][0];
		q[2][1] = m[2][1] - fDot*q[2][0];
		fInvLength = 1.0f / sqrtf(q[0][1] * q[0][1] + q[1][1] * q[1][1] + q[2][1] * q[2][1]);

		q[0][1] *= fInvLength;
		q[1][1] *= fInvLength;
		q[2][1] *= fInvLength;

		fDot = q[0][0] * m[0][2] + q[1][0] * m[1][2] + q[2][0] * m[2][2];
		q[0][2] = m[0][2] - fDot*q[0][0];
		q[1][2] = m[1][2] - fDot*q[1][0];
		q[2][2] = m[2][2] - fDot*q[2][0];
		fDot = q[0][1] * m[0][2] + q[1][1] * m[1][2] + q[2][1] * m[2][2];
		q[0][2] -= fDot*q[0][1];
		q[1][2] -= fDot*q[1][1];
		q[2][2] -= fDot*q[2][1];
		fInvLength = 1 / sqrtf(q[0][2] * q[0][2] + q[1][2] * q[1][2] + q[2][2] * q[2][2]);

		q[0][2] *= fInvLength;
		q[1][2] *= fInvLength;
		q[2][2] *= fInvLength;

		// guarantee that orthogonal matrix has determinant 1 (no reflections)
		r32 fDet = q[0][0] * q[1][1] * q[2][2] + q[0][1] * q[1][2] * q[2][0] +
			q[0][2] * q[1][0] * q[2][1] - q[0][2] * q[1][1] * q[2][0] -
			q[0][1] * q[1][0] * q[2][2] - q[0][0] * q[1][2] * q[2][1];

		if (fDet < 0.0) {
			for (i32 iRow = 0; iRow < 3; iRow++)
				for (i32 iCol = 0; iCol < 3; iCol++)
					q[iRow][iCol] = -q[iRow][iCol];
		}

		// build "right" matrix R
		mat3 r;
		r[0][0] = q[0][0] * m[0][0] + q[1][0] * m[1][0] + q[2][0] * m[2][0];
		r[0][1] = q[0][0] * m[0][1] + q[1][0] * m[1][1] + q[2][0] * m[2][1];
		r[1][1] = q[0][1] * m[0][1] + q[1][1] * m[1][1] + q[2][1] * m[2][1];
		r[0][2] = q[0][0] * m[0][2] + q[1][0] * m[1][2] + q[2][0] * m[2][2];
		r[1][2] = q[0][1] * m[0][2] + q[1][1] * m[1][2] + q[2][1] * m[2][2];
		r[2][2] = q[0][2] * m[0][2] + q[1][2] * m[1][2] + q[2][2] * m[2][2];

		// the scaling component
		d[0] = r[0][0];
		d[1] = r[1][1];
		d[2] = r[2][2];

		// the shear component
		r32 fInvD0 = 1.0f / d[0];
		u[0] = r[0][1] * fInvD0;
		u[1] = r[0][2] * fInvD0;
		u[2] = r[1][2] / d[1];
	}

	void quat::from_rotation_matrix(const mat3& kRot) {
		// Algorithm in Ken Shoemake's article in 1987 SIGGRAPH course notes
		// article "Quat Calculus and Fast Animation".

		r32 fTrace = kRot[0][0] + kRot[1][1] + kRot[2][2];
		r32 fRoot;

		if (fTrace > 0.0) {
			// |w| > 1/2, may as well choose w > 1/2
			fRoot = sqrtf(fTrace + 1.0f);  // 2w
			w = 0.5f*fRoot;
			fRoot = 0.5f / fRoot;  // 1/(4w)
			x = (kRot[1][2] - kRot[2][1])*fRoot;
			y = (kRot[2][0] - kRot[0][2])*fRoot;
			z = (kRot[0][1] - kRot[1][0])*fRoot;
		}
		else {
			// |w| <= 1/2
			static i32 s_iNext[3] = { 1, 2, 0 };
			i32 i = 0;
			if (kRot[1][1] > kRot[0][0])
				i = 1;
			if (kRot[2][2] > kRot[i][i])
				i = 2;
			i32 j = s_iNext[i];
			i32 k = s_iNext[j];

			fRoot = sqrtf(kRot[i][i] - kRot[j][j] - kRot[k][k] + 1.0f);
			r32* apkQuat[3] = { &x, &y, &z };
			*apkQuat[i] = 0.5f*fRoot;
			fRoot = 0.5f / fRoot;
			w = (kRot[j][k] - kRot[k][j])*fRoot;
			*apkQuat[j] = (kRot[i][j] + kRot[j][i])*fRoot;
			*apkQuat[k] = (kRot[i][k] + kRot[k][i])*fRoot;
		}
	}

	void quat::to_rotation_matrix(mat3* out) const {
		r32 fTx = x + x;
		r32 fTy = y + y;
		r32 fTz = z + z;
		r32 fTwx = fTx*w;
		r32 fTwy = fTy*w;
		r32 fTwz = fTz*w;
		r32 fTxx = fTx*x;
		r32 fTxy = fTy*x;
		r32 fTxz = fTz*x;
		r32 fTyy = fTy*y;
		r32 fTyz = fTz*y;
		r32 fTzz = fTz*z;

		(*out)[0][0] = 1.0f - (fTyy + fTzz);
		(*out)[1][0] = fTxy - fTwz;
		(*out)[2][0] = fTxz + fTwy;
		(*out)[0][1] = fTxy + fTwz;
		(*out)[1][1] = 1.0f - (fTxx + fTzz);
		(*out)[2][1] = fTyz - fTwx;
		(*out)[0][2] = fTxz - fTwy;
		(*out)[1][2] = fTyz + fTwx;
		(*out)[2][2] = 1.0f - (fTxx + fTyy);
	}

	void quat::from_angle_axis(r32 angle, const vec3& axis) {
		// assert:  axis[] is unit length
		//
		// The Quat representing the rotation is
		//   q = cos(A/2)+sin(A/2)*(x*i+y*j+z*k)

		r32 half_angle = 0.5f * angle;
		r32 s = sinf(half_angle);
		w = cosf(half_angle);
		x = s * axis.x;
		y = s * axis.y;
		z = s * axis.z;
	}

	void quat::to_angle_axis(r32* angle, vec3* axis) const {
		// The Quat representing the rotation is
		// q = cos(A / 2) + sin(A / 2) * (x*i + y*j + z*k)

		r32 length = sqrtf(x * x + y * y + z * z);
		if (length > 0.0f) {
			*angle = 2.0f * acosf(w);
			r32 length_inv = 1.0f / length;
			axis->x = x * length_inv;
			axis->y = y * length_inv;
			axis->z = z * length_inv;
		}
		else {
			// angle is 0 (mod 2*pi), so any axis will do
			*angle = 0.0f;
			axis->x = 1.0;
			axis->y = 0.0;
			axis->z = 0.0;
		}
	}

	void quat::from_axes(const vec3 akAxis[3]) {
		mat3 kRot;

		for (i32 iCol = 0; iCol < 3; iCol++)
		{
			kRot[0][iCol] = akAxis[iCol].x;
			kRot[1][iCol] = akAxis[iCol].y;
			kRot[2][iCol] = akAxis[iCol].z;
		}

		from_rotation_matrix(kRot);
	}

	void quat::from_axes(const vec3& xaxis, const vec3& yaxis, const vec3& zaxis) {
		mat3 kRot;

		kRot[0][0] = xaxis.x;
		kRot[1][0] = xaxis.y;
		kRot[2][0] = xaxis.z;

		kRot[0][1] = yaxis.x;
		kRot[1][1] = yaxis.y;
		kRot[2][1] = yaxis.z;

		kRot[0][2] = zaxis.x;
		kRot[1][2] = zaxis.y;
		kRot[2][2] = zaxis.z;

		from_rotation_matrix(kRot);
	}

	void quat::to_axes(vec3* axes) const
	{
		mat3 mat;

		to_rotation_matrix(&mat);

		for (i32 iCol = 0; iCol < 3; iCol++)
		{
			axes[iCol].x = mat[0][iCol];
			axes[iCol].y = mat[1][iCol];
			axes[iCol].z = mat[2][iCol];
		}
	}

	vec3 quat::x_axis() const
	{
		//r32 fTx  = 2.0*x;
		r32 fTy = 2.0f*y;
		r32 fTz = 2.0f*z;
		r32 fTwy = fTy*w;
		r32 fTwz = fTz*w;
		r32 fTxy = fTy*x;
		r32 fTxz = fTz*x;
		r32 fTyy = fTy*y;
		r32 fTzz = fTz*z;

		return vec3(1.0f - (fTyy + fTzz), fTxy + fTwz, fTxz - fTwy);
	}

	vec3 quat::y_axis() const {
		r32 fTx = 2.0f*x;
		r32 fTy = 2.0f*y;
		r32 fTz = 2.0f*z;
		r32 fTwx = fTx*w;
		r32 fTwz = fTz*w;
		r32 fTxx = fTx*x;
		r32 fTxy = fTy*x;
		r32 fTyz = fTz*y;
		r32 fTzz = fTz*z;

		return vec3(fTxy - fTwz, 1.0f - (fTxx + fTzz), fTyz + fTwx);
	}

	vec3 quat::z_axis() const {
		r32 fTx = 2.0f*x;
		r32 fTy = 2.0f*y;
		r32 fTz = 2.0f*z;
		r32 fTwx = fTx*w;
		r32 fTwy = fTy*w;
		r32 fTxx = fTx*x;
		r32 fTxz = fTz*x;
		r32 fTyy = fTy*y;
		r32 fTyz = fTz*y;

		return vec3(fTxz + fTwy, fTyz - fTwx, 1.0f - (fTxx + fTyy));
	}

	void quat::to_axes(vec3* xaxis, vec3* yaxis, vec3* zaxis) const {
		mat3 mat;

		to_rotation_matrix(&mat);

		xaxis->x = mat[0][0];
		xaxis->y = mat[1][0];
		xaxis->z = mat[2][0];

		yaxis->x = mat[0][1];
		yaxis->y = mat[1][1];
		yaxis->z = mat[2][1];

		zaxis->x = mat[0][2];
		zaxis->y = mat[1][2];
		zaxis->z = mat[2][2];
	}

	quat quat::operator+ (const quat& rkQ) const {
		return quat(w + rkQ.w, x + rkQ.x, y + rkQ.y, z + rkQ.z);
	}

	quat quat::operator- (const quat& rkQ) const {
		return quat(w - rkQ.w, x - rkQ.x, y - rkQ.y, z - rkQ.z);
	}

	quat quat::operator* (const quat& rkQ) const {
		// NOTE:  Multiplication is not generally commutative, so in most
		// cases p*q != q*p.

		return quat
		(
			w * rkQ.w - x * rkQ.x - y * rkQ.y - z * rkQ.z,
			w * rkQ.x + x * rkQ.w + y * rkQ.z - z * rkQ.y,
			w * rkQ.y + y * rkQ.w + z * rkQ.x - x * rkQ.z,
			w * rkQ.z + z * rkQ.w + x * rkQ.y - y * rkQ.x
		);
	}

	quat& quat::operator*= (const quat& rkQ) {
		r32 _x = x;
		r32 _y = y;
		r32 _z = z;
		r32 _w = w;

		w = _w * rkQ.w - _x * rkQ.x - _y * rkQ.y - _z * rkQ.z;
		x = _w * rkQ.x + _x * rkQ.w + _y * rkQ.z - _z * rkQ.y;
		y = _w * rkQ.y + _y * rkQ.w + _z * rkQ.x - _x * rkQ.z;
		z = _w * rkQ.z + _z * rkQ.w + _x * rkQ.y - _y * rkQ.x;

		return *this;
	}

	quat quat::operator* (r32 fScalar) const {
		return quat(fScalar*w, fScalar*x, fScalar*y, fScalar*z);
	}

	quat operator* (r32 fScalar, const quat& rkQ) {
		return quat(fScalar*rkQ.w, fScalar*rkQ.x, fScalar*rkQ.y, fScalar*rkQ.z);
	}

	quat quat::operator- () const {
		return quat(-w, -x, -y, -z);
	}

	r32 quat::dot(const quat& rkQ) const {
		return w*rkQ.w + x * rkQ.x + y * rkQ.y + z * rkQ.z;
	}

	r32 quat::length() const {
		return sqrtf(w * w + x * x + y * y + z * z);
	}

	quat quat::inverse() const {
		r32 fNorm = w*w + x*x + y*y + z*z;
		if (fNorm > 0.0) {
			r32 fInvNorm = 1.0f / fNorm;
			return quat(w*fInvNorm, -x*fInvNorm, -y*fInvNorm, -z*fInvNorm);
		}
		else {
			// return an invalid result to flag the error
			return zero;
		}
	}

	quat quat::unit_inverse() const {
		return quat(w, -x, -y, -z);
	}

	quat quat::exp() const {
		// If q = A*(x*i+y*j+z*k) where (x,y,z) is unit length, then
		// exp(q) = cos(A)+sin(A)*(x*i+y*j+z*k).  If sin(A) is near zero,
		// use exp(q) = cos(A)+A*(x*i+y*j+z*k) since A/sin(A) has limit 1.

		r32 fAngle = sqrtf(x*x + y*y + z*z);
		r32 fSin = sinf(fAngle);

		quat kResult;
		kResult.w = cosf(fAngle);

		if (fabsf(fSin) >= epsilon) {
			r32 fCoeff = fSin / (fAngle);
			kResult.x = fCoeff*x;
			kResult.y = fCoeff*y;
			kResult.z = fCoeff*z;
		}
		else {
			kResult.x = x;
			kResult.y = y;
			kResult.z = z;
		}

		return kResult;
	}

	quat quat::log() const {
		// If q = cos(A)+sin(A)*(x*i+y*j+z*k) where (x,y,z) is unit length, then
		// log(q) = A*(x*i+y*j+z*k).  If sin(A) is near zero, use log(q) =
		// sin(A)*(x*i+y*j+z*k) since sin(A)/A has limit 1.

		quat kResult;
		kResult.w = 0.0;

		if (fabsf(w) < 1.0) {
			r32 fAngle = acosf(w);
			r32 fSin = sinf(fAngle);
			if (fabsf(fSin) >= epsilon) {
				r32 fCoeff = fAngle / fSin;
				kResult.x = fCoeff*x;
				kResult.y = fCoeff*y;
				kResult.z = fCoeff*z;
				return kResult;
			}
		}

		kResult.x = x;
		kResult.y = y;
		kResult.z = z;

		return kResult;
	}

	vec3 quat::operator* (const vec3& v) const {
		// nVidia SDK implementation
		vec3 uv, uuv;
		vec3 qvec(x, y, z);
		uv = qvec.cross(v);
		uuv = qvec.cross(uv);
		uv *= (2.0f * w);
		uuv *= 2.0f;

		return v + uv + uuv;
	}

	quat quat::euler(r32 roll, r32 yaw, r32 pitch) {
		// assuming the angles are in radians.
		r32 cy = cosf(yaw * 0.5f);
		r32 sy = sinf(yaw * 0.5f);
		r32 cr = cosf(roll * 0.5f);
		r32 sr = sinf(roll * 0.5f);
		r32 cp = cosf(pitch * 0.5f);
		r32 sp = sinf(pitch * 0.5f);
		r32 cycr = cy*cr;
		r32 sysr = sy*sr;

		// yaw, roll, pitch
		/*
		return Quat
		(
		cp*cycr  - sp*sysr,
		cp*sysr  + sp*cycr,
		cp*sy*cr + sp*cy*sr,
		cp*cy*sr - sp*sy*cr
		);
		*/

		// yaw, pitch, roll
		return quat
		(
			cp*cycr + sp*sysr,
			sp*cycr + cp*sysr,
			cp*sy*cr - sp*cy*sr,
			cp*cy*sr - sp*sy*cr
		);
	}

	r32 quat::angle(const quat& a, const quat& b) {
		quat c = a.inverse() * b;
		c.normalize();
		r32 angle;
		vec3 axis;
		c.to_angle_axis(&angle, &axis);
		if (angle > PI)
			angle = fabsf((angle - (PI * 2.0f)));
		return angle;
	}

	quat quat::look(const vec3& dir) {
		if (fabsf(dir.y - 1.0f) < epsilon)
			return quat::euler(0, 0, PI * -0.5f);
		else if (fabsf(dir.y + 1.0f) < epsilon)
			return quat::euler(0, 0, PI * 0.5f);
		else
			return quat::euler(0, atan2f(dir.x, dir.z), -asinf(dir.y));
	}

	quat quat::slerp(r32 amount, const quat& quaternion1, const quat& quaternion2) {
		r32 num = quaternion1.x * quaternion2.x + quaternion1.y * quaternion2.y + quaternion1.z * quaternion2.z + quaternion1.w * quaternion2.w;
		b8 flag = false;
		if (num < 0.0f) {
			flag = true;
			num = -num;
		}
		r32 num2;
		r32 num3;
		if (num > 0.999999f) {
			num2 = 1.0f - amount;
			num3 = (flag ? (-amount) : amount);
		}
		else {
			r32 num4 = acosf(num);
			r32 num5 = 1.0f / sinf(num4);
			num2 = sinf((1.0f - amount) * num4) * num5;
			num3 = flag ? (-sinf(amount * num4) * num5) : (sinf(amount * num4) * num5);
		}
		quat result;
		result.x = num2 * quaternion1.x + num3 * quaternion2.x;
		result.y = num2 * quaternion1.y + num3 * quaternion2.y;
		result.z = num2 * quaternion1.z + num3 * quaternion2.z;
		result.w = num2 * quaternion1.w + num3 * quaternion2.w;
		return result;
	}

	quat quat::slerp_extra_spins(r32 fT, const quat& rkP, const quat& rkQ, i32 iExtraSpins) {
		r32 fCos = rkP.dot(rkQ);
		r32 fAngle = acosf(fCos);

		if (fabsf(fAngle) < epsilon)
			return rkP;

		r32 fSin = sinf(fAngle);
		r32 fPhase = PI*iExtraSpins*fT;
		r32 fInvSin = 1.0f / fSin;
		r32 fCoeff0 = sinf((1.0f - fT)*fAngle - fPhase)*fInvSin;
		r32 fCoeff1 = sinf(fT*fAngle + fPhase)*fInvSin;
		return fCoeff0*rkP + fCoeff1*rkQ;
	}

	void quat::intermediate(const quat& rkQ0, const quat& rkQ1, const quat& rkQ2, quat& rkA, quat& rkB) {
		// assert:  q0, q1, q2 are unit Quats

		quat kQ0inv = rkQ0.unit_inverse();
		quat kQ1inv = rkQ1.unit_inverse();
		quat rkP0 = kQ0inv*rkQ1;
		quat rkP1 = kQ1inv*rkQ2;
		quat kArg = 0.25*(rkP0.log() - rkP1.log());
		quat kMinusArg = -kArg;

		rkA = rkQ1*kArg.exp();
		rkB = rkQ1*kMinusArg.exp();
	}

	quat quat::squad(r32 fT, const quat& rkP, const quat& rkA, const quat& rkB, const quat& rkQ) {
		r32 fSlerpT = 2.0f*fT*(1.0f - fT);
		quat kSlerpP = slerp(fT, rkP, rkQ);
		quat kSlerpQ = slerp(fT, rkA, rkB);
		return slerp(fSlerpT, kSlerpP, kSlerpQ);
	}

	r32 quat::normalize(void) {
		r32 len = length();
		*this = *this * (1.0f / len);
		return len;
	}

	quat quat::normalize(const quat& q) {
		r32 factor = 1.0f / q.length();
		return q * factor;
	}

	quat quat::nlerp(r32 fT, const quat& rkP, const quat& rkQ, b8 shortestPath) {
		quat result;
		r32 fCos = rkP.dot(rkQ);
		if (fCos < 0.0f && shortestPath) {
			result = rkP + fT * ((-rkQ) - rkP);
		}
		else {
			result = rkP + fT * (rkQ - rkP);
		}
		result.normalize();
		return result;
	}

	inline static r32 MINOR(const mat4& m, const i32 r0, const i32 r1, const i32 r2, const i32 c0, const i32 c1, const i32 c2) {
		return m[r0][c0] * (m[r1][c1] * m[r2][c2] - m[r2][c1] * m[r1][c2]) -
			m[r0][c1] * (m[r1][c0] * m[r2][c2] - m[r2][c0] * m[r1][c2]) +
			m[r0][c2] * (m[r1][c0] * m[r2][c1] - m[r2][c0] * m[r1][c1]);
	}

	mat4 mat4::adjoint() const {
		return mat4(MINOR(*this, 1, 2, 3, 1, 2, 3),
			-MINOR(*this, 0, 2, 3, 1, 2, 3),
			MINOR(*this, 0, 1, 3, 1, 2, 3),
			-MINOR(*this, 0, 1, 2, 1, 2, 3),

			-MINOR(*this, 1, 2, 3, 0, 2, 3),
			MINOR(*this, 0, 2, 3, 0, 2, 3),
			-MINOR(*this, 0, 1, 3, 0, 2, 3),
			MINOR(*this, 0, 1, 2, 0, 2, 3),

			MINOR(*this, 1, 2, 3, 0, 1, 3),
			-MINOR(*this, 0, 2, 3, 0, 1, 3),
			MINOR(*this, 0, 1, 3, 0, 1, 3),
			-MINOR(*this, 0, 1, 2, 0, 1, 3),

			-MINOR(*this, 1, 2, 3, 0, 1, 2),
			MINOR(*this, 0, 2, 3, 0, 1, 2),
			-MINOR(*this, 0, 1, 3, 0, 1, 2),
			MINOR(*this, 0, 1, 2, 0, 1, 2));
	}

	r32 mat4::determinant() const {
		return m[0][0] * MINOR(*this, 1, 2, 3, 1, 2, 3) -
			m[0][0] * MINOR(*this, 1, 2, 3, 0, 2, 3) +
			m[0][0] * MINOR(*this, 1, 2, 3, 0, 1, 3) -
			m[0][0] * MINOR(*this, 1, 2, 3, 0, 1, 2);
	}

	mat4 mat4::inverse() const {
		r32 m00 = m[0][0], m01 = m[0][1], m02 = m[0][2], m03 = m[0][3];
		r32 m10 = m[1][0], m11 = m[1][1], m12 = m[1][2], m13 = m[1][3];
		r32 m20 = m[2][0], m21 = m[2][1], m22 = m[2][2], m23 = m[2][3];
		r32 m30 = m[3][0], m31 = m[3][1], m32 = m[3][2], m33 = m[3][3];

		r32 v0 = m20 * m31 - m21 * m30;
		r32 v1 = m20 * m32 - m22 * m30;
		r32 v2 = m20 * m33 - m23 * m30;
		r32 v3 = m21 * m32 - m22 * m31;
		r32 v4 = m21 * m33 - m23 * m31;
		r32 v5 = m22 * m33 - m23 * m32;

		r32 t00 = +(v5 * m11 - v4 * m12 + v3 * m13);
		r32 t10 = -(v5 * m10 - v2 * m12 + v1 * m13);
		r32 t20 = +(v4 * m10 - v2 * m11 + v0 * m13);
		r32 t30 = -(v3 * m10 - v1 * m11 + v0 * m12);

		r32 invDet = 1 / (t00 * m00 + t10 * m01 + t20 * m02 + t30 * m03);

		r32 d00 = t00 * invDet;
		r32 d10 = t10 * invDet;
		r32 d20 = t20 * invDet;
		r32 d30 = t30 * invDet;

		r32 d01 = -(v5 * m01 - v4 * m02 + v3 * m03) * invDet;
		r32 d11 = +(v5 * m00 - v2 * m02 + v1 * m03) * invDet;
		r32 d21 = -(v4 * m00 - v2 * m01 + v0 * m03) * invDet;
		r32 d31 = +(v3 * m00 - v1 * m01 + v0 * m02) * invDet;

		v0 = m10 * m31 - m11 * m30;
		v1 = m10 * m32 - m12 * m30;
		v2 = m10 * m33 - m13 * m30;
		v3 = m11 * m32 - m12 * m31;
		v4 = m11 * m33 - m13 * m31;
		v5 = m12 * m33 - m13 * m32;

		r32 d02 = +(v5 * m01 - v4 * m02 + v3 * m03) * invDet;
		r32 d12 = -(v5 * m00 - v2 * m02 + v1 * m03) * invDet;
		r32 d22 = +(v4 * m00 - v2 * m01 + v0 * m03) * invDet;
		r32 d32 = -(v3 * m00 - v1 * m01 + v0 * m02) * invDet;

		v0 = m21 * m10 - m20 * m11;
		v1 = m22 * m10 - m20 * m12;
		v2 = m23 * m10 - m20 * m13;
		v3 = m22 * m11 - m21 * m12;
		v4 = m23 * m11 - m21 * m13;
		v5 = m23 * m12 - m22 * m13;

		r32 d03 = -(v5 * m01 - v4 * m02 + v3 * m03) * invDet;
		r32 d13 = +(v5 * m00 - v2 * m02 + v1 * m03) * invDet;
		r32 d23 = -(v4 * m00 - v2 * m01 + v0 * m03) * invDet;
		r32 d33 = +(v3 * m00 - v1 * m01 + v0 * m02) * invDet;

		return mat4(
			d00, d01, d02, d03,
			d10, d11, d12, d13,
			d20, d21, d22, d23,
			d30, d31, d32, d33);
	}

	mat4 mat4::inverse_affine(void) const {
		r32 m10 = m[1][0], m11 = m[1][1], m12 = m[1][2];
		r32 m20 = m[2][0], m21 = m[2][1], m22 = m[2][2];

		r32 t00 = m22 * m11 - m21 * m12;
		r32 t10 = m20 * m12 - m22 * m10;
		r32 t20 = m21 * m10 - m20 * m11;

		r32 m00 = m[0][0], m01 = m[0][1], m02 = m[0][2];

		r32 invDet = 1 / (m00 * t00 + m01 * t10 + m02 * t20);

		t00 *= invDet; t10 *= invDet; t20 *= invDet;

		m00 *= invDet; m01 *= invDet; m02 *= invDet;

		r32 r00 = t00;
		r32 r01 = m02 * m21 - m01 * m22;
		r32 r02 = m01 * m12 - m02 * m11;

		r32 r10 = t10;
		r32 r11 = m00 * m22 - m02 * m20;
		r32 r12 = m02 * m10 - m00 * m12;

		r32 r20 = t20;
		r32 r21 = m01 * m20 - m00 * m21;
		r32 r22 = m00 * m11 - m01 * m10;

		r32 m03 = m[0][3], m13 = m[1][3], m23 = m[2][3];

		r32 r03 = -(r00 * m03 + r01 * m13 + r02 * m23);
		r32 r13 = -(r10 * m03 + r11 * m13 + r12 * m23);
		r32 r23 = -(r20 * m03 + r21 * m13 + r22 * m23);

		return mat4(
			r00, r01, r02, r03,
			r10, r11, r12, r13,
			r20, r21, r22, r23,
			0, 0, 0, 1);
	}

	void mat4::make_transform(const vec3& position, const vec3& scale, const quat& orientation) {
		// ordering: scale, rotate, translate

		mat3 rot3x3;
		orientation.to_rotation_matrix(&rot3x3);

		// set up final matrix with scale, rotation and translation
		m[0][0] = scale.x * rot3x3[0][0]; m[1][0] = scale.y * rot3x3[1][0]; m[2][0] = scale.z * rot3x3[2][0]; m[3][0] = position.x;
		m[0][1] = scale.x * rot3x3[0][1]; m[1][1] = scale.y * rot3x3[1][1]; m[2][1] = scale.z * rot3x3[2][1]; m[3][1] = position.y;
		m[0][2] = scale.x * rot3x3[0][2]; m[1][2] = scale.y * rot3x3[1][2]; m[2][2] = scale.z * rot3x3[2][2]; m[3][2] = position.z;

		// no projection term
		m[0][3] = 0; m[1][3] = 0; m[2][3] = 0; m[3][3] = 1;
	}

	void mat4::make_inverse_transform(const vec3& position, const vec3& scale, const quat& orientation) {
		// invert the parameters
		vec3 invTranslate = -position;
		vec3 invScale(1 / scale.x, 1 / scale.y, 1 / scale.z);
		quat invRot = orientation.inverse();

		// because we're inverting, order is translation, rotation, scale
		// so make translation relative to scale & rotation
		invTranslate = invRot * invTranslate; // rotate
		invTranslate *= invScale; // scale

								  // next, make a 3x3 rotation matrix
		mat3 rot3x3;
		invRot.to_rotation_matrix(&rot3x3);

		// set up final matrix with scale, rotation and translation
		m[0][0] = invScale.x * rot3x3[0][0]; m[1][0] = invScale.x * rot3x3[1][0]; m[2][0] = invScale.x * rot3x3[2][0]; m[3][0] = invTranslate.x;
		m[0][1] = invScale.y * rot3x3[0][1]; m[1][1] = invScale.y * rot3x3[1][1]; m[2][1] = invScale.y * rot3x3[2][1]; m[3][1] = invTranslate.y;
		m[0][2] = invScale.z * rot3x3[0][2]; m[1][2] = invScale.z * rot3x3[1][2]; m[2][2] = invScale.z * rot3x3[2][2]; m[3][2] = invTranslate.z;

		// no projection term
		m[0][3] = 0; m[1][3] = 0; m[2][3] = 0; m[3][3] = 1;
	}

	void mat4::decomposition(vec3* position, vec3* scale, quat* orientation) const {
		mat3 m3x3;
		extract_mat3(m3x3);

		mat3 matQ;
		vec3 vecU;
		m3x3.qdu_decomposition(&matQ, scale, &vecU);

		*orientation = quat(matQ);
		*position = vec3(m[3][0], m[3][1], m[3][2]);
	}

	mat4 mat4::perspective(const r32 fov, const r32 aspect, const r32 near, const r32 far) {
		mat4 result = mat4::zero;

		r32 depth = far - near;
		r32 inverse_depth = 1.0f / depth;

		result[1][1] = 1.0f / tanf(fov);

		result[0][0] = -1.0f * result[1][1] / aspect;
		result[2][2] = far * inverse_depth;
		result[3][2] = (-far * near) * inverse_depth;
		result[2][3] = 1;

		return result;
	}

	mat4 mat4::orthographic(const r32 width, const r32 height, const r32 near, const r32 far) {
		mat4 result = mat4::zero;

		result[0][0] = 2.0f / width;
		result[1][1] = 2.0f / height;
		r32 inverse_depth = 1.0f / (near - far);
		result[2][2] = inverse_depth;
		result[3][2] = near * inverse_depth;
		result[3][3] = 1;

		return result;
	}

	mat4 mat4::look(const vec3& eye, const vec3& forward, const vec3& up) {
#if 1
		vec3 const f(vec3::normalize(forward));
		vec3 const s(-vec3::normalize(f.cross(up)));
		//vec3 u(vec3::normalize(up));
		vec3 u = f.cross(s);

		mat4 result = mat4::identity;
		result[0][0] = s.x;
		result[1][0] = s.y;
		result[2][0] = s.z;
		result[0][1] = u.x;
		result[1][1] = u.y;
		result[2][1] = u.z;
		result[0][2] = -f.x;
		result[1][2] = -f.y;
		result[2][2] = -f.z;
		result[3][0] = -s.dot(eye);
		result[3][1] = -u.dot(eye);
		result[3][2] = f.dot(eye);
		result[3][3] = 1.0f;
		return result;
#endif
#if 0
		vec3 f = vec3::normalize(eye - forward);
		vec3 r = vec3::normalize(up).cross(f);
		vec3 u = f.cross(r);

		mat4 result = mat4::identity;

		result[0][0] = r.x;
		result[0][1] = r.y;
		result[0][2] = r.z;
		result[1][0] = u.x;
		result[1][1] = u.y;
		result[1][2] = u.z;
		result[2][0] = f.x;
		result[2][1] = f.y;
		result[2][2] = f.z;

		result[3][0] = eye.x;
		result[3][1] = eye.y;
		result[3][2] = eye.z;

		return result;
#endif
	}

}


