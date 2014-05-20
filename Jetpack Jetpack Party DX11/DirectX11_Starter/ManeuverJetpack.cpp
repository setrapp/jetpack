#include "ManeuverJetpack.h"
#include "Common.h"
#include "Entity.h"
//#include "Player.h"
#include "SoundManager.h"
#include "AssetManager.h"

ManeuverJetpack::ManeuverJetpack(Entity* player) : Jetpack(player)
{
	maxSpeed = 10;
	fuelUseRate = 0;//10;
	forwardAcceleration = 4.0f;
	backwardAcceleration = 4.0f;
	strafeAcceleration = 4.0f;
	ascentAcceleration = 6.0f;
	backSpin = 30 * (PI / 180);
	frontSpin = 30 * (PI / 180);
	sideSpin = 0 * (PI / 180);
	bottomSpin = 10 * (PI / 180);
	actionsPerSecond = 1.9f;
	
	// Thrusters
	thrusterCount = 8;
	CreateThrusters();
	// Bottom Left
	thrusters[Thruster::BOTTOM_LEFT]->transform.SetLocalTranslation(XMFLOAT3(-0.022f, 0.025f, -0.0215f));
	thrusters[Thruster::BOTTOM_LEFT]->transform.SetLocalRotation(XMFLOAT3(0, 0, PI));
	// Bottom Right
	thrusters[Thruster::BOTTOM_RIGHT]->transform.SetLocalTranslation(XMFLOAT3(0.022f, 0.025f, -0.0215f));
	thrusters[Thruster::BOTTOM_RIGHT]->transform.SetLocalRotation(XMFLOAT3(0, 0, PI));
	// Back Left
	thrusters[Thruster::BACK_LEFT]->transform.SetLocalTranslation(XMFLOAT3(-0.013f, 0.082f, -0.05f));
	thrusters[Thruster::BACK_LEFT]->transform.SetLocalRotation(XMFLOAT3(-PI / 2, 0, 0));
	// Back Right
	thrusters[Thruster::BACK_RIGHT]->transform.SetLocalTranslation(XMFLOAT3(0.013f, 0.082f, -0.05f));
	thrusters[Thruster::BACK_RIGHT]->transform.SetLocalRotation(XMFLOAT3(-PI / 2, 0, 0));
	// Front Left
	thrusters[Thruster::FRONT_LEFT]->transform.SetLocalTranslation(XMFLOAT3(-0.028f, 0.103f, -0.005f));
	thrusters[Thruster::FRONT_LEFT]->transform.SetLocalRotation(XMFLOAT3(PI / 2, 0, 0));
	// Front Right
	thrusters[Thruster::FRONT_RIGHT]->transform.SetLocalTranslation(XMFLOAT3(0.028f, 0.103f, 0.005f));
	thrusters[Thruster::FRONT_RIGHT]->transform.SetLocalRotation(XMFLOAT3(PI / 2, 0, 0));
	// Side Left
	thrusters[Thruster::SIDE_LEFT]->transform.SetLocalTranslation(XMFLOAT3(-0.047f, 0.084f, -0.021f));
	thrusters[Thruster::SIDE_LEFT]->transform.SetLocalRotation(XMFLOAT3(0, 0, PI / 2));
	// Side Right
	thrusters[Thruster::SIDE_RIGHT]->transform.SetLocalTranslation(XMFLOAT3(0.047f, 0.084f, -0.021f));
	thrusters[Thruster::SIDE_RIGHT]->transform.SetLocalRotation(XMFLOAT3(0, 0, -PI / 2));
}

void ManeuverJetpack::Update(float dt, XMFLOAT3* velocity, XMFLOAT3* angularVelocity)
{
	// Estimate the propultion direction of each thruster.
	XMFLOAT3 transformRight = player->transform.GetRight(), transformUp = player->transform.GetUp(), transformForward = player->transform.GetForward();
	XMVECTOR right = XMLoadFloat3(&transformRight), up = XMLoadFloat3(&transformUp), forward = XMLoadFloat3(&transformForward);
	XMStoreFloat3(&thrusterDirections[Thruster::BOTTOM_LEFT], XMVector3Normalize(XMVectorAdd(XMVectorScale(up, 10), right)));
	thrusterDirections[Thruster::BOTTOM_RIGHT] = thrusterDirections[Thruster::BOTTOM_LEFT];
	thrusterDirections[Thruster::BOTTOM_RIGHT].x *= -1;
	XMStoreFloat3(&thrusterDirections[Thruster::BACK_LEFT], XMVector3Normalize(XMVectorAdd(XMVectorScale(forward, 2), right)));
	thrusterDirections[Thruster::BACK_RIGHT] = thrusterDirections[Thruster::BACK_LEFT];
	thrusterDirections[Thruster::BACK_RIGHT].x *= -1;
	XMStoreFloat3(&thrusterDirections[Thruster::FRONT_LEFT], XMVector3Normalize(XMVectorAdd(XMVectorScale(-forward, 2), right)));
	thrusterDirections[Thruster::FRONT_RIGHT] = thrusterDirections[Thruster::FRONT_LEFT];
	thrusterDirections[Thruster::FRONT_RIGHT].x *= -1;
	XMStoreFloat3(&thrusterDirections[Thruster::SIDE_LEFT], XMVector3Normalize(XMVectorAdd(XMVectorScale(right, 3), -up)));
	thrusterDirections[Thruster::SIDE_RIGHT] = thrusterDirections[Thruster::SIDE_LEFT];
	thrusterDirections[Thruster::SIDE_RIGHT].x *= -1;

	// Call base update.
	Jetpack::Update(dt, velocity, angularVelocity);
}

void ManeuverJetpack::CheckInput(float dt, XMFLOAT3* velocity, XMFLOAT3* angularVelocity)
{
	// TODO: These should use IPMan
	// Left Side
	if(GetAsyncKeyState('W'))//('A'))
	{
		thrusterActives[Thruster::FRONT_LEFT] = true;
	}
	if(GetAsyncKeyState('A'))//(('S'))
	{
		thrusterActives[Thruster::SIDE_LEFT] = true;
	}
	if(GetAsyncKeyState('S'))//(('D'))
	{
		thrusterActives[Thruster::BACK_LEFT] = true;
	}
	if(GetAsyncKeyState('D'))//(('F'))
	{
		thrusterActives[Thruster::BOTTOM_LEFT] = true;
	}
	// Right Side
	if(GetAsyncKeyState('J'))//(('J'))
	{
		thrusterActives[Thruster::BOTTOM_RIGHT] = true;
	}
	if(GetAsyncKeyState('K'))//(('K'))
	{
		thrusterActives[Thruster::BACK_RIGHT] = true;
	}
	if(GetAsyncKeyState('L'))//(('L'))
	{
		thrusterActives[Thruster::SIDE_RIGHT] = true;
	}
	if(GetAsyncKeyState('I'))//((VK_OEM_1)) // colon on US standard keyboards
	{
		thrusterActives[Thruster::FRONT_RIGHT] = true;
	}
}

void ManeuverJetpack::ApproachTarget(float dt, XMFLOAT3* velocity, XMFLOAT3* angularVelocity)
{
	XMVECTOR desiredAcceleration = XMLoadFloat3(&targetAcceleration);
	XMFLOAT3 thrustDotDir;
	float minDot = 0.7f;

	// Left Side
	XMStoreFloat3(&thrustDotDir, XMVector3Dot(desiredAcceleration, XMLoadFloat3(&thrusterDirections[Thruster::FRONT_LEFT])));
	if(thrustDotDir.x >= minDot)
	{
		thrusterActives[Thruster::FRONT_LEFT] = true;
	}
	XMStoreFloat3(&thrustDotDir, XMVector3Dot(desiredAcceleration, XMLoadFloat3(&thrusterDirections[Thruster::SIDE_LEFT])));
	if(thrustDotDir.x >= minDot)
	{
		thrusterActives[Thruster::SIDE_LEFT] = true;
	}
	XMStoreFloat3(&thrustDotDir, XMVector3Dot(desiredAcceleration, XMLoadFloat3(&thrusterDirections[Thruster::BACK_LEFT])));
	if(thrustDotDir.x >= minDot)
	{
		thrusterActives[Thruster::BACK_LEFT] = true;
	}
	XMStoreFloat3(&thrustDotDir, XMVector3Dot(desiredAcceleration, XMLoadFloat3(&thrusterDirections[Thruster::BOTTOM_LEFT])));
	if(thrustDotDir.x >= minDot)
	{
		thrusterActives[Thruster::BOTTOM_LEFT] = true;
	}
	// Right Side
	XMStoreFloat3(&thrustDotDir, XMVector3Dot(desiredAcceleration, XMLoadFloat3(&thrusterDirections[Thruster::BOTTOM_RIGHT])));
	if(thrustDotDir.x >= minDot)
	{
		thrusterActives[Thruster::BOTTOM_RIGHT] = true;
	}
	XMStoreFloat3(&thrustDotDir, XMVector3Dot(desiredAcceleration, XMLoadFloat3(&thrusterDirections[Thruster::BACK_RIGHT])));
	if(thrustDotDir.x >= minDot)
	{
		thrusterActives[Thruster::BACK_RIGHT] = true;
	}
	XMStoreFloat3(&thrustDotDir, XMVector3Dot(desiredAcceleration, XMLoadFloat3(&thrusterDirections[Thruster::SIDE_RIGHT])));
	if(thrustDotDir.x >= minDot)
	{
		thrusterActives[Thruster::SIDE_RIGHT] = true;
	}
	XMStoreFloat3(&thrustDotDir, XMVector3Dot(desiredAcceleration, XMLoadFloat3(&thrusterDirections[Thruster::FRONT_RIGHT])));
	if(thrustDotDir.x >= minDot)
	{
		thrusterActives[Thruster::FRONT_RIGHT] = true;
	}
}

void ManeuverJetpack::ApplyForces(float dt, XMFLOAT3* velocity, XMFLOAT3* angularVelocity)
{
	// TODO: These should use IPMan
	// Left Side

	if(thrusterActives[Thruster::FRONT_LEFT])
	{
		velocity->z -= backwardAcceleration * dt;
		angularVelocity->y -= frontSpin * dt;
		thrusters[Thruster::FRONT_LEFT]->SetVisible(true);
		active = true;
	}
	if(thrusterActives[Thruster::SIDE_LEFT])
	{
		velocity->x += strafeAcceleration * dt;
		angularVelocity->z -= sideSpin * dt;
		thrusters[Thruster::SIDE_LEFT]->SetVisible(true);
		active = true;
	}
	if(thrusterActives[Thruster::BACK_LEFT])
	{
		velocity->z += forwardAcceleration * dt;
		angularVelocity->y += backSpin * dt;
		thrusters[Thruster::BACK_LEFT]->SetVisible(true);
		active = true;
	}
	if(thrusterActives[Thruster::BOTTOM_LEFT])
	{
		velocity->y += ascentAcceleration * dt;
		angularVelocity->z -= bottomSpin * dt;
		thrusters[Thruster::BOTTOM_LEFT]->SetVisible(true);
		active = true;
	}
	// Right Side
	if(thrusterActives[Thruster::FRONT_RIGHT])
	{
		velocity->z -= backwardAcceleration * dt;
		angularVelocity->y += frontSpin * dt;
		thrusters[Thruster::FRONT_RIGHT]->SetVisible(true);
		active = true;
	}
	if(thrusterActives[Thruster::SIDE_RIGHT])
	{
		velocity->x -= strafeAcceleration * dt;
		angularVelocity->z += sideSpin * dt;
		thrusters[Thruster::SIDE_RIGHT]->SetVisible(true);
		active = true;
	}
	if(thrusterActives[Thruster::BACK_RIGHT])
	{
		velocity->z += forwardAcceleration * dt;
		angularVelocity->y -= backSpin * dt;
		thrusters[Thruster::BACK_RIGHT]->SetVisible(true);
		active = true;
	}
	if(thrusterActives[Thruster::BOTTOM_RIGHT])
	{
		velocity->y += ascentAcceleration * dt;
		angularVelocity->z += bottomSpin * dt;
		thrusters[Thruster::BOTTOM_RIGHT]->SetVisible(true);
		active = true;
	}
	
#ifndef FORCE_MUTE
	if(active)
		AssetManager::Instance()->GetSoundManager()->PlaySoundInstance(SoundId::THRUSTER, false, true, 0.5);
	else
		AssetManager::Instance()->GetSoundManager()->StopSoundInstance(SoundId::THRUSTER);
#endif
}

void ManeuverJetpack::updateThrusters(int thrusterData){

#pragma region ThrusterParsing
	if(thrusterData>=128){
		thrusterActives[Thruster::FRONT_LEFT]=true;
		thrusters[Thruster::FRONT_LEFT]->SetVisible(true);
		thrusterData-=128;
	}
	else{
		thrusterActives[Thruster::FRONT_LEFT]=false;
		thrusters[Thruster::FRONT_LEFT]->SetVisible(false);
	}

	if(thrusterData>=64){
		thrusterActives[Thruster::SIDE_LEFT]=true;
		thrusters[Thruster::SIDE_LEFT]->SetVisible(true);
		thrusterData-=64;
	}
	else{
		thrusterActives[Thruster::SIDE_LEFT]=false;
		thrusters[Thruster::SIDE_LEFT]->SetVisible(false);
	}

	if(thrusterData>=32){
		thrusterActives[Thruster::BACK_LEFT]=true;
		thrusters[Thruster::BACK_LEFT]->SetVisible(true);
		thrusterData-=32;
	}
	else{
		thrusterActives[Thruster::BACK_LEFT]=false;
		thrusters[Thruster::BACK_LEFT]->SetVisible(false);
	}

	if(thrusterData>=16){
		thrusterActives[Thruster::BOTTOM_LEFT]=true;
		thrusters[Thruster::BOTTOM_LEFT]->SetVisible(true);
		thrusterData-=16;
	}
	else{
		thrusterActives[Thruster::BOTTOM_LEFT]=false;
		thrusters[Thruster::BOTTOM_LEFT]->SetVisible(false);
	}

	if(thrusterData>=8){
		thrusterActives[Thruster::FRONT_LEFT]=true;
		thrusters[Thruster::FRONT_LEFT]->SetVisible(true);
		thrusterData-=8;
	}
	else{
		thrusterActives[Thruster::FRONT_RIGHT]=false;
		thrusters[Thruster::FRONT_RIGHT]->SetVisible(false);
	}

	if(thrusterData>=4){
		thrusterActives[Thruster::SIDE_RIGHT]=true;
		thrusters[Thruster::SIDE_RIGHT]->SetVisible(true);
		thrusterData-=4;
	}
	else{
		thrusterActives[Thruster::SIDE_RIGHT]=false;
		thrusters[Thruster::SIDE_RIGHT]->SetVisible(false);
	}

	if(thrusterData>=2){
		thrusterActives[Thruster::BACK_RIGHT]=true;
		thrusters[Thruster::BACK_RIGHT]->SetVisible(true);
		thrusterData-=2;
	}
	else{
		thrusterActives[Thruster::BACK_RIGHT]=false;
		thrusters[Thruster::BACK_RIGHT]->SetVisible(false);
	}

	if(thrusterData>=1){
		thrusterActives[Thruster::BOTTOM_RIGHT]=true;
		thrusters[Thruster::BOTTOM_RIGHT]->SetVisible(true);
	}
	else{
		thrusterActives[Thruster::BOTTOM_RIGHT]=false;
		thrusters[Thruster::BOTTOM_RIGHT]->SetVisible(false);
	}
#pragma endregion ThrusterParsing

}