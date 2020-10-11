#include "Contacts.h"

#ifndef COLLISION_H
#define COLLISION_H


namespace Framework
{


class IntersectionTests;
class CollisionDetector;

class CollisionPrimitive : public GameComponent
{
public:
    friend IntersectionTests;
    friend CollisionDetector;

	enum ShapeId
	{
		Sphere,
		Box,
		NumberOfShapes
	};
	ShapeId Id;
	CollisionPrimitive(){}
	CollisionPrimitive(ShapeId id) : Id(id){}

	RigidBody *body;
	
	//The offset of this primitive from the given rigid body. ??? neva used
	Matrix4 offset;

	virtual void Initialize();
	virtual void Serialize(ISerializer& stream);
	virtual void SendMessage(Message *m);

	CollisionPrimitive * Next;
	CollisionPrimitive * Prev;

	void calculateInternals();
	
	Vector3 getAxis(unsigned index) const
    {
        return transform.getAxisVector(index);
    }

	const Matrix4& getTransform() const
    {
        return transform;
    }

protected:
	Matrix4 transform;

};

class CollisionSphere : public CollisionPrimitive
{
public:
	CollisionSphere() : CollisionPrimitive(Sphere){}
	virtual void Serialize(ISerializer& stream);
    real radius;
};

class CollisionBox : public CollisionPrimitive
{
public:
	CollisionBox():CollisionPrimitive(Box){}
	virtual void Serialize(ISerializer& stream);
    Vector3 halfSize;
};

class CollisionPlane
{
public:
    Vector3 direction;
	//offset from the origin
	real offset;
};

class Ray
{
public:
	Ray(Vector3 pos,Vector3 dir):position(pos),direction(dir){ direction.normalise(); }
	Vector3 position;
	Vector3 direction;

	void normalizeDirection()
	{
		direction.normalise();
	}
};


class IntersectionTests
{
public:

    static bool sphereAndHalfSpace(const CollisionSphere &sphere, const CollisionPlane &plane);
	static bool sphereAndSphere(const CollisionSphere &one, const CollisionSphere &two);
	static bool boxAndBox(const CollisionBox &one, const CollisionBox &two);
	//box and plane that is only one sided 
    static bool boxAndHalfSpace(const CollisionBox &box, const CollisionPlane &plane);
};


//structure used to build the contact data
struct CollisionData
{
    Contact *contactArray;

    /** Holds the contact array to write into. */
    Contact *contacts;

    int contactsLeft;
    unsigned contactCount;

	//update these two to use the material properties later
    real friction;
    real restitution;

    /**
    * Holds the collision tolerance, even uncolliding objects this
    * close should have collisions generated.
    */
    real tolerance;

    bool hasMoreContacts()
    {
        return contactsLeft > 0;
    }

    void reset(unsigned maxContacts)
    {
        contactsLeft = maxContacts;
        contactCount = 0;
        contacts = contactArray;
    }

    void addContacts(unsigned count)
    {
        // Reduce the number of contacts remaining, add number used
        contactsLeft -= count;
        contactCount += count;

        // Move the array forward
        contacts += count;
    }
};



class CollisionDetector
{
public:

    static unsigned sphereAndHalfSpace(const CollisionSphere &sphere,const CollisionPlane &plane,CollisionData *data);
	static unsigned sphereAndTruePlane(const CollisionSphere &sphere,const CollisionPlane &plane,CollisionData *data);
    static unsigned boxAndHalfSpace(const CollisionBox &box,const CollisionPlane &plane,CollisionData *data);

    static unsigned sphereAndSphere(const CollisionSphere &one,const CollisionSphere &two,CollisionData *data);
    static unsigned boxAndBox(const CollisionBox &one,const CollisionBox &two,CollisionData *data);
    static unsigned boxAndPoint(const CollisionBox &box,const Vector3 &point,CollisionData *data);
    static unsigned boxAndSphere(const CollisionBox &box,const CollisionSphere &sphere,CollisionData *data);

	static unsigned rayCastSphere(const CollisionSphere &sphere,const Ray &ray,Vector3 &pointOfIntersection);
	static unsigned rayCastBox(const CollisionBox &box,const Ray &ray,Vector3 &pointOfIntersection);
};


}

#endif