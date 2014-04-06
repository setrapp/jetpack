#include "AssetManager.h"

AssetManager* AssetManager::instance = NULL;
AssetManager* AssetManager::Instance() 
{
	return instance;
}

/* TODO Replacing existing elements in store functions does not actually delete the old elements, need to do so */
/* TODO Destructor also does not delete elements */

AssetManager::AssetManager()
{
	if (!instance) {
		instance = this;
	}

	inputLayouts = new map<ID3D11VertexShader*, ID3D11InputLayout*>();
	vertexShaders = new map<string, ID3D11VertexShader*>();
	pixelShaders = new map<string, ID3D11PixelShader*>();
	materials = new map<string, Material*>();
	meshes = new map<string, vector<Mesh*>*>();
}

AssetManager::~AssetManager()
{
	if (instance == this)
	{
		instance = NULL;
	}

	// TODO: Actually free the memory of elements stored in vectors (erase only destroys the pointer)
	inputLayouts->erase(inputLayouts->begin(), inputLayouts->end());
	delete inputLayouts;
	vertexShaders->erase(vertexShaders->begin(), vertexShaders->end());
	delete vertexShaders;
	pixelShaders->erase(pixelShaders->begin(), pixelShaders->end());
	delete pixelShaders;
	materials->erase(materials->begin(), materials->end());
	delete materials;
}
	
// Input Layouts
ID3D11InputLayout* AssetManager::CreateAndStoreInputLayout(ID3D11VertexShader* vertexShader, ID3DBlob* vsBlob, D3D11_INPUT_ELEMENT_DESC* vertexDescription, int numVertDescElements)
{
	ID3D11InputLayout* inputLayout;
	HR(DXConnection::Instance()->device->CreateInputLayout(
		vertexDescription,
		numVertDescElements,
		vsBlob->GetBufferPointer(),
		vsBlob->GetBufferSize(),
		&inputLayout));
	
	// Attempt to add new element.
	pair<map<ID3D11VertexShader*, ID3D11InputLayout*>::iterator, bool> existing;
	pair<ID3D11VertexShader*, ID3D11InputLayout*> newLayout(vertexShader, inputLayout);
	existing = inputLayouts->insert(newLayout);
	
	// If the first attempt failed, destroy the element that is colliding and replace it.
	if(!existing.second)
	{
		inputLayouts->erase(existing.first);
		inputLayouts->insert(newLayout);
	}

	return inputLayout;
}
ID3D11InputLayout* AssetManager::GetInputLayout(ID3D11VertexShader* vertexShader)
{
	ID3D11InputLayout* inputLayout = NULL;
	map<ID3D11VertexShader*, ID3D11InputLayout*>::iterator layoutIt = inputLayouts->find(vertexShader);
	if(layoutIt != inputLayouts->end()) {
		inputLayout = layoutIt->second;
	}
	return inputLayout;
}

// Vertex Shaders
ID3D11VertexShader* AssetManager::CreateAndStoreVertexShader(string shaderPath, D3D11_INPUT_ELEMENT_DESC* vertexDescription, int numVertDescElements, ID3D11InputLayout** inputLayout, string name)
{
	// Copy shader path to wide character string.
	wchar_t* widePath = new wchar_t[shaderPath.length() + 1];
	copy(shaderPath.begin(), shaderPath.end(), widePath);
	widePath[shaderPath.length()] = '\0';

	// Create the shader on the device.
	ID3DBlob* vsBlob;
	D3DReadFileToBlob(widePath, &vsBlob);
	ID3D11VertexShader* vertexShader;
	HR(DXConnection::Instance()->device->CreateVertexShader(
		vsBlob->GetBufferPointer(),
		vsBlob->GetBufferSize(),
		NULL,
		&vertexShader));

	// Attempt to add new element.
	pair<map<string, ID3D11VertexShader*>::iterator, bool> existing;
	pair<string, ID3D11VertexShader*> newShader(name, vertexShader);
	existing = vertexShaders->insert(newShader);
	
	// If the first attempt failed, destroy the element that is colliding and replace it.
	if(!existing.second)
	{
		vertexShaders->erase(existing.first);
		vertexShaders->insert(newShader);
	}

	// Before cleaning up the data, create the input layout
	ID3D11InputLayout* newInputLayout = CreateAndStoreInputLayout(vertexShader, vsBlob, vertexDescription, numVertDescElements);
	if (inputLayout) {
		*inputLayout = newInputLayout;
	}

	ReleaseMacro(vsBlob);
	delete widePath;

	return vertexShader;
}
ID3D11VertexShader* AssetManager::GetVertexShader(string name)
{
	ID3D11VertexShader* shader = NULL;
	map<string, ID3D11VertexShader*>::iterator shaderIt = vertexShaders->find(name);
	if(shaderIt != vertexShaders->end()) {
		shader = shaderIt->second;
	}
	return shader;
}

// Pixel Shaders
ID3D11PixelShader* AssetManager::CreateAndStorePixelShader(string shaderPath, string name)
{
	// Copy shader path to wide character string.
	wchar_t* widePath = new wchar_t[shaderPath.length() + 1];
	copy(shaderPath.begin(), shaderPath.end(), widePath);
	widePath[shaderPath.length()] = '\0';
	
	// Create the shader on the device.
	ID3DBlob* psBlob;
	D3DReadFileToBlob(widePath, &psBlob);
	ID3D11PixelShader* pixelShader;
	HR(DXConnection::Instance()->device->CreatePixelShader(
		psBlob->GetBufferPointer(),
		psBlob->GetBufferSize(),
		NULL,
		&pixelShader));

	// Attempt to add new element.
	pair<map<string, ID3D11PixelShader*>::iterator, bool> existing;
	pair<string, ID3D11PixelShader*> newShader(name, pixelShader);
	existing = pixelShaders->insert(newShader);
	
	// If the first attempt failed, destroy the element that is colliding and replace it.
	if(!existing.second)
	{
		pixelShaders->erase(existing.first);
		pixelShaders->insert(newShader);
	}

	ReleaseMacro(psBlob);
	delete widePath;

	return pixelShader;
}
ID3D11PixelShader* AssetManager::GetPixelShader(string name)
{
	ID3D11PixelShader* shader = NULL;
	map<string, ID3D11PixelShader*>::iterator shaderIt = pixelShaders->find(name);
	if(shaderIt != pixelShaders->end()) {
		shader = shaderIt->second;
	}
	return shader;
}

// Materials
Material* AssetManager::StoreMaterial(Material* material, string name)
{
	// Attempt to add new element.
	pair<map<string, Material*>::iterator, bool> existing;
	pair<string, Material*> newMaterial(name, material);
	existing = materials->insert(newMaterial);
	
	// If the first attempt failed, destroy the element that is colliding and replace it.
	if(!existing.second)
	{
		materials->erase(existing.first);
		materials->insert(newMaterial);
	}

	return material;
}
Material* AssetManager::GetMaterial(string name)
{
	Material* material = NULL;
	map<string, Material*>::iterator materialIt = materials->find(name);
	if(materialIt != materials->end()) {
		material = materialIt->second;
	}
	return material;
}


// Meshes
vector<Mesh*>* AssetManager::CreateAndStoreMesh(string filePath, string name)
{
	vector<Mesh*>* mesh = new vector<Mesh*>();

	MLModel3D* model = mlModel3DLoadOBJ(filePath.c_str());
	bool hasUVs = mlModel3DGetTextureVertexCount(model) > 1;
	unsigned int faceCount = mlModel3DGetFaceCount(model);
	for (int i = 0; i < faceCount; i++) {
		// Retrieve current face.
		MLFace3D const* face = mlModel3DGetFace(model, i);

		// Retrieve vertices that make up current face.
		unsigned short mlIndex;
		MLVertex3D const* mlVertex;
		MLTexelXY const* mlTexel;
		GUPoint3D guPoint;
		GUNormal3D guNormal;// = mlVertex3DGetNormal(mlVertex);
		GUPoint2D guUV;
		// Vertex 1
		mlIndex = mlFace3DGetVertex1(face);
		mlVertex = mlModel3DGetVertex(model, mlIndex);
		guPoint = mlVertex3DGetPosition(mlVertex);
		guNormal = mlVertex3DGetNormal(mlVertex);
		Vertex vertex1;
		vertex1.Position = XMFLOAT3(guPoint.x, guPoint.y, guPoint.z);
		vertex1.Normal = XMFLOAT3(guNormal.x, guNormal.y, guNormal.z);
		vertex1.Color = XMFLOAT4(1, 0, 0, 1);
		if (hasUVs) {
			MLTexelXY const* mlTexel = mlModel3DGetTextureVertex(model, mlIndex);
			GUPoint2D guUV = mlTexelXYGetPosition(mlTexel);
			vertex1.UV = XMFLOAT2(guUV.x, guUV.y);
		} else {
			vertex1.UV = XMFLOAT2(0, 0);
		}
		// Vertex 2
		mlIndex = mlFace3DGetVertex2(face);
		mlVertex = mlModel3DGetVertex(model, mlIndex);
		guPoint = mlVertex3DGetPosition(mlVertex);
		guNormal = mlVertex3DGetNormal(mlVertex);
		Vertex vertex2;
		vertex2.Position = XMFLOAT3(guPoint.x, guPoint.y, guPoint.z);
		vertex2.Normal = XMFLOAT3(guNormal.x, guNormal.y, guNormal.z);
		vertex2.Color = XMFLOAT4(1, 0, 0, 1);
		if (hasUVs) {
			MLTexelXY const* mlTexel = mlModel3DGetTextureVertex(model, mlIndex);
			GUPoint2D guUV = mlTexelXYGetPosition(mlTexel);
			vertex2.UV = XMFLOAT2(guUV.x, guUV.y);
		} else {
			vertex2.UV = XMFLOAT2(0, 0);
		}
		// Vertex 3
		mlIndex = mlFace3DGetVertex3(face);
		mlVertex = mlModel3DGetVertex(model, mlIndex);
		guPoint = mlVertex3DGetPosition(mlVertex);
		guNormal = mlVertex3DGetNormal(mlVertex);
		Vertex vertex3;
		vertex3.Position = XMFLOAT3(guPoint.x, guPoint.y, guPoint.z);
		vertex3.Normal = XMFLOAT3(guNormal.x, guNormal.y, guNormal.z);
		vertex3.Color = XMFLOAT4(1, 0, 0, 1);
		vertex3.UV = XMFLOAT2(0,0);
		if (hasUVs) {
			MLTexelXY const* mlTexel = mlModel3DGetTextureVertex(model, mlIndex);
			GUPoint2D guUV = mlTexelXYGetPosition(mlTexel);
			vertex3.UV = XMFLOAT2(guUV.x, guUV.y);
		} else {
			vertex3.UV = XMFLOAT2(0, 0);
		}

		// Create usable mesh.
		Vertex vertices[] = {vertex1, vertex2, vertex3};
		UINT indices[] = {0, 1, 2};
		mesh->push_back(new Mesh(vertices, indices, 3, 3));
	}

	return StoreMesh(mesh, name);
}
vector<Mesh*>* AssetManager::StoreMesh(vector<Mesh*>* mesh, string name)
{
	// Attempt to add new element.
	pair<map<string, vector<Mesh*>*>::iterator, bool> existing;
	pair<string, vector<Mesh*>*> newMaterial(name, mesh);
	existing = meshes->insert(newMaterial);
	
	// If the first attempt failed, destroy the element that is colliding and replace it.
	if(!existing.second)
	{
		meshes->erase(existing.first);
		meshes->insert(newMaterial);
	}	
	return mesh;
}
vector<Mesh*>* AssetManager::GetMesh(string name)
{
	vector<Mesh*>* mesh = NULL;
	map<string, vector<Mesh*>*>::iterator meshIt = meshes->find(name);
	if(meshIt != meshes->end()) {
		mesh = meshIt->second;
	}
	return mesh;
}

	