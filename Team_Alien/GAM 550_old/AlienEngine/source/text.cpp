#include "Precompiled.h"
#include "text.h"

namespace Framework
{
#define DEFAULT_LENGTH	20

Text::Text() : content(NULL), maxlength(DEFAULT_LENGTH)
{
}

Text::~Text()
{
	if ( content ) delete [] content;
	content = NULL;
}

void Text::Serialize(ISerializer& stream)
{
	stream.readValue("maxlength", maxlength );
}

void Text::Initialize()
{
	content = new char[maxlength + 1];
	ZeroMemory( content, maxlength + 1);
}
}



