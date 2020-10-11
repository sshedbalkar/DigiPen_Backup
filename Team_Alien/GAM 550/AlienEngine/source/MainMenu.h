#pragma once
//
#include "Singleton.h"
#include "ReturnCodes.h"
//
namespace Framework
{
	class SimpleButton;
	//
	class MainMenu: public Singleton<MainMenu>
	{
	public:
		MainMenu( int wid, int hgt );
		~MainMenu();
		//
	public:
		int				Show( int posx, int posy );
		int				Hide();
		bool			IsOpen();
		//
	private:
		void			Create();
		void			Destroy();
		void			StartCallback( unsigned int id, int state );
		void			ExitCallback( unsigned int id, int state );
		void			OptionsCallback( unsigned int id, int state );
		//
	private:
		SimpleButton*	m_btnStart;
		SimpleButton*	m_btnExit;
		SimpleButton*	m_btnOptions;
		unsigned int	m_Idbg;
		int				m_posX;
		int				m_posY;
		int				m_width;
		int				m_height;
		bool			m_open;
	};
}