#include "../Headers/Object2D.h"

Object2D::Object2D(ResourceManager* res,char* texturePath,D3DXMATRIX* camViewMatrix)
{
	resources = res;
	p_Device = resources->GetDevice();
	cameraView = camViewMatrix;	 
	quad.texture = NULL;
	quad.textureName = texturePath;
	quad.texture = resources->GetTexture(texturePath);
	currentlyPlayingAnimation = "";
	hasAnimation = false;
	sizeX = 0;
	sizeY = 0;
	yRows = 0;
	xRows = 0;
	InitVars();
}
Object2D::Object2D(ResourceManager* res, char* texturePath, D3DXMATRIX* camView, D3DXVECTOR2 LLPos, D3DXVECTOR2 URPos)
{
	resources = res;
	p_Device = res->GetDevice();
	cameraView = camView;	 
	quad.texture = NULL;
	quad.textureName = texturePath;
	quad.texture = resources->GetTexture(texturePath);
	hasAnimation = false;
	
	if(quad.texture != NULL)
	{
		D3DMATERIAL9 mat;
		mat.Ambient.a = 255;
		mat.Ambient.r = 128;
		mat.Ambient.g = 128;
		mat.Ambient.b = 128;
		mat.Diffuse.a = 255;
		mat.Diffuse.r = 128;
		mat.Diffuse.g = 128;
		mat.Diffuse.b = 128;
		quad.meshMaterial = mat;
	}
	scaling.x = 1;
	scaling.y = 1;
	scaling.z = 1;
	position.x = 0;
	position.y = 0;
	position.z = 0;
	rotation.x = 0;
	rotation.y = 0;
	rotation.z = 0;
	timeSinceChange = 0;
	currentXAnimValue = 0;
	currentYAnimValue = 0;
	ZeroMemory(worldMatrix,sizeof(worldMatrix));
	sizeX = 0;
	sizeY = 0;
	yRows = 0;
	xRows = 0;
	
	currentlyPlayingAnimation = "";

	quad.vBuffer = NULL;
	quad.vBuffer = FillCustomVertices(LLPos,URPos);
	quad.iBuffer = NULL;
	quad.iBuffer = FillIndices();
}
Object2D::Object2D(ResourceManager* res,char* texturePath,D3DXMATRIX* camViewMatrix,int xRowsAnim,int yRowsAnim)
{
	resources = res;
	p_Device = res->GetDevice();
	cameraView = camViewMatrix;	 
	quad.texture = NULL;
	quad.textureName = texturePath;
	quad.texture = resources->GetTexture(texturePath);
	xRows = (int)xRowsAnim;
	yRows = (int)yRowsAnim;
	
	currentlyPlayingAnimation = "";
	InitVars();
	hasAnimation = true;
	LoadAnimation();
}
void Object2D::InitVars()
{
	if(quad.texture != NULL)
	{
		D3DMATERIAL9 mat;
		mat.Ambient.a = 255;
		mat.Ambient.r = 128;
		mat.Ambient.g = 128;
		mat.Ambient.b = 128;
		mat.Diffuse.a = 255;
		mat.Diffuse.r = 128;
		mat.Diffuse.g = 128;
		mat.Diffuse.b = 128;
		quad.meshMaterial = mat;
	}
//linkedPhysicsObj = NULL;
	scaling.x = 1;
	scaling.y = 1;
	scaling.z = 1;
	position.x = 0;
	position.y = 0;
	position.z = 0;
	rotation.x = 0;
	rotation.y = 0;
	rotation.z = 0;
	timeSinceChange = 0;
	currentXAnimValue = 0;
	currentYAnimValue = 0;
	quad.vBuffer = NULL;
	quad.vBuffer = FillVertices();
	quad.iBuffer = NULL;
	quad.iBuffer = FillIndices();
	//light = new light etc
}

bool Object2D::IsAnimFinished(std::string anim)
{
	std::string a = LowCaseString(anim);
	for(unsigned int i = 0; i < animations.size();i++)
	{
		if(animations.at(i).AnimationName.compare(a) == 0)
		{
			return animations.at(i).isFinished;
		}
	}
	return false;
}
Object2D::Animation Object2D::GetAnimation(std::string aName)
{
	std::string a = LowCaseString(aName);
	for(unsigned int i = 0; i < animations.size();i++)
	{
		if(animations.at(i).AnimationName.compare(a) == 0)
		{
			return animations.at(i);
		}
	}
	Animation an;
	an.Nullify();
	return an;
}
void Object2D::Draw()
{
	//Matrixes are used for rotation/scaling/position and everything
	D3DXMATRIX m_ViewScale;
	D3DXMATRIX m_ViewWorld;
	D3DXVECTOR3 transScale;
	D3DXMatrixInverse(&m_ViewWorld, 0, cameraView); //turns toward camera 
	//D3DXVec3TransformNormal(&transScale,&scaling,&m_ViewWorld);

	D3DXMATRIX m_Scale;
	D3DXMatrixScaling(&m_Scale,scaling.x,scaling.y,scaling.x); //scaling

	D3DXMATRIX m_Translation;
	D3DXMatrixTranslation(&m_Translation,position.x,position.y,position.z); //positioning

	D3DXMatrixMultiply(&m_ViewScale, &m_ViewWorld, &m_Scale);

	D3DXMatrixMultiply(&worldMatrix, &m_Translation, &m_ViewScale);
	
	worldMatrix._41 = position.x;
	worldMatrix._42 = position.y;
	worldMatrix._43 = position.z;
	worldMatrix._21 = 0;
	worldMatrix._23 = 0;

	HRESULT result;

	result = p_Device->SetFVF( D3DFVF_XYZ |D3DFVF_NORMAL | D3DFVF_TEX1);
	result = p_Device->SetStreamSource(0, quad.vBuffer, 0, sizeof(VertexPosNorTex));
	result = p_Device->SetIndices(quad.iBuffer);
	result = p_Device->SetTransform(D3DTS_WORLD, &worldMatrix);
	result = p_Device->SetMaterial(&quad.meshMaterial);
	result = p_Device->SetTexture(0,quad.texture);

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
}

void Object2D::Update(float deltaTime)
{
	if(currentAnim != -1 && hasAnimation)
	{
		if(currentAnim >= 0 && currentAnim < animations.size())
		{
			Animation* a = &animations.at(currentAnim);
			timeSinceChange+=deltaTime;
			if(hasAnimation && timeSinceChange > a->AnimationSpeed)
			{
				int actionFrameX=0,actionFrameY=0;
				int endX=0, endY=0;
				currentXAnimValue++;		
				if(currentXAnimValue > a->EndPosition.x)
				{
					currentYAnimValue++;
					currentXAnimValue = a->StartPosition.x;
					if(currentYAnimValue > a->EndPosition.y)
					{
						a->isFinished = true;
						a->waitForEnd = false;
						if(a->loop)
						{
							currentYAnimValue = a->StartPosition.y;
							a->isFinished = false;
						}
						else
						{
							currentXAnimValue = a->EndPosition.x;
							currentYAnimValue = a->EndPosition.y;
						}
					}
				}
				if(currentXAnimValue == (int)a->ActionFrame.x &&  currentYAnimValue == (int)a->ActionFrame.y)
				{
					a->doAction = true;
				}
				if(!a->isFinished)
				{
					Animate();
				}
				timeSinceChange = 0;
			}
		}
	}
}

/*
void Object2D::Update(float deltaTime)
{
	if(currentAnim != -1 && hasAnimation)
	{
		if(currentAnim >= 0 && currentAnim < animations.size())
		{
			Animation* a = &animations.at(currentAnim);
			timeSinceChange+=deltaTime;
			if(hasAnimation && timeSinceChange > a->AnimationSpeed)
			{
				Animation* cAnim = &animations.at(currentAnim);
				int cX = 0, cY = 0, aX = 0, aY = 0,eX=0,eY=0;

				currentXAnimValue++;
				cX = (int)floor(currentXAnimValue-0.5f);
				eX = (int)floor(cAnim->EndPosition.x-0.5f);
				
				if((int)ceil(currentXAnimValue-0.5f) > eX)
				{
					currentYAnimValue++;
					cY = (int)floor(currentYAnimValue-0.5f);
					eY = (int)floor(cAnim->EndPosition.y-0.5f);
					
					currentXAnimValue = (int)cAnim->StartPosition.x;

					
					if((int)ceil(currentYAnimValue-0.5f) > eY)
					{
						currentYAnimValue = (int)cAnim->StartPosition.y;
					}
				}

				cY = (int)floor(currentYAnimValue-0.5f);
				eY = (int)floor(cAnim->EndPosition.y-0.5f);
				aX = (int)floor(cAnim->ActionFrame.x-0.5f);
				aY = (int)floor(cAnim->ActionFrame.y-0.5f);
				if((int)floor(currentXAnimValue-0.5f) == aX &&  (int)floor(currentYAnimValue-0.5f) == aY)
				{
					cAnim->doAction = true;
				}
				else
				{
					cAnim->doAction = false;
				}
				if((int)floor(currentXAnimValue-0.5f) > eX && (int)floor(currentYAnimValue-0.5f) > eY)
				{
					currentXAnimValue = cAnim->EndPosition.x;
					currentYAnimValue = cAnim->EndPosition.y;
				}
				if(!cAnim->isFinished)
				{
					Animate();
				}
				if((int)floor(currentXAnimValue-0.5f) >= eX && (int)floor(currentYAnimValue-0.5f) >= eY)
				{
					if(cAnim->loop)
					{
						currentYAnimValue = cAnim->StartPosition.y;
					}
					else
					{
						currentXAnimValue = cAnim->EndPosition.x;
						currentYAnimValue = cAnim->EndPosition.y;
						cAnim->isFinished = true;
					}
				}
				timeSinceChange = 0;
			}
		}
	}
}*/
void Object2D::CheckPlayingAnimation(std::string playAnimFinished)
{
	if(hasAnimation)
	{
		Animation* a = &animations.at(currentAnim);
		if(a->isFinished && timeSinceChange > a->AnimationSpeed)
		{
			a->isFinished = false;
			PlayAnimation(playAnimFinished);
		}
	}
}
void Object2D::SetUVValues()
{
	HRESULT result;
	BYTE* ptr;
	result = quad.vBuffer->Lock(0,sizeof(VertexPosNorTex)*4,(LPVOID*)&ptr,NULL);
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
	stepSizeX = (1.0f / xRows);
	stepSizeY = (1.0f / yRows);
		
	for (DWORD i=0;i<4;i++) // hardcoded 4 because 4 vertices
	{
		// get pointer to location
		VertexPosNorTex* vPtr=(VertexPosNorTex*) ptr;
		switch(i)
		{
		case 0:
			vPtr->texC.x = stepSizeX * currentXAnimValue;
			vPtr->texC.y = stepSizeY * currentYAnimValue;
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
		ptr+=sizeof(VertexPosNorTex);
	}

	// unlock the vertex buffer
	result = quad.vBuffer->Unlock();
	if (result != D3D_OK)
	{
		MessageBox(resources->GetWindowHandle(),"Failed to unlock vertexBuffer of quad","Animate()",MB_OK);
	}
}
void Object2D::Animate()
{		
	Animation* cAnim = &animations.at(currentAnim);

	HRESULT result;
	BYTE* ptr;
	
	result = quad.vBuffer->Lock(0,sizeof(VertexPosNorTex)*4,(LPVOID*)&ptr,NULL);
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
	stepSizeX = (1.0f / xRows);
	stepSizeY = (1.0f / yRows);

		
	for (DWORD i=0;i<4;i++) // hardcoded 4 because 4 vertices
	{
		// get pointer to location
		VertexPosNorTex* vPtr=(VertexPosNorTex*) ptr;
		switch(i)
		{
		case 0:
			vPtr->texC.x = stepSizeX * currentXAnimValue;
			vPtr->texC.y = stepSizeY * currentYAnimValue;
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
		ptr+=sizeof(VertexPosNorTex);
	}
		
	// unlock the vertex buffer
	result = quad.vBuffer->Unlock();
	
	
	if (result != D3D_OK)
	{
		MessageBox(resources->GetWindowHandle(),"Failed to unlock vertexBuffer of quad","Animate()",MB_OK);
	}
}
void Object2D::LoadAnimation()
{
	std::string path2 = quad.textureName;
	std::string T;
	T = path2.substr(0,path2.size()-3);
	T += "txt";

	std::ifstream fin(T);
	if (!fin.good())
	{
		MessageBox(resources->GetWindowHandle(),"Couldn't find animation textfile, Animation won't play. LoadAnimation()",T.c_str(),MB_OK);
		hasAnimation = false;
	}
	else
	{
		hasAnimation = true;
		std::string str;
		float startPosX = 0,startPosY = 0,endPosX = 0,endPosY = 0,animSpeed = 0, actionPosX = 0, actionPosY = 0;
		int loop = 0;
		while(getline(fin, str))
		{
			Animation anim;
			anim.Nullify();
			std::string name;

			fin >> name >> startPosX >> endPosX >> startPosY >> endPosY >> animSpeed >> loop >> actionPosX >> actionPosY;
			anim.AnimationName = LowCaseString(name);
			anim.StartPosition.x = startPosX;
			anim.StartPosition.y = startPosY;
			if(actionPosX == 0 && actionPosY == 0)
			{
				anim.ActionFrame = D3DXVECTOR2(9999,9999);
			}
			else
			{
				anim.ActionFrame = D3DXVECTOR2(actionPosX,actionPosY);
			}
			anim.EndPosition.x = endPosX;
			anim.EndPosition.y = endPosY;
			anim.AnimationSpeed = animSpeed;
			(loop == 1 ? anim.loop = true : anim.loop = false);
			animations.push_back(anim);
		}
	}
	fin.close();
}
bool Object2D::PlayAnimation(std::string aName, bool waitEnd, bool overrideAll)
{
	std::string name = LowCaseString(aName);
	for(unsigned int i=0; i < animations.size();i++)
	{
		if(animations.at(i).AnimationName == name)
		{
			if(currentlyPlayingAnimation.compare("") != 0)
			{
				if(!GetCurrentAnim()->waitForEnd || !GetCurrentAnim()->waitForEnd && waitEnd  || overrideAll)
				{
					currentXAnimValue = (int)animations.at(i).StartPosition.x;
					currentYAnimValue = (int)animations.at(i).StartPosition.y;
					animationSpeed = animations.at(i).AnimationSpeed;
					currentlyPlayingAnimation = name;
					currentAnim = i;
					animations.at(i).isFinished = false;
					animations.at(i).waitForEnd = waitEnd;
					animations.at(i).doAction = false;
					timeSinceChange = animationSpeed/2;
					if(p_Device->TestCooperativeLevel() == D3D_OK)
					{
						SetUVValues();
					}
					return true;			   
				}
				else
				{
					return false;
				}
			}
			else
			{
				currentXAnimValue = (int)animations.at(i).StartPosition.x;
				currentYAnimValue = (int)animations.at(i).StartPosition.y;
				animationSpeed = animations.at(i).AnimationSpeed;
				currentlyPlayingAnimation = name;
				currentAnim = i;
				animations.at(i).isFinished = false;
				animations.at(i).waitForEnd = waitEnd;
				animations.at(i).doAction = false;
				timeSinceChange = animationSpeed/2;
				if(p_Device->TestCooperativeLevel() == D3D_OK)
				{
					SetUVValues();
				}
				return true;			   
			}
			
		}
	}
	if(currentlyPlayingAnimation.compare("") != 0 && !GetCurrentAnim()->waitForEnd)
	{
		currentXAnimValue = 0;
		currentYAnimValue = 0;
		currentlyPlayingAnimation = "None";
		currentAnim = NULL;
		std::cout << "No animation found with that name: " <<name.c_str() << std::endl;
	}
	return false;
}


void Object2D::ReleaseResources()
{
	if(quad.vBuffer != NULL)
	{
		quad.vBuffer->Release();
	}
	if(quad.iBuffer != NULL)
	{
		quad.iBuffer->Release();
	}
	for(unsigned int i = 0 ; i < animations.size();i++)
	{
		animations.at(i).AnimationName.empty();
	}
	animations.clear();
}

void Object2D::SetPosition(float x,float y,float z)
{
	position.x = x;
	position.y = y;
	position.z = z;
}
void Object2D::SetScale(float x,float y,float z)
{
	scaling.x = x;
	scaling.y = y;
	scaling.z = z;
}

LPDIRECT3DVERTEXBUFFER9 Object2D::FillVertices()
{
	if(!hasAnimation)
	{	
		VertexPosNorTex triangleVerts[] = 
		{
			{D3DXVECTOR3(-2.5,5,0),D3DXVECTOR3(1,0,0),D3DXVECTOR2(0,0)},
			{D3DXVECTOR3(2.5,5,0),D3DXVECTOR3(1,0,0),D3DXVECTOR2(1,0)},
			{D3DXVECTOR3(-2.5,-5,0),D3DXVECTOR3(1,0,0),D3DXVECTOR2(0,1.5)},
			{D3DXVECTOR3(2.5,-5,0),D3DXVECTOR3(1,0,0),D3DXVECTOR2(1,1.5)},
		};
		LPDIRECT3DVERTEXBUFFER9 p_dx_VertexBuffer = NULL; //altijd nieuwe variables op NULL zetten (0x000000) zodat we kunnen zien dat het een nullpointer reference is als we een error krijgen, anders krijgt ie random memory space en kunnen we niet checken op NULL

		HRESULT result = p_Device->CreateVertexBuffer(4*sizeof(VertexPosNorTex), 0, D3DFVF_XYZ |D3DFVF_NORMAL | D3DFVF_TEX1, D3DPOOL_MANAGED, &p_dx_VertexBuffer, NULL);
		switch(result) //error checking of het gelukt is, zo niet, sluit af, want anders krijgen we dalijk andere soorten errors die niet opgevangen worden. (Access violation of nullpointer references vanwegen random pointers)
		{
		case D3DERR_INVALIDCALL: 
			MessageBox(handleWindow,"Invalid Call while creating VertexBuffer","Object2D fillverts()",MB_OK);
			return NULL;
			break;
		case D3DERR_OUTOFVIDEOMEMORY:
			MessageBox(handleWindow,"Out of Video Memory while creating VertexBuffer","Object2D fillverts()",MB_OK);
			return NULL;
			break;
		case E_OUTOFMEMORY:
			MessageBox(handleWindow,"Out of Memory while creating VertexBuffer","Object2D fillverts()",MB_OK);
			return NULL;
			break;
		}
		void* p_Vertices;
		result = p_dx_VertexBuffer->Lock(0, 4*sizeof(VertexPosNorTex), (void**)&p_Vertices, 0);
		switch(result)
		{
		case D3DERR_INVALIDCALL: 
			MessageBox(handleWindow,"Error trying to lock","Object2D fillverts()",MB_OK);
			return NULL;
			break;
		}//we konden de vertexbuffer locken dus ga door
		memcpy(p_Vertices, triangleVerts, 4*sizeof(VertexPosNorTex));
		p_dx_VertexBuffer->Unlock();

		return p_dx_VertexBuffer;
	}
	else
	{
		VertexPosNorTex triangleVerts[] = 
		{
			{D3DXVECTOR3(-2.5,5,0),D3DXVECTOR3(1,0,0),D3DXVECTOR2(0,0)},
			{D3DXVECTOR3(2.5,5,0),D3DXVECTOR3(1,0,0),D3DXVECTOR2(0.25,0)},
			{D3DXVECTOR3(-2.5,-5,0),D3DXVECTOR3(1,0,0),D3DXVECTOR2(0,1.5)},
			{D3DXVECTOR3(2.5,-5,0),D3DXVECTOR3(1,0,0),D3DXVECTOR2(0.25,1.5)},
		};
		LPDIRECT3DVERTEXBUFFER9 p_dx_VertexBuffer = NULL; //altijd nieuwe variables op NULL zetten (0x000000) zodat we kunnen zien dat het een nullpointer reference is als we een error krijgen, anders krijgt ie random memory space en kunnen we niet checken op NULL

		HRESULT result = p_Device->CreateVertexBuffer(4*sizeof(VertexPosNorTex), 0, D3DFVF_XYZ|D3DFVF_NORMAL | D3DFVF_TEX1, D3DPOOL_MANAGED, &p_dx_VertexBuffer, NULL);

		switch(result) //error checking of het gelukt is, zo niet, sluit af, want anders krijgen we dalijk andere soorten errors die niet opgevangen worden. (Access violation of nullpointer references vanwegen random pointers)
		{
		case D3DERR_INVALIDCALL: 
			MessageBox(handleWindow,"Invalid Call while creating VertexBuffer","Object2D fillverts()",MB_OK);
			return NULL;
			break;
		case D3DERR_OUTOFVIDEOMEMORY:
			MessageBox(handleWindow,"Out of Video Memory while creating VertexBuffer","Object2D fillverts()",MB_OK);
			return NULL;
			break;
		case E_OUTOFMEMORY:
			MessageBox(handleWindow,"Out of Memory while creating VertexBuffer","Object2D fillverts()",MB_OK);
			return NULL;
			break;
		}
		void* p_Vertices;
		result = p_dx_VertexBuffer->Lock(0, 4*sizeof(VertexPosNorTex), (void**)&p_Vertices, 0);
		switch(result)
		{
		case D3DERR_INVALIDCALL: 
			MessageBox(handleWindow,"Error trying to lock","Object2D fillverts()",MB_OK);
			return NULL;
			break;
		}//we konden de vertexbuffer locken dus ga door
		memcpy(p_Vertices, triangleVerts, 4*sizeof(VertexPosNorTex));
		p_dx_VertexBuffer->Unlock();

		return p_dx_VertexBuffer;
	}
}
LPDIRECT3DVERTEXBUFFER9 Object2D::FillCustomVertices(D3DXVECTOR2 LLPos,D3DXVECTOR2 URPos)
{
	if(!hasAnimation)
	{	
		VertexPosNorTex triangleVerts[] = 
		{
			{D3DXVECTOR3(LLPos.x,URPos.y,0),D3DXVECTOR3(1,0,0),D3DXVECTOR2(0,0)},
			{D3DXVECTOR3(URPos.x,URPos.y,0),D3DXVECTOR3(1,0,0),D3DXVECTOR2(1,0)},
			{D3DXVECTOR3(LLPos.x,LLPos.y,0),D3DXVECTOR3(1,0,0),D3DXVECTOR2(0,1)},
			{D3DXVECTOR3(URPos.x,LLPos.y,0),D3DXVECTOR3(1,0,0),D3DXVECTOR2(1,1)},
		};
		LPDIRECT3DVERTEXBUFFER9 p_dx_VertexBuffer = NULL; //altijd nieuwe variables op NULL zetten (0x000000) zodat we kunnen zien dat het een nullpointer reference is als we een error krijgen, anders krijgt ie random memory space en kunnen we niet checken op NULL

		HRESULT result = p_Device->CreateVertexBuffer(4*sizeof(VertexPosNorTex), 0, D3DFVF_XYZ |D3DFVF_NORMAL | D3DFVF_TEX1, D3DPOOL_MANAGED, &p_dx_VertexBuffer, NULL);
		switch(result) //error checking of het gelukt is, zo niet, sluit af, want anders krijgen we dalijk andere soorten errors die niet opgevangen worden. (Access violation of nullpointer references vanwegen random pointers)
		{
		case D3DERR_INVALIDCALL: 
			MessageBox(handleWindow,"Invalid Call while creating VertexBuffer","Object2D fillverts()",MB_OK);
			return NULL;
			break;
		case D3DERR_OUTOFVIDEOMEMORY:
			MessageBox(handleWindow,"Out of Video Memory while creating VertexBuffer","Object2D fillverts()",MB_OK);
			return NULL;
			break;
		case E_OUTOFMEMORY:
			MessageBox(handleWindow,"Out of Memory while creating VertexBuffer","Object2D fillverts()",MB_OK);
			return NULL;
			break;
		}
		void* p_Vertices;
		result = p_dx_VertexBuffer->Lock(0, 4*sizeof(VertexPosNorTex), (void**)&p_Vertices, 0);
		switch(result)
		{
		case D3DERR_INVALIDCALL: 
			MessageBox(handleWindow,"Error trying to lock","Object2D fillverts()",MB_OK);
			return NULL;
			break;
		}//we konden de vertexbuffer locken dus ga door
		memcpy(p_Vertices, triangleVerts, 4*sizeof(VertexPosNorTex));
		p_dx_VertexBuffer->Unlock();

		return p_dx_VertexBuffer;
	}
	else
	{
		VertexPosNorTex triangleVerts[] = 
		{
			{D3DXVECTOR3(-2.5,5,0),D3DXVECTOR3(1,0,0),D3DXVECTOR2(0,0)},
			{D3DXVECTOR3(2.5,5,0),D3DXVECTOR3(1,0,0),D3DXVECTOR2(0.25,0)},
			{D3DXVECTOR3(-2.5,-5,0),D3DXVECTOR3(1,0,0),D3DXVECTOR2(0,1.5)},
			{D3DXVECTOR3(2.5,-5,0),D3DXVECTOR3(1,0,0),D3DXVECTOR2(0.25,1.5)},
		};
		LPDIRECT3DVERTEXBUFFER9 p_dx_VertexBuffer = NULL; //altijd nieuwe variables op NULL zetten (0x000000) zodat we kunnen zien dat het een nullpointer reference is als we een error krijgen, anders krijgt ie random memory space en kunnen we niet checken op NULL

		HRESULT result = p_Device->CreateVertexBuffer(4*sizeof(VertexPosNorTex), 0, D3DFVF_XYZ|D3DFVF_NORMAL | D3DFVF_TEX1, D3DPOOL_MANAGED, &p_dx_VertexBuffer, NULL);

		switch(result) //error checking of het gelukt is, zo niet, sluit af, want anders krijgen we dalijk andere soorten errors die niet opgevangen worden. (Access violation of nullpointer references vanwegen random pointers)
		{
		case D3DERR_INVALIDCALL: 
			MessageBox(handleWindow,"Invalid Call while creating VertexBuffer","Object2D fillverts()",MB_OK);
			return NULL;
			break;
		case D3DERR_OUTOFVIDEOMEMORY:
			MessageBox(handleWindow,"Out of Video Memory while creating VertexBuffer","Object2D fillverts()",MB_OK);
			return NULL;
			break;
		case E_OUTOFMEMORY:
			MessageBox(handleWindow,"Out of Memory while creating VertexBuffer","Object2D fillverts()",MB_OK);
			return NULL;
			break;
		}
		void* p_Vertices;
		result = p_dx_VertexBuffer->Lock(0, 4*sizeof(VertexPosNorTex), (void**)&p_Vertices, 0);
		switch(result)
		{
		case D3DERR_INVALIDCALL: 
			MessageBox(handleWindow,"Error trying to lock","Object2D fillverts()",MB_OK);
			return NULL;
			break;
		}//we konden de vertexbuffer locken dus ga door
		memcpy(p_Vertices, triangleVerts, 4*sizeof(VertexPosNorTex));
		p_dx_VertexBuffer->Unlock();

		return p_dx_VertexBuffer;
	}
}

LPDIRECT3DINDEXBUFFER9 Object2D::FillIndices() //zelfde als FillVertices, zie uitleg daar
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
		MessageBox(handleWindow,"Invalid Call while creating IndexBuffer","Object2D fill indices()",MB_OK);
		return NULL;
		break;
	case D3DERR_OUTOFVIDEOMEMORY:
		MessageBox(handleWindow,"Out of Video Memory while creating IndexBuffer","Object2D fill indices()",MB_OK);
		return NULL;
		break;
	case E_OUTOFMEMORY:
		MessageBox(handleWindow,"Out of Memory while creating IndexBuffer","Object2D fill indices()",MB_OK);
		return NULL;
		break;
	}

	void* p_Indices;
	result = p_dx_IndexBuffer->Lock(0, 4*sizeof(short), (void**)&p_Indices, 0);
	 
	switch(result)
	{
	case D3DERR_INVALIDCALL: 
		MessageBox(handleWindow,"Error trying to lock","Object2D fill indices()",MB_OK);
		return NULL;
		break;
	}
	memcpy(p_Indices, s_Indices, 4*sizeof(short));
	p_dx_IndexBuffer->Unlock();

	return p_dx_IndexBuffer;
}