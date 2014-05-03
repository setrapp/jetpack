#ifndef _BULLETMANAGER_H_
#define _BULLETMANAGER_H_

#include "Bullet\src\btBulletDynamicsCommon.h"
#include "Entity.h"

class BulletManager
{
private:

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

BulletManager()
{
	//Physics
	{
		InitPhysics();
		btCollisionShape* sphericalBody = new btSphereShape(btScalar(1.));
		btTransform startTransform;
		startTransform.setOrigin(btVector3(0,0,0));
		btScalar mass=1.0;
		btVector3 localInertia(0,0,0);
		sphericalBody->calculateLocalInertia(mass,localInertia);
		this->SetPlayerConstructionInfo(sphericalBody,startTransform,mass,localInertia);
	}
}

//Initialize Bullet Discrete Dynamics World
void InitPhysics()
{
	//Initializations:

	//default collision configuration
	collisionConfiguration = new btDefaultCollisionConfiguration();
	
	//default collision dispatcher
	dispatcher = new	btCollisionDispatcher(collisionConfiguration);
	
	//btDbvtBroadphase is a good general purpose broadphase
	overlappingPairCache = new btDbvtBroadphase();
	
	//default constraint solver
	solver = new btSequentialImpulseConstraintSolver;
	
	//Initialize Discrete Dynamics World
	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher,overlappingPairCache,solver,collisionConfiguration);

	//Set default forces in the world
	dynamicsWorld->setGravity(btVector3(0,-10,0));
}


//Callback
void Update(float dt)
{
}

~BulletManager()
{

}

//Construct Player's Rigid Body
void SetPlayerConstructionInfo(btCollisionShape* playerShape,btTransform& startTransform,btScalar mass,const btVector3& localInertia)
{
	//Gathering Rigid Body Construction Info
	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	playerConstructionInfo = new btRigidBody::btRigidBodyConstructionInfo(mass,myMotionState,playerShape,localInertia);
}

btRigidBody::btRigidBodyConstructionInfo* GetPlayerConstructionInfo()
{
	return playerConstructionInfo;
}

//Construct Track Rigid Body (yet to be implemented)


//Covert XMVector3 to btVector3
void MoveRigidBodyWithEntity(XMFLOAT3& playerTranslation, btRigidBody* entityBody)
{
	btTransform temp;
	temp.setOrigin(btVector3(playerTranslation.x, playerTranslation.y, playerTranslation.z));
	entityBody->getMotionState()->setWorldTransform(temp);
}

//
};
#endif // !_BULLETINIT_H_