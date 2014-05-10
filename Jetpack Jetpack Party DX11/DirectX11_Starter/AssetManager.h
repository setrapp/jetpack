#pragma once

// Disable warning about unsafe iterators (we aren't multi-threading)
#pragma warning(disable:4996)

#include <map>
#include <string>
#include <queue>
#include "Common.h"
#include "Entity.h"
#include "Material.h"
#include "Mesh.h"
#include "ModelLoad\MLModelViewer.h"
#include "InputManager.h"

using namespace std;

class SoundManager;

class AssetManager
{
public:
	AssetManager();
	AssetManager(const AssetManager& am);
	~AssetManager();

	AssetManager& operator= (AssetManager other);

	// Input Layouts
	ID3D11InputLayout* CreateAndStoreInputLayout(ID3D11VertexShader* vertexShader, ID3DBlob* vsBlob, D3D11_INPUT_ELEMENT_DESC* vertexDescription, int numVertDescElements);
	ID3D11InputLayout* GetInputLayout(ID3D11VertexShader* vertexShader);
	
	// Vertex Shaders
	ID3D11VertexShader* CreateAndStoreVertexShader(string shaderPath, D3D11_INPUT_ELEMENT_DESC* vertexDescription, int numVertDescElements, ID3D11InputLayout** inputLayout, string name = "default");
	ID3D11VertexShader* GetVertexShader(string name = "default");

	// Pixel Shaders
	ID3D11PixelShader* CreateAndStorePixelShader(string shaderPath, string name = "default");
	ID3D11PixelShader* GetPixelShader(string name = "default");

	// Textures
	//ID3D11Texture2D StoreTexture2D(wchar_t* path, string name = "default");
	//ID3D11Texture2D GetTexture2D(string name = "default");

	// Materials
	Material* StoreMaterial(Material* material, string name = "default", Model* sourceModel = NULL);
	Material* GetMaterial(string name = "default", Model* sourceModel = NULL);

	// Models
	Model* CreateAndStoreModel(string filePath, string name = "default");
	Model* StoreModel(Model* model, string name = "default");
	Model* GetModel(string name = "default");

	// Mesh Groups
	vector<MeshGroup*>* GetMeshGroupsWithMaterial(vector<MeshGroup*>* meshGroupsOut, Model* sourceModel, string desiredMaterialName = "default");
	Entity* EntifyMeshGroup(Entity* entityOut, Model* sourceModel, MeshGroup* meshGroup);

	SoundManager* GetSoundManager();
	
	static AssetManager* Instance();
	std::queue<Entity*> addedEntities;

private:
	static AssetManager* instance;
	map<ID3D11VertexShader*, ID3D11InputLayout*>* inputLayouts;
	map<string, ID3D11VertexShader*>* vertexShaders;
	map<string, ID3D11PixelShader*>* pixelShaders;
	map<string, ID3D11Texture2D*>* texture2Ds;
	map<string, pair<Material*, Model*>>* materials;
	map<string, Model*>* models;
	SoundManager* soundManager;
};