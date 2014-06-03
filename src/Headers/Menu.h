#ifndef _MENU_H_
#define _MENU_H_

#include "ResourceManager.h"
#include "GUI.h"
#include "InputHandler.h"

class Menu
{
public:
	Menu ( bool* loop, ResourceManager* res, InputHandler* inputHandler);
	InputHandler* input;
	POINT mousePos;

	enum MenuState{StartScreen,PauseScreen,CreditScreen,HiddenScreen};
	MenuState mState;
	bool started;
	void ShowMenu(MenuState state);
	void DrawButtons();
	void Update();
	void Back();
	void StartGame();
	void Quit();
	void Credits();
	void Release();
private:
	struct Button
	{
		void (Menu::*function)(void);
		Menu* mClass;
		D3DXVECTOR2 pos;
		D3DXVECTOR2 size;
		GUI::GUITexture* obj;
		bool render;
		Button(bool doRender, D3DXVECTOR2 position, D3DXVECTOR2 bSize,char* texturePath, void (Menu::*buttonFunction)(void),Menu* context)
		{
			render = doRender;
			function = buttonFunction;
			pos = position;
			size = bSize; 
			mClass = context;
			mClass->buttons.push_back(this);
			obj = new GUI::GUITexture();
			obj->rect.x = pos.x;
			obj->rect.y = pos.y;
			obj->rect.w = bSize.x;
			obj->rect.h = bSize.y;
			obj->vBuffer = mClass->CreateQuadVBuffer(obj);
			obj->iBuffer = mClass->CreateQuadIndices();
			obj->texture = mClass->resources->GetTexture(texturePath);
			obj->textureName = texturePath;
		}
		void Click()
		{
			(mClass->*function)();
		}
		void Draw(LPDIRECT3DDEVICE9 dev)
		{
			if(obj!= NULL)
			{
				HRESULT result;
				D3DXMATRIX world;
		
				D3DXMatrixTranslation(&world,0,0,0);
		
				result = dev->SetFVF( D3DFVF_XYZRHW | D3DFVF_TEX1);
				result = dev->SetStreamSource(0, obj->vBuffer, 0, sizeof(GUI::Vertex2D));
				result = dev->SetIndices(obj->iBuffer);
				result = dev->SetTransform(D3DTS_WORLD,&world);
				result = dev->SetTexture(0,obj->texture);
				result = dev->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP,0,0,4,0,2);
			};
		}
		void Release()
		{
			delete obj;
			obj = NULL;
		}
		void CheckMouseCollision(POINT* p, bool* mouseJustDown, bool* didClick)
		{
			if(render == true)
			{
				if(p->x > obj->rect.x && p->x < obj->rect.x+obj->rect.w)
				{
					if(p->y > obj->rect.y && p->y < obj->rect.y + obj->rect.h)
					{
						if(*mouseJustDown == true)
						{
							*didClick = true;
							Click();
						}
					}
				}
			}
		}
	};
	bool* doLoop;
	bool mouseLeftJustDown;
	bool didClick;
	GUI::GUITexture* background;
	ResourceManager::TextData* text;
	Button* StartButton;
	Button* CreditsButton;
	Button* QuitButton;
	Button* ResumeButton;
	Button* BackButton;

	std::vector<Button*> buttons;
	ResourceManager* resources;


	inline LPDIRECT3DVERTEXBUFFER9 CreateQuadVBuffer(GUI::GUITexture* gui)
	{
		GUI::Vertex2D triangleVerts[] = 
		{
			{gui->rect.x,gui->rect.y,0,1,D3DXVECTOR2(0,0)},
			{gui->rect.x+gui->rect.w,gui->rect.y,0,1,D3DXVECTOR2(1,0)},
			{gui->rect.x,gui->rect.y+gui->rect.h,0,1,D3DXVECTOR2(0,1)},
			{gui->rect.x+gui->rect.w,gui->rect.y+gui->rect.h,0,1,D3DXVECTOR2(1,1)},
		};
		LPDIRECT3DVERTEXBUFFER9 p_dx_VertexBuffer = NULL; 
		HRESULT result = resources->GetDevice()->CreateVertexBuffer(4*sizeof(GUI::Vertex2D), 0, D3DFVF_XYZRHW | D3DFVF_TEX1, D3DPOOL_MANAGED, &p_dx_VertexBuffer, NULL);
		switch(result) 
		{
		case D3DERR_INVALIDCALL: 
			MessageBox(resources->GetWindowHandle(),"Invalid Call","Menu::Button::CreateQuadVBuffer()",MB_OK);
			return NULL;
			break;
		case D3DERR_OUTOFVIDEOMEMORY:
			MessageBox(resources->GetWindowHandle(),"Out of Video Memory while creating VertexBuffer","Menu::Button::CreateQuadVBuffer()",MB_OK);
			return NULL;
			break;
		case E_OUTOFMEMORY:
			MessageBox(resources->GetWindowHandle(),"Out of Memory while creating VertexBuffer","Menu::Button::CreateQuadVBuffer()",MB_OK);
			return NULL;
			break;
		}
		void* p_Vertices;
		result = p_dx_VertexBuffer->Lock(0, 4*sizeof(GUI::Vertex2D), (void**)&p_Vertices, 0);
		switch(result)
		{
		case D3DERR_INVALIDCALL: 
			MessageBox(resources->GetWindowHandle(),"Error trying to lock","Menu::Button::CreateQuadVBuffer()",MB_OK);
			return NULL;
			break;
		}
		memcpy(p_Vertices, triangleVerts, 4*sizeof(GUI::Vertex2D));
		p_dx_VertexBuffer->Unlock();

		return p_dx_VertexBuffer;
	}
	inline LPDIRECT3DINDEXBUFFER9 CreateQuadIndices()
	{
		short s_Indices[4];
		s_Indices[0]=0;
		s_Indices[1]=1;
		s_Indices[2]=2;
		s_Indices[3]=3;


		LPDIRECT3DINDEXBUFFER9 p_dx_IndexBuffer = NULL;
		HRESULT result = resources->GetDevice()->CreateIndexBuffer(4*sizeof(short), D3DUSAGE_WRITEONLY,D3DFMT_INDEX16,D3DPOOL_MANAGED,&p_dx_IndexBuffer,NULL);
		switch(result)
		{
		case D3DERR_INVALIDCALL: 
		
			//MessageBox(resources->GetWindowHandle(),"Invalid Call while creating IndexBuffer","GUI FillIndices()",MB_OK);
			std::cout << "Invalid Call while creating IndexBuffer in Menu::Button::CreateQuadIndices(), You can ignore this, It will be handled in the following FillVertices, This is only for notifying" << std::endl;
			return NULL;
			break;
		case D3DERR_OUTOFVIDEOMEMORY:
			MessageBox(resources->GetWindowHandle(),"Out of Video Memory while creating IndexBuffer","Menu::Button::CreateQuadIndices()",MB_OK);
			return NULL;
			break;
		case E_OUTOFMEMORY:
			MessageBox(resources->GetWindowHandle(),"Out of Memory while creating IndexBuffer","Menu::Button::CreateQuadIndices()",MB_OK);
			return NULL;
			break;
		}

		void* p_Indices;
		result = p_dx_IndexBuffer->Lock(0, 4*sizeof(short), (void**)&p_Indices, 0);
	 
		switch(result)
		{
		case D3DERR_INVALIDCALL: 
			MessageBox(resources->GetWindowHandle(),"Error trying to lock","Menu::Button::CreateQuadIndices()",MB_OK);
			return NULL;
			break;
		}
		memcpy(p_Indices, s_Indices, 4*sizeof(short));
		p_dx_IndexBuffer->Unlock();

		return p_dx_IndexBuffer;
	}
};

#endif