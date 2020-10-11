#pragma once

namespace Framework
{


class ParamData
{
	char* data;
	char* ptr;
public:
	ParamData();
	~ParamData();

	int ParamInt();
	float ParamFloat();
	const char* ParamString();
	bool ParseParam( std::stringstream& str, const char* format );
};

typedef void(*CmdFunction)( ParamData* );

class Cmd
{
	struct CallbackFunc 
	{
		CmdFunction func;
		std::string format;
	};
	typedef std::map<std::string, CallbackFunc> FuncMapType;
private:
	FuncMapType functions;
	std::string current_cmd;
	std::string prev_cmd;
public:
	bool enable;
public:
	Cmd();
	void CharacterInput( char c );
	void RegisterFunc( const char* name, CmdFunction func, const char* format );
	const char* GetStringCmd();
private:
	void ProcessCmd();
};

}
