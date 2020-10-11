#pragma once //Makes sure this header is only included once


namespace Framework
{
	
	class ActivateObject
	{
	public:
		ActivateObject::ActivateObject():m_size(30)
		{
			m_buttons[0]=1;
			for(int  i=1;i<m_size;++i)
			{
				m_buttons[i]=0;
			}
		
		}
		~ActivateObject(){};

		void Update(float t);
		int m_buttons[30];

	private:
		int m_size;
	};
}