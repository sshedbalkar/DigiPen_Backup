#include <fstream>
#include <iostream>
//
#include "MainMenu.h"
#include "Global.h"
#include "TextureDraw.h"
#include "Graphics.h"
#include "GameLogic.h"
#include "SimpleButton.h"
#include "Utility.h"
#include "GameLogic.h"
#include "Core.h"
#include "Audio.h"
#include "XMLSerialization.h"
#include "OptionsMenu.h"
//
namespace Framework
{
	////////////////////////////////////////////////////
	MainMenu::MainMenu( int wid, int hgt )
		:m_Idbg( 0 ),
		m_btnStart( 0 ),
		m_btnExit( 0 ),
		m_width( wid ),
		m_height( hgt ),
		m_posX( 0 ),
		m_posY( 0 ),
		m_open( false )
	{
		//
	}
	//
	MainMenu::~MainMenu()
	{
		Destroy();
	}
	//
	int MainMenu::Show( int posx, int posy )
	{
		m_open = true;
		m_posX = posx;
		m_posY = posy;
		LOGIC->Pause( true );
		GRAPHICS->TurnOnDebugCamera();
		Create();
		//
		return RET_SUCCESS;
	}
	//
	int MainMenu::Hide()
	{
		Destroy();
		GRAPHICS->TurnOffDebugCamera();
		LOGIC->Pause( false );
		m_open = false;
		//
		return RET_SUCCESS;
	}
	//
	void MainMenu::Create()
	{
		int w, h;
		int sx = GRAPHICS->screen_width, sy = GRAPHICS->screen_height;
		m_Idbg = g_textureDraw->CreateTexture( "Assets\\textures\\menu.png" );
		//g_textureDraw->GetDimensions( m_Idbg, w, h );
		g_textureDraw->SetPosition( m_Idbg, 0.0f, 0.0f );
		g_textureDraw->SetDimensions( m_Idbg, sx, sy );
		//
		m_btnStart = new SimpleButton( 1 );
		if( !GRAPHICS->IsPaused() )
		{
			m_btnStart->Init( "Assets\\textures\\start1.png","Assets\\textures\\start2.png", "Assets\\textures\\start3.png", 100, 100 );
		}
		else
		{
			m_btnStart->Init( "Assets\\textures\\Resume1.png","Assets\\textures\\Resume2.png", "Assets\\textures\\Resume3.png", 100, 100 );
		}
		m_btnStart->AddCallback( &MainMenu::StartCallback, this );
		w = m_btnStart->Width();
		h = m_btnStart->Height();
		m_btnStart->SetPosition( (int(m_posX + 200.0f)), int(m_posY - 10.0f) );
		//
		m_btnOptions = new SimpleButton( 3 );
		m_btnOptions->Init( "Assets\\textures\\options1.png","Assets\\textures\\options2.png", "Assets\\textures\\options3.png", 100, 100 );
		m_btnOptions->AddCallback( &MainMenu::OptionsCallback, this );
		w = m_btnOptions->Width();
		h = m_btnOptions->Height();
		m_btnOptions->SetPosition( int(m_posX + 200.0f), int(m_posY + 90.0f) );
		//
		m_btnExit = new SimpleButton( 2 );
		m_btnExit->Init( "Assets\\textures\\exit1.png","Assets\\textures\\exit2.png", "Assets\\textures\\exit3.png", 100, 100 );
		m_btnExit->AddCallback( &MainMenu::ExitCallback, this );
		w = m_btnExit->Width();
		h = m_btnExit->Height();
		m_btnExit->SetPosition( int(m_posX + 200.0f), int(m_posY + 190.0f) );
	}
	//
	bool MainMenu::IsOpen()
	{
		return m_open;
	}
	//
	void MainMenu::Destroy()
	{
		if( m_Idbg )
		{
			g_textureDraw->DeleteTexture( m_Idbg );
			m_Idbg = 0;
		}
		//
		utility::safeDelete( m_btnOptions );
		utility::safeDelete( m_btnExit );
		utility::safeDelete( m_btnStart );
	}
	//
	void MainMenu::StartCallback( unsigned int id, int state )
	{
		AUDIO->Play("main");
		Hide();
		GRAPHICS->SetStartGame();
		GRAPHICS->Unpause();
	}
	//
	void MainMenu::ExitCallback( unsigned int id, int state )
	{
		Hide();
		Message m(Mid::Quit);
		CORE->BroadcastMessage(&m);
	}
	//
	void MainMenu::OptionsCallback( unsigned int id, int state )
	{
		Hide();
		g_optionsMenu->Show( int(GRAPHICS->screen_width*0.5f), int(GRAPHICS->screen_height*0.5f) );
	}
}