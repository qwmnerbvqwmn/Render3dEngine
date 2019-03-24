// RenderEngine3D.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include "olcConsoleGameEngine.h"
#include <iostream>

typedef struct vec3d {
	float x;
	float y;
	float z;
} point;

struct Triangle {
	point points[3];
};

struct Mesh {
	std::vector<Triangle> triangles;
	void funcToEachPoint(std::function<void(point*)>func) {
		for (int i = 0; i < this->triangles.size(); ++i) {
			Triangle* tri = &this->triangles[i];
			for (int j = 0; j < 3; ++j) {
// 				tri->points[j].x *= halfWidth;
// 				tri->points[j].y *= halfHeight;
				func(&(tri->points[j]));
			}
		}
	}
};

// void getMeshCopy(Mesh source) {
// 	Mesh rst;
// 	rst.triangles.resize(source.triangles.size());
// 	int i = 0;
// 	source.funcToEachPoint([&rst](point* p) {
// 		rst.triangles[i++] =
// 	});
// }



struct Mat4x4 {
	float m[4][4] = { 0 };
};

typedef struct PerspectiveProjectionMatrixParams {
	float zFar;
	float zNear;
	float viewAngle;
	float aspect;
} PPMatParmas;

inline float getRadByAngle(float angle) {
	return angle * 3.14 / 180;
}

Mat4x4 getPPMatByPPMatParmas(PPMatParmas ppmatParams) {
	float halfRad = getRadByAngle(ppmatParams.viewAngle * 0.5f);
	Mat4x4 ppmat;
	ppmat.m[0][0] = 1.0f / (tanf(halfRad) * ppmatParams.aspect);
	ppmat.m[1][1] = 1.0f / tanf(halfRad);
	ppmat.m[2][2] = ppmatParams.zFar / (ppmatParams.zFar - ppmatParams.zNear);
	ppmat.m[2][3] = 1;
	ppmat.m[3][2] = -1 * ppmatParams.zFar*ppmatParams.zNear / (ppmatParams.zFar - ppmatParams.zNear);
	return ppmat;
}

// proj.m[0][0] = 1 / (tan(fov * 0.5f) *aspect);
// proj.m[1][1] = 1 / tan(fov * 0.5f);
// proj.m[2][2] = zf / (zf - zn);
// proj.m[2][3] = 1.0f;
// proj.m[3][2] = (zn * zf) / (zn - zf);

// vec * mat4x4
void MultiplyMatrixVector(vec3d &i, vec3d &o, Mat4x4 &m)
{
	o.x = i.x * m.m[0][0] + i.y * m.m[1][0] + i.z * m.m[2][0] + m.m[3][0];
	o.y = i.x * m.m[0][1] + i.y * m.m[1][1] + i.z * m.m[2][1] + m.m[3][1];
	o.z = i.x * m.m[0][2] + i.y * m.m[1][2] + i.z * m.m[2][2] + m.m[3][2];
	float w = i.x * m.m[0][3] + i.y * m.m[1][3] + i.z * m.m[2][3] + m.m[3][3];

	if (w != 0.0f)
	{
		o.x /= w; o.y /= w; o.z /= w;
	}
}

Mat4x4 MultiplyMat4x4WithMat4x4(Mat4x4 m1, Mat4x4 m2) {
	Mat4x4 output;
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			output.m[i][j] =
				m1.m[i][0] * m2.m[0][j] +
				m1.m[i][1] * m2.m[1][j] +
				m1.m[i][2] * m2.m[2][j] +
				m1.m[i][3] * m2.m[3][j];
		}
	}
	return output;
}

bool transformViewportToHocCoordinate(const Triangle input, Triangle & output, Mat4x4 ppmat) {
	auto i = 0;
	for (auto pt : input.points) {
		MultiplyMatrixVector(pt, output.points[i++], ppmat);
	}
	return true;
}

bool transformViewportToHocCoordinate(const Mesh input, Mesh & output, Mat4x4 ppmat) {
	
	auto i = 0;
	for (auto tri : input.triangles) {
		transformViewportToHocCoordinate(tri, output.triangles[i++], ppmat);
	}
	return true;
}

bool scaleHocCordinateToScreen(Mesh & hocedMesh, float screenWidth, float screenHeight) {
	auto halfWidth = screenWidth / 2.0f;
	auto halfHeight = screenHeight / 2.0f;
// 	for (auto tri : hocedMesh.triangles) {
// 		for (auto pt : tri.points) {
// 			pt.x *= halfWidth;
// 			pt.y *= halfHeight;
// 		}
// 	}
 	for (int i = 0; i < hocedMesh.triangles.size(); ++i) {
 		Triangle* tri = & hocedMesh.triangles[i];
 		for (int j = 0; j < 3; ++j) {
			// tri.p[0].x += 1.0f; triProjected.p[0].y += 1.0f;
			//tri->points[j].x += 1.0f;
			//tri->points[j].y += 1.0f;
			tri->points[j].x *= halfWidth;
			tri->points[j].x += halfWidth;
			tri->points[j].y *= halfHeight * (-1.0f);
			tri->points[j].y += halfHeight;
 		}
 	}
	return true;
}


Mat4x4 getIdentityMat() {
	Mat4x4 identMat;
	identMat.m[0][0] = 1;
	identMat.m[1][1] = 1;
	identMat.m[2][2] = 1;
	identMat.m[3][3] = 1;
	return identMat;
}

Mat4x4 getRotateZMat(float fTheta) {
	// Rotation Z
	auto rad = fTheta * 3.14 / 180;
	Mat4x4 rotateZMat;
	rotateZMat.m[0][0] = cosf(rad);
	rotateZMat.m[0][1] = sinf(rad);
	rotateZMat.m[1][0] = -sinf(rad);
	rotateZMat.m[1][1] = cosf(rad);
	rotateZMat.m[2][2] = 1;
	rotateZMat.m[3][3] = 1;
	return rotateZMat;
}

Mat4x4 getRotateXMat(float fTheta) {
	auto rad = fTheta * 3.14 / 180;
	Mat4x4 rotateXMat;
	rotateXMat.m[0][0] = 1;
	rotateXMat.m[1][1] = cosf(rad);
	rotateXMat.m[1][2] = sinf(rad);
	rotateXMat.m[2][1] = -sinf(rad);
	rotateXMat.m[2][2] = cosf(rad);
	rotateXMat.m[3][3] = 1;
	return rotateXMat;
}

Mat4x4 getMoveMat(float xMove = 0, float yMove = 0, float zMove = 0) {
	Mat4x4 moveMat = getIdentityMat();
	moveMat.m[3][0] = xMove;
	moveMat.m[3][1] = yMove;
	moveMat.m[3][2] = zMove;
	return moveMat;
}


class RenderEngine3D : public olcConsoleGameEngine
{
public:
	bool OnUserCreate() override {
		_box.triangles = {
			// SOUTH
			{ 0.0f, 0.0f, 0.0f + 3.0f,    0.0f, 1.0f, 0.0f + 3.0f,    1.0f, 1.0f, 0.0f + 3.0f },
			{ 0.0f, 0.0f, 0.0f + 3.0f,    1.0f, 1.0f, 0.0f + 3.0f,    1.0f, 0.0f, 0.0f + 3.0f },

			// EAST                                                      
			{ 1.0f, 0.0f, 0.0f + 3.0f,    1.0f, 1.0f, 0.0f + 3.0f,    1.0f, 1.0f, 1.0f + 3.0f },
			{ 1.0f, 0.0f, 0.0f + 3.0f,    1.0f, 1.0f, 1.0f + 3.0f,    1.0f, 0.0f, 1.0f + 3.0f },

			// NORTH                                                     
			{ 1.0f, 0.0f, 1.0f + 3.0f,    1.0f, 1.0f, 1.0f + 3.0f,    0.0f, 1.0f, 1.0f + 3.0f },
			{ 1.0f, 0.0f, 1.0f + 3.0f,    0.0f, 1.0f, 1.0f + 3.0f,    0.0f, 0.0f, 1.0f + 3.0f },

			// WEST                                                      
			{ 0.0f, 0.0f, 1.0f + 3.0f,    0.0f, 1.0f, 1.0f + 3.0f,    0.0f, 1.0f, 0.0f + 3.0f },
			{ 0.0f, 0.0f, 1.0f + 3.0f,    0.0f, 1.0f, 0.0f + 3.0f,    0.0f, 0.0f, 0.0f + 3.0f },

			// TOP                                                       
			{ 0.0f, 1.0f, 0.0f + 3.0f,    0.0f, 1.0f, 1.0f + 3.0f,    1.0f, 1.0f, 1.0f + 3.0f },
			{ 0.0f, 1.0f, 0.0f + 3.0f,    1.0f, 1.0f, 1.0f + 3.0f,    1.0f, 1.0f, 0.0f + 3.0f },

			// BOTTOM                                                    
			{ 1.0f, 0.0f, 1.0f + 3.0f,    0.0f, 0.0f, 1.0f + 3.0f,    0.0f, 0.0f, 0.0f + 3.0f },
			{ 1.0f, 0.0f, 1.0f + 3.0f,    0.0f, 0.0f, 0.0f + 3.0f,    1.0f, 0.0f, 0.0f + 3.0f },
		};

		_ppmatParmas = { 1000, 0.1, 90, (float)ScreenWidth() / (float)ScreenHeight() };

		_ppmat = getPPMatByPPMatParmas(_ppmatParmas);
		
		
		return true;
		
	};

	bool OnUserUpdate(float fElapsedTime) override {
		Mesh temp;
		temp.triangles.resize(this->_box.triangles.size());
		Fill(0, 0, ScreenWidth(), ScreenHeight(), PIXEL_SOLID, FG_BLACK);
		doChange(this->_box, fElapsedTime);
		//temp = this->_box;
		transformViewportToHocCoordinate(this->_box, temp, _ppmat);
		scaleHocCordinateToScreen(temp, ScreenWidth(), ScreenHeight());
		drawMesh(temp);
		return true;
	};

private:
	Mesh _box;
	PPMatParmas _ppmatParmas;
	Mat4x4 _ppmat;

	bool drawMesh(const Mesh mesh) {
		for (auto tri : mesh.triangles) {
			DrawTriangle(
				tri.points[0].x, tri.points[0].y,
				tri.points[1].x, tri.points[1].y,
				tri.points[2].x, tri.points[2].y
			);
		}
// 		DrawTriangle(0,0, 
// 			256/2-100, 240/2,
// 			256,240
// 		);
		return true;
	}

	bool doChange(Mesh & mesh, float deltaTime) {

		Mat4x4 finalMat = getIdentityMat();
		finalMat = MultiplyMat4x4WithMat4x4(finalMat, getMoveMat(0,0,-3));
 		finalMat = MultiplyMat4x4WithMat4x4(finalMat, getRotateXMat(1));
  		finalMat = MultiplyMat4x4WithMat4x4(finalMat, getRotateZMat(2));
 		finalMat = MultiplyMat4x4WithMat4x4(finalMat, getMoveMat(0, 0, 3));
		//Mat4x4 finalMat = MultiplyMat4x4WithMat4x4(getIdentityMat(), getRotateZMat(2));
		point temp;
		mesh.funcToEachPoint(
			[&temp, &finalMat](point* p) {
				MultiplyMatrixVector(*p, temp, finalMat);
				p->x = temp.x;
				p->y = temp.y;
				p->z = temp.z;
			}
		);
		return true;
		//mesh = MultiplyMat4x4WithMat4x4(mesh, finalMat);
		
	}
	
	//	bool DrawTriangle() override {
	//
	//	}
	
};

int main()
{
	RenderEngine3D re3d;
	if (re3d.ConstructConsole(256, 240, 4, 4)) {
		re3d.Start();
	}

    //std::cout << "Hello World!\n"; 
}
