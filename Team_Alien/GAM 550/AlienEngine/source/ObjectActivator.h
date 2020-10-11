#pragma once //Makes sure this header is only included once


namespace Framework
{
	
	class ObjectActivator
	{
	public:
		ObjectActivator():m_size(50)
		{
			for(int  i=0;i<m_size;++i)
			{
				m_button2Toclear[i]=0;
			}
		
		}
		~ObjectActivator(){};

		void Update(const float &t);
		void UpdateButtons(const float &);
		void UpdateOtherObjects(const float &);
		void UpdateAttachedObjects(const float&);
		void InitializeAttachedObjects();
		void ClearButtonsState();
		

	private:
		const int m_size;
		int m_button2Toclear[50];
		
	};
}