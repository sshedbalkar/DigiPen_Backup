#ifndef ILEVEL_H
#define ILEVEL_H
//
namespace Framework
{
	enum GAME_STATE
	{
		LEVEL_RESTART,
		LEVEL_QUIT
	};
	//
	class ILevel
	{
	public:
		std::string filepath;

	};
	//
}
//
#endif