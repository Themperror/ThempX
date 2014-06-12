#include "../Headers/ResourceManager.h"

ResourceManager::ResourceManager(LPDIRECT3DDEVICE9 d3d_Device, HWND handle)
{
	p_Device = d3d_Device;
	wHandle = handle;
	setData = false;
	setSound = false;
}
int ResourceManager::CreateTextObject(char* font,char* text,int fontsize, int posXPercentage, int posYPercentage, int widthPercentage, int heightPercentage, D3DXCOLOR color)
{
	TextData* obj = new TextData();
	
	obj->textRect.left =		GetScreenWidth()	*	posXPercentage	/ 100;
	obj->textRect.top=		GetScreenHeight()	*	posYPercentage	/ 100;
	obj->textRect.right =	GetScreenWidth()	*	widthPercentage+posXPercentage / 100;
	obj->textRect.bottom =	GetScreenHeight()	*	heightPercentage+posYPercentage/ 100;
	obj->fontsize = fontsize;
	obj->font = font;
	obj->color = color;
	obj->text = text;
	obj->hasLostDone = false;
	obj->render = true;
	if(texts.size() == 0)
	{
		D3DXCreateFont(p_Device,fontsize, 0, 1 ,0, false, DEFAULT_CHARSET, OUT_TT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_PITCH,font,&obj->gameFont);
		texts.push_back(obj);
		return texts.size()-1;
	}
	else if(texts.size() > 0)
	{
		for(unsigned int i = 0; i < texts.size(); i++)
		{
			if(strcmp(font,texts.at(i)->font) == 0 && texts.at(i)->fontsize == obj->fontsize)
			{
				obj->gameFont = texts.at(0)->gameFont;
				texts.push_back(obj);
				return texts.size()-1;
			}
		}
		D3DXCreateFont(p_Device,fontsize, 0, 1 ,0, false, DEFAULT_CHARSET, OUT_TT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_PITCH,font,&obj->gameFont);
		texts.push_back(obj);
		return texts.size()-1;
	}
	return 0;
}
void ResourceManager::Release()	//main data release
{
	for(unsigned int i=0;i<models.size();i++)
	{
		models.at(i).materialBuffer->Release();
		models.at(i).materialBuffer = NULL;
		models.at(i).mesh->Release();
		models.at(i).mesh = NULL;
		models.erase(models.begin()+i);
	}
	models.clear();
	for(unsigned int i=0;i<quads.size();i++)
	{
		if(quads.at(i).texture != NULL)
		{
			quads.at(i).texture->Release();
			quads.at(i).texture = NULL;
			quads.erase(quads.begin()+i);
		}
	}
	quads.clear();
	for(unsigned int i = 0; i < fonts.size(); i++)
	{
		fonts.at(i)->Release();
		fonts.at(i) = NULL;
		fonts.erase(fonts.begin()+i);
	}
	fonts.clear();
	
}
bool ResourceManager::CheckAvailableTexture(std::string tName)
{
	std::string name = LowCaseString(tName);
	for(unsigned int i = 0; i < quads.size(); i++)
	{
		if(strcmp(quads.at(i).textureName,_strdup(name.c_str()))== 0)
		{
			return true;
		}
	}
	return false;
}
LPDIRECT3DTEXTURE9 ResourceManager::GetTexture(std::string tName)
{
	std::string name = LowCaseString(tName);
	if(CheckAvailableTexture(name))
	{
		for(unsigned int  i =0;i< quads.size();i++)
		{
			if(strcmp(quads.at(i).textureName,_strdup(name.c_str()))== 0)
			{
				//std::cout << "texture found and reused" << std::endl;
				return quads.at(i).texture;
			}
		}
	}
	else
	{
		Quad quad;
		quad.textureName = _strdup(name.c_str());
		quad.texture = NULL;
		HRESULT result = D3DXCreateTextureFromFile(p_Device, _strdup(name.c_str()),&quad.texture);
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
			//std::cout << "texture NOT found and was created" << std::endl; 
			quads.push_back(quad);
			return quad.texture;
		}
		else
		{
			std::string text;
			text.append(name +" was not found");
			MessageBox(wHandle,_strdup(text.c_str()), "ResourceManager::GetTexture()",MB_OK);
			return NULL;
		}
	}
	//should be impossible to come here

	MessageBox(wHandle,"No texture returned","ResourceManager::GetTexture()",MB_OK);
	return NULL;
}
int ResourceManager::GetMeshData(std::string mName)
{
	std::string name = LowCaseString(mName);
	if(CheckAvailableModel(name))
	{
		for(unsigned int i = 0; i < models.size(); i++)
		{
			if(strcmp(models.at(i).meshName ,_strdup(name.c_str())) == 0)
			{
				//model was found and will be re-used
				return i;
			}
		}
		return  -1;
	}
	else
	{
		Model model;
		model.mesh = NULL;
		HRESULT result = D3DXLoadMeshFromX(_strdup(name.c_str()), D3DXMESH_SYSTEMMEM,p_Device, NULL, &model.materialBuffer,NULL, &model.numMaterials,  &model.mesh );
		switch(result)
		{
			case D3DERR_INVALIDCALL: std::cout << "invalid call" << std::endl; break;
			case E_OUTOFMEMORY: std::cout << "out of memory" << std::endl;break;
		}
		if(model.mesh == NULL)
		{
			std::string str = "Could not find mesh at path: ";
			str.append(name);
			MessageBox(GetWindowHandle(),str.c_str(),"ResourceManager::GetMeshData()",MB_OK);
			return 0;
		}
		model.d3dxMaterials = (D3DXMATERIAL*)model.materialBuffer->GetBufferPointer();
		model.meshMaterials = new D3DMATERIAL9[model.numMaterials];
		model.meshTextures  = new LPDIRECT3DTEXTURE9[model.numMaterials];   
		ZeroMemory(model.meshTextures,sizeof(model.meshTextures));
		// Filling material and texture arrays
		for (DWORD i=0; i<model.numMaterials; i++)
		{
			model.meshMaterials[i] = model.d3dxMaterials[i].MatD3D;

			model.meshMaterials[i].Ambient = model.meshMaterials[i].Diffuse;

			if(model.d3dxMaterials[i].pTextureFilename != NULL)
			{
				std::string texturePath = model.d3dxMaterials[i].pTextureFilename;
				texturePath = "Resources/Models/"+texturePath;
				model.meshTextures[i] =  GetTexture(_strdup(texturePath.c_str()));
				if(model.meshTextures[i] == NULL)
				{
					std::cout << "Couldn't find texture belonging to model, path is: " << texturePath << std::endl;
				}
			}
		}
		model.meshName = _strdup(name.c_str());
		models.push_back(model);
		return models.size()-1;
	}
	MessageBox(wHandle,"No mesh returned.","ResourceManager::GetMesh()",MB_OK);
	return false;
}
bool ResourceManager::CheckAvailableModel(std::string mName)
{
	std::string name = LowCaseString(mName);
	for(unsigned int i = 0; i < models.size(); i++)
	{
		if(strcmp(models.at(i).meshName,_strdup(name.c_str()))== 0)
		{
			return true;
		}
	}
	return false;
}