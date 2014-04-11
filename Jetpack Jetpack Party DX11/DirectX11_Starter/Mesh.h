#define BUFFERED_STUFF
#ifndef MESH_H_
#define MESH_H_
#pragma once

#include "Vertex.h"
#include <DirectXMath.h>
#include <d3d11.h>
#include "Common.h"

class Mesh
{
public:
	Mesh(UINT* i, short noOfIndices);
	~Mesh(void);
	void Update(float dt);
	void Draw();
	UINT* GetIndices(short **total);

private:	
	short totalIndices;
	UINT* indices;
};

#endif