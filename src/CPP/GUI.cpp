#include "../Headers/GUI.h"

GUI::GUI( LPDIRECT3DDEVICE9 d3dDev, ResourceManager* res)
{
	p_Device = res->GetDevice();
	resources = res;

	health = 100;
	armour = 0;
	healthText = resources->GetText(resources->CreateTextObject("Arial","Health: ",20,8,84,10,20,D3DXCOLOR(1,1,1,1)));
	gameOverText = resources->GetText(resources->CreateTextObject("System","Game Over",46,28,50,100,100,0xFFFF0000));
	gameOverText->render = false;
	armourText = resources->GetText(resources->CreateTextObject("Arial","Armour: ",20,8,92,10,40,D3DXCOLOR(1,1,1,1)));
	
	LoadGUI();
	
	attackGUI = GetGUIObj("Resources/GUI/Slash.png");
	levelCompleteGUI = GetGUIObj("Resources/GUI/LevelComplete.png");
	levelCompleteGUI->render = false;

	resources->CreateTextObject("Arial"," Movement: W/A/S/D  \n Jump: Space \n Attack: Left Mouse Button \n\n Fun: \n Spawn Boxes: F \n Clear Boxes: Z \n \n Enter Editor Mode: 0 \n Change Object: 8/9 \n Scale Object: Numpad 4,5,8 \n Reverse Scaling: Right CTRL",16,2,2,100,100,0xFFFFFFFF);
	D3DXMatrixOrthoLH(&matProj,resources->GetScreenWidth(),resources->GetScreenHeight(),0,1);
}
void GUI::Release()
{
	for(unsigned int  i = 0; i < guiObjs.size();i++)
	{
		if(guiObjs.at(i).iBuffer != NULL)		guiObjs.at(i).iBuffer->Release();
		if(guiObjs.at(i).vBuffer != NULL)		guiObjs.at(i).vBuffer->Release();
	}
	guiObjs.clear();
}
void GUI::ReloadGUI()
{
	Release();
	LoadGUI();
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
		bool hasAnimation,relativeEndX,relativeEndY,doRender;
		std::string startAnimation;
		while(getline(fin,str))
		{
			fin >> path >> posX >> posY >> sizeX >> sizeY >> xRows >> yRows >> hasAnimation >> relativeEndX >> relativeEndY >> startAnimation >> doRender;
			Rectangle rect;
			rect.Nullify();
			if(!relativeEndX)
			{
				rect.x = posX;
			}
			else
			{
				rect.x = resources->GetScreenWidth()-posX;
				std::cout << "relative posX = " << rect.x << std::endl;
			}
			if(!relativeEndY)
			{
				rect.y = posY;
			}
			else
			{
				rect.y = resources->GetScreenHeight()-posY;
				std::cout << "relative posY = " << rect.y << std::endl;
			}
			rect.w = sizeX;
			rect.h = sizeY;

			
			rect.x = resources->GetScreenWidth() * posX / 100;
			rect.y = resources->GetScreenHeight() * posY / 100;
			rect.w = resources->GetScreenWidth() * sizeX / 100;
			rect.h = resources->GetScreenHeight() * sizeY / 100;
			path = LowCaseString(path);
			if(hasAnimation)
			{
				if(CreateGUIObject(rect,_strdup(path.c_str()),xRows,yRows))
				{
					if(std::strcmp(_strdup(startAnimation.c_str()),"NULL") != 0)
					{
						std::cout << "start animation was : " << startAnimation << std::endl;
						PlayAnimation(&guiObjs.at(guiObjs.size()-1),startAnimation);
					}
				}
				else
				{
					fin.close();
				}
			}
			else
			{
				if(!CreateGUIObject(rect,_strdup(path.c_str())))
				{
					fin.close();
				}
			}
			guiObjs.at(guiObjs.size()-1).render = doRender;
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
	if(GText.vBuffer != NULL)
	{
		GText.iBuffer = CreateQuadIndices();
	}
	else
	{
		return false;
	}
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
	if(GText.vBuffer != NULL)
	{
		GText.iBuffer = CreateQuadIndices();
	}
	else
	{
		return false;
	}
	GText.animationSpeed = 0.25f;
	GText.xRows = xRows;
	GText.yRows = yRows;
	GText.currentAnim = -1;

	LoadAnimation(&GText);
	guiObjs.push_back(GText);
	return true;
}
void GUI::Update(float deltaTime)
{
	for(unsigned int i = 0; i < guiObjs.size();i++)
	{
		GUITexture* t = &guiObjs.at(i);
		if(t->currentAnim != -1)
		{
			if(t->currentAnim >= 0 && t->currentAnim < t->animations.size())
			{
				Animation* a = &t->animations.at(t->currentAnim);
			
				if(t->hasAnimation && !a->isFinished)
				{
					Animate(&guiObjs.at(i),deltaTime,guiObjs.at(i).animationSpeed);
				}
				if(t->hasAnimation && a->isFinished && a->loop)
				{
					a->isFinished = false;
				}
			}
		}
	}
}
void GUI::SetUVValues(GUITexture* obj)
{
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
		// unlock the vertex buffer
		result = obj->vBuffer->Unlock();
		if (result != D3D_OK)
		{
			MessageBox(resources->GetWindowHandle(),"Failed to unlock vertexBuffer of quad","Animate()",MB_OK);
		}
}
void GUI::Animate(GUITexture* obj , float dTime ,float animSpeed)
{		
	obj->timeSinceChange += dTime;
	
	if(obj->timeSinceChange > obj->animations.at(obj->currentAnim).AnimationSpeed)
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
			Vertex2D* vPtr=(Vertex2D*) ptr;
			switch(i)
			{
			case 0:
				vPtr->texC.x = stepSizeX * obj->currentXAnimValue;
				vPtr->texC.y = stepSizeY * obj->currentYAnimValue;
				
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
			ptr+=sizeof(Vertex2D);
		}

		// unlock the vertex buffer
		result = obj->vBuffer->Unlock();

		obj->currentXAnimValue++;
		if(obj->currentXAnimValue > currentAnim->EndPosition.x)
		{
			obj->currentYAnimValue++;
			obj->currentXAnimValue = currentAnim->StartPosition.x;
			if(obj->currentYAnimValue > currentAnim->EndPosition.y)
			{
				if(currentAnim->loop)
				{
					obj->currentYAnimValue = currentAnim->StartPosition.y;
				}
				else
				{
					obj->currentXAnimValue = currentAnim->EndPosition.x;
					obj->currentYAnimValue = currentAnim->EndPosition.y;
				}
				
				currentAnim->isFinished = true;
			}
		}
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
		float startPosX = 0,startPosY = 0,endPosX = 0,endPosY = 0,animSpeed = 0;
		int loop = 0;
		while(getline(fin, str))
		{
			Animation anim;
			anim.Nullify();
			std::string name;


			fin >> name >> startPosX >> endPosX >> startPosY >> endPosY >> animSpeed >> loop;
			//std::cout << endPosX << " 1 "<< endPosY << std::endl;
			anim.AnimationName = name;
			anim.StartPosition = D3DXVECTOR2(0,0);
			anim.EndPosition = D3DXVECTOR2(0,0);
			anim.StartPosition.x = startPosX;
			anim.StartPosition.y = startPosY;
			anim.EndPosition.x = endPosX;
			anim.EndPosition.y = endPosY;
			anim.AnimationSpeed = animSpeed;
			anim.loop = loop;
			//(loop == 1 ? anim.loop = true : anim.loop = false);
			obj->animations.push_back(anim);
		}
	}
	fin.close();
}
bool GUI::PlayAnimation(GUITexture* obj, std::string name)
{
	for(unsigned int i=0; i < obj->animations.size();i++)
	{
		if(obj->animations.at(i).AnimationName == name)
		{
			//std::cout << "Animation executed" << std::endl;
			obj->currentXAnimValue = (int)obj->animations.at(i).StartPosition.x;
			obj->currentYAnimValue = (int)obj->animations.at(i).StartPosition.y;
			obj->animationSpeed = obj->animations.at(i).AnimationSpeed;
			obj->currentlyPlayingAnimation = name;
			obj->currentAnim = i;
			obj->animations.at(i).isFinished = false;
			obj->timeSinceChange = obj->animationSpeed/2;
			if(p_Device->TestCooperativeLevel() == D3D_OK)
			{
				SetUVValues(obj);
			}
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
void GUI::DrawAllText()
{
	if(healthText != NULL)
	{
		std::string healthT = "";
		std::stringstream ss;
		ss << health;
		healthT.append(ss.str());
		healthText->text = _strdup(healthT.c_str());
		healthText->DrawFont();
	}
	if(armourText != NULL)
	{
		std::string armourT = "";
		std::stringstream ss;
		ss << armour;
		armourT.append(ss.str());
		armourText->text = _strdup(armourT.c_str());
		armourText->DrawFont();
	}
	if(gameOverText != NULL && gameOverText->render)
	{
		gameOverText->text = "         Game Over..\n\n Press R to restart game.";
		gameOverText->DrawFont();
	}
}
void GUI::Render()
{
	D3DXMatrixOrthoLH(&matProj,resources->GetScreenWidth(),resources->GetScreenHeight(),0,1);
	p_Device->SetTransform(D3DTS_PROJECTION, &matProj);
	DrawAllText();
	for(unsigned int i = 0; i < guiObjs.size();i++)
	{
		if(guiObjs.at(i).render)
		{
			HRESULT result;
			D3DXMATRIX world;
		
			D3DXMatrixTranslation(&world,0,0,0); //vertices are made on position, that's why the world is 0,0,0, look at CreateQuadVBuffer
		
			result = p_Device->SetFVF( D3DFVF_XYZRHW | D3DFVF_TEX1);
			result = p_Device->SetStreamSource(0, guiObjs.at(i).vBuffer, 0, sizeof(Vertex2D));
			result = p_Device->SetIndices(guiObjs.at(i).iBuffer);
			result = p_Device->SetTransform(D3DTS_WORLD,&world);
			result = p_Device->SetTexture(0,guiObjs.at(i).texture);

			GUITexture* g =  &guiObjs.at(i);
		
			if(g->currentAnim >= 0 && g->currentAnim < g->animations.size())
			{
				Animation* anim = &g->animations.at(g->currentAnim);
				if(g->hasAnimation && g->currentAnim != -1 && anim->isFinished == false)
				{
					result = p_Device->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP,0,0,4,0,2);
				}
				else if(g->hasAnimation && anim->loop == false && anim->isFinished)
				{
					result = p_Device->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP,0,0,4,0,2);
					g->timeSinceChange += 0.016f;
					if(g->timeSinceChange > anim->AnimationSpeed)
					{
						g->timeSinceChange = 0;
						if(!anim->showEnd)
						{
							g->currentAnim = -1;
						}
					}
				}
			}
			if(!g->hasAnimation)
			{
				result = p_Device->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP,0,0,4,0,2);
			}
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
		}
	}
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
		switch(result) 
		{
		case D3DERR_INVALIDCALL: 
			//MessageBox(resources->GetWindowHandle(),"Invalid Call while creating VertexBuffer","GUI createQuad()",MB_OK);
			//Not sure whether this is a smart move due to the possibility of an infinite loop;
			ReloadGUI();


			return NULL;
			break;
		case D3DERR_OUTOFVIDEOMEMORY:
			MessageBox(resources->GetWindowHandle(),"Out of Video Memory while creating VertexBuffer","GUI createQuad()",MB_OK);
			return NULL;
			break;
		case E_OUTOFMEMORY:
			MessageBox(resources->GetWindowHandle(),"Out of Memory while creating VertexBuffer","GUI createQuad()",MB_OK);
			return NULL;
			break;
		}
		void* p_Vertices;
		result = p_dx_VertexBuffer->Lock(0, 4*sizeof(Vertex2D), (void**)&p_Vertices, 0);
		switch(result)
		{
		case D3DERR_INVALIDCALL: 
			MessageBox(resources->GetWindowHandle(),"Error trying to lock","GUI FillVertices()",MB_OK);
			return NULL;
			break;
		}
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
			//MessageBox(resources->GetWindowHandle(),"Invalid Call while creating VertexBuffer"," GUI FillVertices()",MB_OK);
			ReloadGUI();
			return NULL;
			break;
		case D3DERR_OUTOFVIDEOMEMORY:
			MessageBox(resources->GetWindowHandle(),"Out of Video Memory while creating VertexBuffer","GUI FillVertices()",MB_OK);
			return NULL;
			break;
		case E_OUTOFMEMORY:
			MessageBox(resources->GetWindowHandle(),"Out of Memory while creating VertexBuffer","GUI FillVertices()",MB_OK);
			return NULL;
			break;
		}
		void* p_Vertices;
		result = p_dx_VertexBuffer->Lock(0, 4*sizeof(Vertex2D), (void**)&p_Vertices, 0);
		switch(result)
		{
		case D3DERR_INVALIDCALL: 
			MessageBox(resources->GetWindowHandle(),"Error trying to lock","GUI FillVertices()",MB_OK);
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
		
		//MessageBox(resources->GetWindowHandle(),"Invalid Call while creating IndexBuffer","GUI FillIndices()",MB_OK);
		std::cout << "Invalid Call while creating IndexBuffer in GUI FillIndices(), You can ignore this, It will be handled in the following FillVertices, This is only for notifying" << std::endl;
		return NULL;
		break;
	case D3DERR_OUTOFVIDEOMEMORY:
		MessageBox(resources->GetWindowHandle(),"Out of Video Memory while creating IndexBuffer","GUI FillIndices()",MB_OK);
		return NULL;
		break;
	case E_OUTOFMEMORY:
		MessageBox(resources->GetWindowHandle(),"Out of Memory while creating IndexBuffer","GUI FillIndices()",MB_OK);
		return NULL;
		break;
	}

	void* p_Indices;
	result = p_dx_IndexBuffer->Lock(0, 4*sizeof(short), (void**)&p_Indices, 0);
	 
	switch(result)
	{
	case D3DERR_INVALIDCALL: 
		MessageBox(resources->GetWindowHandle(),"Error trying to lock","GUI FillIndices()",MB_OK);
		return NULL;
		break;
	}
	memcpy(p_Indices, s_Indices, 4*sizeof(short));
	p_dx_IndexBuffer->Unlock();

	return p_dx_IndexBuffer;
}