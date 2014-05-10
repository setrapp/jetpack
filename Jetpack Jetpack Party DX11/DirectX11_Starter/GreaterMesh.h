//#ifndef _GMESH_H_
//#define _GMESH_H_
//#pragma once
//
//#include <DirectXMath.h>
//#include <d3d11.h>
//#include "Common.h"
//#include <vector>
//
//class GMesh
//{	
//public:
//	GMesh();
//	GMesh(const GMesh& dxc);
//	~GMesh();
//
//	GMesh& operator=(GMesh other);
//
//	// Static Instance for accessing everywhere.
//	static GMesh* Instance();
//
//	// DirectX related elements that need wide access.
//	ID3D11Buffer* indexBuffer;
//	ID3D11Buffer* vertexBuffer;
//
//	static void SaveMeshData(Vertex* v, UINT* i, int noOfIndices, int noOfVertices);
//	static void FinalizeData();
//	static void BufferedDraw();
//private:
//	vector<Vertex> vertices;
//	vector<UINT> indices;
//	long totalVertices;
//	long totalIndices;
//	static GMesh* instance;
//};
//#endif