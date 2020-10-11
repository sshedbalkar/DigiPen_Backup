#pragma once
//
#include "Singleton.h"
#include "ReturnCodes.h"
//
#define MENU_FILE_PATH "Objects\\MainMenu.xml"
//
namespace Framework
{
	class SimpleButton;
	class RadioButtonGroup;
	class Checkbox;
	//
	struct MenuOptions
	{
		unsigned	Resolution;
		bool		Vsync;
		bool		Antialiasing;
		bool		Windowed;
		//
		MenuOptions();
	};
	//
	class OptionsMenu: public Singleton<OptionsMenu>
	{
	public:
		OptionsMenu( int wid, int hgt );
		~OptionsMenu();
		//
	public:
		int				Show( int posx, int posy );
		int				Hide();
		//
	private:
		void			Create();
		void			Destroy();
		void			ApplyCallback( unsigned int id, int state );
		void			BackCallback( unsigned int id, int state );
		int				ReadFile( const char* file, MenuOptions& opt );
		int				WriteFile( const char* file, MenuOptions& opt );
		int				SetOptions();
		int				GetOptions();
		//
	private:
		SimpleButton*		m_btnApply;
		SimpleButton*		m_btnBack;
		RadioButtonGroup*	m_resolution;
		Checkbox*			m_aa;
		Checkbox*			m_vsync;
		Checkbox*			m_windowed;
		unsigned int		m_Idbg;
		unsigned int		m_IdRes;
		int					m_posX;
		int					m_posY;
		int					m_width;
		int					m_height;
		MenuOptions			m_options;
	};
}