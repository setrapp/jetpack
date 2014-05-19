#include "Player.h"
#include "AssetManager.h"

Player::Player()
{
	minPosture = 0.95f;
	respawnPosition = XMFLOAT3(0, 0, 0);
	respawnLocalRotation = XMFLOAT3(0, 0, 0);
	worldVelocity = XMFLOAT3(0, 0, 0);
	angularVelocity = XMFLOAT3(0, 0, 0);
	maxSpeed = 200;
	playerName="";
	forwardAcceleration = 100.0f;
	backwardAcceleration = 100.0f;
	strafeAcceleration = 100.0f;
	gravityAcceleration = 600.0f;
	terminalVelocity = 500000;
	groundSpeedDampening = 0.1f;
	airSpeedDampening = 0.3f;

	groundSpeedDampening = 0.0f;
	airSpeedDampening = 0.0f;
	grounded = true;
	jetpack = new ManeuverJetpack(this);
	controllable = false;
	targetPosition = XMFLOAT3(400, 200, 5000);
}

Player::~Player()
{
	delete jetpack;
}

void Player::Update(float dt)
{
	//TODO account for networked players.
	jetpack->playerControllable = controllable;
	jetpack->playerAI = !controllable;

	XMFLOAT3 velocity = transform.InverseTransformDirection(worldVelocity);

	// Figure out the optimal acceleration to the target position.
	// TODO should only be done if AI controlled.
	XMVECTOR desiredDirection = XMVectorSubtract(XMLoadFloat3(&targetPosition), XMLoadFloat3(&transform.GetTranslation()));
	XMStoreFloat3(&jetpack->targetAcceleration, XMVectorSubtract(desiredDirection, XMLoadFloat3(&velocity)));

	// Check for user input.
	if (controllable)
	{
		CheckInput(dt);
	}
	// Update jetpack.

	jetpack->allowInputForces = !grounded; //TODO This might not be useful anymore
	jetpack->Update(dt, &velocity, &angularVelocity);

	// Clamp velocity within max speed.
	transform.ClampVector(&velocity, (grounded ? maxSpeed : jetpack->maxSpeed), 0);

	// Update world velocity and apply world space accelerations.
	worldVelocity = transform.TransformDirection(velocity);

	// TODO gravity should be handled by rigid body.
	XMFLOAT3 position = transform.GetTranslation();


	//if the y position is greater than 0, add the gravity vector to the existing y velocity.
	if (position.y > 0)
	{
		grounded = false;
		if (worldVelocity.y > -terminalVelocity)
		{
			XMFLOAT3 gravity = XMFLOAT3(0, worldVelocity.y - (gravityAcceleration * dt), 0);
			transform.ClampVector(&gravity, terminalVelocity, 0);
			worldVelocity.y = gravity.y;
		}
	} 
	//if you are below 0, handle situation for when the ground is hit
	else if (worldVelocity.y <= 0 && (position.y < 0 || !grounded))
	{
		// When landing, either respawn or stand straight up, depending on standing direction.
		XMFLOAT3 upDot;
		XMStoreFloat3(&upDot, XMVector3Dot(XMLoadFloat3(&transform.GetUp()), XMLoadFloat3(&XMFLOAT3(0, 1, 0))));
		if (upDot.x < minPosture || jetpack->fuel <= 0) // TODO should only kill for lack of fuel if not landing on fuel station.
		{
			Respawn();
		}
		else
		{
			grounded = true;
			worldVelocity.y = 0;
			transform.SetLocalRotation(XMFLOAT3(0, transform.GetLocalEulerAngles().y, 0));
			angularVelocity = XMFLOAT3(0, 0, 0);
		}
	}
	velocity = transform.InverseTransformDirection(worldVelocity);

	// Apply world velocity.
	XMFLOAT3 dtVelocity;
	XMStoreFloat3(&dtVelocity, XMVectorScale(XMLoadFloat3(&velocity), dt));
	transform.Translate(dtVelocity);

	// Apply world angular velocity.
	XMFLOAT3 dtAngularVelocity;
	XMStoreFloat3(&dtAngularVelocity, XMVectorScale(XMLoadFloat3(&angularVelocity), dt));
	transform.Rotate(dtAngularVelocity);

	// Slow the character a bit so that it comes to a nice stop over time. (friction)
	if (grounded) {
		XMStoreFloat3(&velocity, XMVectorScale(XMLoadFloat3(&velocity), (1 - ((1 - groundSpeedDampening) * dt))));
		XMStoreFloat3(&angularVelocity, XMVectorScale(XMLoadFloat3(&angularVelocity), (1 - ((1 - groundSpeedDampening) * dt))));
	} else {
		XMStoreFloat3(&velocity, XMVectorScale(XMLoadFloat3(&velocity), (1 - ((1 - airSpeedDampening) * dt))));
		XMStoreFloat3(&angularVelocity, XMVectorScale(XMLoadFloat3(&angularVelocity), (1 - ((1 - airSpeedDampening) * dt))));
	}

	// Stop character if not moving much.
	XMFLOAT3 velMag;
	XMStoreFloat3(&velMag, XMVector3LengthSq(XMLoadFloat3(&velocity)));
	if (velMag.x > 0 && velMag.x < 0.001f)
	{
		velocity.x = velocity.y = velocity.z = 0;
	}
	// Stop character if not rotating much.
	XMFLOAT3 angVelMag;
	XMStoreFloat3(&angVelMag, XMVector3LengthSq(XMLoadFloat3(&angularVelocity)));
	if (angVelMag.x > 0 && angVelMag.x < 0.000000001f)
	{
		angularVelocity.x = angularVelocity.y = angularVelocity.z = 0;
	}


	Entity::Update(dt);
}

void Player::CheckInput(float dt)
{
}

void Player::Respawn()
{
	transform.SetTranslation(respawnPosition);
	transform.SetLocalRotation(respawnLocalRotation);
	worldVelocity = XMFLOAT3(0, 0, 0);
	angularVelocity = XMFLOAT3(0, 0, 0);
	jetpack->Refuel(Jetpack::MAX_FUEL);
}
