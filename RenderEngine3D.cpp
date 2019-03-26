// RenderEngine3D.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include "olcConsoleGameEngine.h"
#include <iostream>
#include <strstream>
#include <fstream>
#include <algorithm>

typedef struct vec3d {
	float x;
	float y;
	float z;

// 	vec3d() {}
// 
// 	vec3d(float x, float y, float z) {
// 		this->x = x;
// 		this->y = y;
// 		this->z = z;
// 	}
// 
// 	vec3d(float vars[]) {
// 
// 	}

	vec3d operator-(const vec3d & subtrahendVec) {
		vec3d rst;
		rst.x = this->x - subtrahendVec.x;
		rst.y = this->y - subtrahendVec.y;
		rst.z = this->z - subtrahendVec.z;
		// 		return vec3d(
		// 			this->x - subtrahendVec.x,
		// 			this->y - subtrahendVec.y,
		// 			this->z - subtrahendVec.z
		// 		);
		return rst;
	}

} point;


vec3d getCrossProduct(vec3d vec1, vec3d vec2) {
	// 	normal.x = line1.y * line2.z - line1.z * line2.y;
	// 	normal.y = line1.z * line2.x - line1.x * line2.z;
	// 	normal.z = line1.x * line2.y - line1.y * line2.x;
	// 
	// 	// It's normally normal to normalise the normal
	// 	float l = sqrtf(normal.x*normal.x + normal.y*normal.y + normal.z*normal.z);
	// 	normal.x /= l; normal.y /= l; normal.z /= l;
	vec3d rst;
	rst.x = vec1.y * vec2.z - vec1.z * vec2.y;
	rst.y = vec1.z * vec2.x - vec1.x * vec2.z;
	rst.z = vec1.x * vec2.y - vec1.y * vec2.x;
	return rst;
}

float getDotProduct(vec3d vec1, vec3d vec2) {
	return vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z;
}

// @ warning: 改变了params vec
vec3d getUnitVec(vec3d vec) {
	//vec3d normalise;
	float length = sqrtf(vec.x*vec.x + vec.y*vec.y + vec.z*vec.z);
	vec.x /= length;
	vec.y /= length;
	vec.z /= length;
	return vec;
}

struct Triangle {
	point points[3];
// private:
// 	vec3d _identityNormalVector;
// 	bool isHadCal

	float grayScale;

	vec3d getIdentityNormalVector() {
		vec3d line1 = points[1] - points[0];
		vec3d line2 = points[2] - points[0];
		return getUnitVec(getCrossProduct(line1, line2));
	}
};

struct Mesh {
	std::vector<Triangle> triangles;

	bool loadFromObjFile(std::string fileName) {
		std::ifstream ifs(fileName);
		if (!ifs.is_open()) {
			return false;
		}
		
		std::vector<vec3d> verts;
		char lineData[128];
		while (!ifs.eof()) {
			ifs.getline(lineData, 128);
			std::strstream strstrm;
			strstrm << lineData;
			char junk;
			if (lineData[0] == 'v' && lineData[1] == ' ') {
				vec3d v;
				strstrm >> junk >> v.x >> v.y >> v.z;
				verts.push_back(v);
			} else if (lineData[0] == 'f'  && lineData[1] == ' ') {
				int vidx[3];
				strstrm >> junk >> vidx[0] >> vidx[1] >> vidx[2];
				this->triangles.push_back({
					verts[vidx[0] - 1], 
					verts[vidx[1] - 1],
					verts[vidx[2] - 1],
					});
			}
		}
		return true;
	}

	bool sortTriangles() {
		sort(this->triangles.begin(), this->triangles.end(), [](Triangle & a, Triangle & b) {
			return a.points[0].z + a.points[1].z + a.points[2].z 
				> b.points[0].z + b.points[1].z + b.points[2].z;
		});
		return true;
	}

	void funcToEachTriangle(std::function<void(Triangle *)> func) {
		for (int i = 0; i < this->triangles.size(); ++i) {
			func(&(this->triangles[i]));
		}
	}

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

// bool syncInfomation(Mesh source, Mesh output) {
// 	output.triangles.resize(source.triangles.size());
// 
// }

bool transformViewportToHocCoordinate(const Mesh input, Mesh & output, Mat4x4 ppmat) {
	output.triangles.resize(input.triangles.size());

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

// 筛选可见的三角形（法线朝着camera的3角形）
bool filterVisibleTriangles(Mesh mesh, point camera, Mesh & outputMesh) {
	//outputMesh.triangles.resize(mesh.triangles.size() / 2);
	mesh.funcToEachTriangle([&outputMesh, &camera](Triangle* tri) {
		auto unitedNormalVec = tri->getIdentityNormalVector();
		vec3d cameraToTriangleUnitVec = getUnitVec(tri->points[0] - camera);
		if (getDotProduct(cameraToTriangleUnitVec, unitedNormalVec) < 0.0f) {
			outputMesh.triangles.push_back(*tri);
		}
	});
	return true;
}

bool lightColorTriangles(Mesh & mesh, vec3d directLight) {
	mesh.funcToEachTriangle([directLight](Triangle *tri) {
		tri->grayScale = getDotProduct(tri->getIdentityNormalVector(), getUnitVec(directLight));
	});
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

Mat4x4 getRotateYMat(float fTheta) {
	auto rad = fTheta * 3.14 / 180;
	Mat4x4 rotateYMat;
	rotateYMat.m[0][0] = cosf(rad);
	rotateYMat.m[0][2] = sinf(rad);
	rotateYMat.m[1][1] = 1;
	rotateYMat.m[2][0] = -sinf(rad);
	rotateYMat.m[2][2] = cosf(rad);
	rotateYMat.m[3][3] = 1;
	return rotateYMat;
}

Mat4x4 getMoveMat(float xMove = 0, float yMove = 0, float zMove = 0) {
	Mat4x4 moveMat = getIdentityMat();
	moveMat.m[3][0] = xMove;
	moveMat.m[3][1] = yMove;
	moveMat.m[3][2] = zMove;
	return moveMat;
}

CHAR_INFO GetColour(float lum)
{
	short bg_col, fg_col;
	wchar_t sym;
	int pixel_bw = (int)(13.0f*lum);
	switch (pixel_bw)
	{
	case 0: bg_col = BG_BLACK; fg_col = FG_BLACK; sym = PIXEL_SOLID; break;

	case 1: bg_col = BG_BLACK; fg_col = FG_DARK_GREY; sym = PIXEL_QUARTER; break;
	case 2: bg_col = BG_BLACK; fg_col = FG_DARK_GREY; sym = PIXEL_HALF; break;
	case 3: bg_col = BG_BLACK; fg_col = FG_DARK_GREY; sym = PIXEL_THREEQUARTERS; break;
	case 4: bg_col = BG_BLACK; fg_col = FG_DARK_GREY; sym = PIXEL_SOLID; break;

	case 5: bg_col = BG_DARK_GREY; fg_col = FG_GREY; sym = PIXEL_QUARTER; break;
	case 6: bg_col = BG_DARK_GREY; fg_col = FG_GREY; sym = PIXEL_HALF; break;
	case 7: bg_col = BG_DARK_GREY; fg_col = FG_GREY; sym = PIXEL_THREEQUARTERS; break;
	case 8: bg_col = BG_DARK_GREY; fg_col = FG_GREY; sym = PIXEL_SOLID; break;

	case 9:  bg_col = BG_GREY; fg_col = FG_WHITE; sym = PIXEL_QUARTER; break;
	case 10: bg_col = BG_GREY; fg_col = FG_WHITE; sym = PIXEL_HALF; break;
	case 11: bg_col = BG_GREY; fg_col = FG_WHITE; sym = PIXEL_THREEQUARTERS; break;
	case 12: bg_col = BG_GREY; fg_col = FG_WHITE; sym = PIXEL_SOLID; break;
	default:
		bg_col = BG_BLACK; fg_col = FG_BLACK; sym = PIXEL_SOLID;
	}

	CHAR_INFO c;
	c.Attributes = bg_col | fg_col;
	c.Char.UnicodeChar = sym;
	return c;
}



class RenderEngine3D : public olcConsoleGameEngine
{
public:
	bool OnUserCreate() override {
// 		_box.triangles = {
// 			// SOUTH
// 			{ 0.0f, 0.0f, 0.0f + 3.0f,    0.0f, 1.0f, 0.0f + 3.0f,    1.0f, 1.0f, 0.0f + 3.0f },
// 			{ 0.0f, 0.0f, 0.0f + 3.0f,    1.0f, 1.0f, 0.0f + 3.0f,    1.0f, 0.0f, 0.0f + 3.0f },
// 
// 			// EAST                                                      
// 			{ 1.0f, 0.0f, 0.0f + 3.0f,    1.0f, 1.0f, 0.0f + 3.0f,    1.0f, 1.0f, 1.0f + 3.0f },
// 			{ 1.0f, 0.0f, 0.0f + 3.0f,    1.0f, 1.0f, 1.0f + 3.0f,    1.0f, 0.0f, 1.0f + 3.0f },
// 
// 			// NORTH                                                     
// 			{ 1.0f, 0.0f, 1.0f + 3.0f,    1.0f, 1.0f, 1.0f + 3.0f,    0.0f, 1.0f, 1.0f + 3.0f },
// 			{ 1.0f, 0.0f, 1.0f + 3.0f,    0.0f, 1.0f, 1.0f + 3.0f,    0.0f, 0.0f, 1.0f + 3.0f },
// 
// 			// WEST                                                      
// 			{ 0.0f, 0.0f, 1.0f + 3.0f,    0.0f, 1.0f, 1.0f + 3.0f,    0.0f, 1.0f, 0.0f + 3.0f },
// 			{ 0.0f, 0.0f, 1.0f + 3.0f,    0.0f, 1.0f, 0.0f + 3.0f,    0.0f, 0.0f, 0.0f + 3.0f },
// 
// 			// TOP                                                       
// 			{ 0.0f, 1.0f, 0.0f + 3.0f,    0.0f, 1.0f, 1.0f + 3.0f,    1.0f, 1.0f, 1.0f + 3.0f },
// 			{ 0.0f, 1.0f, 0.0f + 3.0f,    1.0f, 1.0f, 1.0f + 3.0f,    1.0f, 1.0f, 0.0f + 3.0f },
// 
// 			// BOTTOM                                                    
// 			{ 1.0f, 0.0f, 1.0f + 3.0f,    0.0f, 0.0f, 1.0f + 3.0f,    0.0f, 0.0f, 0.0f + 3.0f },
// 			{ 1.0f, 0.0f, 1.0f + 3.0f,    0.0f, 0.0f, 0.0f + 3.0f,    1.0f, 0.0f, 0.0f + 3.0f },
// 		};

		_box.loadFromObjFile("IA2.obj");

		_ppmatParmas = { 1000, 0.1, 90, (float)ScreenWidth() / (float)ScreenHeight() };

		_ppmat = getPPMatByPPMatParmas(_ppmatParmas);

		_camera = { 0,0,0 };
		
		_directLight = { 0, 0, -1 };
		
		doChangeInit(_box);
		
		return true;
		
	};

	bool OnUserUpdate(float fElapsedTime) override {
		Mesh temp;
		
		Fill(0, 0, ScreenWidth(), ScreenHeight(), PIXEL_SOLID, FG_BLACK);
		doChange(this->_box, fElapsedTime);
		//temp = this->_box;
		Mesh visibleTriangles;
		filterVisibleTriangles(this->_box, this->_camera, visibleTriangles);
		lightColorTriangles(visibleTriangles, this->_directLight);
		temp = visibleTriangles;
		/*syncInfomation(visibleTriangles, temp);*/
		transformViewportToHocCoordinate(visibleTriangles, temp, _ppmat);
		scaleHocCordinateToScreen(temp, ScreenWidth(), ScreenHeight());
		drawMesh(temp);
		return true;
	};

private:
	Mesh _box;
	PPMatParmas _ppmatParmas;
	Mat4x4 _ppmat;
	point _camera;
	vec3d _directLight;

	bool drawMesh(Mesh mesh) {
		mesh.sortTriangles();
		for (auto tri : mesh.triangles) {
			
			FillTriangleWithGray(
				tri.points[0].x, tri.points[0].y,
				tri.points[1].x, tri.points[1].y,
				tri.points[2].x, tri.points[2].y,
				tri.grayScale
			);
// 			DrawTriangle(
// 				tri.points[0].x, tri.points[0].y,
// 				tri.points[1].x, tri.points[1].y,
// 				tri.points[2].x, tri.points[2].y,
// 				PIXEL_SOLID, FG_BLACK
// 			);
		}
// 		DrawTriangle(0,0, 
// 			256/2-100, 240/2,
// 			256,240
// 		);
		return true;
	}

	void FillTriangleWithGray(int x1, int y1, int x2, int y2, int x3, int y3, float grayScale) {
		auto colourInfo = GetColour(grayScale);
		FillTriangle(x1, y1, x2, y2, x3, y3, colourInfo.Char.UnicodeChar, colourInfo.Attributes);
	}
		
	bool doChangeInit(Mesh & mesh) {
		Mat4x4 finalMat = getIdentityMat();
		finalMat = MultiplyMat4x4WithMat4x4(finalMat, getMoveMat(0, -2, 4));
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
	}

	bool doChange(Mesh & mesh, float deltaTime) {
		Mat4x4 finalMat = getIdentityMat();
		finalMat = MultiplyMat4x4WithMat4x4(finalMat, getMoveMat(0,2,-4));
 		//finalMat = MultiplyMat4x4WithMat4x4(finalMat, getRotateXMat(1));
		finalMat = MultiplyMat4x4WithMat4x4(finalMat, getRotateYMat(6));
		//finalMat = MultiplyMat4x4WithMat4x4(finalMat, getRotateZMat(1));
 		finalMat = MultiplyMat4x4WithMat4x4(finalMat, getMoveMat(0, -2, 4));
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
