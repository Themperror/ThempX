#include "../Headers/ResourceManager.h"

ResourceManager::ResourceManager(LPDIRECT3DDEVICE9 d3d_Device, HWND handle)
{
	p_Device = d3d_Device;
	wHandle = handle;
}
void ResourceManager::ReleaseResources()	//main data release			  //Ben ik weer, lagged out
{
	for(unsigned int i=0;i<models.size();i++)
	{
		models.at(i).materialBuffer->Release();
		models.at(i).mesh->Release();
		for(unsigned int x=0; x < models.at(i).numMaterials;x++)
		{
			if(models.at(i).meshTextures != NULL)
			{
				models.at(i).meshTextures[x]->Release();  //dit is dus de main data houder maar dat zou voor reloaden van het level niet voor extra data moeten zorgen 
				//want dit wordt 1x bij elke texture/model inladen 1x uitgevoerd zodat hij de model/texture heeft, daarna hergebruikt ie em heletijd
			}
		}
	}
	for(unsigned int i=0;i<quads.size();i++)
	{
		quads.at(i).texture->Release();
	}
}
bool ResourceManager::CheckAvailableTexture(char* name)
{
	for(unsigned int i = 0; i < quads.size(); i++)
	{
		if(strcmp(quads.at(i).textureName,name)== 0)
		{
			return true;
		}
	}
	return false;
}
LPDIRECT3DTEXTURE9 ResourceManager::GetTexture(char* name)
{
	if(CheckAvailableTexture(name))
	{
		for(unsigned int  i =0;i< quads.size();i++)
		{
			if(strcmp(quads.at(i).textureName,name)== 0)
			{
				//std::cout << "texture found and reused" << std::endl;
				return quads.at(i).texture;
			}
		}
	}
	else
	{
		Quad quad;
		quad.textureName = name;
		quad.texture = NULL;
		HRESULT result = D3DXCreateTextureFromFile(p_Device, name,&quad.texture);
		switch(result)
		{
		case D3DERR_NOTAVAILABLE: std::cout << "Texture not found/available" << std::endl; break;
		case D3DERR_OUTOFVIDEOMEMORY:  std::cout << "Out of Video Memory" << std::endl; break;
		case D3DERR_INVALIDCALL: std::cout << "Invalid Call (Is the path correct? is it a string? does it point to a png or jpg?)" << std::endl;  break;
		case D3DXERR_INVALIDDATA:  std::cout << "Invalid Data" << std::endl; break;
		case E_OUTOFMEMORY:  std::cout << "System out of memory" << std::endl;  break; 
		}
		if(quad.texture != NULL)
		{
			std::cout << "texture NOT found and was created" << std::endl; 
			quads.push_back(quad);
			return quad.texture;
		}
		else
		{
			MessageBox(wHandle,"Texture was not found", "ResourceManager::GetTexture()",MB_OK);
			return NULL;
		}
	} 

	//should be impossible to come here
	MessageBox(wHandle,"No texture returned, the program will probably crash now.","GetTexture()",MB_OK);
	return NULL;
}
int ResourceManager::GetMeshData(char* name)
{
	if(CheckAvailableModel(name))
	{
		for(unsigned int i = 0; i < models.size(); i++)
		{
			if(strcmp(models.at(i).meshName ,name) == 0)
			{
				std::cout << "model found and reused  "<<std::endl;
				return i;
			}
		}
	}
	else
	{
		Model model;
		HRESULT result = D3DXLoadMeshFromX(name, D3DXMESH_SYSTEMMEM,p_Device, NULL, &model.materialBuffer,NULL, &model.numMaterials,  &model.mesh );
		switch(result)
		{
			case D3DERR_INVALIDCALL: std::cout << "invalid call" << std::endl; break;
			case E_OUTOFMEMORY: std::cout << "out of memory" << std::endl;break;
		}
		model.d3dxMaterials = (D3DXMATERIAL*)model.materialBuffer->GetBufferPointer();
		model.meshMaterials = new D3DMATERIAL9[model.numMaterials];
		model.meshTextures  = new LPDIRECT3DTEXTURE9[model.numMaterials];   
		// Filling material and texture arrays
		for (DWORD i=0; i<model.numMaterials; i++)
		{
			// Copy the material
			model.meshMaterials[i] = model.d3dxMaterials[i].MatD3D;

			// Set the ambient color for the material (D3DX does not do this)
			model.meshMaterials[i].Ambient = model.meshMaterials[i].Diffuse;

			// Create the texture if it exists - it may not
			model.meshTextures[i] = NULL;
			if (model.d3dxMaterials[i].pTextureFilename)
			{
				D3DXCreateTextureFromFile(p_Device, model.d3dxMaterials[i].pTextureFilename, &model.meshTextures[i]);
			}
		}
		model.meshName = name;
		models.push_back(model);

		std::cout << "pre-created model is NOT found and is created" << std::endl;
		return models.size()-1;
	}
	MessageBox(wHandle,"No mesh returned, the program will probably crash now.","GetMesh()",MB_OK);
	return false;
}
bool ResourceManager::CheckAvailableModel(char* name)
{
	for(unsigned int i = 0; i < models.size(); i++)
	{
		if(strcmp(models.at(i).meshName,name)== 0)
		{
			return true;
		}
	}
	return false;
}