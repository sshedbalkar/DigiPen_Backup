
namespace Framework
{
	
	class ObjectActivator
	{
	public:
		ObjectActivator::ObjectActivator():m_size(30)
		{
			m_buttons[0]=1;
			for(int  i=1;i<m_size;++i)
			{
				m_buttons[i]=0;
			}
		
		}
		~ObjectActivator(){};

		void Update(float t);
		int m_buttons[100];

	private:
		int m_size;
	};
}