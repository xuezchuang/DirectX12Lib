﻿#include "MathLib.h"

FMatrix::FMatrix()
	: r0(1.f, 0.f, 0.f, 0.f)
	, r1(0.f, 1.f, 0.f, 0.f)
	, r2(0.f, 0.f, 1.f, 0.f)
	, r3(0.f, 0.f, 0.f, 1.f)
{

}

FMatrix::FMatrix(const Vector3f& r0, const Vector3f& r1, const Vector3f& r2, const Vector3f& r3)
	: r0(Vector4f(r0, 0.f))
	, r1(Vector4f(r1, 0.f))
	, r2(Vector4f(r2, 0.f))
	, r3(Vector4f(r3, 1.f))
{

}

FMatrix::FMatrix(const Vector4f& r0, const Vector4f& r1, const Vector4f& r2, const Vector4f& r3)
	: r0(r0), r1(r1), r2(r2), r3(r3)
{

}

FMatrix::FMatrix(float m00, float m01, float m02, float m03, 
	float m10, float m11, float m12, float m13, 
	float m20, float m21, float m22, float m23, 
	float m30, float m31, float m32, float m33)
	: r0(Vector4f(m00, m01, m02, m03))
	, r1(Vector4f(m10, m11, m12, m13))
	, r2(Vector4f(m20, m21, m22, m23))
	, r3(Vector4f(m30, m31, m32, m33))
{
}

Vector4f FMatrix::Column(int i) const
{
	return Vector4f(r0[i], r1[i], r2[i], r3[i]);
}

FMatrix FMatrix::operator*(const FMatrix& rhs) const
{
	Vector4f c0 = rhs.Column(0);
	Vector4f c1 = rhs.Column(1);
	Vector4f c2 = rhs.Column(2);
	Vector4f c3 = rhs.Column(3);

	return FMatrix(
		r0.Dot(c0), r0.Dot(c1), r0.Dot(c2), r0.Dot(c3),
		r1.Dot(c0), r1.Dot(c1), r1.Dot(c2), r1.Dot(c3),
		r2.Dot(c0), r2.Dot(c1), r2.Dot(c2), r2.Dot(c3),
		r3.Dot(c0), r3.Dot(c1), r3.Dot(c2), r3.Dot(c3)
	);
}

FMatrix& FMatrix::operator*=(const FMatrix& rhs)
{
	*this = (*this) * rhs;
	return *this;
}

FMatrix FMatrix::operator * (float rhs) const
{
	return FMatrix(r0 * rhs, r1 * rhs, r2 * rhs, r3 * rhs);
}

FMatrix& FMatrix::operator*=(float rhs)
{
	*this = (*this) * rhs;
	return *this;
}

Vector3f FMatrix::TranslateVector(const Vector3f& vector) const
{
	Vector4f Res = Vector4f(vector, 0.f) * (*this);
	return Vector3f(Res.x, Res.y, Res.z);
}

Vector3f FMatrix::TransformPosition(const Vector3f& position) const
{
	Vector4f Res = Vector4f(position, 1.f) * (*this);
	return Vector3f(Res.x, Res.y, Res.z);
}

FBoundingBox FMatrix::TransformBoundingBox(const FBoundingBox& BoundBox) const
{
	Vector3f BoundMin = TransformPosition(BoundBox.BoundMin);
	Vector3f BoundMax = TransformPosition(BoundBox.BoundMax);

	FBoundingBox Result;
	Result.BoundMin = Min(BoundMin, BoundMax);
	Result.BoundMax = Max(BoundMin, BoundMax);
	return Result;
}

FBoundingBox FMatrix::TransformBoundingBox(const Vector3f& BoundMin, const Vector3f& BoundMax) const
{
	Vector3f BMin = TransformPosition(BoundMin);
	Vector3f BMax = TransformPosition(BoundMax);

	FBoundingBox Result;
	Result.BoundMin = Min(BMin, BMax);
	Result.BoundMax = Max(BMin, BMax);
	return Result;
}

FMatrix FMatrix::Transpose() const
{
	return FMatrix(
		r0.x, r1.x, r2.x, r3.x,
		r0.y, r1.y, r2.y, r3.y,
		r0.z, r1.z, r2.z, r3.z,
		r0.w, r1.w, r2.w, r3.w
	);
}

FMatrix FMatrix::Inverse() const
{
	// https://semath.info/src/inverse-cofactor-ex4.html
	float det =  r0.x*r1.y*r2.z*r3.w + r0.x*r1.z*r2.w*r3.y + r0.x*r1.w*r2.y*r3.z
				-r0.x*r1.w*r2.z*r3.y - r0.x*r1.z*r2.y*r3.w - r0.x*r1.y*r2.w*r3.z
				-r0.y*r1.x*r2.z*r3.w - r0.z*r1.x*r2.w*r3.y - r0.w*r1.x*r2.y*r3.z
				+r0.w*r1.x*r2.z*r3.y + r0.z*r1.x*r2.y*r3.w + r0.y*r1.x*r2.w*r3.z
				+r0.y*r1.z*r2.x*r3.w + r0.z*r1.w*r2.x*r3.y + r0.w*r1.y*r2.x*r3.z
				-r0.w*r1.z*r2.x*r3.y - r0.z*r1.y*r2.x*r3.w - r0.y*r1.w*r2.x*r3.z
				-r0.y*r1.z*r2.w*r3.x - r0.z*r1.w*r2.y*r3.x - r0.w*r1.y*r2.z*r3.x
				+r0.w*r1.z*r2.y*r3.x + r0.z*r1.y*r2.w*r3.x + r0.y*r1.w*r2.z*r3.x;

	float A11 =  r1.y*r2.z*r3.w + r1.z*r2.w*r3.y + r1.w*r2.y*r3.z - r1.w*r2.z*r3.y - r1.z*r2.y*r3.w - r1.y*r2.w*r3.z;
	float A12 = -r0.y*r2.z*r3.w - r0.z*r2.w*r3.y - r0.w*r2.y*r3.z + r0.w*r2.z*r3.y + r0.z*r2.y*r3.w + r0.y*r2.w*r3.z;
	float A13 =  r0.y*r1.z*r3.w + r0.z*r1.w*r3.y + r0.w*r1.y*r3.z - r0.w*r1.z*r3.y - r0.z*r1.y*r3.w - r0.y*r1.w*r3.z;
	float A14 = -r0.y*r1.z*r2.w - r0.z*r1.w*r2.y - r0.w*r1.y*r2.z + r0.w*r1.z*r2.y + r0.z*r1.y*r2.w + r0.y*r1.w*r2.z;

	float A21 = -r1.x*r2.z*r3.w - r1.z*r2.w*r3.x - r1.w*r2.x*r3.z + r1.w*r2.z*r3.x + r1.z*r2.x*r3.w + r1.x*r2.w*r3.z;
	float A22 =  r0.x*r2.z*r3.w + r0.z*r2.w*r3.x + r0.w*r2.x*r3.z - r0.w*r2.z*r3.x - r0.z*r2.x*r3.w - r0.x*r2.w*r3.z;
	float A23 = -r0.x*r1.z*r3.w - r0.z*r1.w*r3.x - r0.w*r1.x*r3.z + r0.w*r1.z*r3.x + r0.z*r1.x*r3.w + r0.x*r1.w*r3.z;
	float A24 =  r0.x*r1.z*r2.w + r0.z*r1.w*r2.x + r0.w*r1.x*r2.z - r0.w*r1.z*r2.x - r0.z*r1.x*r2.w - r0.x*r1.w*r2.z;

	float A31 =  r1.x*r2.y*r3.w + r1.y*r2.w*r3.x + r1.w*r2.x*r3.y - r1.w*r2.y*r3.x - r1.y*r2.x*r3.w - r1.x*r2.w*r3.y;
	float A32 = -r0.x*r2.y*r3.w - r0.y*r2.w*r3.x - r0.w*r2.x*r3.y + r0.w*r2.y*r3.x + r0.y*r2.x*r3.w + r0.x*r2.w*r3.y;
	float A33 =  r0.x*r1.y*r3.w + r0.y*r1.w*r3.x + r0.w*r1.x*r3.y - r0.w*r1.y*r3.x - r0.y*r1.x*r3.w - r0.x*r1.w*r3.y;
	float A34 = -r0.x*r1.y*r2.w - r0.y*r1.w*r2.x - r0.w*r1.x*r2.y + r0.w*r1.y*r2.x + r0.y*r1.x*r2.w + r0.x*r1.w*r2.y;

	float A41 = -r1.x*r2.y*r3.z - r1.y*r2.z*r3.x - r1.z*r2.x*r3.y + r1.z*r2.y*r3.x + r1.y*r2.x*r3.z + r1.x*r2.z*r3.y;
	float A42 =  r0.x*r2.y*r3.z + r0.y*r2.z*r3.x + r0.z*r2.x*r3.y - r0.z*r2.y*r3.x - r0.y*r2.x*r3.z - r0.x*r2.z*r3.y;
	float A43 = -r0.x*r1.y*r3.z - r0.y*r1.z*r3.x - r0.z*r1.x*r3.y + r0.z*r1.y*r3.x + r0.y*r1.x*r3.z + r0.x*r1.z*r3.y;
	float A44 =  r0.x*r1.y*r2.z + r0.y*r1.z*r2.x + r0.z*r1.x*r2.y - r0.z*r1.y*r2.x - r0.y*r1.x*r2.z - r0.x*r1.z*r2.y;

	return FMatrix(A11, A12, A13, A14, A21, A22, A23, A24, A31, A32, A33, A34, A41, A42, A43, A44) * (1.f / det);
}

FMatrix FMatrix::TranslateMatrix(const Vector3f& T)
{
	return FMatrix(
		1.f, 0.f, 0.f, 0.f,
		0.f, 1.f, 0.f, 0.f,
		0.f, 0.f, 1.f, 0.f,
		T.x, T.y, T.z, 1.f);
}

FMatrix FMatrix::ScaleMatrix(float s)
{
	return FMatrix(
		s, 0.f, 0.f, 0.f,
		0.f, s, 0.f, 0.f,
		0.f, 0.f, s, 0.f,
		0.f, 0.f, 0.f, 1.f);
}

FMatrix FMatrix::ScaleMatrix(const Vector3f& s)
{
	return FMatrix(
		s.x, 0.f, 0.f, 0.f,
		0.f, s.y, 0.f, 0.f,
		0.f, 0.f, s.z, 0.f,
		0.f, 0.f, 0.f, 1.f);
}

FMatrix FMatrix::RotateX(float v)
{
	float c = (float)cos(v);
	float s = (float)sin(v);
	return FMatrix(
		1.f, 0.f, 0.f, 0.f,
		0.f, c,   s,   0.f,
		0.f, -s,  c,   0.f,
		0.f, 0.f, 0.f, 1.f);
}

FMatrix FMatrix::RotateY(float v)
{
	float c = (float)cos(v);
	float s = (float)sin(v);
	return FMatrix(
		c,   0.f, -s, 0.f,
		0.f, 1.f, 0.f, 0.f,
		s,   0.f, c,   0.f,
		0.f, 0.f, 0.f, 1.f);
}

FMatrix FMatrix::RotateZ(float v)
{
	float c = (float)cos(v);
	float s = (float)sin(v);
	return FMatrix(
		c,   s,   0.f, 0.f,
		-s,  c,   0.f, 0.f,
		0.f, 0.f, 1.f, 0.f,
		0.f, 0.f, 0.f, 1.f);
}

FMatrix FMatrix::MatrixRotationRollPitchYaw(float Roll, float Pitch, float Yaw)
{
	// roll(z), pitch(x), yaw(y)
	return FMatrix::RotateZ(Roll) * FMatrix::RotateX(Pitch) * FMatrix::RotateY(Yaw);
}

FMatrix FMatrix::MatrixLookAtLH(const Vector3f& EyePosition, const Vector3f& FocusPosition, const Vector3f& UpDirection)
{
	Vector3f Forward = FocusPosition - EyePosition;
	Forward = Forward.Normalize();
	Vector3f Up = UpDirection.Normalize();

	Vector3f Right = Cross(UpDirection, Forward);
	Up = Cross(Forward, Right);

	float D0 = -EyePosition.Dot(Right);
	float D1 = -EyePosition.Dot(Up);
	float D2 = -EyePosition.Dot(Forward);

	FMatrix Result(Vector4f(Right, D0), Vector4f(Up, D1), Vector4f(Forward, D2), Vector4f(0.f, 0.f, 0.f, 1.f));
	return Result.Transpose();
}

FMatrix FMatrix::MatrixPerspectiveFovLH(float FovAngleY, float AspectHByW, float NearZ, float FarZ)
{
	float h = 1.f / (float)tan(FovAngleY/2);
	float w = h / AspectHByW;
	return FMatrix(
		w, 0.f, 0.f, 0.f,
		0.f, h, 0.f, 0.f,
		0.f, 0.f, FarZ/(FarZ-NearZ), 1,
		0.f, 0.f, -NearZ*FarZ/(FarZ-NearZ), 0.f
	);
}

FMatrix FMatrix::MatrixOrthoLH(float Width, float Height, float NearZ, float FarZ)
{
	float Dist = FarZ - NearZ;
	return FMatrix(
		2/Width, 0, 0, 0,
		0, 2/Height, 0, 0,
		0, 0, 1/Dist, 0,
		0, 0, -NearZ/Dist, 1
);
}

//Vector4f operator*(const FMatrix& mat, const Vector4f& vec)
//{
//	return Vector4f();
//}

Vector4f operator*(const Vector4f& vec, const FMatrix& mat)
{
	float d0 = vec.Dot(mat.Column(0));
	float d1 = vec.Dot(mat.Column(1));
	float d2 = vec.Dot(mat.Column(2));
	float d3 = vec.Dot(mat.Column(3));
	return Vector4f(d0, d1, d2, d3);
}

void FBoundingBox::Include(const FBoundingBox& Other)
{
	BoundMin = Min(BoundMin, Other.BoundMin);
	BoundMax = Max(BoundMax, Other.BoundMax);
}
