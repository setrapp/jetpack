#ifndef _BULLETMANAGER_H_
#define _BULLETMANAGER_H_

#include "Bullet\src\btBulletDynamicsCommon.h"
#include "Entity.h"

class BulletManager
{
private:

	static BulletManager* instance;

	//Needed to initialize 
	///collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
	btDefaultCollisionConfiguration* collisionConfiguration;
	
	///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
	btCollisionDispatcher* dispatcher;
	
	///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
	btBroadphaseInterface* overlappingPairCache;
	
	///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
	btSequentialImpulseConstraintSolver* solver;

public:
	
	btDynamicsWorld* dynamicsWorld;

	//Player Construction Info
	btRigidBody::btRigidBodyConstructionInfo* playerConstructionInfo;

	//Track Construction Info
	btRigidBody::btRigidBodyConstructionInfo* trackConstructionInfo;

	//Player Collision Shape
	btCollisionShape* playerCollisionShape;

	//Track Collision Shape
	btBvhTriangleMeshShape* trackCollisionShape;

BulletManager();

BulletManager* Instance();

//End Bullet
void ExitPhysics();

//Callback
void Update(float dt);

//
~BulletManager();

//Construct Player's Rigid Body. 
//Implement box collider
//Rigid Body constructor needs a struct of parameters. 
//Collision manager creates that and stores it (for all other player objects that may be instantiated later).
void SetPlayerConstructionInfo(btCollisionShape* playerShape,btTransform& startTransform,btScalar mass,const btVector3& localInertia);

btRigidBody::btRigidBodyConstructionInfo* GetPlayerConstructionInfo();

//Construct Track Rigid Body (yet to be implemented)
//Needs a mesh collider.
void SetTrackConstructionInfo(Entity* track);

btRigidBody::btRigidBodyConstructionInfo* GetTrackConstructionInfo();

//Covert XMVector3 to btVector3
void MoveRigidBodyWithEntity(XMFLOAT3& playerTranslation, btRigidBody* entityBody);


//Create rigid body from entity and model.

//
};
#endif // !_BULLETINIT_H_