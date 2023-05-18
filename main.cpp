#include <Novice.h>
#include<cmath>

const char kWindowTitle[] = "LE2B_05_オイカワユウマ";

struct Vector3 {
	float x, y, z;
};
struct Matrix4x4 {
	float m[4][4];
};

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
	m4.m[3][0] = (left + right)/ (left-right);
	m4.m[3][1] = (top + bottom) / (bottom - top);
	m4.m[3][2] = nearClip/(nearClip-farClip);
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
	m4.m[0][0] = width/2;
	m4.m[1][1] = -(height/2);
	m4.m[2][2] = maxDepth - minDepth;
	m4.m[3][0] = left + (width / 2);
	m4.m[3][1] = top + height / 2;
	m4.m[3][2] = minDepth;
	m4.m[3][3] = 1;



	return m4;
};

static const int kRowHeight = 20;
static const int kColumnWidth = 60;

void MatrixScreenPrintf(int x, int y, const Matrix4x4& matrix, const char* label) {
	Novice::ScreenPrintf(x, y, label);

	for (int row = 0; row < 4; ++row) {
		for (int column = 0; column < 4; ++column) {
			Novice::ScreenPrintf(x + column * kColumnWidth, y + row * kRowHeight+20, "%6.02f", matrix.m[row][column], label);
		}
	}
}

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = {0};
	char preKeys[256] = {0};

	Matrix4x4 orthographicMatrix =
		MakeOrthographicMatrix(-160.0f,160.0f,200.0f,300.0f,0.0f,1000.0f);

	Matrix4x4 perspectiveFovmatrix =
		MakePerspectiveFovMatrix(0.63f,1.33f,0.1f,1000.0f);

	Matrix4x4 viewportMatrix =
		MakeViewportMatrix(100.f, 200.0f, 600.0f, 300.0f, 0.0f, 1.0f);
	
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

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		MatrixScreenPrintf(0, 0, orthographicMatrix, "orthographicMatrix");
		MatrixScreenPrintf(0, kRowHeight * 5, perspectiveFovmatrix, "perspectiveFovmatrix");
		MatrixScreenPrintf(0, kRowHeight*10, viewportMatrix, "viewportMatrix");

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
