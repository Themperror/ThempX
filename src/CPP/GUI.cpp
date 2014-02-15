#include "../Headers/GUI.h"

GUI::GUI( LPDIRECT3DDEVICE9 d3dDev, ResourceManager* res)
{
	p_Device = res->GetDevice();
	resources = res;

	LoadGUI();

	D3DXMatrixOrthoLH(&matProj,resources->GetScreenWidth(),resources->GetScreenHeight(),0,1);
}
void GUI::Release()
{
	for(unsigned int  i = 0; i < guiObjs.size();i++)
	{
		guiObjs.at(i).iBuffer->Release();
		guiObjs.at(i).vBuffer->Release();
	}
}
void GUI::LoadGUI()
{
	std::ifstream fin("Resources/guilayout.txt");
	if(!fin.good())
	{
		MessageBox(resources->GetWindowHandle(),"Can't find guilayout.txt, GUI won't show","LoadGUI()",MB_OK);
	}
	else
	{
		std::string str;
		std::string path;
		float posX,posY,sizeX,sizeY;
		int xRows,yRows;
		xRows = 0;
		yRows = 0;
		bool hasAnimation,relativeEndX,relativeEndY;
		while(getline(fin,str))
		{
			fin >> path >> posX >> posY >> sizeX >> sizeY >> xRows >> yRows >> hasAnimation >> relativeEndX >> relativeEndY;
			Rectangle rect;
			rect.Nullify();
			if(!relativeEndX)
			{
				rect.x = posX;
			}
			else
			{
				rect.x = resources->GetWindowWidth()-posX;
				std::cout << "relative posX = " << rect.x << std::endl;
			}
			if(!relativeEndY)
			{
				rect.y = posY;
			}
			else
			{
				rect.y = resources->GetWindowHeight()-posY;
				std::cout << "relative posY = " << rect.y << std::endl;
			}
			rect.w = sizeX;
			rect.h = sizeY;
			if(hasAnimation)
			{
				CreateGUIObject(rect,_strdup(path.c_str()),xRows,yRows);
			}
			else
			{
				std::cout << "should be made" << std::endl;
				CreateGUIObject(rect,_strdup(path.c_str()));
			}
		}
		fin.close();
	}
}
bool GUI::CreateGUIObject(Rectangle rect,char* textureName)
{
	GUITexture GText;
	GText.Nullify();
	GText.textureName = textureName;
	GText.texture = resources->GetTexture(textureName);
	GText.rect = rect;
	GText.hasAnimation = false;
	GText.vBuffer = CreateQuadVBuffer(&GText);
	GText.iBuffer = CreateQuadIndices();
	GText.animationSpeed = 0.25f;
	guiObjs.push_back(GText);
	return true;
}
bool GUI::CreateGUIObject(Rectangle rect,char* textureName, int xRows,int yRows)
{
	GUITexture GText;
	GText.Nullify();
	GText.textureName = textureName;
	GText.texture = resources->GetTexture(textureName);
	GText.rect = rect;
	GText.hasAnimation = true;
	GText.vBuffer = CreateQuadVBuffer(&GText);
	GText.iBuffer = CreateQuadIndices();
	GText.animationSpeed = 0.25f;
	GText.xRows = xRows;
	GText.yRows = yRows;

	LoadAnimation(&GText);
	guiObjs.push_back(GText);
	
	PlayAnimation(&GText,"Walk");
	return true;
}
void GUI::Render()
{
	p_Device->SetTransform(D3DTS_PROJECTION, &matProj);
	
	for(unsigned int i = 0; i < guiObjs.size();i++)
	{
		HRESULT result;
		D3DXMATRIX world;
		
		D3DXMatrixTranslation(&world,0,0,0);
		
		result = p_Device->SetFVF( D3DFVF_XYZRHW | D3DFVF_TEX1);
		result = p_Device->SetStreamSource(0, guiObjs.at(i).vBuffer, 0, sizeof(Vertex2D));
		result = p_Device->SetIndices(guiObjs.at(i).iBuffer);
		result = p_Device->SetTransform(D3DTS_WORLD,&world);
		result = p_Device->SetTexture(0,guiObjs.at(i).texture);

		result = p_Device->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP,0,0,4,0,2);
		switch(result)
		{
			case D3DERR_INVALIDCALL: std::cout << "Invalid Call" << std::endl;
				break;
			case D3DERR_CONFLICTINGRENDERSTATE: std::cout << "Conflicting Renderstate" << std::endl;
				break ;
			case D3DERR_DRIVERINVALIDCALL: std::cout <<"Driver invalid call"<< std::endl;
				break;
			case D3DERR_TOOMANYOPERATIONS : std::cout << "too many operations" << std::endl; 
				break;
		}
		//std::cout << "draw gui" << std::endl;
	}
}
void GUI::Update(float deltaTime)
{
	for(unsigned int i = 0; i < guiObjs.size();i++)
	{
		if(guiObjs.at(i).hasAnimation)
		{
			Animate(&guiObjs.at(i),deltaTime,guiObjs.at(i).animationSpeed);
		}
	}
}
void GUI::Animate(GUITexture* obj , float dTime ,float animSpeed)
{		
	obj->timeSinceChange += dTime;
	if(obj->timeSinceChange > animSpeed)
	{
		Animation* currentAnim = &obj->animations.at(obj->currentAnim);
		obj->timeSinceChange = 0;
		HRESULT result;
		BYTE* ptr;
		result = obj->vBuffer->Lock(0,sizeof(Vertex2D)*4,(LPVOID*)&ptr,NULL);
		if(result != D3D_OK)
		{
			MessageBox(resources->GetWindowHandle(),"Failed to lock vertexBuffer of quad","Animate()",MB_OK);
		}
		// loop through the vertices
		float toAdd = 0;
		float stepSizeX = 0;
		float stepSizeY = 0;
		D3DXVECTOR2 UVs;
		D3DXVECTOR2 UVs1;
		D3DXVECTOR2 UVs2;
		D3DXVECTOR2 UVs3;
		stepSizeX = (1.0f / obj->xRows);
		stepSizeY = (1.0f / obj->yRows);

		for (DWORD i=0;i<4;i++) // hardcoded 4 because 4 vertices
		{
			// get pointer to location
			Vertex2D* vPtr=(Vertex2D*) ptr;
			switch(i)
			{
			case 0:
				vPtr->texC.x = stepSizeX * obj->currentXAnimValue;
				vPtr->texC.y = stepSizeY * obj->currentYAnimValue;
				if(vPtr->texC.x	> 1 - stepSizeX*(obj->xRows-(currentAnim->EndPosition.x-currentAnim->StartPosition.x)))
				{
					vPtr->texC.x = 0;
				}
				if(vPtr->texC.y	> 1 - stepSizeY*(obj->yRows-(currentAnim->EndPosition.y-currentAnim->StartPosition.y)))
				{
					vPtr->texC.y = 0;
				}
				UVs.x = vPtr->texC.x;
				UVs.y = vPtr->texC.y;
			break;
			case 1:
				vPtr->texC.x = UVs.x+stepSizeX;
				vPtr->texC.y = UVs.y;
				UVs1.x = vPtr->texC.x;
				UVs1.y = vPtr->texC.y;
			break;
			case 2:
				vPtr->texC.x = UVs.x;
				vPtr->texC.y = UVs.y + stepSizeY;
				UVs2.x = vPtr->texC.x;
				UVs2.y = vPtr->texC.y;
			break;
			case 3:
				vPtr->texC.x = UVs.x + stepSizeX;
				vPtr->texC.y = UVs.y + stepSizeY;
				UVs3.x = vPtr->texC.x;
				UVs3.y = vPtr->texC.y;
			break;
			}

			
			// increment pointer to next vertex
			ptr+=sizeof(Vertex2D);
		}
		obj->currentXAnimValue++;
		if(obj->currentXAnimValue > currentAnim->EndPosition.x)
		{
			obj->currentYAnimValue++;
			obj->currentXAnimValue = 0;
		}
		if(obj->currentYAnimValue > currentAnim->EndPosition.y)
		{
			obj->currentYAnimValue = 0;
		}
		// unlock the vertex buffer
		result = obj->vBuffer->Unlock();
		if (result != D3D_OK)
		{
			MessageBox(resources->GetWindowHandle(),"Failed to unlock vertexBuffer of quad","Animate()",MB_OK);
		}
	}
}

void GUI::LoadAnimation(GUITexture* obj)
{
	std::string path2 = obj->textureName;
	std::string T;
	T = path2.substr(0,path2.size()-3);
	T += "txt";

	std::ifstream fin(T);
	if (!fin.good())
	{
		MessageBox(resources->GetWindowHandle(),"Couldn't find animation textfile, Animation won't play. LoadAnimation()",T.c_str(),MB_OK);
		obj->hasAnimation = false;
	}
	else
	{
		obj->hasAnimation = true;
		std::string str;
		float startPosX = 0,startPosY = 0,endPosX = 0,endPosY = 0;
		while(getline(fin, str))
		{
			Animation anim;
			anim.Nullify();
			std::string name;


			fin >> name >> startPosX >> endPosX >> startPosY >> endPosY;
			//std::cout << endPosX << " 1 "<< endPosY << std::endl;
			anim.AnimationName = name;
			anim.StartPosition = D3DXVECTOR2(0,0);
			anim.EndPosition = D3DXVECTOR2(0,0);
			anim.StartPosition.x = startPosX;
			anim.StartPosition.y = startPosY;
			anim.EndPosition.x = endPosX;
			anim.EndPosition.y = endPosY;
			obj->currentAnim = NULL;
			obj->animations.push_back(anim);
		}
	}
	fin.close();
	if(obj->hasAnimation)
	{
		PlayAnimation(obj,"Walk");
	}
}
bool GUI::PlayAnimation(GUITexture* obj,std::string name)
{
	for(unsigned int i=0; i < obj->animations.size();i++)
	{
		if(obj->animations.at(i).AnimationName == name)
		{
			obj->currentlyPlayingAnimation = name;
			obj->currentAnim = i;
			return true;			   
		}
	}
	obj->currentXAnimValue = 0;
	obj->currentYAnimValue = 0;
	obj->currentlyPlayingAnimation = "None";
	obj->currentAnim = NULL;
	std::cout << "No animation found with that name: " << name << std::endl;
	MessageBox(resources->GetWindowHandle(),"No animation found with that name",name.c_str(),MB_OK);
	return false;
}
LPDIRECT3DVERTEXBUFFER9 GUI::CreateQuadVBuffer(GUI::GUITexture* gui)
{
	if(!gui->hasAnimation)
	{	
		Vertex2D triangleVerts[] = 
		{
			{gui->rect.x,gui->rect.y,0,1,D3DXVECTOR2(0,0)},
			{gui->rect.x+gui->rect.w,gui->rect.y,0,1,D3DXVECTOR2(1,0)},
			{gui->rect.x,gui->rect.y+gui->rect.h,0,1,D3DXVECTOR2(0,1)},
			{gui->rect.x+gui->rect.w,gui->rect.y+gui->rect.h,0,1,D3DXVECTOR2(1,1)},
		};
		LPDIRECT3DVERTEXBUFFER9 p_dx_VertexBuffer = NULL; //altijd nieuwe variables op NULL zetten (0x000000) zodat we kunnen zien dat het een nullpointer reference is als we een error krijgen, anders krijgt ie random memory space en kunnen we niet checken op NULL

		HRESULT result = p_Device->CreateVertexBuffer(4*sizeof(Vertex2D), 0, D3DFVF_XYZRHW | D3DFVF_TEX1, D3DPOOL_MANAGED, &p_dx_VertexBuffer, NULL);
		switch(result) //error checking of het gelukt is, zo niet, sluit af, want anders krijgen we dalijk andere soorten errors die niet opgevangen worden. (Access violation of nullpointer references vanwegen random pointers)
		{
		case D3DERR_INVALIDCALL: 
			MessageBox(resources->GetWindowHandle(),"Invalid Call while creating VertexBuffer","FillVertices()",MB_OK);
			return NULL;
			break;
		case D3DERR_OUTOFVIDEOMEMORY:
			MessageBox(resources->GetWindowHandle(),"Out of Video Memory while creating VertexBuffer","FillVertices()",MB_OK);
			return NULL;
			break;
		case E_OUTOFMEMORY:
			MessageBox(resources->GetWindowHandle(),"Out of Memory while creating VertexBuffer","FillVertices()",MB_OK);
			return NULL;
			break;
		}
		void* p_Vertices;
		result = p_dx_VertexBuffer->Lock(0, 4*sizeof(Vertex2D), (void**)&p_Vertices, 0);
		switch(result)
		{
		case D3DERR_INVALIDCALL: 
			MessageBox(resources->GetWindowHandle(),"Error trying to lock","FillVertices()",MB_OK);
			return NULL;
			break;
		}//we konden de vertexbuffer locken dus ga door
		memcpy(p_Vertices, triangleVerts, 4*sizeof(Vertex2D));
		p_dx_VertexBuffer->Unlock();

		return p_dx_VertexBuffer;
	}
	else
	{
		Vertex2D triangleVerts[] = 
		{
			{gui->rect.x,gui->rect.y,0,1,D3DXVECTOR2(0,0)},
			{gui->rect.x+gui->rect.w,gui->rect.y,0,1,D3DXVECTOR2(1,0)},
			{gui->rect.x,gui->rect.y+gui->rect.h,0,1,D3DXVECTOR2(0,1)},
			{gui->rect.x+gui->rect.w,gui->rect.y+gui->rect.h,0,1,D3DXVECTOR2(1,1)},
		};
		LPDIRECT3DVERTEXBUFFER9 p_dx_VertexBuffer = NULL; //altijd nieuwe variables op NULL zetten (0x000000) zodat we kunnen zien dat het een nullpointer reference is als we een error krijgen, anders krijgt ie random memory space en kunnen we niet checken op NULL

		HRESULT result = p_Device->CreateVertexBuffer(4*sizeof(Vertex2D), 0, D3DFVF_XYZRHW | D3DFVF_TEX1, D3DPOOL_MANAGED, &p_dx_VertexBuffer, NULL);

		switch(result) //error checking of het gelukt is, zo niet, sluit af, want anders krijgen we dalijk andere soorten errors die niet opgevangen worden. (Access violation of nullpointer references vanwegen random pointers)
		{
		case D3DERR_INVALIDCALL: 
			MessageBox(resources->GetWindowHandle(),"Invalid Call while creating VertexBuffer","FillVertices()",MB_OK);
			return NULL;
			break;
		case D3DERR_OUTOFVIDEOMEMORY:
			MessageBox(resources->GetWindowHandle(),"Out of Video Memory while creating VertexBuffer","FillVertices()",MB_OK);
			return NULL;
			break;
		case E_OUTOFMEMORY:
			MessageBox(resources->GetWindowHandle(),"Out of Memory while creating VertexBuffer","FillVertices()",MB_OK);
			return NULL;
			break;
		}
		void* p_Vertices;
		result = p_dx_VertexBuffer->Lock(0, 4*sizeof(Vertex2D), (void**)&p_Vertices, 0);
		switch(result)
		{
		case D3DERR_INVALIDCALL: 
			MessageBox(resources->GetWindowHandle(),"Error trying to lock","FillVertices()",MB_OK);
			return NULL;
			break;
		}//we konden de vertexbuffer locken dus ga door
		memcpy(p_Vertices, triangleVerts, 4*sizeof(Vertex2D));
		p_dx_VertexBuffer->Unlock();

		return p_dx_VertexBuffer;
	}
}

LPDIRECT3DINDEXBUFFER9 GUI::CreateQuadIndices() //zelfde als FillVertices, zie uitleg daar
{
    short s_Indices[4];
    s_Indices[0]=0;
    s_Indices[1]=1;
    s_Indices[2]=2;
	s_Indices[3]=3;


	LPDIRECT3DINDEXBUFFER9 p_dx_IndexBuffer = NULL;
	HRESULT result = p_Device->CreateIndexBuffer(4*sizeof(short), D3DUSAGE_WRITEONLY,D3DFMT_INDEX16,D3DPOOL_MANAGED,&p_dx_IndexBuffer,NULL);
	switch(result)
	{
	case D3DERR_INVALIDCALL: 
		MessageBox(resources->GetWindowHandle(),"Invalid Call while creating IndexBuffer","FillIndices()",MB_OK);
		return NULL;
		break;
	case D3DERR_OUTOFVIDEOMEMORY:
		MessageBox(resources->GetWindowHandle(),"Out of Video Memory while creating IndexBuffer","FillIndices()",MB_OK);
		return NULL;
		break;
	case E_OUTOFMEMORY:
		MessageBox(resources->GetWindowHandle(),"Out of Memory while creating IndexBuffer","FillIndices()",MB_OK);
		return NULL;
		break;
	}

	void* p_Indices;
	result = p_dx_IndexBuffer->Lock(0, 4*sizeof(short), (void**)&p_Indices, 0);
	 
	switch(result)
	{
	case D3DERR_INVALIDCALL: 
		MessageBox(resources->GetWindowHandle(),"Error trying to lock","FillIndices()",MB_OK);
		return NULL;
		break;
	}
	memcpy(p_Indices, s_Indices, 4*sizeof(short));
	p_dx_IndexBuffer->Unlock();

	return p_dx_IndexBuffer;
}