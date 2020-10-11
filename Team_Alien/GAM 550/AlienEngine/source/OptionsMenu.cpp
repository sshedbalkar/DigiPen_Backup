#include <fstream>
#include <iostream>
//
#include "OptionsMenu.h"
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
#include "Checkbox.h"
#include "RadioButtonGroup.h"
#include "MainMenu.h"
//
namespace Framework
{
	MenuOptions::MenuOptions()
		:Resolution( 1 ),
		Vsync( false ),
		Antialiasing( false ),
		Windowed( false )
	{
		//
	}
	////////////////////////////////////////////////////
	OptionsMenu::OptionsMenu( int wid, int hgt )
		:m_Idbg( 0 ),
		m_IdRes( 0 ),
		m_btnApply( 0 ),
		m_btnBack( 0 ),
		m_width( wid ),
		m_height( hgt ),
		m_posX( 0 ),
		m_posY( 0 ),
		m_resolution( 0 ),
		m_aa( 0 ),
		m_vsync( 0 ),
		m_windowed( 0 )
	{
		//
	}
	//
	OptionsMenu::~OptionsMenu()
	{
		Destroy();
	}
	//
	int OptionsMenu::Show( int posx, int posy )
	{
		m_posX = posx;
		m_posY = posy;
		LOGIC->Pause( true );
		ReadFile( MENU_FILE_PATH, m_options );
		GRAPHICS->TurnOnDebugCamera();
		Create();
		SetOptions();
		//
		return RET_SUCCESS;
	}
	//
	int OptionsMenu::Hide()
	{
		Destroy();
		GRAPHICS->TurnOffDebugCamera();
		LOGIC->Pause( false );
		//
		return RET_SUCCESS;
	}
	//
	void OptionsMenu::Create()
	{
		int w, h;
		int sx = GRAPHICS->screen_width, sy = GRAPHICS->screen_height;
		m_Idbg = g_textureDraw->CreateTexture( "Assets\\textures\\optionsbg.png" );
		g_textureDraw->SetDimensions( m_Idbg, sx, sy );
		g_textureDraw->SetPosition( m_Idbg, 0.0f, 0.0f );
		//
		m_btnApply = new SimpleButton( 1 );
		m_btnApply->Init( "Assets\\textures\\apply1.png","Assets\\textures\\apply2.png", "Assets\\textures\\apply3.png", 100, 100 );
		m_btnApply->AddCallback( &OptionsMenu::ApplyCallback, this );
		w = m_btnApply->Width();
		h = m_btnApply->Height();
		m_btnApply->SetPosition( (int(m_posX - (w*0.5f) - 200.0f)), int(m_posY + 250.0f ));
		//
		m_btnBack = new SimpleButton( 2 );
		m_btnBack->Init( "Assets\\textures\\back1.png","Assets\\textures\\back2.png", "Assets\\textures\\back3.png", 100, 100 );
		m_btnBack->AddCallback( &OptionsMenu::BackCallback, this );
		w = m_btnBack->Width();
		h = m_btnBack->Height();
		m_btnBack->SetPosition( int(m_posX - (w*0.5f) + 200.0f), int(m_posY + 250.0f) );
		//
		m_aa = new Checkbox( 3 );
		m_aa->Init( "Assets\\textures\\checked.png", "Assets\\textures\\checkbox.png", 
			"Assets\\textures\\aa.png", "Assets\\textures\\aa.png", "Assets\\textures\\aa.png", 
			m_posX + 160, m_posY - 100 );
		//
		m_vsync = new Checkbox( 4 );
		m_vsync->Init( "Assets\\textures\\checked.png", "Assets\\textures\\checkbox.png", 
			"Assets\\textures\\vsync.png", "Assets\\textures\\vsync.png", "Assets\\textures\\vsync.png", 
			m_posX + 160, m_posY - 20 );
		//
		m_windowed = new Checkbox( 5 );
		m_windowed->Init( "Assets\\textures\\checked.png", "Assets\\textures\\checkbox.png", 
			"Assets\\textures\\windowed.png", "Assets\\textures\\windowed.png", "Assets\\textures\\windowed.png", 
			m_posX + 160, m_posY + 60 );
		//
		m_IdRes = g_textureDraw->CreateTexture( "Assets\\textures\\resolution.png" );
		g_textureDraw->GetDimensions( m_IdRes, w, h );
		g_textureDraw->SetDimensions( m_IdRes, w, h );
		g_textureDraw->SetPosition( m_IdRes,m_posX - 450, m_posY - 105 );
		const char* imgs[] =
		{
			"Assets\\textures\\1080p.png", "Assets\\textures\\1080p.png", "Assets\\textures\\1080p.png", 
			"Assets\\textures\\720p.png", "Assets\\textures\\720p.png", "Assets\\textures\\720p.png", 
			"Assets\\textures\\1024p.png", "Assets\\textures\\1024p.png", "Assets\\textures\\1024p.png"
		};
		m_resolution = new RadioButtonGroup( 3 );
		m_resolution->Init( "Assets\\textures\\radiochecked.png", "Assets\\textures\\radio.png", imgs, m_posX - 460, m_posY - 40 );
		//m_resolution->SetSelected( 0 );
	}
	//
	int OptionsMenu::SetOptions()
	{
		m_aa->Selected( m_options.Antialiasing );
		m_vsync->Selected( m_options.Vsync );
		m_windowed->Selected( m_options.Windowed );
		m_resolution->SetSelected( m_options.Resolution );
		//
		return RET_SUCCESS;
	}
	//
	int OptionsMenu::GetOptions()
	{
		m_options.Antialiasing	= m_aa->IsSelected();
		m_options.Vsync			= m_vsync->IsSelected();
		m_options.Windowed		= m_windowed->IsSelected();
		m_options.Resolution	= m_resolution->GetSelected();
		//
		return RET_SUCCESS;
	}
	//
	void OptionsMenu::Destroy()
	{
		if( m_Idbg )
		{
			g_textureDraw->DeleteTexture( m_Idbg );
			m_Idbg = 0;
		}
		//
		if( m_IdRes )
		{
			g_textureDraw->DeleteTexture( m_IdRes );
			m_IdRes = 0;
		}
		//
		utility::safeDelete( m_btnBack );
		utility::safeDelete( m_btnApply );
		utility::safeDelete( m_resolution );
		utility::safeDelete( m_aa );
		utility::safeDelete( m_vsync );
		utility::safeDelete( m_windowed );
	}
	//
	void OptionsMenu::ApplyCallback( unsigned int id, int state )
	{
		//LOGIC->Pause(false);
		//AUDIO->Play("main");
		std::cout<<"Apply clicked\n";
		GetOptions();
		WriteFile( MENU_FILE_PATH, m_options );
		Hide();
		GRAPHICS->SetGraphicsOptions( &m_options );
		Show( (int)GRAPHICS->screen_width*0.5f, (int)GRAPHICS->screen_height*0.5f );
	}
	//
	void OptionsMenu::BackCallback( unsigned int id, int state )
	{
		std::cout<<"Back clicked\n";
		Hide();
		g_mainMenu->Show( (int)GRAPHICS->screen_width*0.5f, (int)GRAPHICS->screen_height*0.5f );
	}
	//
	int OptionsMenu::ReadFile( const char* file, MenuOptions& opt )
	{
		XMLReader *reader = new XMLReader();
		if(reader->open(file))
		{
			ISerializer* str = reader->data().clone();
			int v;
			str->readProperty( "options", *str );
			//
			str->readValue( "resolution", v );
			opt.Resolution = v;
			//
			str->readValue( "antialiasing", v );
			opt.Antialiasing = ( v == 0 )? false : true;
			//
			str->readValue( "vsync", v );
			opt.Vsync = ( v == 0 )? false : true;
			//
			str->readValue( "windowed", v );
			opt.Windowed = ( v == 0 )? false : true;
			//
			delete str;
		}
		delete reader;
		//
		return RET_SUCCESS;
	}
	//
	int OptionsMenu::WriteFile( const char* file, MenuOptions& opt )
	{
		const int len=16;
		char str[len];
		//
		std::fstream os;
		os.open ( file, std::ios::binary | std::fstream::out | std::fstream::trunc );
		//
		if( os.is_open() )
		{
			XMLSerializer root;
			ISerializer *str1;
			root.allocateNew();
			ISerializer* stream = root.clone();
			str1 = stream;
			stream->writeNode( "options" );
			stream->readProperty( "options", *stream );
			//
			sprintf_s( str, len, "%d",  opt.Resolution );
			stream->writeNodeAndValue( "resolution", str );
			//
			sprintf_s( str, len, "%d",  opt.Antialiasing );
			stream->writeNodeAndValue( "antialiasing", str );
			//
			sprintf_s( str, len, "%d",  opt.Vsync );
			stream->writeNodeAndValue( "vsync", str );
			//
			sprintf_s( str, len, "%d",  opt.Windowed );
			stream->writeNodeAndValue( "windowed", str );
			//
			XMLSerializer::saveNodeToStream( root, &os );
			os.close();
			delete str1;
		}
		else
		{
			std::cout<<"OptionsMenu: Can't save file: " <<file<<"\n";
		}
		return RET_SUCCESS;
	}
}