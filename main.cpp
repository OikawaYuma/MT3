#include <Novice.h>
#include <cmath>
#include <math.h>
#define _USE_MATH_DEFINES
#include <cassert>
#include <Vector2.h>
#include <stdlib.h>
#include <Vector3.h>
#include<imGui.h>
#include "Quaternion.h"
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
Matrix4x4 Add(const Matrix4x4 m1, const Matrix4x4& m2) {
	Matrix4x4 m4;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			m4.m[i][j] = m1.m[i][j] + m2.m[i][j];
		}
	}
	return m4;
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
			Novice::ScreenPrintf(x + column * kColumnWidth, y + row * kRowHeight + 20, "%6.03f", matrix.m[row][column], label);
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

void QuaternionScreenPrintf(int x, int y, const Quaternion& quaternion, const char* label) {
	Novice::ScreenPrintf(x + kColumnWidth * 4, y, "%s", label);
	Novice::ScreenPrintf(
		x, y, "%6.02f", quaternion.x);
	Novice::ScreenPrintf(
		x + kColumnWidth * 1, y, "%6.02f", quaternion.y);
	Novice::ScreenPrintf(
		x + kColumnWidth * 2, y, "%6.02f", quaternion.z);
	Novice::ScreenPrintf(
		x + kColumnWidth * 3, y, "%6.02f", quaternion.w);



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
//Matrix4x4 MakeCosS(float angle) {
//	Matrix4x4 S;
//	S.m[0][0] = cos(angle);
//	S.m[0][1] = 0;
//	S.m[0][2] = 0;
//	S.m[0][3] = 0;
//
//	S.m[1][0] = 0;
//	S.m[1][1] = cos(angle);
//	S.m[1][2] = 0;
//	S.m[1][3] = 0;
//
//	S.m[2][0] = 0;
//	S.m[2][1] = 0;
//	S.m[2][2] = cos(angle);
//	S.m[2][3] = 0;
//
//	S.m[3][0] = 0;
//	S.m[3][1] = 0;
//	S.m[3][2] = 0;
//	S.m[3][3] = 1;
//
//	return S;
//}
Matrix4x4 MakeScaleMatrix(const Vector3& scale) {

	Matrix4x4 m4;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			m4.m[i][j] = 0;
		}
	}
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (i == j) {
				m4.m[i][j] = 1.0f;
			}
		}
	}
	m4.m[0][0] = scale.x;
	m4.m[1][1] = scale.y;
	m4.m[2][2] = scale.z;
	return m4;

};
Matrix4x4 MakeCosS(float angle) {
	Matrix4x4 S;
	S.m[0][0] = cos(angle);
	S.m[0][1] = 0;
	S.m[0][2] = 0;
	S.m[0][3] = 0;

	S.m[1][0] = 0;
	S.m[1][1] = cos(angle);
	S.m[1][2] = 0;
	S.m[1][3] = 0;

	S.m[2][0] = 0;
	S.m[2][1] = 0;
	S.m[2][2] = cos(angle);
	S.m[2][3] = 0;

	S.m[3][0] = 0;
	S.m[3][1] = 0;
	S.m[3][2] = 0;
	S.m[3][3] = 1;

	return S;
}
Matrix4x4 Subtract(const Matrix4x4 m1, const Matrix4x4& m2) {
	Matrix4x4 m4;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			m4.m[i][j] = m1.m[i][j] - m2.m[i][j];
		}
	}
	return m4;
};


Matrix4x4 MakeRotateAxisAngle(const Vector3& axis, float angle) {
	Matrix4x4 r;
	Matrix4x4 S = MakeCosS(angle);
	Matrix4x4 P;
	//Matrix4x4 P = MakeCosS(angle);
	//Matrix4x4 C = MakeCosS(angle);;
	P.m[0][0] = axis.x * axis.x * (1 - cos(angle));
	P.m[0][1] = axis.x * axis.y * (1 - cos(angle));
	P.m[0][2] = axis.x * axis.z * (1 - cos(angle));
	P.m[0][3] = 0;

	P.m[1][0] = axis.x * axis.y * (1 - cos(angle));
	P.m[1][1] = axis.y * axis.y * (1 - cos(angle));
	P.m[1][2] = axis.y * axis.z * (1 - cos(angle));
	P.m[1][3] = 0;

	P.m[2][0] = axis.x * axis.z * (1 - cos(angle));
	P.m[2][1] = axis.y * axis.z * (1 - cos(angle));
	P.m[2][2] = axis.z * axis.z * (1 - cos(angle));
	P.m[2][3] = 0;

	P.m[3][0] = 0;
	P.m[3][1] = 0;
	P.m[3][2] = 0;
	P.m[3][3] = 0;
	Matrix4x4 C;
	C.m[0][0] = 0;
	C.m[0][1] = -axis.z * sin(angle);
	C.m[0][2] = axis.y * sin(angle);
	C.m[0][3] = 0;

	C.m[1][0] = axis.z * sin(angle);
	C.m[1][1] = 0;
	C.m[1][2] = -axis.x * sin(angle);
	C.m[1][3] = 0;

	C.m[2][0] = -axis.y * sin(angle);
	C.m[2][1] = axis.x * sin(angle);
	C.m[2][2] = 0;
	C.m[2][3] = 0;

	C.m[3][0] = 0;
	C.m[3][1] = 0;
	C.m[3][2] = 0;
	C.m[3][3] = 0;
	r = Add(S, Subtract(P, C));
	return r;
}

Vector3 SLerp(const Vector3& v1, const Vector3& v2, float t) {
	Vector3 p;

	Vector3 s;
	Vector3 e;

	s = Normalize(v1);
	e = Normalize(v2);
	float angle = acos(Dot(s, e));
	// SinΘ
	float SinTh = sin(angle);

	// 補間係数
	float Ps = sin(angle * (1 - t));
	float Pe = sin(angle * t);

	p.x = (Ps * s.x + Pe * e.x) / SinTh;
	p.y = (Ps * s.y + Pe * e.y) / SinTh;
	p.z = (Ps * s.z + Pe * e.z) / SinTh;

	p = Normalize(p);

	return p;
};

Vector3 Lerp(const Vector3& v1, const Vector3& v2, float t) {
	Vector3 p;
	p.x = v1.x + t * (v2.x - v1.x);
	p.y = v1.y + t * (v2.y - v1.y);
	p.z = v1.z + t * (v2.z - v1.z);
	return p;
};

Matrix4x4 DirectionToDirection(const Vector3& from, const Vector3& to) {
	Matrix4x4 result;
	Vector3 CrossN = Cross(from, to);
	if (CrossN.x == 0 && CrossN.y == 0 && CrossN.z == 0) {
		CrossN.x = from.y;
		CrossN.y = -from.x;
		CrossN.z = 0;
	}
	Vector3 n = Normalize(CrossN);
	float cosUV = Dot(from, to);
	float sinUV = Length(Cross(from, to));

	result.m[0][0] = n.x * n.x * (1 - cosUV) + cosUV;
	result.m[0][1] = n.x * n.y * (1 - cosUV) + n.z * sinUV;
	result.m[0][2] = n.x * n.z * (1 - cosUV) - n.y * sinUV;
	result.m[0][3] = 0;

	result.m[1][0] = n.x * n.y * (1 - cosUV) - n.z * sinUV;
	result.m[1][1] = n.y * n.y * (1 - cosUV) + cosUV;
	result.m[1][2] = n.y * n.z * (1 - cosUV) + n.x * sinUV;
	result.m[1][3] = 0;

	result.m[2][0] = n.x * n.z * (1 - cosUV) + n.y * sinUV;
	result.m[2][1] = n.y * n.z * (1 - cosUV) - n.x * sinUV;
	result.m[2][2] = n.z * n.z * (1 - cosUV) + cosUV;
	result.m[2][3] = 0;

	result.m[3][0] = 0;
	result.m[3][1] = 0;
	result.m[3][2] = 0;
	result.m[3][3] = 1;

	return result;
};

Quaternion IdentityQuaternion() {
	Quaternion result;
	result.x = 0.0f;
	result.y = 0.0f;
	result.z = 0.0f;
	result.w = 1.0f;
	return result;
}

Quaternion Conjugate(Quaternion quaternion) {
	Quaternion result = quaternion;
	result.x *= -1.0f;
	result.y *= -1.0f;
	result.z *= -1.0f;
	result.w *= 1.0f;
	return result;
}

Quaternion Inverse(Quaternion quaternion) {
	Quaternion result2 = quaternion;
	result2.x *= -1.0f;
	result2.y *= -1.0f;
	result2.z *= -1.0f;
	result2.w *= 1.0f;
	float result3 = sqrt(quaternion.x * quaternion.x + quaternion.y * quaternion.y + quaternion.z * quaternion.z + quaternion.w * quaternion.w)* sqrt(quaternion.x * quaternion.x + quaternion.y * quaternion.y + quaternion.z * quaternion.z + quaternion.w * quaternion.w);
	Quaternion result;
	result.x = result2.x/ result3;
	result.y = result2.y / result3;
	result.z = result2.z / result3;
	result.w = result2.w / result3;
	return result;
}

Quaternion Normalize(Quaternion quaternion) {

	Quaternion m3;
	float mag = 1 / sqrtf(quaternion.x * quaternion.x + quaternion.y * quaternion.y + quaternion.z * quaternion.z + quaternion.w * quaternion.w);
	m3 = { quaternion.x * mag,quaternion.y * mag,quaternion.z * mag,quaternion.w * mag };
	return m3;

}

Quaternion Multiply(Quaternion quaternion1, Quaternion quaternion2) {
	Quaternion result;

	result.x = quaternion1.y * quaternion2.z - quaternion1.z * quaternion2.y + quaternion2.w * quaternion1.x + quaternion1.w *quaternion2.x;
	result.y = quaternion1.z * quaternion2.x - quaternion1.x * quaternion2.z + quaternion2.w * quaternion1.y + quaternion1.w * quaternion2.y;
	result.z = quaternion1.x * quaternion2.y - quaternion1.y * quaternion2.x + quaternion2.w * quaternion1.z + quaternion1.w * quaternion2.z;
	result.w = quaternion1.w*quaternion2.w-Dot({ quaternion1.x,quaternion1.y ,quaternion1.z }, { quaternion2.x,quaternion2.y ,quaternion2.z });

	return result;
}

float Norm(Quaternion quaternion) {
	float result = sqrt(quaternion.x * quaternion.x + quaternion.y * quaternion.y + quaternion.z * quaternion.z + quaternion.w * quaternion.w);
	
	return result;
}

// 任意軸回転を表すQuaternionの生成
Quaternion MakeRotateAxisAngleQuaternion(const Vector3& axis,float angle) {
	Quaternion result;
	result.w = cos(angle / 2);
	result.x = axis.x*(sin(angle / 2));
	result.y = axis.y*sin(angle / 2);
	result.z = axis.z* sin(angle / 2);
	return result;
}

// ベクトルをQuaternionで回転させた結果のベクトルを求める
Vector3 RotateVector(const Vector3& vector, const Quaternion& quaternion) {
	Vector3 result;
	Quaternion vectorChenge;
	vectorChenge.x = vector.x;
	vectorChenge.y = vector.y;
	vectorChenge.z = vector.z;
	vectorChenge.w = Length(vector);
	Quaternion rotateVector = Multiply(Multiply(quaternion, vectorChenge), Conjugate(quaternion));
	result.x = rotateVector.x;
	result.y = rotateVector.y;
	result.z = rotateVector.z;
	return result;
}

// Quaternionから回転行列を求める
Matrix4x4 MakeRotateMatrix(const Quaternion& quaternion) {
	Matrix4x4 result;
	result.m[0][0] = quaternion.w* quaternion.w + quaternion.x*quaternion.x- quaternion.y*quaternion.y- quaternion.z* quaternion.z;
	result.m[0][1] = 2 * (quaternion.x * quaternion.y + quaternion.w * quaternion.z);
	result.m[0][2] = 2 * (quaternion.x * quaternion.z - quaternion.w * quaternion.y);
	result.m[0][3] = 0;

	result.m[1][0] = 2 * (quaternion.x * quaternion.y + quaternion.w * quaternion.z);
	result.m[1][1] = quaternion.w * quaternion.w - quaternion.x * quaternion.x + quaternion.y * quaternion.y - quaternion.z * quaternion.z;
	result.m[1][2] = 2 * (quaternion.y * quaternion.z + quaternion.w * quaternion.x);
	result.m[1][3] = 0;

	result.m[2][0] = 2 * (quaternion.x * quaternion.z + quaternion.w * quaternion.x);
	result.m[2][1] = 2 * (quaternion.y * quaternion.z - quaternion.w * quaternion.x);
	result.m[2][2] = quaternion.w * quaternion.w - quaternion.x * quaternion.x - quaternion.y * quaternion.y + quaternion.z * quaternion.z;
	result.m[2][3] = 0;

	result.m[3][0] = 0;
	result.m[3][1] = 0;
	result.m[3][2] = 0;
	result.m[3][3] = 1;
	return result;
}



// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	



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
		
		Quaternion rotation = MakeRotateAxisAngleQuaternion(
			Normalize(Vector3({ 1.0f,0.4f,-0.2f })), 0.45f);
		Vector3 pointY = { 2.1f,-0.9f,1.3f };
		Matrix4x4 rotateMatrix = MakeRotateMatrix(rotation);
		Vector3 rotateByQuaternion = RotateVector(pointY, rotation);
		Vector3 rotateByMatrix = Transform(pointY, rotateMatrix);

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		QuaternionScreenPrintf(0,kRowHeight*0,rotation," rotation");
		MatrixScreenPrintf(0, kRowHeight * 1, rotateMatrix, "   : rotateMatrix");
		VectorScreenPrintf(0, kRowHeight * 6, rotateByQuaternion, "   : rotateByQuaternion");
		VectorScreenPrintf(0, kRowHeight * 7, rotateByMatrix, "   : rotateByMatrix");


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