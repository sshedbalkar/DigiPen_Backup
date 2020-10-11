#pragma once

namespace Framework
{

class Font
{
	struct LetterData
	{
		float left, right, size, y;
	};

	LetterData letters[95];

public:
	void Initialize( const char* path );
	void Apply( const char* sentence, float size, float aspect, void* vdata );
	void Release();
};

}