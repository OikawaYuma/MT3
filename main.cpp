#include <Novice.h>
#include<cmath>
#include<math.h>
#define _USE_MATH_DEFINES
#include<cassert>
#include <Vector2.h>
#include<imGui.h>
const char kWindowTitle[] = "LE2B_05_オイカワユウマ";

struct Vector3 {
	float x, y, z;
};
struct Matrix4x4 {
	float m[4][4];
};

struct Sphere {
	Vector3 center; //!< 中心点
	float radius; //!< 半径
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
				(cos(lat) * cos(lon))*sphere.radius +sphere.center.x,
				(sin(lat))* sphere.radius + sphere.center.y,
				(cos(lat) * sin(lon))* sphere.radius + sphere.center.z };

			b = { 
				(cos(lat + latD) * cos(lon))* sphere.radius + sphere.center.x,
				(sin(lat + latD))* sphere.radius + sphere.center.y,
				(cos(lat + latD) * sin(lon))* sphere.radius + sphere.center.z };

			c = { 
				(cos(lat ) * cos(lon+lonD) * sphere.radius + sphere.center.x),
				(sin(lat))* sphere.radius + sphere.center.y,
				(cos(lat) * sin(lon+lonD) )* sphere.radius + sphere.center.z };


			a = Transform(a, viewProjectionMatrix);
			b = Transform(b, viewProjectionMatrix);
			c = Transform(c, viewProjectionMatrix);

			a = Transform(a, viewportmatrix);
			b = Transform(b, viewportmatrix);
			c = Transform(c, viewportmatrix);
			// a,b,cをScreen座標系まで変換...
			// ab,bcで線を引く
			Novice::DrawLine((int)a.x, (int)a.y, (int)b.x, (int)b.y, color);
			Novice::DrawLine((int)a.x, (int)a.y, (int)c.x, (int)c.y ,color);
		}
	}
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

	Sphere sphere = { { 0,0,0 } ,0.5};

	//float pi = 3.1415f;

	//Matrix4x4 orthographicMatrix =
	//	MakeOrthographicMatrix(-160.0f,160.0f,200.0f,300.0f,0.0f,1000.0f);

	//Matrix4x4 perspectiveFovmatrix =
	//	MakePerspectiveFovMatrix(0.63f,1.33f,0.1f,1000.0f);

	//Matrix4x4 viewportMatrix =
	//	MakeViewportMatrix(100.f, 200.0f, 600.0f, 300.0f, 0.0f, 1.0f);

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
			translate.z += 0.1f;
		}
		if (keys[DIK_S]) {
			translate.z -= 0.1f;
		}
		if (keys[DIK_D]) {
			translate.x += 0.1f;
		}
		if (keys[DIK_A]) {
			translate.x -= 0.1f;
		}

		if (keys[DIK_R]) {
			rotate.y += 0.1f;
		}
		if (keys[DIK_Q]) {
			rotate.y -= 0.1f;
		}



		Matrix4x4 worldMatrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, rotate, translate);
		Matrix4x4 cameraMatrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, { cameraRotate }, cameraTranslate);
		Matrix4x4 viewMatrix = Inverse(cameraMatrix);
		Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(1280) / float(720), 0.1f, 100.0f);
		Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));
		Matrix4x4 viewportMatrix = MakeViewportMatrix(0, 0, float(1280), float(720), 0.0f, 1.0f);
		Vector3 screenVertices[3];
		for (uint32_t i = 0; i < 3; ++i) {
			Vector3 ndcVertex = Transform(kLocalkVertices[i], worldViewProjectionMatrix);
			screenVertices[i] = Transform(ndcVertex, viewportMatrix);
		}

		ImGui::Begin("Window");
		ImGui::DragFloat3("CameraTranslate", &cameraTranslate.x, 0.01f);
		ImGui::DragFloat3("CameraRotate", &cameraRotate.x, 0.01f);
		ImGui::DragFloat3("Sphere", &sphere.center.x, 0.01f);
		ImGui::DragFloat("Sphere", &sphere.radius, 0.01f);
		ImGui::End();

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		DrawSphere(sphere,worldViewProjectionMatrix,viewportMatrix,BLACK);
		DrawGrid(worldViewProjectionMatrix, viewportMatrix);
		Novice::DrawTriangle(
			int(screenVertices[0].x), int(screenVertices[0].y), int(screenVertices[1].x), int(screenVertices[1].y),
			int(screenVertices[2].x), int(screenVertices[2].y), RED, kFillModeSolid);
		VectorScreenPrintf(0, 0, cross, "Cross");


		MatrixScreenPrintf(0, 20, viewportMatrix, "v");


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
