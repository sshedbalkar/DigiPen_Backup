#pragma once //Makes sure this header is only included once

#include "Composition.h"

namespace Framework
{
	///The transform component provides a shared position and rotation.
	class Text : public GameComponent
	{
	public:	
		Text();
		virtual ~Text();
		void Serialize(ISerializer& str);
		virtual void Initialize();

		int maxlength;
		char* content;
	};
}
