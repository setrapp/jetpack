#include "BulletManager.h"

BulletManager* BulletManager::instance = NULL;


BulletManager::BulletManager()
{
	if (!instance)
		instance = this;


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

	//Temporary Sphere Shape collider for player rigid body
	/*btCollisionShape* sphericalBody = new btSphereShape(btScalar(1.));
	btTransform startTransform;
	startTransform.setOrigin(btVector3(0,0,0));
	btScalar mass=1.0;
	btVector3 localInertia(0,0,0);
	sphericalBody->calculateLocalInertia(mass,localInertia);
	this->SetPlayerConstructionInfo(sphericalBody,startTransform,mass,localInertia);*/
}

//Get instance
BulletManager* BulletManager::Instance()
{
	return instance;
}


//Construct Player's Rigid Body. 
//Implement box collider
//Rigid Body constructor needs a struct of parameters. 
//Collision manager creates that and stores it (for all other player objects that may be instantiated later).
void BulletManager::SetPlayerConstructionInfo(btCollisionShape* playerShape,btTransform& startTransform,btScalar mass,const btVector3& localInertia)
{
	//Gathering Rigid Body Construction Info
	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	playerConstructionInfo = new btRigidBody::btRigidBodyConstructionInfo(mass,myMotionState,playerShape,localInertia);
}

btRigidBody::btRigidBodyConstructionInfo* BulletManager::GetPlayerConstructionInfo()
{
	return playerConstructionInfo;
}

//Construct Track Rigid Body (yet to be implemented)
//Needs a mesh collider.
void BulletManager::SetTrackConstructionInfo(Entity* track)
{
	trackCollisionShape = track->phys_entityPhysicsData->entityMeshShape;
}

btRigidBody::btRigidBodyConstructionInfo* BulletManager::GetTrackConstructionInfo()
{
	return trackConstructionInfo;
}

//Temporarily moving rigid body with the entity. Needs to be reversed
void BulletManager::MoveRigidBodyWithEntity(XMFLOAT3& playerTranslation, btRigidBody* entityBody)
{
	btTransform temp;
	temp.setOrigin(btVector3(playerTranslation.x, playerTranslation.y, playerTranslation.z));
	entityBody->getMotionState()->setWorldTransform(temp);
}

BulletManager::~BulletManager()
{
	if (instance == this)
		instance = NULL;

	//World Physics Info
	delete collisionConfiguration;
	delete dispatcher;
	delete overlappingPairCache;
	delete solver;
	//delete dynamicsWorld;

	//Rigid Bodies' Info
	delete playerConstructionInfo;
	delete trackConstructionInfo;
	delete playerCollisionShape;
	delete trackCollisionShape;
}

// Non-class functions
bool CollisionProcessed(btManifoldPoint& collisionPoint, void* body0, void* body1)
{
	int a = 0;
	return true;
}