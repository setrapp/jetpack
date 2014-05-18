#include "BulletManager.h"

BulletManager::BulletManager()
{
}


void BulletManager::BulletInit()
{
//Initializations:

//collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
collisionConfiguration = new btDefaultCollisionConfiguration();

//use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
dispatcher = new	btCollisionDispatcher(collisionConfiguration);

//btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
overlappingPairCache = new btDbvtBroadphase();

//the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
solver = new btSequentialImpulseConstraintSolver();

//Initialize Discrete Dynamics World
dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher,overlappingPairCache,solver,collisionConfiguration);
dynamicsWorld->setGravity(btVector3(0,-10,0));
}

void BulletManager::Update(float dt)
{
	//dynamicsWorld->stepSimulation(dt);
}

BulletManager::~BulletManager()
{
}
