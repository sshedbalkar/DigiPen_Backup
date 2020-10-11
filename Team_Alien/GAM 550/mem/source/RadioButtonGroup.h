#pragma once
//
#include "ReturnCodes.h"
#define MAX_RADIO_BUTTONS	16
//
namespace Framework
{
	class RadioButton;
	//
	class RadioButtonGroup
	{
	public:
		RadioButtonGroup( unsigned max );
		~RadioButtonGroup();
		//
	public:
		int				Init( const char* img_cbSelect, const char* img_cbDeselect, const char** imgs, int posX, int posY );
		//
	private:
		void			Callback( unsigned id, bool val );
	private:
		unsigned		m_max;
		RadioButton*	m_btns[MAX_RADIO_BUTTONS];
		unsigned		m_selected;
	};
}