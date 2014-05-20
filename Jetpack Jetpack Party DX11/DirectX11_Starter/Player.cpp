#include "Player.h"
#include "AssetManager.h"

Player::Player()
{
	minPosture = 0.95f;
	respawnPosition = XMFLOAT3(0, 0, 0);
	respawnLocalRotation = XMFLOAT3(0, 0, 0);
	worldVelocity = XMFLOAT3(0, 0, 0);
	angularVelocity = XMFLOAT3(0, 0, 0);
	playerName="";
	gravityAcceleration = 6.0f;
	terminalVelocity = 5000;
	groundSpeedDampening = 0.0f;
	airSpeedDampening = 0.1f;

	groundSpeedDampening = 0.0f;
	airSpeedDampening = 0.0f;
	grounded = true;
	jetpack = new ManeuverJetpack(this);
	controllable = false;
	targetPosition = XMFLOAT3(0, 0, 0);
	targetUp = XMFLOAT3(0, 1, 0);
	targetCheckpoint = NULL;
	targetFuelStation = NULL;
	desiredMinAltitude = 500;
	keepHeightMaxWeight = 10;
	navMeshSegment = NULL;
}

Player::~Player()
{
	delete jetpack;
}

void Player::Update(float dt)
{
	//TODO account for networked players.
	jetpack->playerControllable = controllable;
	jetpack->playerAI = ai;

	XMFLOAT3 velocity = transform.InverseTransformDirection(worldVelocity);

	// Figure out the optimal acceleration to the target position.
	if (jetpack->playerAI)
	{
		XMFLOAT3 desiredVelocity = ComputeDesiredVelocity(velocity, angularVelocity);
		//XMStoreFloat3(&jetpack->targetAcceleration, XMVectorSubtract(desiredVelocity, XMLoadFloat3(&velocity)));

		// Push AI controlled players along vector to next target.
		//TODO Take this out when jetpack thruster work
		velocity = desiredVelocity;
		XMFLOAT3 lookAtTarget;
		transform.LookAt(transform.GetTranslation(), targetPosition, XMFLOAT3(0, 1, 0));
		XMStoreFloat3(&worldVelocity, XMVectorScale(XMLoadFloat3(&transform.GetForward()), jetpack->maxSpeed));
		velocity = transform.InverseTransformDirection(worldVelocity);
	}

	// Check for user input.
	if (controllable)
	{
		CheckInput(dt);
	}
	// Update jetpack.

	jetpack->allowInputForces = !grounded; //TODO This might not be useful anymore
	jetpack->Update(dt, &velocity, &angularVelocity);

	// Clamp velocity within max speed.
	transform.ClampVector(&velocity, jetpack->maxSpeed, 0);

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
		position.y = 0;
		transform.SetLocalTranslation(position);

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

	// Slow the character a bit so that it comes to a nice stop over time. (friction)
	if (grounded) {
		XMStoreFloat3(&velocity, XMVectorScale(XMLoadFloat3(&velocity), (1 - ((1 - groundSpeedDampening) * dt))));
		XMStoreFloat3(&angularVelocity, XMVectorScale(XMLoadFloat3(&angularVelocity), (1 - ((1 - groundSpeedDampening) * dt))));
	} else {
		XMStoreFloat3(&velocity, XMVectorScale(XMLoadFloat3(&velocity), (1 - ((1 - airSpeedDampening) * dt))));
		XMStoreFloat3(&angularVelocity, XMVectorScale(XMLoadFloat3(&angularVelocity), (1 - ((1 - airSpeedDampening) * dt))));
	}

	Entity::Update(dt);
}

void Player::CheckInput(float dt)
{
	//Should use IPMan
	if (GetAsyncKeyState('R'))
	{
		Respawn();
	}
}

void Player::Respawn()
{
	transform.SetTranslation(respawnPosition);
	transform.SetLocalRotation(respawnLocalRotation);
	worldVelocity = XMFLOAT3(0, 0, 0);
	angularVelocity = XMFLOAT3(0, 0, 0);
	jetpack->Refuel(Jetpack::MAX_FUEL);
}
XMFLOAT3 Player::ComputeDesiredVelocity(XMFLOAT3 currentVelocity, XMFLOAT3 currentAngularVelocity)
{
	// Moving toward target.
	float towardTargetWeight = 1;
	XMFLOAT3 towardTarget;
	XMStoreFloat3(&towardTarget, XMVector3Normalize(XMVectorSubtract(XMLoadFloat3(&targetPosition), XMLoadFloat3(&transform.GetTranslation()))));

	// Staying up in the air.
	float keepHeightWeight = 0;
	XMFLOAT3 keepHeight;
	XMStoreFloat3(&keepHeight, XMVector3Normalize(XMLoadFloat3(&transform.InverseTransformDirection(XMFLOAT3(0, 1, 0)))));
	float altitude = transform.GetLocalTranslation().y; // TODO raycast to ground, don't avoid fuel stations
	float minAltitude = targetPosition.y >= desiredMinAltitude ? desiredMinAltitude : targetPosition.y;
	if (altitude < minAltitude)
	{
		keepHeightWeight = (1 - (altitude / desiredMinAltitude)) * keepHeightMaxWeight;
	}

	XMVECTOR desiredDirection = XMLoadFloat3(&XMFLOAT3(0, 0, 0));//&currentVelocity);
	desiredDirection = XMVectorAdd(desiredDirection, XMVectorScale(XMLoadFloat3(&towardTarget), towardTargetWeight));
	desiredDirection = XMVectorAdd(desiredDirection, XMVectorScale(XMLoadFloat3(&keepHeight), keepHeightWeight));
	//desiredDirection = XMVector3Normalize(desiredDirection);
	XMStoreFloat3(&currentVelocity, XMVectorScale(XMVector3Normalize(desiredDirection), jetpack->maxSpeed));

	// Figure out if the character needs to try turning up-right.
	/*jetpack->targetAngularAcceleration = XMFLOAT3(0, 0, 0);
	float desiredForwardDotUp;
	XMStoreFloat(&desiredForwardDotUp, XMVector3Dot(XMLoadFloat3(&targetUp), XMLoadFloat3(&transform.GetUp())));
	if (desiredForwardDotUp < minPosture)
	{
		float desiredForwardDotRight;
		XMStoreFloat(&desiredForwardDotRight, XMVector3Dot(XMLoadFloat3(&targetUp), XMLoadFloat3(&transform.GetRight())));
		if (desiredForwardDotRight < 0)
		{
			jetpack->targetAngularAcceleration.z = 1;
		}
		else
		{
			jetpack->targetAngularAcceleration.z = -1;
		}
	}*/

	// Figure out if the character needs to try turning up-right.
	/*jetpack->targetAngularAcceleration = XMFLOAT3(0, 0, 0);
	float desiredUpDotUp;
	XMStoreFloat(&desiredUpDotUp, XMVector3Dot(XMLoadFloat3(&targetUp), XMLoadFloat3(&transform.GetUp())));
	if (desiredUpDotUp < minPosture)
	{
		float desiredUpDotRight;
		XMStoreFloat(&desiredUpDotRight, XMVector3Dot(XMLoadFloat3(&targetUp), XMLoadFloat3(&transform.GetRight())));
		if (desiredUpDotRight < 0)
		{
			jetpack->targetAngularAcceleration.z = 1;
		}
		else
		{
			jetpack->targetAngularAcceleration.z = -1;
		}
	}*/

	return currentVelocity;
}
