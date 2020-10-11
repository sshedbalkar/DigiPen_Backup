#include <iostream>
//
#include "RadioButtonGroup.h"
#include "RadioButton.h"
//
namespace Framework
{
	RadioButtonGroup::RadioButtonGroup( unsigned max )
		: m_max( max ),
		m_selected( 0 )
	{
		//
	}
	//
	RadioButtonGroup::~RadioButtonGroup()
	{
		for( unsigned i = 0; i < m_max; ++i )
		{
			delete m_btns[i];
		}
	}
	//
	int RadioButtonGroup::Init( const char* img_cbSelect, const char* img_cbDeselect, const char** imgs, int posX, int posY )
	{
		RadioButton* rb;
		for( unsigned i = 0; i < m_max; ++i )
		{
			rb = new RadioButton( i );
			rb->Init( img_cbSelect, img_cbDeselect, imgs[i*3], imgs[(i*3)+1], imgs[(i*3)+2], posX, posY + (i*100) );
			rb->AddCallback( &RadioButtonGroup::Callback, this );
			m_btns[i] = rb;
		}
		int h = rb->Height();
		for( unsigned i = 0; i < m_max; ++i )
		{
			rb = m_btns[i];
			rb->SetPosition( posX, posY + (i*(h+10)) );
		}
		//
		return RET_SUCCESS;
	}
	//
	void RadioButtonGroup::Callback( unsigned id, bool val )
	{
		m_selected = id;
		for( unsigned i = 0; i < m_max; ++i )
		{
			if( i == m_selected ) continue;
			m_btns[i]->Selected( false );
		}
	}
}