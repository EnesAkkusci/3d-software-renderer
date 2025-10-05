#include "linear_algebra.h"
#include <cmath>

Vec3f Vec3Cross(const Vec3f &rhs, const Vec3f &lhs) {
	return Vec3f(
		rhs.y * lhs.z -rhs.z * lhs.y,
		rhs.z * lhs.x -rhs.x * lhs.z,
		rhs.x * lhs.y -rhs.y * lhs.x
	);
}

float Vec3Dot(const Vec3f &rhs, const Vec3f &lhs) {
	return rhs.x * lhs.x + rhs.y * lhs.y + rhs.z * lhs.z;
}

Vec4f Vec4MultMat4 (const Vec4f &v, const Mat4f &m) {
	float x,y,z,w;

	x = v.x * m[0][0] + v.y * m[1][0] + v.z * m[2][0] + v.w * m[3][0];
	y = v.x * m[0][1] + v.y * m[1][1] + v.z * m[2][1] + v.w * m[3][1];
	z = v.x * m[0][2] + v.y * m[1][2] + v.z * m[2][2] + v.w * m[3][2];
	w = v.x * m[0][3] + v.y * m[1][3] + v.z * m[2][3] + v.w * m[3][3];

	return Vec4f(x,y,z,w);
}

Mat4f GetScaleMat(float sx, float sy, float sz) {
	return {
		sx,  0,   0,   0,
		0,  sy,   0,   0,
		0,   0,  sz,   0,
		0,   0,   0,   1
	};
}

Mat4f GetTranslationMat(float tx, float ty, float tz) {
	return {
		1,   0,   0,   0,
		0,   1,   0,   0,
		0,   0,   1,   0,
		tx,  ty,  tz,  1
	};
}

Mat4f GetRotationMat(float ax, float ay, float az) {
	float cx = cos(ax); float sx = sin(ax);
	float cy = cos(ay); float sy = sin(ay);
	float cz = cos(az); float sz = sin(az);
	return {
		cy*cz,              cy*sz,             -sy,    0.0f,
		cz*sx*sy - cx*sz,   cx*cz + sx*sy*sz,  cy*sx,  0.0f,
		cx*cz*sy + sx*sz,   cx*sy*sz - cz*sx,  cx*cy,  0.0f,
		0.0f,               0.0f,              0.0f,   1.0f
	};
}

//aspectRation = h/w
Mat4f GetPerspectiveMat (float verticalFov, int width, int height, float znear, float zfar) {
	float a = (float)height/width;
	float f = 1 / tan(verticalFov/2);
	float lamb = zfar / (zfar - znear);
	return {
		a*f, 0, 0,           0,
		0  , f, 0,           0,
		0  , 0, lamb,        1,
		0  , 0, -lamb*znear, 0
	};
}

Mat4f GetLookAtMat(Vec3f eye, Vec3f target, Vec3f up) {
	Vec3f z = (target - eye).Normalized();
	Vec3f x = Vec3Cross(up,z).Normalized();
	Vec3f y = Vec3Cross(z, x);

	return {
		x.x            , y.x            , z.x            , 0,
		x.y            , y.y            , z.y            , 0,
		x.z            , y.z            , z.z            , 0,
		-Vec3Dot(x,eye), -Vec3Dot(y,eye), -Vec3Dot(z,eye), 1
	};
}

Mat4f GetLookTowardsMat(Vec3f eye, Vec3f direction, Vec3f up) {
	Vec3f z = direction.Normalized();
	Vec3f x = Vec3Cross(up,z).Normalized();
	Vec3f y = Vec3Cross(z, x);

	return {
		x.x            , y.x            , z.x            , 0,
		x.y            , y.y            , z.y            , 0,
		x.z            , y.z            , z.z            , 0,
		-Vec3Dot(x,eye), -Vec3Dot(y,eye), -Vec3Dot(z,eye), 1
	};
}

//////////////////////////////////////////////////
/// Vec2f
//////////////////////////////////////////////////
Vec2f::Vec2f() : x(0), y(0) {}
Vec2f::Vec2f(float n) : x(n), y(n) {}
Vec2f::Vec2f(float x,float y) : x(x), y(y) {}
Vec2f::Vec2f(Vec4f v) : x(v.x), y(v.y) {}

Vec2f Vec2f::operator+ (const Vec2f &other) const { return Vec2f(x + other.x, y + other.y); }
Vec2f Vec2f::operator- (const Vec2f &other) const { return Vec2f(x - other.x, y - other.y); }
Vec2f Vec2f::operator* (const float k) const { return Vec2f(x*k, y*k); }
Vec2f Vec2f::operator/ (const float k ) const { return Vec2f(x/k, y/k); }

//////////////////////////////////////////////////
/// Vec3f
//////////////////////////////////////////////////
Vec3f::Vec3f() : x(float(0)), y(float(0)), z(float(0)) {}
Vec3f::Vec3f(float n) : x(n), y(n), z(n) {}
Vec3f::Vec3f(float x, float y, float z) : x(x), y(y), z(z) {}
Vec3f::Vec3f(Vec4f v) : x(v.x), y(v.y), z(v.z) {}

Vec3f Vec3f::operator+ (const Vec3f &other) { return Vec3f(x + other.x, y + other.y, z + other.z); }
Vec3f Vec3f::operator- (const Vec3f &other) { return Vec3f(x - other.x, y - other.y, z - other.z); }
Vec3f Vec3f::operator* (const float k) { return Vec3f (x*k, y*k, z*k); }
Vec3f Vec3f::operator/ (const float k) { return Vec3f (x/k, y/k, z/k); }
Vec3f Vec3f::operator* (const Vec3f &other) {return Vec3f(x * other.x, y * other.y, z * other.z); }

float Vec3f::Norm() { return x*x + y*y + z*z; }
float Vec3f::Length() { return sqrt(Norm()); }

Vec3f& Vec3f::Normalize() {
	float len = Length();
	if(len > 0) {
		float invLen = 1 / len;
		x *= invLen; y *= invLen; z *= invLen;
	}
	return *this;
}

Vec3f Vec3f::Normalized() {
	float len = Length();
	if(len > 0) {
		float invLen = 1 / len;
		float new_x = x * invLen; float new_y = y * invLen; float new_z = z * invLen;
		return Vec3f(new_x, new_y, new_z);
	}
	return Vec3f(0);
}

//////////////////////////////////////////////////
/// Vec4f
//////////////////////////////////////////////////
Vec4f::Vec4f() : x(float(0)), y(float(0)), z(float(0)), w(float(0)) {}
Vec4f::Vec4f(float n) : x(n), y(n), z(n), w(n){}
Vec4f::Vec4f(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
Vec4f::Vec4f(Vec3f v) : x(v.x), y(v.y), z(v.z), w(1) {}

Vec4f Vec4f::operator+ (const Vec4f &o) const { return Vec4f(x + o.x, y + o.y, z + o.z, w + o.w); }
Vec4f Vec4f::operator- (const Vec4f &o) const { return Vec4f(x - o.x, y - o.y, z - o.z, w - o.w); }
Vec4f Vec4f::operator* (const float k) const { return Vec4f(x*k, y*k, z*k, w*k); }
Vec4f Vec4f::operator/ (const float k ) const { return Vec4f(x/k, y/k, z/k, w/k); }

//////////////////////////////////////////////////
/// Mat4f
//////////////////////////////////////////////////
Mat4f::Mat4f() {};
Mat4f::Mat4f(std::initializer_list<float> init) {
	auto it = init.begin();
	for(int i = 0; i < 4; i++) {
		for(int j = 0; j < 4; j++) {
			data[i][j] = *it++;
		}
	}
}

float* Mat4f::operator[] (uint8_t i) { return data[i]; }
const float* Mat4f::operator[](uint8_t i) const { return data[i]; }

//Matrix multiplication
Mat4f Mat4f::operator* (const Mat4f& other) {
	Mat4f temp;
	for (uint8_t i = 0; i < 4; ++i) {
		for (uint8_t j = 0; j < 4; ++j) {
			temp[i][j] = data[i][0] * other[0][j] + data[i][1] * other[1][j] +
				data[i][2] * other[2][j] + data[i][3] * other[3][j];
		}
	}
	return temp;
}

Mat4f Mat4f::Transposed() {
	Mat4f temp;
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			temp[i][j] = data[j][i];

	return temp;
}

Mat4f& Mat4f::Transpose() {
	*this = Transposed();
	return *this;
}

//Taking the inverse of the matrix via the Gause-Jordan method.
Mat4f Mat4f::Inverse() {
	int i,j,k;
	Mat4f s; //The default constructor for matrix makes this I.
	Mat4f t (*this);

	//Forward elimination
	for (i = 0; i < 3 ; i++) {
		int pivot = i; float pivotsize = t[i][i];

		if (pivotsize < 0) pivotsize = -pivotsize;
		for (j = i + 1; j < 4; j++) {
			float tmp = t[j][i];
			if (tmp < 0) tmp = -tmp;
			if (tmp > pivotsize) {
				pivot = j;
				pivotsize = tmp;
			}
		}
		if (pivotsize == 0) { return Mat4f(); } // Cannot invert singular matrix
		if (pivot != i) {
			for (j = 0; j < 4; j++) {
				float tmp;
				tmp = t[i][j];
				t[i][j] = t[pivot][j];
				t[pivot][j] = tmp;
				tmp = s[i][j];
				s[i][j] = s[pivot][j];
				s[pivot][j] = tmp;
			}
		}
		for (j = i + 1; j < 4; j++) {
			float f = t[j][i] / t[i][i];
			for (k = 0; k < 4; k++) {
				t[j][k] -= f * t[i][k];
				s[j][k] -= f * s[i][k];
			}
		}
	}

	// Backward substitution
	for (i = 3; i >= 0; --i) {
		float f;
		if ((f = t[i][i]) == 0) { return Mat4f(); } // Cannot invert singular matrix
		for (j = 0; j < 4; j++) {
			t[i][j] /= f;
			s[i][j] /= f;
		}
		for (j = 0; j < i; j++) {
			for (k = 0; k < 4; k++) {
				t[j][k] -= f * t[i][k];
				s[j][k] -= f * s[i][k];
			}
		}
	}
	return s;
}

Mat4f& Mat4f::Invert() {
	*this = Inverse();
	return *this;
}
