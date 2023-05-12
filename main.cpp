#include <Novice.h>
#include<math.h>

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
Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectratio, float nearClip, float farClip) {

};

//// 2. 正射影行列
//Matrix4x4 MakeOrthographicMatrix(float left,float top,float right,float bottom,float nearClip,float farClip);
//
//// 3. ビューポート変換行列
//Matrix4x4 MakeViewportMatrix(float left,float top,float width,float height, float minDepth,float maxDepth);
//
static const int kRowHeight = 20;
static const int kColumnWidth = 60;

void MatrixScreenPrintf(int x, int y, const Matrix4x4& matrix, const char* label) {
	for (int row = 0; row < 4; ++row) {
		for (int column = 0; column < 4; ++column) {
			Novice::ScreenPrintf(x + column * kColumnWidth, y + row * kRowHeight, "%6.02f", matrix.m[row][column], label);
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

	/*Matrix4x4 orthographicMatrix =
		MakeOrthographicMatrix(-160.0f,160.0f,200.0f,300.0f,0.0f,1000.0f);*/

	Matrix4x4 perspectiveFovmatrix =
		MakePerspectiveFovMatrix(0.63f,1.33f,0.1f,1000.0f);

	/*Matrix4x4 viewportMatrix =
		MakeViewportMatrix(100.f, 200.0f, 600.0f, 300.0f, 0.0f, 1.0f);*/
	
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

		//MatrixScreenPrintf(0, 0, orthographicMatrix, "orthographicMatrix");
		MatrixScreenPrintf(0, kRowHeight * 5, perspectiveFovmatrix, "perspectiveFovmatrix");
		//MatrixScreenPrintf(0, kRowHeight, viewportMatrix, "viewportMatrix");

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
