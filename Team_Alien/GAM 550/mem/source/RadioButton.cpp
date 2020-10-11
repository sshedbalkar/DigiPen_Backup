#include "RadioButton.h"
#include <iostream>
#include "Global.h"
#include "TextureDraw.h"
#include "InteractiveObject.h"
#include "Utility.h"
//
namespace Framework
{
	RadioButton::RadioButton( unsigned int id )
		:m_id( id ),
		m_active( true ),
		m_interactor( 0 ),
		m_selected( false ),
		m_callback( 0 )
	{
		//
	}
	//
	RadioButton::~RadioButton()
	{
		utility::safeDelete( m_callback );
		utility::safeDelete( m_interactor );
		//
		g_textureDraw->DeleteTexture( m_idUp );
		g_textureDraw->DeleteTexture( m_idOver );
		g_textureDraw->DeleteTexture( m_idDown );
		g_textureDraw->DeleteTexture( m_idCBSelect );
		g_textureDraw->DeleteTexture( m_idCBDeselect );
	}
	//
	int RadioButton::Init( const char* img_cbSelect, const char* img_cbDeselect, const char* img_Up, const char* img_Over, const char* img_Down, int posX, int posY )
	{
		int w, h;
		m_idUp = g_textureDraw->CreateTexture( img_Up );
		g_textureDraw->SetPosition( m_idUp, (float)posX, (float)posY );
		g_textureDraw->SetDisplay( m_idUp, true );
		//
		m_idOver = g_textureDraw->CreateTexture( img_Over );
		g_textureDraw->SetPosition( m_idOver, (float)posX,(float) posY );
		g_textureDraw->SetDisplay( m_idOver, false );
		//
		m_idDown = g_textureDraw->CreateTexture( img_Down );
		g_textureDraw->SetPosition( m_idDown, (float)posX, (float)posY );
		g_textureDraw->SetDisplay( m_idDown, false );
		//
		g_textureDraw->GetDimensions( m_idUp, m_width, m_height );
		//
		m_idCBDeselect = g_textureDraw->CreateTexture( img_cbDeselect );
		g_textureDraw->GetDimensions( m_idCBDeselect, w, h );
		m_offsetX = 10;
		m_offsetY = (int) ((m_height - h) * 0.5f);
		g_textureDraw->SetPosition( m_idCBDeselect, (float)posX + m_offsetX, (float)posY + m_offsetY );
		g_textureDraw->SetDisplay( m_idCBDeselect, true );
		//
		m_idCBSelect = g_textureDraw->CreateTexture( img_cbSelect );
		g_textureDraw->SetPosition( m_idCBSelect, (float)posX + m_offsetX, (float)posY + m_offsetY );
		g_textureDraw->SetDisplay( m_idCBSelect, false );
		//
		m_interactor = new InteractiveObject( 1, posX, posY, m_width, m_height );
		m_interactor->AddCallback( &RadioButton::InteractorCallback, this );
		//
		m_state = m_interactor->State();
		m_idActive = m_idUp;
		//
		return RET_SUCCESS;
	}
	//
	void RadioButton::InteractorCallback( unsigned int id, int state )
	{
		
		if( state == SimpleButton::MOUSE_OUT )
		{
			m_state = state;
			g_textureDraw->SetDisplay( m_idActive, false );
			g_textureDraw->SetDisplay( m_idUp, true );
			m_idActive = m_idUp;
		}
		else if( state == SimpleButton::MOUSE_OVER )
		{
			if( m_state != SimpleButton::LEFT_CLICK )
			{
				m_state = state;
				g_textureDraw->SetDisplay( m_idActive, false );
				g_textureDraw->SetDisplay( m_idOver, true );
				m_idActive = m_idOver;
			}
		}
		else if( state == SimpleButton::LEFT_CLICK )
		{
			m_state = state;
			g_textureDraw->SetDisplay( m_idActive, false );
			g_textureDraw->SetDisplay( m_idDown, true );
			m_idActive = m_idDown;
			//
			ChangeSelected( true );
			CallCallback( m_selected );
		}
		else if( state == SimpleButton::RIGHT_CLICK )
		{

		}
	}
	//
	void RadioButton::CallCallback( bool val )
	{
		if( m_callback )
		{
			utility::ArgumentVector v1;
			v1.push_back( m_id );
			v1.push_back( val );
			m_callback->Invoke( m_obj, v1 );
		}
	}
	//
	bool RadioButton::IsSelected()
	{
		return m_selected;
	}
	//
	void RadioButton::ChangeSelected( bool val )
	{
		if( m_selected != val )
		{
			m_selected = val;
			if( val )
			{
				g_textureDraw->SetDisplay( m_idCBDeselect, false );
				g_textureDraw->SetDisplay( m_idCBSelect, true );
			}
			else
			{
				g_textureDraw->SetDisplay( m_idCBSelect, false );
				g_textureDraw->SetDisplay( m_idCBDeselect, true );
			}
		}
	}
	//
	int RadioButton::SetPosition( int posX, int posY )
	{
		g_textureDraw->SetPosition( m_idUp, (float)posX, (float)posY );
		g_textureDraw->SetPosition( m_idOver, (float)posX, (float)posY );
		g_textureDraw->SetPosition( m_idDown, (float)posX,(float) posY );
		g_textureDraw->SetPosition( m_idCBSelect, (float)posX + m_offsetX,(float) posY + m_offsetY );
		g_textureDraw->SetPosition( m_idCBDeselect, (float)posX + m_offsetX,(float) posY + m_offsetY );
		m_interactor->SetPosition( posX, posY );
		//
		return RET_SUCCESS;
	}
	//
	int RadioButton::Active( bool val )
	{
		m_active = val;
		m_interactor->Active( m_active );
		//
		return RET_SUCCESS;
	}
	//
	int RadioButton::Width()
	{
		return m_width;
	}
	//
	int RadioButton::Height()
	{
		return m_height;
	}
	//
	int RadioButton::State()
	{
		return m_state;
	}
	//
	void RadioButton::Selected( bool val )
	{
		ChangeSelected( val );
	}
	//
	void RadioButton::State( int st )
	{
		if( st == SimpleButton::MOUSE_OUT )
		{
			g_textureDraw->SetDisplay( m_idActive, false );
			g_textureDraw->SetDisplay( m_idUp, true );
			m_idActive = m_idUp;
			m_state = st;
		}
		else if( st == SimpleButton::MOUSE_OVER )
		{
			g_textureDraw->SetDisplay( m_idActive, false );
			g_textureDraw->SetDisplay( m_idOver, true );
			m_idActive = m_idOver;
			m_state = st;
		}
		else if( st == SimpleButton::LEFT_CLICK )
		{
			g_textureDraw->SetDisplay( m_idActive, false );
			g_textureDraw->SetDisplay( m_idDown, true );
			m_idActive = m_idDown;
			m_state = st;
		}
		else if( st == SimpleButton::RIGHT_CLICK )
		{

		}
	}

}