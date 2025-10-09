#pragma once
#include <cstdint>
#include <initializer_list>

class Vec2f;
class Vec3f;
class Vec4f;
class Mat4f;

Vec3f Vec3Cross(const Vec3f &lhs, const Vec3f &rhs);
float Vec2Cross(const Vec2f &lhs, const Vec2f &rhs);
float Vec3Dot(const Vec3f &lhs, const Vec3f &rhs);
Vec4f Vec4MultMat4 (const Vec4f &v, const Mat4f &m);

Mat4f GetScaleMat(float sx, float sy, float sz);
Mat4f GetTranslationMat(float tx, float ty, float tz);
Mat4f GetRotationMat(float ax, float ay, float az);
Mat4f GetPerspectiveMat(float verticalFov, int width, int height, float znear, float zfar);
Mat4f GetLookAtMat(Vec3f eye, Vec3f target, Vec3f up);
Mat4f GetLookTowardsMat(Vec3f eye, Vec3f direction, Vec3f up);

class Vec2f {
public:
	float x,y;

	Vec2f();
	Vec2f(float n);
	Vec2f(float x, float y);
	Vec2f(Vec4f v);

	//Vector operations
	Vec2f operator+ (const Vec2f &other) const;
	Vec2f operator- (const Vec2f &other) const;

	//Scalar operations
	Vec2f operator* (const float k) const;
	Vec2f operator/ (const float k ) const;
};

class Vec3f {
public:
	float x,y,z;

	Vec3f();
	Vec3f(float n);
	Vec3f(float x, float y, float z);
	Vec3f(Vec4f v);

	//Vector operations
	Vec3f operator+ (const Vec3f &other);
	Vec3f operator- (const Vec3f &other);

	//Scalar operations
	Vec3f operator* (const float k);
	Vec3f operator/ (const float k);

	//Component vise vector multiplication
	Vec3f operator* (const Vec3f &other);

	float Norm();
	float Length();

	Vec3f& Normalize();
	Vec3f Normalized();
};

class Vec4f {
public:
	float x,y,z,w;

	Vec4f();
	Vec4f(float n);
	Vec4f(float x, float y, float z, float w);
	Vec4f(Vec3f v);

	//Vector operations
	Vec4f operator+ (const Vec4f &o) const;
	Vec4f operator- (const Vec4f &o) const;

	//Scalar operations
	Vec4f operator* (const float k) const;
	Vec4f operator/ (const float k ) const;
};

class Mat4f {
public:
	//Default constructor gives an identity matrix
	//data[row][column]
	float data[4][4] = {{1,0,0,0},
					{0,1,0,0},
					{0,0,1,0},
					{0,0,0,1}
	};

	Mat4f();
	Mat4f(std::initializer_list<float> init);

	float* operator[] (uint8_t i);
	const float* operator[](uint8_t i) const;

	Mat4f operator* (const Mat4f& other);

	Mat4f Transposed();
	Mat4f& Transpose();

	Mat4f Inverse();
	Mat4f& Invert();
};
