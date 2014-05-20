#include "LobbyScreen.h"
#include <DirectXMath.h>
#include "GUIText.h"
#include "GUITextBox.h"
#include "GUITextures.h"
#include <d3d11.h>
#include "DXGame.h"

#define TICKER_WIDTH			50
#define TICKER_TOP_OFFSET		0
#define TICKER_HEIGHT			80 - TICKER_TOP_OFFSET

#define MENUBUTTONS_TOP_OFFSET  screenHeight / 2	+ 200
#define MENUBUTTONS_LEFT_OFFSET screenWidth / 2		- 300


#include <DirectXMath.h>
#include "GUIText.h"
#include "GUITextBox.h"
#include "GUITextures.h"
#include <d3d11.h>
#include "DXGame.h"

#define TICKER_WIDTH			50
#define TICKER_TOP_OFFSET		0
#define TICKER_HEIGHT			80 - TICKER_TOP_OFFSET

#define MENUBUTTONS_TOP_OFFSET  screenHeight / 2	+ 200
#define MENUBUTTONS_LEFT_OFFSET screenWidth / 2		- 300

LobbyScreen::LobbyScreen(FontRenderer* renderer, SpriteRenderer* spRenderer, const short ScreenWidth, const short ScreenHeight) 
{
	playerNames[0]= "0";
	playerNames[1]= "1";
	playerNames[2]= "2";
	playerNames[3]= "3";
	playerIndex=-1;
	readyChecks[0]=false;
	readyChecks[1]=false;
	readyChecks[2]=false;
	readyChecks[3]=false;
	longPlayerNames[0] = L" ";
	longPlayerNames[1] = L" ";
	longPlayerNames[2] = L" ";
	longPlayerNames[3] = L" ";
	check0=false;
	check1=false;
	check2=false;
	check3=false;
	this->checkboxRenderer = spRenderer;
	additionCount=0;
	multiplayerClicked=false;
	guiMan = new GUIManager();	
	sendTimer=0.0f;

	GUIText* sp = new GUIText(&Rect(80 + MENUBUTTONS_LEFT_OFFSET, -100 + MENUBUTTONS_TOP_OFFSET, 0 ,0), L"SINGLEPLAYER", 1, AnimationType::BOTTOMTOTOP, renderer, 4000, Colors::White);
	guiMan->Add("SINGLEPLAYER", sp);
	GUIText* mp = new GUIText(&Rect(80 + MENUBUTTONS_LEFT_OFFSET, 0 + MENUBUTTONS_TOP_OFFSET, 0 ,0), L"MULTIPLAYER", 3, AnimationType::BOTTOMTOTOP, renderer, 4000, Colors::White);
	guiMan->Add("MULTIPLAYER", mp);
	GUIText* settings = new GUIText(&Rect(80 + MENUBUTTONS_LEFT_OFFSET, 100 + MENUBUTTONS_TOP_OFFSET, 0 ,0), L"SETTINGS", 5, AnimationType::BOTTOMTOTOP, renderer, 4000, Colors::White);
	guiMan->Add("SETTINGS", settings);
	GUIText* exit = new GUIText(&Rect(80 + MENUBUTTONS_LEFT_OFFSET, 200 + MENUBUTTONS_TOP_OFFSET, 0 ,0), L"EXIT", 7, AnimationType::BOTTOMTOTOP, renderer, 4000, Colors::White);
	guiMan->Add("EXIT", exit);	

	for(int i=0; i<4; i++){
		
		guiMan->Add((char*)playerNames[i].c_str(), new GUIText(&Rect(50, (screenHeight * -0.75)+100+ (100* i) + MENUBUTTONS_TOP_OFFSET, 0 ,0),(wchar_t*)longPlayerNames[i].c_str(), 1, AnimationType::BOTTOMTOTOP, renderer, 4000, Colors::Green));
		guiMan->_guiElements[(char*)playerNames[i].c_str()]->SetDepth(0.5f);
	}

		checkBoxArray[0]= new RECT();
		checkBoxArray[0]->left		= -50;
		checkBoxArray[0]->top		= 30;
		checkBoxArray[0]->right		= 0;			//End it where the text starts
		checkBoxArray[0]->bottom	= 80;
		guiMan->Add("cb0", new GUITexture(checkBoxArray[0], L"../Assets/Textures/checkself1.png", spRenderer));
		guiMan->_guiElements["cb0"]->SetDepth(0.1f);

		checkBoxArray[1]= new RECT();
		checkBoxArray[1]->left		= -50;
		checkBoxArray[1]->top		= 130;
		checkBoxArray[1]->right		= 0;			//End it where the text starts
		checkBoxArray[1]->bottom	= 180;
		guiMan->Add("cb1", new GUITexture(checkBoxArray[1], L"../Assets/Textures/checkself1.png", spRenderer));
		guiMan->_guiElements["cb1"]->SetDepth(0.1f);

		checkBoxArray[2]= new RECT();
		checkBoxArray[2]->left		= -50;
		checkBoxArray[2]->top		= 230;
		checkBoxArray[2]->right		= 0;			//End it where the text starts
		checkBoxArray[2]->bottom	= 280;
		guiMan->Add("cb2", new GUITexture(checkBoxArray[2], L"../Assets/Textures/checkself1.png", spRenderer));
		guiMan->_guiElements["cb2"]->SetDepth(0.1f);

		checkBoxArray[3]= new RECT();
		checkBoxArray[3]->left		= -50;
		checkBoxArray[3]->top		= 330;
		checkBoxArray[3]->right		= 0;			//End it where the text starts
		checkBoxArray[3]->bottom	= 380;
		guiMan->Add("cb3", new GUITexture(checkBoxArray[3], L"../Assets/Textures/checkself1.png", spRenderer));
		guiMan->_guiElements["cb3"]->SetDepth(0.1f);

	//guiMan->Add("Texture", new GUITexture(r, L"../Assets/Textures/checkself1.png", spRenderer));


	guiMan->_guiElements["SINGLEPLAYER"]->SetDepth(0.5f);
	guiMan->_guiElements["MULTIPLAYER"]->SetDepth(0.5f);
	guiMan->_guiElements["EXIT"]->SetDepth(0.5f);

	this->fontRenderer = renderer;	
	
	currstate = GameState::GameLobby;
	menuOptions = MENU_OPTIONS::SINGLEPLAYER;
}

void LobbyScreen::OnClickNewGame()
{
	LobbyScreen::currstate = GameState::Playing;
}

GameState LobbyScreen::Update(const float dt)
{	sendTimer-=dt;
	guiMan->Update(dt);
	if(guiMan->_guiElements["SINGLEPLAYER"]->Clicked())
		return GameState::Playing;	

	if(guiMan->_guiElements["MULTIPLAYER"]->Clicked()){
		if(!multiplayerClicked){
			AddPlayer("Player " + to_string(additionCount), false);
		}
	}
	else{
		multiplayerClicked=false;
	}

	if(guiMan->_guiElements["cb0"]->Clicked() && sendTimer<0.0f){
		if(playerIndex==0 && !check0){
			sendTimer=2.0f;
			guiMan->Remove("cb0");
				if(!readyChecks[0])
					guiMan->Add("cb0", new GUITexture(checkBoxArray[0], L"../Assets/Textures/checkself2.png", checkboxRenderer));
				else
					guiMan->Add("cb0", new GUITexture(checkBoxArray[0], L"../Assets/Textures/checkself1.png", checkboxRenderer));
				readyChecks[0]= !readyChecks[0];
				guiMan->_guiElements["cb0"]->SetDepth(0.1f);
			check0=true;

			string toPush1;
			toPush1.assign(longPlayerNames[0].begin(), longPlayerNames[0].end());
				if(readyChecks[0])
					readyOutputQueue.push(toPush1+ "\n1");
				else
					readyOutputQueue.push(toPush1+ "\n0");
		}
	}
	else{
		check0=false;
	}

	if(guiMan->_guiElements["cb1"]->Clicked()&& sendTimer<0.0f){
		if(playerIndex==1 && !check1){
			sendTimer=2.0f;
		guiMan->Remove("cb1");
			if(!readyChecks[1])
				guiMan->Add("cb1", new GUITexture(checkBoxArray[1], L"../Assets/Textures/checkself2.png", checkboxRenderer));
			else
				guiMan->Add("cb1", new GUITexture(checkBoxArray[1], L"../Assets/Textures/checkself1.png", checkboxRenderer));
			readyChecks[1]= !readyChecks[1];
			guiMan->_guiElements["cb1"]->SetDepth(0.1f);
		check1=true;

		string toPush2;
		toPush2.assign(longPlayerNames[1].begin(), longPlayerNames[1].end());
			if(readyChecks[1])
				readyOutputQueue.push(toPush2+ "\n1");
			else
				readyOutputQueue.push(toPush2+ "\n0");
		}
	}
	else{
		check1=false;
	}


	if(guiMan->_guiElements["cb2"]->Clicked()&& sendTimer<0.0f){ 
		if(playerIndex==2 && !check2){
			sendTimer=2.0f;
		guiMan->Remove("cb2");
			if(!readyChecks[2])
				guiMan->Add("cb2", new GUITexture(checkBoxArray[2], L"../Assets/Textures/checkself2.png", checkboxRenderer));
			else
				guiMan->Add("cb2", new GUITexture(checkBoxArray[2], L"../Assets/Textures/checkself1.png", checkboxRenderer));
			readyChecks[2]= !readyChecks[2];
			guiMan->_guiElements["cb2"]->SetDepth(0.1f);
			check2=true;
			std::stringstream ss;
			ss <<longPlayerNames[2].c_str();
			string toPush3;
			toPush3.assign(longPlayerNames[2].begin(), longPlayerNames[2].end());
			
			if(readyChecks[2])
				readyOutputQueue.push(toPush3+ "\n1");
			else
				readyOutputQueue.push(toPush3+ "\n0");
		}
	}
	else{
		check2=false;
	}

	if(guiMan->_guiElements["cb3"]->Clicked()&& sendTimer<0.0f) {
		if(playerIndex==3 && !check3){
			sendTimer=2.0f;
			guiMan->Remove("cb3");
				if(!readyChecks[3])
					guiMan->Add("cb3", new GUITexture(checkBoxArray[3], L"../Assets/Textures/checkself2.png", checkboxRenderer));
				else
					guiMan->Add("cb3", new GUITexture(checkBoxArray[3], L"../Assets/Textures/checkself1.png", checkboxRenderer));
				readyChecks[3]= !readyChecks[3];
				guiMan->_guiElements["cb3"]->SetDepth(0.1f);
				check3=true;

				string toPush4;
				toPush4.assign(longPlayerNames[3].begin(), longPlayerNames[3].end());
				if(readyChecks[3])
					readyOutputQueue.push(toPush4+ "\n1");
				else
					readyOutputQueue.push(toPush4+ "\n0");
		}
	}
	else{
		check3=false;
	}




	if(guiMan->_guiElements["EXIT"]->Clicked())
		PostQuitMessage(0);



	while(!playerAdditionQueue.empty()){
		string curString= playerAdditionQueue.front();
		
		std::vector<std::string> stringParts;
		std::istringstream stringsplitter(curString);
		while(std::getline(stringsplitter,curString, '\n')){
			stringParts.push_back(curString);
		}

		nameHoldingVector.push_back(new string());
		nameHoldingVector.at(nameHoldingVector.size()-1)->append(stringParts.at(0));
		int isCurPlayer= atoi(stringParts.at(1).c_str());

		if(isCurPlayer==1){

			AddPlayer(*nameHoldingVector.at(nameHoldingVector.size()-1), true);
		}
		else{
			AddPlayer(*nameHoldingVector.at(nameHoldingVector.size()-1), false);
		}

		playerAdditionQueue.pop();
	}

	while(!readyToggleQueue.empty()){
		string curString2= readyToggleQueue.front();
		std::vector<std::string> stringParts;
		std::istringstream stringsplitter(curString2);
		while(std::getline(stringsplitter,curString2, '\n')){
			stringParts.push_back(curString2);
		}
		std::wstring wideName;
		wideName.assign(stringParts.at(0).begin(),stringParts.at(0).end());
		int isReady = atoi(stringParts.at(1).c_str());
		for(int i=0; i<4; i++){
			if(wideName == longPlayerNames[i]){
				switch(i){
				case 0:
					guiMan->Remove("cb0");
					if(isReady==1){
						readyChecks[0]= true;
						guiMan->Add("cb0", new GUITexture(checkBoxArray[0], L"../Assets/Textures/checkother2.png", checkboxRenderer));
					}
					else{
						guiMan->Add("cb0", new GUITexture(checkBoxArray[0], L"../Assets/Textures/checkother1.png", checkboxRenderer));
						readyChecks[0]= false;
					}
					guiMan->_guiElements["cb0"]->SetDepth(0.1f);
					break;
				case 1:
					guiMan->Remove("cb1");
					if(isReady==1){
						readyChecks[1]= true;
						guiMan->Add("cb1", new GUITexture(checkBoxArray[1], L"../Assets/Textures/checkother2.png", checkboxRenderer));
					}
					else{
						guiMan->Add("cb1", new GUITexture(checkBoxArray[1], L"../Assets/Textures/checkother1.png", checkboxRenderer));
						readyChecks[1]= false;
					}
					guiMan->_guiElements["cb1"]->SetDepth(0.1f);
					break;
				case 2:
					guiMan->Remove("cb2");
					if(isReady==1){
						readyChecks[2]= true;
						guiMan->Add("cb2", new GUITexture(checkBoxArray[2], L"../Assets/Textures/checkother2.png", checkboxRenderer));
					}
					else{
						guiMan->Add("cb2", new GUITexture(checkBoxArray[2], L"../Assets/Textures/checkother1.png", checkboxRenderer));
						readyChecks[2]= false;
					}
					guiMan->_guiElements["cb2"]->SetDepth(0.1f);
					break;
				case 3:
					guiMan->Remove("cb3");
					if(isReady==1){
						readyChecks[3]= true;
						guiMan->Add("cb3", new GUITexture(checkBoxArray[3], L"../Assets/Textures/checkother2.png", checkboxRenderer));
					}
					else{
						guiMan->Add("cb3", new GUITexture(checkBoxArray[3], L"../Assets/Textures/checkother1.png", checkboxRenderer));
						readyChecks[3]= false;
					}
					guiMan->_guiElements["cb3"]->SetDepth(0.1f);
					break;
				}
			}
		}

		readyToggleQueue.pop();
	}





	/*if(guiMan->_guiElements["SINGLEPLAYER"]->IsMouseHovering() || menuOptions == MENU_OPTIONS::SINGLEPLAYER)
	{
		guiMan->_guiElements["SINGLEPLAYER"]->SetColor(XMFLOAT4(1, 1, 1, 1));
		guiMan->_guiElements["SINGLEPLAYER"]->SetScale(XMFLOAT2(1.105f, 1.105f));
		menuOptions = MENU_OPTIONS::SINGLEPLAYER;
		GUITexture* tex = (GUITexture*)(guiMan->_guiElements["Texture"]);
		RECT* rectBig = GetRECTFromRect(guiMan->_guiElements["SINGLEPLAYER"]->GetRect());
		tex->SetRect(rectBig);
		delete rectBig;
	}
	else
	{
		guiMan->_guiElements["SINGLEPLAYER"]->SetColor(XMFLOAT4(1, 1, 1, 1));
		guiMan->_guiElements["SINGLEPLAYER"]->SetScale(XMFLOAT2(1, 1));

	}
	*/
	currstate = KeyboardInputProcess();
	return currstate;
}

GameState LobbyScreen::KeyboardInputProcess()
{
	if (!r)
	{
		return currstate;
	}

	if(IPMan::GetIPMan()->GetKeyDown(KeyType::BACKWARD))
	{
		//Because the height of the fonts is 100
		r->top += 100;
		r->bottom += 100;
		if(r->top > 200 + TICKER_TOP_OFFSET)
			{
				r->top = TICKER_TOP_OFFSET;
				r->bottom = guiMan->_guiElements["SINGLEPLAYER"]->GetRect()->y + TICKER_HEIGHT; 
			}
		GUITexture* tex = (GUITexture*)(guiMan->_guiElements["Texture"]);
		tex->SetRect(r);	
	}
	else 
		if(IPMan::GetIPMan()->GetKeyDown(KeyType::FORWARD))
	{
		//Because the height of the fonts is 100
		r->top -= 100;
		r->bottom -= 100;
		float x = guiMan->_guiElements["SINGLEPLAYER"]->GetRect()->y	+ TICKER_TOP_OFFSET;
		if(r->top < x)
			{
				r->top = guiMan->_guiElements["EXIT"]->GetRect()->y+ TICKER_TOP_OFFSET;
				r->bottom = guiMan->_guiElements["EXIT"]->GetRect()->y+TICKER_HEIGHT;
			}
		GUITexture* tex = (GUITexture*)(guiMan->_guiElements["Texture"]);
		tex->SetRect(r);	
	}

	
		if(IPMan::GetIPMan()->GetKey(VK_RETURN))
			{
				switch(menuOptions)
				{
				case MENU_OPTIONS::SINGLEPLAYER: return GameState::Playing;
					break;
				case MENU_OPTIONS::MULTIPLAYER: return GameState::GameLobby;
					break;
				case MENU_OPTIONS::SETTINGS: return GameState::Settings;
					break;
				case MENU_OPTIONS::EXIT: 
					exit(1);
					return GameState::Lost;
					break;
				default: return GameState::GameLobby;
					break;
				}
			}
		return GameState::GameLobby;
}

void LobbyScreen::Render()
{
	guiMan->Render();
}

LobbyScreen::~LobbyScreen(void)
{
	delete b ;	
	delete r;	
	delete guiMan;
}


void LobbyScreen::WindowResize()
{
	RECT rect;
	GetClientRect(GetActiveWindow(), &rect);
	if(guiMan)
	{
		auto p = static_cast<GUITexture*>(guiMan->_guiElements["Background"]);
		if(p)
		{
			RECT* rectBig = Rect::GetRECTFromRect(new Rect(0, 0, rect.left + rect.right, rect.top + rect.bottom));
			p->SetRect(rectBig);
			delete rectBig;
		}
	}
}

void LobbyScreen::AddPlayer(string player, bool isUs)
{
	if (additionCount < 4)
	{
		std::wstring wsTmp(player.begin(), player.end());
		playerNames[additionCount]= player.c_str();
		longPlayerNames[additionCount]= (wchar_t*)wsTmp.c_str();
		((GUIText*)guiMan->_guiElements[(char*)playerNames[additionCount].c_str()])->str = (wchar_t*)longPlayerNames[additionCount].c_str();
		if(!isUs)
			guiMan->_guiElements[(char*)playerNames[additionCount].c_str()]->SetColor(XMFLOAT4(1,0,0,1));
		else{
			guiMan->_guiElements[(char*)playerNames[additionCount].c_str()]->SetColor(XMFLOAT4(0,1,0,1));
			playerIndex=additionCount;
		}

		checkBoxArray[additionCount]->left = 0;
		checkBoxArray[additionCount]->right = 50;

		if(additionCount==0){
			guiMan->Remove("cb0");
			if(isUs)
			guiMan->Add("cb0", new GUITexture(checkBoxArray[0], L"../Assets/Textures/checkself1.png", checkboxRenderer));
			else
			guiMan->Add("cb0", new GUITexture(checkBoxArray[0], L"../Assets/Textures/checkother1.png", checkboxRenderer));
			guiMan->_guiElements["cb0"]->SetDepth(0.1f);
		}
		if(additionCount==1){
			guiMan->Remove("cb1");
			if(isUs)
			guiMan->Add("cb1", new GUITexture(checkBoxArray[1], L"../Assets/Textures/checkself1.png", checkboxRenderer));
			else
			guiMan->Add("cb1", new GUITexture(checkBoxArray[1], L"../Assets/Textures/checkother1.png", checkboxRenderer));
			guiMan->_guiElements["cb1"]->SetDepth(0.1f);
		}
		if(additionCount==2){
			guiMan->Remove("cb2");
			if(isUs)
			guiMan->Add("cb2", new GUITexture(checkBoxArray[2], L"../Assets/Textures/checkself1.png", checkboxRenderer));
			else
			guiMan->Add("cb2", new GUITexture(checkBoxArray[2], L"../Assets/Textures/checkother1.png", checkboxRenderer));
			guiMan->_guiElements["cb2"]->SetDepth(0.1f);
		}
		if(additionCount==3){
			guiMan->Remove("cb3");
			if(isUs)
			guiMan->Add("cb3", new GUITexture(checkBoxArray[3], L"../Assets/Textures/checkself1.png", checkboxRenderer));
			else
			guiMan->Add("cb3", new GUITexture(checkBoxArray[3], L"../Assets/Textures/checkother1.png", checkboxRenderer));
			guiMan->_guiElements["cb3"]->SetDepth(0.1f);
		}


		/*

		addedLongNameVector.push_back(wsTmp);
		addedNameVector.push_back(player);
		if(isUs)
			guiMan->Add((char*)addedNameVector.at(addedNameVector.size()-1).c_str(), new GUIText(&Rect(0, (screenHeight * -0.75)+ (100* additionCount) + MENUBUTTONS_TOP_OFFSET, 0 ,0),(wchar_t*)addedLongNameVector.at(addedLongNameVector.size()-1).c_str(), 1, AnimationType::BOTTOMTOTOP, this->fontRenderer, 4000, Colors::Green));
		else
			guiMan->Add((char*)addedNameVector.at(addedNameVector.size()-1).c_str(), new GUIText(&Rect(0, (screenHeight * -0.75)+ (100* additionCount) + MENUBUTTONS_TOP_OFFSET, 0 ,0),(wchar_t*)addedLongNameVector.at(addedLongNameVector.size()-1).c_str(), 1, AnimationType::BOTTOMTOTOP, this->fontRenderer, 4000, Colors::Red));
		guiMan->_guiElements[(char*)addedNameVector.at(addedNameVector.size()-1).c_str()]->SetDepth(0.5f);*/
		multiplayerClicked=true;
		additionCount++;
	}
}



RECT* LobbyScreen::GetRECTFromRect(Rect* rect)
{
	RECT* r = new RECT();
	r->left		= rect->x	-	TICKER_WIDTH;
	r->top		= rect->y	+	TICKER_TOP_OFFSET;
	r->right	= rect->x;						//End it where the text starts
	r->bottom	= rect->y	+	TICKER_HEIGHT; //End its y where the 
	return r;
}
