#pragma once
//
#include "ReturnCodes.h"
#include "SimpleButton.h"
//
namespace Framework
{
	class InteractiveObject;
	//
	class Checkbox
	{
	public:
		Checkbox( unsigned int id );
		~Checkbox();
		//
	public:
		int							Init( const char* img_cbSelect, const char* img_cbDeselect, const char* img_Up, const char* img_Over, const char* img_Down, int posX, int posY );
		int							Active( bool val );
		int							SetPosition( int posX, int posY );
		int							Width();
		int							Height();
		int							State();
		void						State( int st );
		bool						IsSelected();
		void						Selected( bool val );
		//
	private:
		void						InteractorCallback( unsigned int id, int state );
		void						ChangeSelected( bool val );
		//
	private:
		unsigned int				m_id;
		InteractiveObject*			m_interactor;
		unsigned int				m_idUp;
		unsigned int				m_idOver;
		unsigned int				m_idDown;
		unsigned int				m_idActive;
		unsigned int				m_idCBSelect;
		unsigned int				m_idCBDeselect;
		bool						m_active;
		int							m_state;
		int							m_width;
		int							m_height;
		bool						m_selected;
		int							m_offsetX;
		int							m_offsetY;
	};
}