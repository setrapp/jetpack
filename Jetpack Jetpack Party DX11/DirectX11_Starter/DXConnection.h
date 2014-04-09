#pragma once

#include <d3d11.h>

class DXConnection
{
public:
	DXConnection();
	DXConnection(const DXConnection& dxc);
	~DXConnection();

	DXConnection& operator=(DXConnection other);

	// Static Instance for accessing everywhere.
	static DXConnection* Instance();

	// DirectX related elements that need wide access.
	ID3D11Device* device;
	ID3D11DeviceContext* deviceContext;

private:
	static DXConnection* instance;
};