#include "Menu.h"

Menu::Menu(bool* loop, ResourceManager* res, InputHandler* inputHandler)
{
	resources = res;
	input = inputHandler;
	started = false;
	mouseLeftJustDown = false;
	didClick = false;
	doLoop = loop;
	mState = StartScreen;
	int height = 50;
	int spacing = 5;

	background = new GUI::GUITexture();
	background->texture = resources->GetTexture("Resources/GUI/background.png");
	background->textureName = "Resources/GUI/background.png";
	background->rect.w = 1024;
	background->rect.h = 1024;
	background->rect.x = -1;
	background->rect.y = -1;
	background->vBuffer = CreateQuadVBuffer(background);
	background->iBuffer = CreateQuadIndices();
	StartButton		= new Button(true, D3DXVECTOR2(res->GetScreenWidth()/2-128,res->GetScreenHeight()/2-height+32),D3DXVECTOR2(256,64),"Resources/GUI/Start.png",&Menu::StartGame,this);
	CreditsButton	= new Button(true, D3DXVECTOR2(res->GetScreenWidth()/2-128,res->GetScreenHeight()/2-height+96+spacing),D3DXVECTOR2(256,64),"Resources/GUI/Credits.png",&Menu::Credits,this);
	ResumeButton	= new Button(false, D3DXVECTOR2(res->GetScreenWidth()/2-128,res->GetScreenHeight()/2-height+96+spacing),D3DXVECTOR2(256,64),"Resources/GUI/Resume.png",&Menu::StartGame,this);
	QuitButton		= new Button(true, D3DXVECTOR2(res->GetScreenWidth()/2-128,res->GetScreenHeight()/2-height+160+(spacing*2)),D3DXVECTOR2(256,64),"Resources/GUI/Quit.png",&Menu::Quit,this);
	BackButton		= new Button(false, D3DXVECTOR2(res->GetScreenWidth()/2-128,res->GetScreenHeight()/2-height+160+(spacing*2)),D3DXVECTOR2(256,64),"Resources/GUI/Back.png",&Menu::Back,this);
	text = resources->GetText(resources->CreateTextObject("Arial","              Developer: Floyd \n               Spriter: Gerson \n          Texture Artist: Gerson \n              3D Artist: Floyd \n Music done by: Other game companies",20,32,30,400,200,D3DXCOLOR(0xFFFFFFFF)));
	text->render = false;
	
	buttons.push_back(StartButton);
	buttons.push_back(CreditsButton);
	buttons.push_back(ResumeButton);
	buttons.push_back(QuitButton);
	buttons.push_back(BackButton);

	ShowMenu(StartScreen);
}
Menu::~Menu()
{
	for(unsigned int i=0; i < buttons.size();i++)
	{
		buttons.at(i)->Release();
		delete buttons.at(i);
		buttons.erase(buttons.begin()+i);
	}
}
void Menu::ShowMenu(MenuState state)
{
	ShowCursor(true);
	resources->GetDevice()->SetRenderState(D3DRS_FILLMODE,D3DFILL_SOLID);
	mState = state;
	LARGE_INTEGER currentTicks;
	LARGE_INTEGER currentFrequency;
	LARGE_INTEGER oldTicks;
	QueryPerformanceCounter(&oldTicks);
	LONGLONG deltaTimerPrecision = 10000;

	resources->SetTextRenderingFalse();

	if(state == StartScreen)
	{
		StartButton->render = true;
		CreditsButton->render = true;
		ResumeButton->render = false;
		QuitButton->render = true;
		BackButton->render = false;
		text->render = false;
	}
	else if(state == CreditScreen)
	{
		text->render = true;
		StartButton->render = false;
		CreditsButton->render = false;
		ResumeButton->render = false;
		QuitButton->render = false;
		BackButton->render = true;
	}
	else if(state == PauseScreen)
	{
		text->render = false;
		StartButton->render = false;
		CreditsButton->render = false;
		ResumeButton->render = true;
		QuitButton->render = true;
		BackButton->render = false;
	}

	started = false;
	MSG msg;
	while(!started)
	{		
		while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);	
			if(msg.message == WM_QUIT)
			{
				started = false;
				*doLoop = false;
			}
        }
		QueryPerformanceFrequency(&currentFrequency);
		QueryPerformanceCounter(&currentTicks);
		currentTicks.QuadPart = currentTicks.QuadPart*deltaTimerPrecision / currentFrequency.QuadPart;
		DWORD valu = currentTicks.QuadPart-oldTicks.QuadPart ;
		if(valu > ((1.0f/60.0f)*deltaTimerPrecision))
		{
			Update();
			DrawButtons();
			oldTicks = currentTicks;
		}
		Sleep(1);
	}
	
	ShowCursor(false);
}
void Menu::Back()
{
	StartButton->render = true;
	CreditsButton->render = true;
	ResumeButton->render = false;
	QuitButton->render = true;
	BackButton->render = false;
	text->render = false;
}
void Menu::Update()
{
	POINT p;
	GetCursorPos(&p);
	ScreenToClient(resources->GetWindowHandle(), &p);
	input->Update();
	if(input->MouseButtonDown(0))
	{
		if(mouseLeftJustDown == false)
		{
			mouseLeftJustDown = true;
			for(unsigned int i = 0; i < buttons.size(); i++)
			{
				if(mouseLeftJustDown == true && didClick == false)
				{
					buttons.at(i)->CheckMouseCollision(&p,&mouseLeftJustDown,&didClick);
				}
				else
				{
					return;
				}
			}
		}
	}
	else
	{
		mouseLeftJustDown = false;
		didClick = false;
	}
	
	
}
void Menu::DrawButtons()
{
	LPDIRECT3DDEVICE9 dev = resources->GetDevice();
	
    dev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(255,0,0), 1.0f, 0);
	dev->Clear(0, NULL, D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
	D3DXMATRIX matProj;
	D3DXMatrixOrthoLH(&matProj,resources->GetScreenWidth(),resources->GetScreenHeight(),0,1);
	dev->BeginScene();
	dev->SetTransform(D3DTS_PROJECTION, &matProj);


	D3DXMATRIX world;
		
	D3DXMatrixTranslation(&world,0,0,0); //vertices are made on position, that's why the world is 0,0,0, look at CreateQuadVBuffer
		
	dev->SetFVF( D3DFVF_XYZRHW | D3DFVF_TEX1);
	dev->SetStreamSource(0, background->vBuffer, 0, sizeof(GUI::Vertex2D));
	dev->SetIndices(background->iBuffer);
	dev->SetTransform(D3DTS_WORLD,&world);
	dev->SetTexture(0,background->texture);
	dev->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP,0,0,4,0,2);


	resources->DrawAllText();
	for(unsigned int i = 0; i < buttons.size(); i++)
	{
		if(buttons.at(i)->render)
		{
			buttons.at(i)->Draw(dev);
		}
	}
	dev->EndScene();
    dev->Present(NULL, NULL, NULL, NULL);
}
void Menu::StartGame()
{
	started = true;
}
void Menu::Quit()
{
	started = true;
	*doLoop = false;
	//game.loop = false;
}
void Menu::Credits()
{
	StartButton->render = false;
	CreditsButton->render = false;
	ResumeButton->render = false;
	QuitButton->render = false;
	BackButton->render = true;
	text->render = true;
	//DrawText("Developer: Floyd")
	//DrawText("2D/Texture Artist: Gerson")
	//DrawText("3D Artist: Floyd")
	//DrawText("Music done by: Other game companies")
}