#include <Novice.h>
#include <cmath>
#include <math.h>
#define _USE_MATH_DEFINES
#include <cassert>
#include <Vector2.h>
#include <stdlib.h>
#include <Vector3.h>
#include<imGui.h>

#include<algorithm>
#include <iostream>

const char kWindowTitle[] = "LE2B_05_オイカワユウマ";


struct Matrix4x4 {
	float m[4][4];
};

struct Sphere {
	Vector3 center; //!< 中心点
	float radius; //!< 半径
	int color;
	Vector3 pos;
	float mass;
	Vector3 velocity;
	Vector3 acceleration;
	Matrix4x4 world;
	Matrix4x4 worldView;
};

struct AABB {
	Vector3 min;
	Vector3 max;
	Vector3 pos;
	float width;
	float height;
	float length;
	int color;
};

struct Line {
	Vector3 origin;
	Vector3 diff;
};

struct Ray {
	Vector3 origin;
	Vector3 diff;
};

struct Segment {
	Vector3 origin;
	Vector3 diff;
};
struct Plane {
	Vector3 normal; // !< 法線
	float distance; //!< 距離
	int color;

};

struct Triangle {
	Vector3 vertices[3];//!< 頂点
};


/*------------------------------------------------------------------
						   MT3_01_00使用
------------------------------------------------------------------*/
Vector3 Multiply(float scalar, const Vector3& v) {
	Vector3 m3;
	m3.x = v.x * scalar;
	m3.y = v.y * scalar;
	m3.z = v.z * scalar;

	return m3;
};

// 1. 透視投影行列
Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip) {
	Matrix4x4 m4;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			m4.m[i][j] = 0;
		}
	}
	m4.m[0][0] = 1 / aspectRatio * 1 / std::tan(fovY / 2);
	m4.m[1][1] = 1 / std::tan(fovY / 2);
	m4.m[2][2] = farClip / (farClip - nearClip);
	m4.m[2][3] = 1;
	m4.m[3][2] = (-nearClip * farClip) / (farClip - nearClip);


	return m4;
};

// 2. 正射影行列
Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip) {
	Matrix4x4 m4;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			m4.m[i][j] = 0;
		}
	}
	m4.m[0][0] = 2 / (top - left);
	m4.m[1][1] = 2 / (top - bottom);
	m4.m[2][2] = 1 / (farClip - nearClip);
	m4.m[3][0] = (left + right) / (left - right);
	m4.m[3][1] = (top + bottom) / (bottom - top);
	m4.m[3][2] = nearClip / (nearClip - farClip);
	m4.m[3][3] = 1;


	return m4;
};

// 3. ビューポート変換行列
Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth) {
	Matrix4x4 m4;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			m4.m[i][j] = 0;
		}
	}
	m4.m[0][0] = width / 2;
	m4.m[1][1] = -(height / 2);
	m4.m[2][2] = maxDepth - minDepth;
	m4.m[3][0] = left + (width / 2);
	m4.m[3][1] = top + height / 2;
	m4.m[3][2] = minDepth;
	m4.m[3][3] = 1;



	return m4;
};
/*/////////////////////////////////////////////////////////////////////*/

Matrix4x4 Multiply(const Matrix4x4 m1, const Matrix4x4 m2) {
	Matrix4x4 m4;
	m4.m[0][0] = m1.m[0][0] * m2.m[0][0] + m1.m[0][1] * m2.m[1][0] + m1.m[0][2] * m2.m[2][0] + m1.m[0][3] * m2.m[3][0];
	m4.m[0][1] = m1.m[0][0] * m2.m[0][1] + m1.m[0][1] * m2.m[1][1] + m1.m[0][2] * m2.m[2][1] + m1.m[0][3] * m2.m[3][1];
	m4.m[0][2] = m1.m[0][0] * m2.m[0][2] + m1.m[0][1] * m2.m[1][2] + m1.m[0][2] * m2.m[2][2] + m1.m[0][3] * m2.m[3][2];
	m4.m[0][3] = m1.m[0][0] * m2.m[0][3] + m1.m[0][1] * m2.m[1][3] + m1.m[0][2] * m2.m[2][3] + m1.m[0][3] * m2.m[3][3];

	m4.m[1][0] = m1.m[1][0] * m2.m[0][0] + m1.m[1][1] * m2.m[1][0] + m1.m[1][2] * m2.m[2][0] + m1.m[1][3] * m2.m[3][0];
	m4.m[1][1] = m1.m[1][0] * m2.m[0][1] + m1.m[1][1] * m2.m[1][1] + m1.m[1][2] * m2.m[2][1] + m1.m[1][3] * m2.m[3][1];
	m4.m[1][2] = m1.m[1][0] * m2.m[0][2] + m1.m[1][1] * m2.m[1][2] + m1.m[1][2] * m2.m[2][2] + m1.m[1][3] * m2.m[3][2];
	m4.m[1][3] = m1.m[1][0] * m2.m[0][3] + m1.m[1][1] * m2.m[1][3] + m1.m[1][2] * m2.m[2][3] + m1.m[1][3] * m2.m[3][3];

	m4.m[2][0] = m1.m[2][0] * m2.m[0][0] + m1.m[2][1] * m2.m[1][0] + m1.m[2][2] * m2.m[2][0] + m1.m[2][3] * m2.m[3][0];
	m4.m[2][1] = m1.m[2][0] * m2.m[0][1] + m1.m[2][1] * m2.m[1][1] + m1.m[2][2] * m2.m[2][1] + m1.m[2][3] * m2.m[3][1];
	m4.m[2][2] = m1.m[2][0] * m2.m[0][2] + m1.m[2][1] * m2.m[1][2] + m1.m[2][2] * m2.m[2][2] + m1.m[2][3] * m2.m[3][2];
	m4.m[2][3] = m1.m[2][0] * m2.m[0][3] + m1.m[2][1] * m2.m[1][3] + m1.m[2][2] * m2.m[2][3] + m1.m[2][3] * m2.m[3][3];

	m4.m[3][0] = m1.m[3][0] * m2.m[0][0] + m1.m[3][1] * m2.m[1][0] + m1.m[3][2] * m2.m[2][0] + m1.m[3][3] * m2.m[3][0];
	m4.m[3][1] = m1.m[3][0] * m2.m[0][1] + m1.m[3][1] * m2.m[1][1] + m1.m[3][2] * m2.m[2][1] + m1.m[3][3] * m2.m[3][1];
	m4.m[3][2] = m1.m[3][0] * m2.m[0][2] + m1.m[3][1] * m2.m[1][2] + m1.m[3][2] * m2.m[2][2] + m1.m[3][3] * m2.m[3][2];
	m4.m[3][3] = m1.m[3][0] * m2.m[0][3] + m1.m[3][1] * m2.m[1][3] + m1.m[3][2] * m2.m[2][3] + m1.m[3][3] * m2.m[3][3];
	return m4;
};

// 3次元アフィン変換行列
Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate) {
	Matrix4x4 m4;
	Matrix4x4 m1;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			m1.m[i][j] = 0;
		}
	}
	m1.m[0][0] = 1;
	m1.m[1][1] = std::cos(rotate.x);
	m1.m[1][2] = std::sin(rotate.x);
	m1.m[2][1] = -std::sin(rotate.x);
	m1.m[2][2] = std::cos(rotate.x);
	m1.m[3][3] = 1;

	Matrix4x4 m2;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			m2.m[i][j] = 0;
		}
	}
	m2.m[0][0] = std::cos(rotate.y);
	m2.m[1][1] = 1;
	m2.m[2][2] = std::cos(rotate.y);
	m2.m[0][2] = -std::sin(rotate.y);
	m2.m[2][0] = std::sin(rotate.y);
	m2.m[3][3] = 1;

	Matrix4x4 m3;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			m3.m[i][j] = 0;
		}
	}
	m3.m[0][0] = std::cos(rotate.z);
	m3.m[2][2] = 1;
	m3.m[1][1] = std::cos(rotate.z);
	m3.m[1][0] = -std::sin(rotate.z);
	m3.m[0][1] = std::sin(rotate.z);
	m3.m[3][3] = 1;

	Matrix4x4 xyz = Multiply(m1, Multiply(m2, m3));



	m4.m[0][0] = xyz.m[0][0] * scale.x;
	m4.m[0][1] = xyz.m[0][1] * scale.x;
	m4.m[0][2] = xyz.m[0][2] * scale.x;

	m4.m[1][0] = xyz.m[1][0] * scale.y;
	m4.m[1][1] = xyz.m[1][1] * scale.y;
	m4.m[1][2] = xyz.m[1][2] * scale.y;

	m4.m[2][0] = xyz.m[2][0] * scale.z;
	m4.m[2][1] = xyz.m[2][1] * scale.z;
	m4.m[2][2] = xyz.m[2][2] * scale.z;


	m4.m[3][0] = translate.x;
	m4.m[3][1] = translate.y;
	m4.m[3][2] = translate.z;
	m4.m[0][3] = 0;
	m4.m[1][3] = 0;
	m4.m[2][3] = 0;
	m4.m[3][3] = 1.0f;

	return m4;
};
Vector3 Add(const Vector3& v1, const Vector3& v2) {
	Vector3 m3;
	m3.x = v1.x + v2.x;
	m3.y = v1.y + v2.y;
	m3.z = v1.z + v2.z;

	return m3;

};
//減算
Vector3 Subtract(const Vector3& v1, const Vector3& v2) {
	Vector3 m3;
	m3.x = v1.x - v2.x;
	m3.y = v1.y - v2.y;
	m3.z = v1.z - v2.z;
	return m3;
};

//内績
float Dot(const Vector3& v1, const Vector3& v2) {
	float m3;
	m3 = v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;


	return m3;
};

// 1. X軸回転行列
Matrix4x4 MakeRotateXMatrix(float radian) {
	Matrix4x4 m4;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			m4.m[i][j] = 0;
		}
	}
	m4.m[0][0] = 1;
	m4.m[1][1] = std::cos(radian);
	m4.m[1][2] = std::sin(radian);
	m4.m[2][1] = -std::sin(radian);
	m4.m[2][2] = std::cos(radian);
	m4.m[3][3] = 1;
	return m4;
};

// 2. Y軸回転行列
Matrix4x4 MakeRotateYMatrix(float radian) {
	Matrix4x4 m4;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			m4.m[i][j] = 0;
		}
	}
	m4.m[0][0] = std::cos(radian);
	m4.m[1][1] = 1;
	m4.m[2][2] = std::cos(radian);
	m4.m[0][2] = -std::sin(radian);
	m4.m[2][0] = std::sin(radian);
	m4.m[3][3] = 1;
	return m4;
};

// 3. Z軸回転行列
Matrix4x4 MakeRotateZMatrix(float radian) {
	Matrix4x4 m4;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			m4.m[i][j] = 0;
		}
	}
	m4.m[0][0] = std::cos(radian);
	m4.m[2][2] = 1;
	m4.m[1][1] = std::cos(radian);
	m4.m[1][0] = -std::sin(radian);
	m4.m[0][1] = std::sin(radian);
	m4.m[3][3] = 1;
	return m4;
};;

// 4. 逆行列
Matrix4x4 Inverse(const Matrix4x4& m) {
	float m4s;
	Matrix4x4 m4;
	m4s =
		m.m[0][0] * m.m[1][1] * m.m[2][2] * m.m[3][3] + m.m[0][0] * m.m[1][2] * m.m[2][3] * m.m[3][1] + m.m[0][0] * m.m[1][3] * m.m[2][1] * m.m[3][2]
		- m.m[0][0] * m.m[1][3] * m.m[2][2] * m.m[3][1] - m.m[0][0] * m.m[1][2] * m.m[2][1] * m.m[3][3] - m.m[0][0] * m.m[1][1] * m.m[2][3] * m.m[3][2]
		- m.m[0][1] * m.m[1][0] * m.m[2][2] * m.m[3][3] - m.m[0][2] * m.m[1][0] * m.m[2][3] * m.m[3][1] - m.m[0][3] * m.m[1][0] * m.m[2][1] * m.m[3][2]
		+ m.m[0][3] * m.m[1][0] * m.m[2][2] * m.m[3][1] + m.m[0][2] * m.m[1][0] * m.m[2][1] * m.m[3][3] + m.m[0][1] * m.m[1][0] * m.m[2][3] * m.m[3][2]
		+ m.m[0][1] * m.m[1][2] * m.m[2][0] * m.m[3][3] + m.m[0][2] * m.m[1][3] * m.m[2][0] * m.m[3][1] + m.m[0][3] * m.m[1][1] * m.m[2][0] * m.m[3][2]
		- m.m[0][3] * m.m[1][2] * m.m[2][0] * m.m[3][1] - m.m[0][2] * m.m[1][1] * m.m[2][0] * m.m[3][3] - m.m[0][1] * m.m[1][3] * m.m[2][0] * m.m[3][2]
		- m.m[0][1] * m.m[1][2] * m.m[2][3] * m.m[3][0] - m.m[0][2] * m.m[1][3] * m.m[2][1] * m.m[3][0] - m.m[0][3] * m.m[1][1] * m.m[2][2] * m.m[3][0]
		+ m.m[0][3] * m.m[1][2] * m.m[2][1] * m.m[3][0] + m.m[0][2] * m.m[1][1] * m.m[2][3] * m.m[3][0] + m.m[0][1] * m.m[1][3] * m.m[2][2] * m.m[3][0];
	m4.m[0][0] = 1 / m4s * (
		m.m[1][1] * m.m[2][2] * m.m[3][3] + m.m[1][2] * m.m[2][3] * m.m[3][1] + m.m[1][3] * m.m[2][1] * m.m[3][2]
		- m.m[1][3] * m.m[2][2] * m.m[3][1] - m.m[1][2] * m.m[2][1] * m.m[3][3] - m.m[1][1] * m.m[2][3] * m.m[3][2]
		);
	m4.m[0][1] = 1 / m4s * (
		-m.m[0][1] * m.m[2][2] * m.m[3][3] - m.m[0][2] * m.m[2][3] * m.m[3][1] - m.m[0][3] * m.m[2][1] * m.m[3][2]
		+ m.m[0][3] * m.m[2][2] * m.m[3][1] + m.m[0][2] * m.m[2][1] * m.m[3][3] + m.m[0][1] * m.m[2][3] * m.m[3][2]);
	m4.m[0][2] = 1 / m4s * (
		m.m[0][1] * m.m[1][2] * m.m[3][3] + m.m[0][2] * m.m[1][3] * m.m[3][1] + m.m[0][3] * m.m[1][1] * m.m[3][2]
		- m.m[0][3] * m.m[1][2] * m.m[3][1] - m.m[0][2] * m.m[1][1] * m.m[3][3] - m.m[0][1] * m.m[1][3] * m.m[3][2]);
	m4.m[0][3] = 1 / m4s * (
		-m.m[0][1] * m.m[1][2] * m.m[2][3] - m.m[0][2] * m.m[1][3] * m.m[2][1] - m.m[0][3] * m.m[1][1] * m.m[2][2]
		+ m.m[0][3] * m.m[1][2] * m.m[2][1] + m.m[0][2] * m.m[1][1] * m.m[2][3] + m.m[0][1] * m.m[1][3] * m.m[2][2]);

	//2
	m4.m[1][0] = 1 / m4s * (-m.m[1][0] * m.m[2][2] * m.m[3][3] - m.m[1][2] * m.m[2][3] * m.m[3][0] - m.m[1][3] * m.m[2][0] * m.m[3][2]
		+ m.m[1][3] * m.m[2][2] * m.m[3][0] + m.m[1][2] * m.m[2][0] * m.m[3][3] + m.m[1][0] * m.m[2][3] * m.m[3][2]);
	m4.m[1][1] = 1 / m4s * (+m.m[0][0] * m.m[2][2] * m.m[3][3] + m.m[0][2] * m.m[2][3] * m.m[3][0] + m.m[0][3] * m.m[2][0] * m.m[3][2]
		- m.m[0][3] * m.m[2][2] * m.m[3][0] - m.m[0][2] * m.m[2][0] * m.m[3][3] - m.m[0][0] * m.m[2][3] * m.m[3][2]);
	m4.m[1][2] = 1 / m4s * (-m.m[0][0] * m.m[1][2] * m.m[3][3] - m.m[0][2] * m.m[1][3] * m.m[3][0] - m.m[0][3] * m.m[1][0] * m.m[3][2]
		+ m.m[0][3] * m.m[1][2] * m.m[3][0] + m.m[0][2] * m.m[1][0] * m.m[3][3] + m.m[0][0] * m.m[1][3] * m.m[3][2]);
	m4.m[1][3] = 1 / m4s * (m.m[0][0] * m.m[1][2] * m.m[2][3] + m.m[0][2] * m.m[1][3] * m.m[2][0] + m.m[0][3] * m.m[1][0] * m.m[2][2]
		- m.m[0][3] * m.m[1][2] * m.m[2][0] - m.m[0][2] * m.m[1][0] * m.m[2][3] - m.m[0][0] * m.m[1][3] * m.m[2][2]);



	m4.m[2][0] = 1 / m4s * (
		m.m[1][0] * m.m[2][1] * m.m[3][3] + m.m[1][1] * m.m[2][3] * m.m[3][0] + m.m[1][3] * m.m[2][0] * m.m[3][1]
		- m.m[1][3] * m.m[2][1] * m.m[3][0] - m.m[1][1] * m.m[2][0] * m.m[3][3] - m.m[1][0] * m.m[2][3] * m.m[3][1]
		);
	m4.m[2][1] = 1 / m4s * (
		-m.m[0][0] * m.m[2][1] * m.m[3][3] - m.m[0][1] * m.m[2][3] * m.m[3][0] - m.m[0][3] * m.m[2][0] * m.m[3][1]
		+ m.m[0][3] * m.m[2][1] * m.m[3][0] + m.m[0][1] * m.m[2][0] * m.m[3][3] + m.m[0][0] * m.m[2][3] * m.m[3][1]);
	m4.m[2][2] = 1 / m4s * (
		m.m[0][0] * m.m[1][1] * m.m[3][3] + m.m[0][1] * m.m[1][3] * m.m[3][0] + m.m[0][3] * m.m[1][0] * m.m[3][1]
		- m.m[0][3] * m.m[1][1] * m.m[3][0] - m.m[0][1] * m.m[1][0] * m.m[3][3] - m.m[0][0] * m.m[1][3] * m.m[3][1]);
	m4.m[2][3] = 1 / m4s * (
		-m.m[0][0] * m.m[1][1] * m.m[2][3] - m.m[0][1] * m.m[1][3] * m.m[2][0] - m.m[0][3] * m.m[1][0] * m.m[2][1]
		+ m.m[0][3] * m.m[1][1] * m.m[2][0] + m.m[0][1] * m.m[1][0] * m.m[2][3] + m.m[0][0] * m.m[1][3] * m.m[2][1]);


	m4.m[3][0] = 1 / m4s * (-m.m[1][0] * m.m[2][1] * m.m[3][2] - m.m[1][1] * m.m[2][2] * m.m[3][0] - m.m[1][2] * m.m[2][0] * m.m[3][1]
		+ m.m[1][2] * m.m[2][1] * m.m[3][0] + m.m[1][1] * m.m[2][0] * m.m[3][2] + m.m[1][0] * m.m[2][2] * m.m[3][1]);
	m4.m[3][1] = 1 / m4s * (+m.m[0][0] * m.m[2][1] * m.m[3][2] + m.m[0][1] * m.m[2][2] * m.m[3][0] + m.m[0][2] * m.m[2][0] * m.m[3][1]
		- m.m[0][2] * m.m[2][1] * m.m[3][0] - m.m[0][1] * m.m[2][0] * m.m[3][2] - m.m[0][0] * m.m[2][2] * m.m[3][1]);
	m4.m[3][2] = 1 / m4s * (-m.m[0][0] * m.m[1][1] * m.m[3][2] - m.m[0][1] * m.m[1][2] * m.m[3][0] - m.m[0][2] * m.m[1][0] * m.m[3][1]
		+ m.m[0][2] * m.m[1][1] * m.m[3][0] + m.m[0][1] * m.m[1][0] * m.m[3][2] + m.m[0][0] * m.m[1][2] * m.m[3][1]);
	m4.m[3][3] = 1 / m4s * (m.m[0][0] * m.m[1][1] * m.m[2][2] + m.m[0][1] * m.m[1][2] * m.m[2][0] + m.m[0][2] * m.m[1][0] * m.m[2][1]
		- m.m[0][2] * m.m[1][1] * m.m[2][0] - m.m[0][1] * m.m[1][0] * m.m[2][2] - m.m[0][0] * m.m[1][2] * m.m[2][1]);

	return m4;

}


// 3. 座標返還
Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix) {
	Vector3 result;
	result.x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + vector.z * matrix.m[2][0] + 1.0f * matrix.m[3][0];
	result.y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + vector.z * matrix.m[2][1] + 1.0f * matrix.m[3][1];
	result.z = vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + vector.z * matrix.m[2][2] + 1.0f * matrix.m[3][2];
	float w = vector.x * matrix.m[0][3] + vector.y * matrix.m[1][3] + vector.z * matrix.m[2][3] + 1.0f * matrix.m[3][3];
	assert(w != 0.0f);
	result.x /= w;
	result.y /= w;
	result.z /= w;
	return result;
};




//クロス績
Vector3 Cross(const Vector3& v1, const Vector3& v2) {
	Vector3 v3;
	v3.x = v1.y * v2.z - v1.z * v2.y;
	v3.y = v1.z * v2.x - v1.x * v2.z;
	v3.z = v1.x * v2.y - v1.y * v2.x;
	return v3;
};

static const int kRowHeight = 20;
static const int kColumnWidth = 60;

void MatrixScreenPrintf(int x, int y, const Matrix4x4& matrix, const char* label) {
	Novice::ScreenPrintf(x, y, label);

	for (int row = 0; row < 4; ++row) {
		for (int column = 0; column < 4; ++column) {
			Novice::ScreenPrintf(x + column * kColumnWidth, y + row * kRowHeight + 20, "%6.02f", matrix.m[row][column], label);
		}
	}
}

void VectorScreenPrintf(int x, int y, const Vector3& vector, const char* label) {
	Novice::ScreenPrintf(x + kColumnWidth * 3, y, "%s", label);
	Novice::ScreenPrintf(
		x, y, "%6.02f", vector.x);
	Novice::ScreenPrintf(
		x + kColumnWidth * 1, y, "%6.02f", vector.y);
	Novice::ScreenPrintf(
		x + kColumnWidth * 2, y, "%6.02f", vector.z);



}



/*----------------------------------
			   01_02
---------------------------------*/

void DrawGrid(const Matrix4x4 viewprojectionMatrix, const Matrix4x4& viewportMatrix) {
	const float kGridHalfWidth = 2.0f;										// Gridの半分の幅
	const uint32_t kSubdivision = 10;										// 分割数
	const float kGridEvery = (kGridHalfWidth * 2.0f) / float(kSubdivision); // 1つ分の長さ
	// 奥から手前への線を順々に引いていく
	for (uint32_t xIndex = 0; xIndex <= kSubdivision; ++xIndex) {
		Vector3 startPos = { xIndex * kGridEvery,0,0 };
		Vector3 goalPos = { xIndex * kGridEvery,0,kSubdivision * kGridEvery };
		startPos = Transform(startPos, viewprojectionMatrix);
		goalPos = Transform(goalPos, viewprojectionMatrix);

		startPos = Transform(startPos, viewportMatrix);
		goalPos = Transform(goalPos, viewportMatrix);

		Novice::DrawLine(static_cast<int>(startPos.x), static_cast<int>(startPos.y), static_cast<int>(goalPos.x), static_cast<int>(goalPos.y), 0xAAAAAAFF);
	}
	//左から右も同じように順々に引いていく
	for (uint32_t zIndex = 0; zIndex <= kSubdivision; ++zIndex) {
		Vector3 startPos = { 0,0,zIndex * kGridEvery };
		Vector3 goalPos = { kSubdivision * kGridEvery ,0,zIndex * kGridEvery };
		startPos = Transform(startPos, viewprojectionMatrix);
		goalPos = Transform(goalPos, viewprojectionMatrix);

		startPos = Transform(startPos, viewportMatrix);
		goalPos = Transform(goalPos, viewportMatrix);

		Novice::DrawLine(static_cast<int>(startPos.x), static_cast<int>(startPos.y), static_cast<int>(goalPos.x), static_cast<int>(goalPos.y), 0xAAAAAAFF);
	}


}



void DrawSphere(const Sphere& sphere, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportmatrix, uint32_t color) {
	const uint32_t kSubdivision = 16;
	float pi = 3.1415f;
	const float kLonEvery = pi * 2.0f / (float)kSubdivision;  // 軽度分割1つ分の角度
	const float kLatEvery = pi / (float)kSubdivision;  // 緯度分割1つ分の角度


	// 緯度の方向に分割 -π/2 ~ π/2
	for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex) {
		float lat = -pi / 2.0f + kLatEvery * latIndex;// 現在の緯度
		// 緯度の方向に分割 0 ~ 2π
		for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {
			float lon = lonIndex * kLonEvery;// 現在の緯度
			// world座標系でのa,b,cを求める
			Vector3 a, b, c;
			const float latD = pi / kSubdivision;
			const float lonD = 2 * pi / kSubdivision;
			a = {
				(cos(lat) * cos(lon)) * sphere.radius + sphere.center.x,
				(sin(lat)) * sphere.radius + sphere.center.y,
				(cos(lat) * sin(lon)) * sphere.radius + sphere.center.z };

			b = {
				(cos(lat + latD) * cos(lon)) * sphere.radius + sphere.center.x,
				(sin(lat + latD)) * sphere.radius + sphere.center.y,
				(cos(lat + latD) * sin(lon)) * sphere.radius + sphere.center.z };

			c = {
				(cos(lat) * cos(lon + lonD) * sphere.radius + sphere.center.x),
				(sin(lat)) * sphere.radius + sphere.center.y,
				(cos(lat) * sin(lon + lonD)) * sphere.radius + sphere.center.z };


			a = Transform(a, viewProjectionMatrix);
			b = Transform(b, viewProjectionMatrix);
			c = Transform(c, viewProjectionMatrix);

			a = Transform(a, viewportmatrix);
			b = Transform(b, viewportmatrix);
			c = Transform(c, viewportmatrix);
			// a,b,cをScreen座標系まで変換...
			// ab,bcで線を引く
			Novice::DrawLine((int)a.x, (int)a.y, (int)b.x, (int)b.y, color);
			Novice::DrawLine((int)a.x, (int)a.y, (int)c.x, (int)c.y, color);
		}
	}
}
//正規化
Vector3 Normalize(const Vector3& v) {
	Vector3 m3;
	float mag = 1 / sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
	m3 = { v.x * mag,v.y * mag,v.z * mag };

	return m3;

};
/*-----------------------------------
			02_00
---------------------------------------*/
Vector3 Project(const Vector3& v1, const Vector3& v2) {
	float m3 = Dot(v1, Normalize(v2));
	Vector3 result;
	Vector3 demo = Normalize(v2);
	result.x = m3 * demo.x;
	result.y = m3 * demo.y;
	result.z = m3 * demo.z;
	return result;
}

Vector3 ClosestPoint(const Vector3& point, const Segment& segment) {
	Vector3 cp;
	cp.x = point.x + segment.origin.x;
	cp.y = point.y + segment.origin.y;
	cp.z = point.z + segment.origin.z;
	return cp;
}

//長さ（ノルム）
float Length(const Vector3& v) {

	float m3 = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);


	return m3;
};
bool IsCollision(const Vector3& pos, float radius, const Plane& s2) {
	bool g = false;
	//Vector3 q = { s1.center.x - s2.normal.x * s2.distance,s1.center.y - s2.normal.y * s2.distance, s1.center.z - s2.normal.z * s2.distance, };
	//float d = Dot(s2.normal, q);
	// 2つの弾の中心点間の距離を求める
	float distance = fabsf(Dot(s2.normal, pos) - s2.distance);
	if (distance <= radius) {
		g = true;
	}
	else { g = false; }
	return g;
}

Vector3 Parpendicular(const Vector3& vector) {
	if (vector.x != 0.0f || vector.y != 0.0f) {
		return { -vector.y,vector.x,0.0f };

	}
	return { 0.0f,-vector.z,vector.y };
}

void DrawPlane(const Plane& plane, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {
	Vector3 center = Multiply(plane.distance, plane.normal);
	Vector3 perpendiculars[4];
	perpendiculars[0] = Normalize(Parpendicular(plane.normal)); // 2
	perpendiculars[1] = { -perpendiculars[0].x, -perpendiculars[0].y, -perpendiculars[0].z };// 3
	perpendiculars[2] = Cross(plane.normal, perpendiculars[0]); // 4
	perpendiculars[3] = { -perpendiculars[2].x, -perpendiculars[2].y, -perpendiculars[2].z }; // 5
	// 6
	Vector3 points[4];
	for (int32_t index = 0; index < 4; ++index) {
		Vector3 extend = Multiply(2.0f, perpendiculars[index]);
		Vector3 point = Add(center, extend);
		points[index] = Transform(Transform(point, viewProjectionMatrix), viewportMatrix);
	}
	Novice::DrawLine((int)points[0].x, (int)points[0].y, (int)points[2].x, (int)points[2].y, color);
	Novice::DrawLine((int)points[2].x, (int)points[2].y, (int)points[1].x, (int)points[1].y, color);
	Novice::DrawLine((int)points[1].x, (int)points[1].y, (int)points[3].x, (int)points[3].y, color);
	Novice::DrawLine((int)points[0].x, (int)points[0].y, (int)points[3].x, (int)points[3].y, color);
}

Vector3 Reflect(const Vector3& input, const Vector3& normal) {
	Vector3 r;
	Vector3 proj = Project(input, normal);
	r.x = input.x - 2 * proj.x;
	r.y = input.y - 2 * proj.y;
	r.z = input.z - 2 * proj.z;
	return r;
}








bool IsCollisionAABB(const AABB& aabb, float radius, Vector3 pos) {
	bool g = false;

	// 最近接点を求める
	Vector3 closestPoint{
		std::clamp(pos.x,aabb.min.x,aabb.max.x),
		std::clamp(pos.y,aabb.min.y,aabb.max.y),
		std::clamp(pos.z,aabb.min.z,aabb.max.z)
	};

	// 最近接点と弾の中心との距離を求める
	float distance = Length({
		closestPoint.x - pos.x,
		closestPoint.y - pos.y,
		closestPoint.z - pos.z });
	// 距離が半径よりも小さければ衝突
	if (distance <= radius) {
		g = true;
	}
	else { g = false; }
	return g;
}

void DrawAABB(const AABB& aabb, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {
	Vector3 minMinMin;
	minMinMin = Transform(Transform({ aabb.min.x,aabb.min.y ,aabb.min.z }, viewProjectionMatrix), viewportMatrix);
	Vector3 minMaxMin;
	minMaxMin = Transform(Transform({ aabb.min.x,aabb.max.y ,aabb.min.z }, viewProjectionMatrix), viewportMatrix);
	Vector3 maxMinMin;
	maxMinMin = Transform(Transform({ aabb.max.x,aabb.min.y ,aabb.min.z }, viewProjectionMatrix), viewportMatrix);
	Vector3 maxMaxMin;
	maxMaxMin = Transform(Transform({ aabb.max.x,aabb.max.y ,aabb.min.z }, viewProjectionMatrix), viewportMatrix);

	Vector3 minMinMax;
	minMinMax = Transform(Transform({ aabb.min.x,aabb.min.y ,aabb.max.z }, viewProjectionMatrix), viewportMatrix);
	Vector3 minMaxMax;
	minMaxMax = Transform(Transform({ aabb.min.x,aabb.max.y ,aabb.max.z }, viewProjectionMatrix), viewportMatrix);
	Vector3 maxMinMax;
	maxMinMax = Transform(Transform({ aabb.max.x,aabb.min.y ,aabb.max.z }, viewProjectionMatrix), viewportMatrix);
	Vector3 maxMaxMax;
	maxMaxMax = Transform(Transform({ aabb.max.x,aabb.max.y ,aabb.max.z }, viewProjectionMatrix), viewportMatrix);



	Novice::DrawLine((int)minMinMin.x, (int)minMinMin.y, (int)minMaxMin.x, (int)minMaxMin.y, color);
	Novice::DrawLine((int)minMinMin.x, (int)minMinMin.y, (int)maxMinMin.x, (int)maxMinMin.y, color);
	Novice::DrawLine((int)minMaxMin.x, (int)minMaxMin.y, (int)maxMaxMin.x, (int)maxMaxMin.y, color);
	Novice::DrawLine((int)maxMinMin.x, (int)maxMinMin.y, (int)maxMaxMin.x, (int)maxMaxMin.y, color);

	Novice::DrawLine((int)minMinMin.x, (int)minMinMin.y, (int)minMinMax.x, (int)minMinMax.y, color);
	Novice::DrawLine((int)minMaxMin.x, (int)minMaxMin.y, (int)minMaxMax.x, (int)minMaxMax.y, color);
	Novice::DrawLine((int)maxMaxMin.x, (int)maxMaxMin.y, (int)maxMaxMax.x, (int)maxMaxMax.y, color);
	Novice::DrawLine((int)maxMinMin.x, (int)maxMinMin.y, (int)maxMinMax.x, (int)maxMinMax.y, color);

	Novice::DrawLine((int)minMinMax.x, (int)minMinMax.y, (int)minMaxMax.x, (int)minMaxMax.y, color);
	Novice::DrawLine((int)minMinMax.x, (int)minMinMax.y, (int)maxMinMax.x, (int)maxMinMax.y, color);
	Novice::DrawLine((int)minMaxMax.x, (int)minMaxMax.y, (int)maxMaxMax.x, (int)maxMaxMax.y, color);
	Novice::DrawLine((int)maxMinMax.x, (int)maxMinMax.y, (int)maxMaxMax.x, (int)maxMaxMax.y, color);
	/*Novice::DrawLine((int)minD.x, (int)minD.y, (int)minD.x, (int)maxD.y, color);
	Novice::DrawLine((int)minD.x, (int)maxD.y, (int)maxD.x, (int)maxD.y, color);
	Novice::DrawLine((int)minD.x, (int)minD.y, (int)maxD.x, (int)minD.y, color);*/

}


// ベクトルの差を計算する関数
Vector3 subtract(const Vector3& a, const Vector3& b) {
	return { a.x - b.x, a.y - b.y, a.z - b.z };
}

// ベクトルの外積を計算する関数
Vector3 crossProduct(const Vector3& a, const Vector3& b) {
	return {
		a.y * b.z - a.z * b.y,
		a.z * b.x - a.x * b.z,
		a.x * b.y - a.y * b.x
	};
}

// ベクトルを正規化する関数
Vector3 normalize(const Vector3& v) {
	float length = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
	return { v.x / length, v.y / length, v.z / length };
}

// 原点から平面までの距離を計算する関数
float distanceToPlane(const Vector3& normal, const Vector3& pointOnPlane) {
	return -(normal.x * pointOnPlane.x + normal.y * pointOnPlane.y + normal.z * pointOnPlane.z);
}

// ベクトルの内積を計算する関数
float dotProduct(const Vector3& a, const Vector3& b) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
}
// 指定した点から平面までの距離を計算する関数
float adistanceToPlane(const Vector3& normal, const Vector3& pointOnPlane, const Vector3& fromPoint) {
	Vector3 vectorToPoint = subtract(fromPoint, pointOnPlane);
	return dotProduct(normal, vectorToPoint);
}


struct OBB {
	Vector3 center;
	Vector3 orientations[3];
	Vector3 size;
};

void DrawOBB(const OBB& obb, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {
	Vector3 minMinMin;
	minMinMin = Transform(Transform({ -obb.size.x,-obb.size.y ,-obb.size.z }, viewProjectionMatrix), viewportMatrix);
	Vector3 minMaxMin;
	minMaxMin = Transform(Transform({ -obb.size.x,obb.size.y ,-obb.size.z }, viewProjectionMatrix), viewportMatrix);
	Vector3 maxMinMin;
	maxMinMin = Transform(Transform({ obb.size.x,-obb.size.y ,-obb.size.z }, viewProjectionMatrix), viewportMatrix);
	Vector3 maxMaxMin;
	maxMaxMin = Transform(Transform({ obb.size.x,obb.size.y ,-obb.size.z }, viewProjectionMatrix), viewportMatrix);

	Vector3 minMinMax;
	minMinMax = Transform(Transform({ -obb.size.x,-obb.size.y ,obb.size.z }, viewProjectionMatrix), viewportMatrix);
	Vector3 minMaxMax;
	minMaxMax = Transform(Transform({ -obb.size.x,obb.size.y ,obb.size.z }, viewProjectionMatrix), viewportMatrix);
	Vector3 maxMinMax;
	maxMinMax = Transform(Transform({ obb.size.x,-obb.size.y ,obb.size.z }, viewProjectionMatrix), viewportMatrix);
	Vector3 maxMaxMax;
	maxMaxMax = Transform(Transform({ obb.size.x,obb.size.y ,obb.size.z }, viewProjectionMatrix), viewportMatrix);



	Novice::DrawLine((int)minMinMin.x, (int)minMinMin.y, (int)minMaxMin.x, (int)minMaxMin.y, RED);
	Novice::DrawLine((int)minMinMin.x, (int)minMinMin.y, (int)maxMinMin.x, (int)maxMinMin.y, RED);
	Novice::DrawLine((int)minMaxMin.x, (int)minMaxMin.y, (int)maxMaxMin.x, (int)maxMaxMin.y, RED);
	Novice::DrawLine((int)maxMinMin.x, (int)maxMinMin.y, (int)maxMaxMin.x, (int)maxMaxMin.y, RED);

	Novice::DrawLine((int)minMinMin.x, (int)minMinMin.y, (int)minMinMax.x, (int)minMinMax.y, color);
	Novice::DrawLine((int)minMaxMin.x, (int)minMaxMin.y, (int)minMaxMax.x, (int)minMaxMax.y, BLUE);
	Novice::DrawLine((int)maxMaxMin.x, (int)maxMaxMin.y, (int)maxMaxMax.x, (int)maxMaxMax.y, 0xFFFF00FF);
	Novice::DrawLine((int)maxMinMin.x, (int)maxMinMin.y, (int)maxMinMax.x, (int)maxMinMax.y, 0xFF00FFFF);

	Novice::DrawLine((int)minMinMax.x, (int)minMinMax.y, (int)minMaxMax.x, (int)minMaxMax.y, GREEN);
	Novice::DrawLine((int)minMinMax.x, (int)minMinMax.y, (int)maxMinMax.x, (int)maxMinMax.y, GREEN);
	Novice::DrawLine((int)minMaxMax.x, (int)minMaxMax.y, (int)maxMaxMax.x, (int)maxMaxMax.y, GREEN);
	Novice::DrawLine((int)maxMinMax.x, (int)maxMinMax.y, (int)maxMaxMax.x, (int)maxMaxMax.y, GREEN);
};

Vector3 multiplyMatrixVector(const Matrix4x4& mat, const Vector3& vec) {
	Vector3 result;
	result.x = mat.m[0][0] * vec.x + mat.m[1][0] * vec.y + mat.m[2][0] * vec.z + mat.m[3][0];
	result.y = mat.m[0][1] * vec.x + mat.m[1][1] * vec.y + mat.m[2][1] * vec.z + mat.m[3][1];
	result.z = mat.m[0][2] * vec.x + mat.m[1][2] * vec.y + mat.m[2][2] * vec.z + mat.m[3][2];
	return result;
}

Vector3 computeNormal(const Vector3& v0, const Vector3& v1, const Vector3& v2) {
	Vector3 edge1 = { v1.x - v0.x, v1.y - v0.y, v1.z - v0.z };
	Vector3 edge2 = { v2.x - v0.x, v2.y - v0.y, v2.z - v0.z };

	Vector3 normal = {
		edge1.y * edge2.z - edge1.z * edge2.y,
		edge1.z * edge2.x - edge1.x * edge2.z,
		edge1.x * edge2.y - edge1.y * edge2.x
	};

	// 法線を正規化
	float length = std::sqrt(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
	normal.x /= length;
	normal.y /= length;
	normal.z /= length;

	return normal;
}
// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	Vector3 v1 = { 1.2f,-3.9f,2.5f };

	Vector3 v2 = { 2.8f,0.4f,-1.3f };

	Vector3 cross = Cross(v1, v2);

	VectorScreenPrintf(0, 0, cross, "Cross");

	Vector3 rotate{ 0.0f,0.0f,0.0f };

	Vector3 translate{ 0.0f,0.0f,0.0f };

	Vector3 cameraTranslate = { 0.0f,1.9f,-6.49f };
	Vector3 cameraRotate = { 0.26f,0.0f,0.0f };
	Vector3 kLocalkVertices[3] = {
		{0,0.1f,0},
		{-0.1f,0,0},
		{0.1f,0,0}
	};
	float deltaTime = 60.0f;
	Sphere sphere = { { 0,0,0 } ,0.5 };
	sphere.pos = { 0.0f,1.2f,0.0f };
	sphere.velocity = { 0,0,0 };
	sphere.acceleration = { 0.0f,-9.8f,0.0f };

	sphere.radius = 0.05f;
	sphere.color = WHITE;


	Plane plane;
	plane.normal = Normalize({ -0.2f,0.9f,-0.3f });
	plane.distance = 0.0f;
	plane.color = WHITE;

	Vector3 tmpNormal = {0};

	Plane plane2;
	plane2.normal = { -0.882f,-0.424f,-0.139f };
	plane2.distance = 2.740f;
	plane2.color = WHITE;

	AABB aabb1;
	aabb1.pos = { 0.0f,0.0f,0.0f };
	aabb1.width = 1.0f;
	aabb1.height = 1.0f;
	aabb1.length = 1.0f;
	aabb1.min = {
		aabb1.pos.x - aabb1.width,
		aabb1.pos.y - aabb1.height,
		aabb1.pos.z - aabb1.length };
	aabb1.max = {
		aabb1.pos.x + aabb1.width,
		aabb1.pos.y + aabb1.height,
		aabb1.pos.z + aabb1.length };
	aabb1.color = WHITE;

	//bool f = false;

	Vector3 OBBtranslate{ 0.0f,0.0f,0.0f };
	Vector3 OBBrotate{ 0.0f,0.0f,0.0f };
	OBB obb{
		.center{-1.0f,0.0f,0.0f},
		.orientations = {{1.0f,0.0f,0.0f},
						 {0.0f,1.0f,0.0f},
						 {0.0f,0.0f,1.0f}},
		.size{0.5f,0.5f,0.5f}
	};

	AABB aabbOBBLoacal;
	aabbOBBLoacal.min = { -obb.size.x,-obb.size.y ,-obb.size.z };
	aabbOBBLoacal.max = obb.size;

	// 各辺を結んだベクトルと、頂点と衝突点pを結んだ

	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0) {
		// フレームの開始
		Novice::BeginFrame();

		// キー入力を受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		///
		/// ↓更新処理ここから
		///
		if (keys[DIK_W]) {
			OBBtranslate.z += 0.1f;
		}
		if (keys[DIK_S]) {
			OBBtranslate.z -= 0.1f;
		}
		if (keys[DIK_D]) {
			OBBtranslate.x += 0.1f;
		}
		if (keys[DIK_A]) {
			OBBtranslate.x -= 0.1f;
		}
		if (keys[DIK_R]) {
			OBBrotate.y += 0.1f;
		}
		if (keys[DIK_Q]) {
			OBBrotate.y -= 0.1f;
		}
		if (keys[DIK_T]) {
			OBBrotate.x += 0.1f;
		}
		if (keys[DIK_Y]) {
			OBBrotate.x -= 0.1f;
		}
		if (keys[DIK_G]) {
			OBBrotate.z += 0.1f;
		}
		if (keys[DIK_H]) {
			OBBrotate.z -= 0.1f;
		}

		if (keys[DIK_I]) {
			obb.size.x+= 0.1f;
		}
		if (keys[DIK_O]) {
			obb.size.y += 0.1f;
		}
		if (keys[DIK_P]) {
			obb.size.z += 0.1f;
		}

		if (keys[DIK_J]) {
			obb.size.x -= 0.1f;
		}
		if (keys[DIK_K]) {
			obb.size.y -= 0.1f;
		}
		if (keys[DIK_L]) {
			obb.size.z -= 0.1f;
		}
		// 回転行列を作成
		Matrix4x4 rotateMatrix = Multiply(MakeRotateXMatrix(OBBrotate.x), Multiply(MakeRotateYMatrix(OBBrotate.y), MakeRotateZMatrix(OBBrotate.z)));
		Matrix4x4 translateMatrix;

		translateMatrix.m[3][0] = OBBtranslate.x;
		translateMatrix.m[3][1] = OBBtranslate.y;
		translateMatrix.m[3][2] = OBBtranslate.z;

		Matrix4x4 OBBWorldMatrix = rotateMatrix;
		OBBWorldMatrix.m[3][0] = translateMatrix.m[3][0];
		OBBWorldMatrix.m[3][1] = translateMatrix.m[3][1];
		OBBWorldMatrix.m[3][2] = translateMatrix.m[3][2];
		Matrix4x4 OBBInverseWorldMatrix = { 0 };
		OBBInverseWorldMatrix = Inverse(OBBWorldMatrix);


		// 回転行列から軸を抽出
		obb.orientations[0].x = rotateMatrix.m[0][0];
		obb.orientations[0].y = rotateMatrix.m[0][1];
		obb.orientations[0].z = rotateMatrix.m[0][2];

		obb.orientations[1].x = rotateMatrix.m[1][0];
		obb.orientations[1].y = rotateMatrix.m[1][1];
		obb.orientations[1].z = rotateMatrix.m[1][2];

		obb.orientations[2].x = rotateMatrix.m[2][0];
		obb.orientations[2].y = rotateMatrix.m[2][1];
		obb.orientations[2].z = rotateMatrix.m[2][2];


		Vector3 centerInOBBLoacalSpace =
			Transform(sphere.pos, OBBInverseWorldMatrix);
		
		aabbOBBLoacal.min = { -obb.size.x,-obb.size.y ,-obb.size.z };
		aabbOBBLoacal.max = obb.size;

		Sphere sphereOBBLocal;
		sphereOBBLocal.pos = centerInOBBLoacalSpace;
		sphereOBBLocal.radius = sphere.radius;

		// ローカル空間で衝突判定
		/*if (IsCollisionAABB(aabbOBBLoacal, sphereOBBLocal.radius,sphereOBBLocal.pos)) {
			sphere.color = RED;
		}
		else { sphere.color = WHITE; }*/
		Vector3 vertices[]
			= {
		{aabbOBBLoacal.min.x, aabbOBBLoacal.min.y, aabbOBBLoacal.min.z},
		{aabbOBBLoacal.max.x, aabbOBBLoacal.min.y, aabbOBBLoacal.min.z},
		{aabbOBBLoacal.max.x, aabbOBBLoacal.max.y, aabbOBBLoacal.min.z},
		{aabbOBBLoacal.min.x, aabbOBBLoacal.max.y, aabbOBBLoacal.min.z},
		{aabbOBBLoacal.min.x, aabbOBBLoacal.min.y, aabbOBBLoacal.max.z},
		{aabbOBBLoacal.max.x, aabbOBBLoacal.min.y, aabbOBBLoacal.max.z},
		{aabbOBBLoacal.max.x, aabbOBBLoacal.max.y, aabbOBBLoacal.max.z},
		{aabbOBBLoacal.min.x, aabbOBBLoacal.max.y, aabbOBBLoacal.max.z}
		};

		for (int i = 0; i < 8; ++i) {
			vertices[i] = multiplyMatrixVector(OBBWorldMatrix, vertices[i]);
		}
		if (IsCollisionAABB(aabbOBBLoacal, sphereOBBLocal.radius, sphereOBBLocal.pos)) {
			
			// 各面の頂点インデックス（頂点の時計回りまたは反時計回りの順序が重要）
			int faceIndices[][4] = {
				//{0, 1, 2, 3},  // Bottom
				{1, 0, 3, 2},  // Bottom
				{4, 5, 6, 7},  // Top
				{2, 3, 6, 7},  // Front
				{1, 0, 5, 4},  // Back
				{0, 4, 7, 3},  // Left
				{5, 1, 2, 6}   // Right
			};
			 // 各面の法線を計算して表示
        for (int i = 0; i < 6; ++i) {
            Vector3 normal = computeNormal(vertices[faceIndices[i][0]], vertices[faceIndices[i][1]], vertices[faceIndices[i][2]]);
            std::cout << "Face " << i + 1 << " Normal: (" << normal.x << ", " << normal.y << ", " << normal.z << ")\n";
        }
			// 各面の法線ベクトルを計算して表示
			for (int i = 0; i < 6; ++i) {
								Vector3 edge1 = subtract(vertices[faceIndices[i][1]], vertices[faceIndices[i][0]]);
								Vector3 edge2 = subtract(vertices[faceIndices[i][3]], vertices[faceIndices[i][0]]);
								Vector3 normal = crossProduct(edge1, edge2);
				normal = normalize(normal);

				// 法線面から指定した点までの距離を計算
				float distance = adistanceToPlane(normal, vertices[faceIndices[i][0]], OBBtranslate);

				Plane planeTemp;
				planeTemp.normal = normal;
				tmpNormal = normal;
				planeTemp.distance = distance;
				if (IsCollision(sphereOBBLocal.pos, sphereOBBLocal.radius, planeTemp)) {
					sphere.velocity = Reflect(sphere.velocity, planeTemp.normal);
					sphere.velocity.x *= 0.7f;
					sphere.velocity.y *= 0.7f;
					sphere.velocity.z *= 0.7f;
					sphere.color = BLUE;
				}
				else {
					//sphere.color = RED;
				}
				//std::cout << "Face " << i + 1 << " Normal: (" << normal.x << ", " << normal.y << ", " << normal.z << ")\n";
			}




			/*sphere.velocity = Reflect(sphere.velocity, plane.normal);
			sphere.velocity.x *= 0.7f;
			sphere.velocity.y *= 0.7f;
			sphere.velocity.z *= 0.7f;*/
			//sphere.color = RED;
			//aabb1.color = RED;
		}
		else { //sphere.color = WHITE; 
		}



		sphere.world = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, rotate, sphere.pos);


		Matrix4x4 worldMatrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, rotate, translate);
		Matrix4x4 cameraMatrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, { cameraRotate }, cameraTranslate);
		Matrix4x4 viewMatrix = Inverse(cameraMatrix);
		Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(1280) / float(720), 0.1f, 100.0f);
		Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));
		Matrix4x4 viewportMatrix = MakeViewportMatrix(0, 0, float(1280), float(720), 0.0f, 1.0f);
		sphere.worldView = Multiply(sphere.world, Multiply(viewMatrix, projectionMatrix));


		Matrix4x4 OBBworldViewProjectionMatrix = Multiply(OBBWorldMatrix, Multiply(viewMatrix, projectionMatrix));
		Matrix4x4 worldInverseViewProjectionMatrix = Multiply(OBBInverseWorldMatrix, Multiply(viewMatrix, projectionMatrix));

		Vector3 screenVertices[3];
		for (uint32_t i = 0; i < 3; ++i) {
			Vector3 ndcVertex = Transform(kLocalkVertices[i], worldViewProjectionMatrix);
			screenVertices[i] = Transform(ndcVertex, viewportMatrix);
		}




		ImGui::Begin("Window");
		ImGui::DragFloat3("CameraTranslate", &cameraTranslate.x, 0.01f);
		ImGui::DragFloat3("CameraRotate", &cameraRotate.x, 0.01f);
		ImGui::DragFloat3("Sphere", &sphere.pos.x, 0.01f);
		ImGui::DragFloat("Sphere", &sphere.radius, 0.01f);
		ImGui::DragFloat3("Plane", &plane.normal.x, 0.01f);
		ImGui::DragFloat("Plane", &plane.distance, 0.01f);
		ImGui::DragFloat3("Plane2", &plane2.normal.x, 0.01f);
		ImGui::DragFloat("Plane2", &plane2.distance, 0.01f);
		ImGui::Text("%f , %f, %f ",tmpNormal.x, tmpNormal.y, tmpNormal.z );
		if (ImGui::Button("Reset")) {
			sphere.pos = { 0.0f,1.2f,0.0f };
			sphere.velocity = { 0,0,0 };
			sphere.acceleration = { 0.0f,-9.8f,0.0f };
			sphere.color = WHITE;
		}
		if (ImGui::Button("rotateReset")) {
			OBBrotate = { 0.0f,0.0f,0.0f };
			
		}
		if (ImGui::Button("rotateX")) {
			OBBrotate.x += 3.1415f/180*90;
		}
		if (ImGui::Button("rotateY")) {
			OBBrotate.y += 3.1415f / 180*90;
		}
		if (ImGui::Button("rotateZ")) {
			OBBrotate.z += 3.1415f / 180*90;
		}
		ImGui::DragFloat("width", &aabb1.width, 0.01f);
		ImGui::DragFloat("height", &aabb1.height, 0.01f);



		ImGui::End();
		sphere.velocity = Add(sphere.velocity, { sphere.acceleration.x / deltaTime,sphere.acceleration.y / deltaTime,sphere.acceleration.z / deltaTime });

		sphere.pos.x += sphere.velocity.x / deltaTime;
		sphere.pos.y += sphere.velocity.y / deltaTime;
		sphere.pos.z += sphere.velocity.z / deltaTime;



		plane.normal = Normalize(plane.normal);
		plane2.normal = Normalize(plane2.normal);

		/*if (IsCollision(sphere.pos, sphere.radius, plane)) {
			sphere.velocity = Reflect(sphere.velocity, plane.normal);
			sphere.velocity.x *= 0.7f;
			sphere.velocity.y *= 0.7f;
			sphere.velocity.z *= 0.7f;
			sphere.color = RED;
		}
		else { sphere.color = WHITE;
		}*/


		
		

		//if (IsCollision(sphere.pos, sphere.radius, plane2)) {
		//	sphere.velocity = Reflect(sphere.velocity, plane2.normal);
		//	/*sphere.velocity.x *= 0.;
		//	sphere.velocity.y *= 0.5;
		//	sphere.velocity.z *= 0.5;*/

		//	sphere.color = RED;
		//}
		//else {
		//	sphere.color = WHITE;
		//}

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///
		//DrawAABB(aabb1, worldViewProjectionMatrix, viewportMatrix, aabb1.color);
		DrawOBB(obb, OBBworldViewProjectionMatrix, viewportMatrix, WHITE);
		DrawSphere(sphere, sphere.worldView, viewportMatrix, sphere.color);
		//DrawPlane(plane, worldViewProjectionMatrix, viewportMatrix, plane.color);
		//DrawPlane(plane2, worldViewProjectionMatrix, viewportMatrix, plane2.color);

		DrawGrid(worldViewProjectionMatrix, viewportMatrix);
		/*Novice::DrawTriangle(
			int(screenVertices[0].x), int(screenVertices[0].y), int(screenVertices[1].x), int(screenVertices[1].y),
			int(screenVertices[2].x), int(screenVertices[2].y), RED, kFillModeSolid);*/
		//VectorScreenPrintf(0, 0, cross, "Cross");
		//
		//
		//MatrixScreenPrintf(0, 20, viewportMatrix, "v");


		///
		/// ↑描画処理ここまで
		///

		// フレームの終了
		Novice::EndFrame();

		// ESCキーが押されたらループを抜ける
		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
			break;
		}
	}

	// ライブラリの終了
	Novice::Finalize();
	return 0;
}
