#ifndef _BULLETMANAGER_H_
#define _BULLETMANAGER_H_


#include "Bullet\src\btBulletDynamicsCommon.h"
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

btDiscreteDynamicsWorld* dynamicsWorld;
BulletManager();

//Initialize Bullet Discrete Dynamics World
void BulletInit();

void Update(float dt);

~BulletManager();

//add these somewhere:
//dynamicsWorld->setGravity(btVector3(0,-10,0));
};
#endif // !_BULLETINIT_H_