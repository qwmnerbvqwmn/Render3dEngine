// RenderEngine3D.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include "olcConsoleGameEngine.h"
#include <iostream>
#include <strstream>
#include <fstream>
#include <algorithm>
#include <functional>

typedef struct vec3d {
	float x;
	float y;
	float z;
	//float w = 1;

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

	vec3d operator+(const vec3d & additionVec) {
		vec3d rst;
		rst.x = this->x + additionVec.x;
		rst.y = this->y + additionVec.y;
		rst.z = this->z + additionVec.z;
		return rst;
	}

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

	vec3d operator*(const float & multiplier) {
		vec3d rst;
		rst.x = this->x * multiplier;
		rst.y = this->y * multiplier;
		rst.z = this->z * multiplier;
		return rst;
	}

	void operator+=(const vec3d & addition) {
		this->x += addition.x;
		this->y += addition.y;
		this->z += addition.z;
	}

	void operator-=(const vec3d & addition) {
		this->x -= addition.x;
		this->y -= addition.y;
		this->z -= addition.z;
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

	void copyValues(Triangle tri) {
		this->points[0] = tri.points[0];
		this->points[1] = tri.points[1];
		this->points[2] = tri.points[2];
	}

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

	//Mat4x4
	void setValues(float (newValues)[4][4]) {
		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < 4; ++j) {
				m[i][j] = newValues[i][j];
			}
		}
	}

	vec3d getXAxisRotateToTargetVec() const {
		return { m[0][0], m[0][1], m[0][2] };
	}

	vec3d getYAxisRotateToTargetVec() const {
		return { m[1][0], m[1][1], m[1][2] };
	}

	vec3d getZAxisRotateToTargetVec() const {
		return { m[2][0], m[2][1], m[2][2] };
	}

	point getTargetPosition() const {
		return { m[3][0], m[3][1], m[3][2] };
	}

};




Mat4x4 getTransposeMatrix(const Mat4x4 & matSource) {
	Mat4x4 matTranspose;
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			matTranspose.m[i][j] = matSource.m[j][i];
		}
	}
	return matTranspose;
}

Mat4x4 getAffineMatInverse(const Mat4x4 & matSource) {
	//return getTransposeMatrix(matSource);
	Mat4x4 rst = matSource;
	rst.m[1][0] = matSource.m[0][1];
	rst.m[2][0] = matSource.m[0][2];
	rst.m[2][1] = matSource.m[1][2];

	rst.m[0][1] = matSource.m[1][0];
	rst.m[0][2] = matSource.m[2][0];
	rst.m[1][2] = matSource.m[2][1];

	point targetPositionPoint = matSource.getTargetPosition();
	rst.m[3][0] = -1 * getDotProduct(targetPositionPoint, matSource.getXAxisRotateToTargetVec());
	rst.m[3][1] = -1 * getDotProduct(targetPositionPoint, matSource.getYAxisRotateToTargetVec());
	rst.m[3][2] = -1 * getDotProduct(targetPositionPoint, matSource.getZAxisRotateToTargetVec());
	return rst;
}







inline float getRadByAngle(float angle) {
	return angle * 3.14 / 180;
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

	if (w > 0.00001 || w < -0.00001)
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
	rotateYMat.m[0][2] = -sinf(rad);
	rotateYMat.m[1][1] = 1;
	rotateYMat.m[2][0] = sinf(rad);
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


struct Plane
{
	point onePointInPanel;
	vec3d normalVec;

	float getDistance(point p) {
		return getDotProduct((p - onePointInPanel), normalVec);
	}

	bool isOutside(point p) {
		return getDistance(p) > 0;
	}
};

struct Line
{
	point onePointInLine;
	vec3d directVec;

	// p = onePoint + d * directVec，点斜式
	point getThePointByMultiples(float multiples) {
		return onePointInLine + directVec * multiples;
	}
};

Line getLineByTwoPoint(point p1, point p2) {
	Line l;
	l = { p1, p2 - p1 };
	return l;
}



typedef struct Camera
{
	point _position = {0,0,0};
	vec3d _lookAt = {0,0,1};
	vec3d _upVec = { 0,1,0 };
	//PPMatParmas _ppma

	void moveForward(float distance) {
		_position += _lookAt * distance;
	}

	void moveBackward(float distance) {
		_position += _lookAt * (-1 * distance);
	}

	void moveLeft(float distance) {
		auto vec_left = getVecLeft();
		_position += vec_left * distance;
	}

	void moveRight(float distance) {
		auto vec_right = getVecLeft() * (-1);
		_position += vec_right * distance;
	}

	void moveUp(float distance) {
		auto vec_up = getVecUp();
		_position += vec_up * distance;
	}

	void moveDown(float distance) {
		moveUp(-1.0f * distance);
	}

	void rotateLeft(float angle) {
		vec3d temp;
		auto rotateMat = getRotateYMat(-1 * angle);
		MultiplyMatrixVector(_lookAt, temp, rotateMat);
		_lookAt = getUnitVec(temp); 
	}

	void rotateRight(float angle) {
		rotateLeft(-1.0 * angle);
	}

	void rotateUp(float angle){
		// 需要同时旋转_up和_lookAt
	}

	void rotateClockwise(float angle) {

	}

	// 点头，摇头，牙通牙
	void rotateTotal(float anglePitch, float angleYaw, float angleRoll) {

	}

	vec3d getVecLeft() {
		return getCrossProduct(_upVec, _lookAt) * (-1);
	}
	vec3d getVecUp() {
		return _upVec;
	}

	Mat4x4 getMatTransformToPosition() {
		Mat4x4 matRst = getIdentityMat();
		matRst.m[3][0] = _position.x;
		matRst.m[3][1] = _position.y;
		matRst.m[3][2] = _position.z;
		return matRst;
	}

	// 获取旋转到当前姿态的矩阵
	Mat4x4 getMatRotateToPosture() {
		auto vecRight = getVecLeft() * (-1);
		Mat4x4 matRst;
		float t[4][4] = {
			{vecRight.x,    vecRight.y,		vecRight.z,		0},
			{_upVec.x,  	_upVec.y,		_upVec.z,		0},
			{_lookAt.x, 	_lookAt.y,		_lookAt.z,		0},
			{0,				0,				0,				1},
		};
		matRst.setValues(t);
		return matRst;
	}

	// 	获取当前原点到camera的矩阵
	Mat4x4 getMatAffine() {
		
		return MultiplyMat4x4WithMat4x4(getMatRotateToPosture(), getMatTransformToPosition());
	}

	Mat4x4 getMatWorldToCamera() {
		return getAffineMatInverse(getMatAffine());
	}



};

typedef struct PerspectiveProjectionMatrixParams {
	float zFar;
	float zNear;
	float viewAngle;
	float aspect;

	Plane getZNearPlane() {
		// 		return 	point onePointInPanel;
		// 	vec3d normalVec;
		return { { 0, 0, zNear }, {0,0,-1} };
	}

	int getAroundPlane(Plane* output) {
		auto rad = getRadByAngle(viewAngle/2.0f);
		auto tanTheta = tanf(rad);
		auto cosTheta = cosf(rad);
		auto sinTheta = sinf(rad);
//		output[0] = { {0,tanTheta * zNear,zNear },	{0,cosTheta,-sinTheta} )};
		// // 上界面
		// output[0] = { {0,tanTheta * zNear,zNear },	{0,cosTheta,-sinTheta} };
		// // 左界面
		// output[1] = { {-tanTheta * zNear,0,zNear},	{-cosTheta,0,-sinTheta} };
		// // 下界面
		// output[2] = { {0,-tanTheta * zNear,zNear },	{0,-cosTheta,-sinTheta} };
		// // 右界面
		// output[3] = { {tanTheta * zNear,0,zNear},	{cosTheta,0,-sinTheta} };

		// 上界面
		output[0] = { {0,0,0},	{0,cosTheta,-sinTheta} };
		// 左界面
		output[1] = { {0,0,0},	{-cosTheta,0,-sinTheta} };
		// 下界面
		output[2] = { {0,0,0},	{0,-cosTheta,-sinTheta} };
		// 右界面
		output[3] = { {0,0,0},	{cosTheta,0,-sinTheta} };


		return 4;

		// return { 
		// 	{0, 0, zNear} ,
		// 	{0, 0, zFar} ,
		// 	{}
		// };
	}



	Mat4x4 getPPMat() {
		auto ppmatParams = *this;
		float halfRad = getRadByAngle(ppmatParams.viewAngle * 0.5f);
		Mat4x4 ppmat;
		ppmat.m[0][0] = 1.0f / (tanf(halfRad) * ppmatParams.aspect);
		ppmat.m[1][1] = 1.0f / tanf(halfRad);
		ppmat.m[2][2] = ppmatParams.zFar / (ppmatParams.zFar - ppmatParams.zNear);
		ppmat.m[2][3] = 1;
		ppmat.m[3][2] = -1 * ppmatParams.zFar*ppmatParams.zNear / (ppmatParams.zFar - ppmatParams.zNear);
		return ppmat;
	}

} PPMatParmas;

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

bool transformViewportToHocCoordinate(Mesh & mesh, Mat4x4 ppmat) {
	auto i = 0;
	Triangle outTri;
// 	for (auto tri : mesh.triangles) {
// 		transformViewportToHocCoordinate(tri, outTri, ppmat);
// 		tri.copyValues(outTri);
// 	}
	mesh.funcToEachTriangle([&outTri, &ppmat, &mesh](Triangle* tri) {
		transformViewportToHocCoordinate(*tri, outTri, ppmat);
		tri->copyValues(outTri);
	});
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
		Triangle* tri = &hocedMesh.triangles[i];
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
bool filterVisibleTriangles(Mesh mesh, Camera camera, Mesh & outputMesh) {
	//outputMesh.triangles.resize(mesh.triangles.size() / 2);
	mesh.funcToEachTriangle([&outputMesh, &camera](Triangle* tri) {
		auto unitedNormalVec = tri->getIdentityNormalVector();
		vec3d cameraToTriangleUnitVec = getUnitVec(tri->points[0] - camera._position);
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

	std::vector<float> grayScale;
	mesh.funcToEachTriangle([&grayScale](Triangle *tri) {
		grayScale.push_back(tri->grayScale);
	});

	return true;
}

bool transformWorldCoordinateToViewCoordinate(Mesh & mesh, Camera camera) {
	Mat4x4 worldToViewMat = camera.getMatWorldToCamera();
	point temp;
	mesh.funcToEachPoint([&temp, &worldToViewMat](point* pt) {
		MultiplyMatrixVector(*pt, temp, worldToViewMat);
		*pt = temp;
	});
	return true;
}

point getIntersectionInLineWithPlane(Plane plane, Line line) {
	float multiplesInLine = -1.0 * (
		getDotProduct(line.onePointInLine - plane.onePointInPanel, plane.normalVec) /
		getDotProduct(line.directVec, plane.normalVec)
		);
	return line.getThePointByMultiples(multiplesInLine);
}


int getTriangleCutByPlane(Triangle triangle, Plane plane, Triangle rstTriangles[2]) {
	point inTris[3];
	point outTris[3];
	point* triArrays[2] = { inTris , outTris };
	int triArrayLen[] = { 0, 0 };

	bool isOutsideArr[3];
	int idx = 0;
	for (auto pt : triangle.points) {
		bool isOutside = plane.isOutside(pt);
		triArrays[isOutside][triArrayLen[isOutside]] = pt;
		++triArrayLen[isOutside];
		isOutsideArr[idx++] = isOutside;
	}

	// 当2和13不在同一边时，13的顺序是反的，要弄成31，否则分割的三角形会逆序
	// if (isOutsideArr[2] == isOutsideArr[0] && isOutsideArr[2] != isOutsideArr[1]) {
	// 	
	// 	auto temp = triArrays[isOutsideArr[2]][1];
	// 	triArrays[isOutsideArr[2]][1] = triArrays[isOutsideArr[2]][0];
	// 	triArrays[isOutsideArr[2]][0] = temp;
	// }
	bool isRepeatReset = isOutsideArr[2] == isOutsideArr[0] && isOutsideArr[2] != isOutsideArr[1];
	// isRepeatReset = true;
	bool outlinePointArr = true;
	switch (triArrayLen[outlinePointArr])
	{
	case 0:
		rstTriangles[0] = triangle;
		return 1;
		break;
	case 1:
		rstTriangles[0] = { inTris[!isRepeatReset],
			getIntersectionInLineWithPlane(plane, getLineByTwoPoint(inTris[!isRepeatReset], outTris[0])),
				inTris[isRepeatReset], triangle.grayScale};
		rstTriangles[1] = {
			inTris[isRepeatReset],
			rstTriangles[0].points[1],
			getIntersectionInLineWithPlane(plane, getLineByTwoPoint(inTris[isRepeatReset], outTris[0])),
			triangle.grayScale
		};
		// // 没有2/13的情况
		// rstTriangles[0] = { 
		// 	inTris[0], 
		// 	inTris[1],
		// 	getIntersectionInLineWithPlane(plane, getLineByTwoPoint(inTris[1], outTris[0])),
		// 	triangle.grayScale
		// };
		// rstTriangles[1] = {
		// 	inTris[1],
		// 	rstTriangles[0].points[2],
		// 	getIntersectionInLineWithPlane(plane, getLineByTwoPoint(inTris[0], outTris[0])),
		// 	triangle.grayScale
		// };
		// // rstTriangles[0] = { 
		// // 	inTris[isRepeatReset],
		// // 	inTris[!isRepeatReset],
		// // 	getIntersectionInLineWithPlane(plane, getLineByTwoPoint(inTris[!isRepeatReset], outTris[0])),
		// // 	triangle.grayScale
		// // };
		// // rstTriangles[1] = {
		// // 	inTris[!isRepeatReset],
		// // 	rstTriangles[isRepeatReset].points[2],
		// // 	getIntersectionInLineWithPlane(plane, getLineByTwoPoint(inTris[isRepeatReset], outTris[0])),
		// // 	triangle.grayScale
		// // };
		return 2;
		break;
	case 2:
		rstTriangles[0] = { inTris[0],
			getIntersectionInLineWithPlane(plane, getLineByTwoPoint(inTris[0], outTris[isRepeatReset])),
			getIntersectionInLineWithPlane(plane, getLineByTwoPoint(inTris[0], outTris[!isRepeatReset])),
			triangle.grayScale
		};
		// // 没有2/13的情况
		// rstTriangles[0] = { 
		// 	inTris[0],
		// 	getIntersectionInLineWithPlane(plane, getLineByTwoPoint(inTris[0], outTris[0])),
		// 	getIntersectionInLineWithPlane(plane, getLineByTwoPoint(inTris[0], outTris[1])), 
		// 	triangle.grayScale
		// };
		// // rstTriangles[0] = { 
		// // 	inTris[0],
		// // 	getIntersectionInLineWithPlane(plane, getLineByTwoPoint(inTris[0], outTris[isRepeatReset])),
		// // 	getIntersectionInLineWithPlane(plane, getLineByTwoPoint(inTris[0], outTris[!isRepeatReset])),
		// // 	triangle.grayScale
		// // };
		return 1;
		break;
	case 3:
		return 0;
		break;
	default:
		break;
	}

}


Mesh getClipedMesh(Mesh mesh, PPMatParmas ppMatParams) {
	Mesh afterClipedMesh;
	Triangle cutTriRst[2];
	


	// 筛选近平面
	Plane nearPlane = ppMatParams.getZNearPlane();
	mesh.funcToEachTriangle([&](Triangle* tri) {
		int afterCutCount = getTriangleCutByPlane(*tri, nearPlane, cutTriRst);
		for (int i = 0; i < afterCutCount; ++i) {
			afterClipedMesh.triangles.push_back(cutTriRst[i]);
		}
	});
	


	Plane aroundPanel[4];

	ppMatParams.getAroundPlane(aroundPanel);



	for (int i = 0; i < 4; ++i) {
		mesh = afterClipedMesh;
		afterClipedMesh.triangles.clear();
		mesh.funcToEachTriangle([&](Triangle * tri) {
			int afterCutCount = getTriangleCutByPlane(*tri, aroundPanel[i], cutTriRst);
			for (int j = 0; j < afterCutCount; ++j) {
				afterClipedMesh.triangles.push_back(cutTriRst[j]);
			}
		});

		// vec3d aaa[10];
		// 
		// for (int k = 0; k < afterClipedMesh.triangles.size(); ++k) {
		// 	aaa[k] = afterClipedMesh.triangles[k].getIdentityNormalVector();
		// }
		// std::cout << "666" << std::endl;

	}



	// 筛选上边
	// 筛选左边
	// 筛选下边
	// 筛选右边
	


	return afterClipedMesh;
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

		_box.loadFromObjFile("mountains.obj"); 

		_ppmatParmas = { 1000, 0.1, 90, (float)ScreenWidth() / (float)ScreenHeight() };

		_ppmat = _ppmatParmas.getPPMat();

/*		_camera = { 0,0,0 };*/
		
		_directLight = { 0, 0.2, -1 };
		
		//doChangeInit(_box);
		
		return true;
		
	};

	bool OnUserUpdate(float fElapsedTime) override {

		Fill(0, 0, ScreenWidth(), ScreenHeight(), PIXEL_SOLID, FG_BLACK);
		checkInput();
		doChange(this->_box, fElapsedTime);
		//temp = this->_box;
		
		
		
		Mesh finalDrawMesh;
		filterVisibleTriangles(this->_box, this->_camera, finalDrawMesh);
		transformWorldCoordinateToViewCoordinate(finalDrawMesh, this->_camera);
		finalDrawMesh = getClipedMesh(finalDrawMesh, this->_ppmatParmas);
		lightColorTriangles(finalDrawMesh, this->_directLight);
		transformViewportToHocCoordinate(finalDrawMesh, _ppmat);
		scaleHocCordinateToScreen(finalDrawMesh, ScreenWidth(), ScreenHeight());
// 		finalDrawMesh.triangles[0].points[0] = { 128.000000,	138.014343, 0.990099072 };
// 		finalDrawMesh.triangles[0].points[1] = { 140.009552,	132.009552, 0.990099072 };
// 		finalDrawMesh.triangles[0].points[2] = { 128.000000,	144.019119, 0.990099072 };

		drawMesh(finalDrawMesh);

// 		Mesh testMesh;
// 		Triangle testTriangle;
// 		testTriangle.grayScale = 0.8;
// 		testTriangle.points[0] = { 0,0,0 };
// 		testTriangle.points[1] = { 0,0.5,0 };
// 		testTriangle.points[2] = { -1,1,0 };
// 		testMesh.triangles.push_back(testTriangle);
// 		scaleHocCordinateToScreen(testMesh, ScreenWidth(), ScreenHeight());
// 		testMesh.triangles[0].points[0] = { 128.000000,	138.014343, 0.990099072};
// 		testMesh.triangles[0].points[1] = { 140.009552,	132.009552, 0.990099072};
// 		testMesh.triangles[0].points[2] = { 128.000000,	144.019119, 0.990099072};
// 		drawMesh(testMesh);
		return true;
	};

private:
	Mesh _box;
	PPMatParmas _ppmatParmas;
	Mat4x4 _ppmat;
	Camera _camera;
	vec3d _directLight;
	float _speedMove = 1;
	float _speedRotate = 1;

	bool drawMesh(Mesh mesh) {
		mesh.sortTriangles();
		// std::vector<Triangle> testTri;
		// for (int i = 0; i < 3; ++i) {
		// 	if (i+1 > mesh.triangles.size()) {
		// 		break;
		// 	}
		// 	testTri.push_back(mesh.triangles[i]);
		// }
		
		for (auto tri : mesh.triangles) {
		//for (auto tri : mesh.triangles) {
			
			// grayScale == 1时变为纯黑，上层bug
			FillTriangleWithGray(
				tri.points[0].x, tri.points[0].y,
				tri.points[1].x, tri.points[1].y,
				tri.points[2].x, tri.points[2].y,
				tri.grayScale*0.9
			);
			// DrawTriangle(
			// 	tri.points[0].x, tri.points[0].y,
			// 	tri.points[1].x, tri.points[1].y,
			// 	tri.points[2].x, tri.points[2].y,
			// 	PIXEL_SOLID, FG_BLACK
			// );
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

	// void FillUpAngleTriangle(
	// 	int xUp, int yUp, 
	// 	int xDown1, int xDown2, int yDown

	// 	) {

	// }

	// void FillDownAngleTriangle(
	// 	int xUp1, int xUp2, int x
	// ) {

	// }

	// void FillTriangleWithSprite(
	// 		int x1, int y1, int u1, int v1, int w1,
	// 		int x2, int y2, int u2, int v2, int w2,
	// 		int x3, int y3, int u3, int v3, int w3,
	// 		olcSprite sprite
	// 	) {
			


	// }
		
	bool doChangeInit(Mesh & mesh) {
		Mat4x4 finalMat = getIdentityMat();
		finalMat = MultiplyMat4x4WithMat4x4(finalMat, getMoveMat(0, -2, 5));
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

	bool checkInput() {
		if (GetKey(L'W').bHeld) {
			_camera.moveForward(_speedMove);
		} else if (GetKey(L'S').bHeld) {
			_camera.moveBackward(_speedMove);
		}

		if (GetKey(L'A').bHeld) {
			_camera.moveLeft(_speedMove);
		}
		else if (GetKey(L'D').bHeld) {
			_camera.moveRight(_speedMove);
		}

		if (GetKey(VK_UP).bHeld) {
			_camera.moveUp(_speedMove);
		}
		else if (GetKey(VK_DOWN).bHeld) {
			_camera.moveDown(_speedMove);
		}

		if (GetKey(VK_LEFT).bHeld) {
			_camera.rotateLeft(_speedRotate);
		}
		else if (GetKey(VK_RIGHT).bHeld) {
			_camera.rotateRight(_speedRotate);
		}

		return true;
	}

	bool doChange(Mesh & mesh, float deltaTime) {
		return false;
		Mat4x4 finalMat = getIdentityMat();
		finalMat = MultiplyMat4x4WithMat4x4(finalMat, getMoveMat(0,2,-5));
		finalMat = MultiplyMat4x4WithMat4x4(finalMat, getRotateYMat(6));
 		finalMat = MultiplyMat4x4WithMat4x4(finalMat, getMoveMat(0, -2, 5));
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
	if (re3d.ConstructConsole(150, 128, 4, 4)) {
		re3d.Start();
	}

    //std::cout << "Hello World!\n"; 
}
