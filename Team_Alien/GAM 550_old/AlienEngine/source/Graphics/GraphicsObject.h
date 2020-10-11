#include "Component.h"

namespace Framework
{

class Material;
class Transform;
class Text;
class Camera;
class AnimationController;
class GraphicsObject;

class GraphicsComposition : public GameComponent
{
	typedef std::vector<GraphicsObject*> GraphicsObjectListType;
	GraphicsObjectListType instance_list;

public:
	int flag;
	Transform* transform;
	Text* text;
	Camera* camera;
	AnimationController* animctrl;
	GraphicsComposition();
	virtual ~GraphicsComposition();

	virtual void Serialize( ISerializer& stream );
	virtual void Initialize();

	void AddGraphicsObject( GraphicsObject* go );
};

class GraphicsObject
{
	Material* material;
	GraphicsComposition* owner;
public:
	char* data;

	GraphicsObject* Prev;
	GraphicsObject* Next;
public:
	GraphicsObject( GraphicsComposition* _owner );
	~GraphicsObject();
	void Serialize( ISerializer& stream );
	void Initialize( Material* material, ISerializer* stream );
	int Flag();
	Transform* Transform();
	Text* Text();
	Camera* Camera();
	AnimationController* AnimCtrl();
};

}