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

	soundManager = new SoundManager();
	inputLayouts = new map<ID3D11VertexShader*, ID3D11InputLayout*>();
	vertexShaders = new map<string, ID3D11VertexShader*>();
	pixelShaders = new map<string, ID3D11PixelShader*>();
	materials = new map<string, Material*>();
	models = new map<string, Model*>();
}

AssetManager::~AssetManager()
{
	if (instance == this)
	{
		instance = NULL;
	}

	for(map<ID3D11VertexShader*, ID3D11InputLayout*>::iterator it = inputLayouts->begin(); it != inputLayouts->end(); it++)
	{
		ReleaseMacro(it->second);
	}
	delete inputLayouts;

	for(map<string, ID3D11VertexShader*>::iterator it = vertexShaders->begin(); it != vertexShaders->end(); it++)
	{
		ReleaseMacro(it->second);
	}
	delete vertexShaders;

	for(map<string, ID3D11PixelShader*>::iterator it = pixelShaders->begin(); it != pixelShaders->end(); it++)
	{
		ReleaseMacro(it->second);
	}
	delete pixelShaders;

	for(map<string, Material*>::iterator it = materials->begin(); it != materials->end(); it++)
	{
		delete it->second;
	}
	delete materials;

	for(map<string, Model*>::iterator it = models->begin(); it != models->end(); it++)
	{
		delete it->second;
	}
	delete models;

	delete soundManager;
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

	int a = vsBlob->GetBufferSize();

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


// Model
Model* AssetManager::CreateAndStoreModel(string filePath, string name)
{
	Model* model = new Model();

	MLModel3D* objModel = mlModel3DLoadOBJ(filePath.c_str());
	
	bool hasUVs = mlModel3DGetTextureVertexCount(objModel) > 1;

	// Load Vertices.
	unsigned int vertexCount = mlModel3DGetFaceCount(objModel);
	for (int i = 0; i < vertexCount; i++) {
		MLVertex3D const*  mlVertex = mlModel3DGetVertex(objModel, i);
		GUPoint3D guPoint = mlVertex3DGetPosition(mlVertex);
		GUNormal3D guNormal = mlVertex3DGetNormal(mlVertex);
		Vertex vertex;
		vertex.Position = XMFLOAT3(guPoint.x, guPoint.y, guPoint.z);
		vertex.Normal = XMFLOAT3(guNormal.x, guNormal.y, guNormal.z);
		if (hasUVs) {
			MLTexelXY const* mlTexel = mlModel3DGetTextureVertex(objModel, i);
			GUPoint2D guUV = mlTexelXYGetPosition(mlTexel);
			vertex.UV = XMFLOAT2(guUV.x, guUV.y);
		} else {
			vertex.UV = XMFLOAT2(0, 0);
		}
		model->vertices.push_back(vertex);
	}

	// Load Mesh Indices.
	unsigned int faceCount = mlModel3DGetFaceCount(objModel);
	for (int i = 0; i < faceCount; i++) {
		// Retrieve current face.
		MLFace3D const* face = mlModel3DGetFace(objModel, i);

		// Retrieve vertices that make up current face.
		UINT mlIndices[3];
		mlIndices[0] = mlFace3DGetVertex1(face);
		mlIndices[1] = mlFace3DGetVertex2(face);
		mlIndices[2] = mlFace3DGetVertex3(face);

		model->meshes.push_back(Mesh(mlIndices));
	}

	MLModel3DDelete(objModel);

	return StoreModel(model, name);
}
Model* AssetManager::StoreModel(Model* model, string name)
{
	// Attempt to add new element.
	pair<map<string, Model*>::iterator, bool> existing;
	pair<string, Model*> newModel(name, model);
	existing = models->insert(newModel);
	
	// If the first attempt failed, destroy the element that is colliding and replace it.
	if(!existing.second)
	{
		models->erase(existing.first);
		models->insert(newModel);
	}	
	return model;
}
Model* AssetManager::GetModel(string name)
{
	Model* model = NULL;
	map<string, Model*>::iterator modelIt = models->find(name);
	if(modelIt != models->end()) {
		model = modelIt->second;
	}
	return model;
}

SoundManager* AssetManager::GetSoundManager()
{
	return soundManager;
}

	