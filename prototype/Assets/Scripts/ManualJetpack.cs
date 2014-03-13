using UnityEngine;
using System.Collections;
using System.Collections.Generic;

public class ManualJetpack : SimpleJetpack {
	public bool readyToRotateBody = false;
	private bool wasInFlight = false;
	public GameObject normalFocalPoint = null;
	public GameObject flightFocalPoint = null;
	public float cameraSwitchSpeed;
	public JetpackGear[] gears = null;
	public int inGear = 6;	//Gear 0 is neutral and Gear 6 is hover
	public GameObject character = null;
	public List<MouseLook> looks = null;
	//public MouseLook characterLook = null;
	//public MouseLook cameraLook = null;
	public float pitchSpeed, yawSpeed, rollSpeed;
	private bool forcingRotate = false;
	public Shifter shifter = null;
	private bool shifting = false;
	public JetpackerCamera jetpackerCamera = null;

	void Update () {
		EnterJetpackFrame();

		// Shift
		if (Input.GetAxis ("ShiftGears") != 0) {
			shifting = true;
		} else if (shifting){
			int newGear = shifter.shifterSpots.GetGear();
			if (newGear != inGear) {
				ShiftGears(newGear);
			}
			shifting = false;
		}

		// Update jetpack speed parameters based on current gear.
		acceleration = 0;
		if (inGear == 6 || Input.GetAxis("Vertical") != 0) {
			acceleration = gears[inGear].acceleration;
			if (Input.GetAxis("Vertical") < 0) {
				acceleration = -gears[inGear].deceleration;
			}
		}
		minSpeed = gears[inGear].minSpeed;
		maxSpeed = gears[inGear].maxSpeed;

		// Rotation
		if (InManualGear()) {
			if (readyToRotateBody) {
				// Pitch
				/*float pitchChange = 0;
				if (cameraLook.ClampingY > 0) {
					pitchChange = pitchSpeed * Time.deltaTime;
				} else if(cameraLook.ClampingY < 0) {
					pitchChange = -pitchSpeed * Time.deltaTime;
				}
				if(pitchChange != 0) {
					transform.Rotate(-Vector3.right, pitchChange * speedScale);
				}

				// Yaw
				float yawChange = 0;
				if (cameraLook.ClampingX > 0) {
					yawChange = yawSpeed * Time.deltaTime;
				} else if(cameraLook.ClampingX < 0) {
					yawChange = -yawSpeed * Time.deltaTime;
				}
				if(yawChange != 0) {
					transform.Rotate(-Vector3.forward, yawChange * speedScale);
				}

				// Roll
				if (Input.GetAxis("Horizontal") != 0) {
					transform.Rotate(-Vector3.up, rollSpeed * Input.GetAxis("Horizontal") * Time.deltaTime * speedScale);
				}*/
			} else if(Input.GetAxis("Horizontal") == 0) {
				readyToRotateBody = !forcingRotate;
			}

			// Hard stop if the jetpack is not moving at the correct speed for the current gear.
			if (jetpackSpeed < minSpeed || jetpackSpeed >= maxSpeed) {
				ForceStopJetpack(true);
			}
		}

		if (wasInFlight && characterMotor.grounded) {
			ForceStopJetpack(true);
			ToggleFlightMode(false);
			ShiftGears(6);

			// Why does this need to be hardcoded. Shift gears if messed up here.
			jetpackerCamera.behind = 5;
			jetpackerCamera.above = 2;
			jetpackerCamera.minDistance = 5;
			jetpackerCamera.maxDistance = 10;
			jetpackerCamera.lerpSpeed = 5;
			//characterLook.transform.rotation = Quaternion.identity;
			//characterLook.transform.forward = Vector3.Cross(Vector3.up, characterLook.transform.right);
			//characterLook.transform.up = -Vector3.up;
			//characterLook.ResetLook();
		}

		for (int i = 0; i < looks.Count; i++) {
			looks[i].enabled = !shifter.shifting && !jetpackerCamera.IsLerping();
		}

		ExitJetpackFrame();
	}

	public bool InManualGear() {
		return inGear > 0 && inGear < 6;
	}
	
	private void ShiftGears(int newGear) {
		inGear = newGear;
		inGear = (int)Mathf.Clamp(inGear, 0, gears.Length - 1);


		if (inGear != 0) {
			// Toggle flight mode based on change to/from a non-hover gear.
			if (!wasInFlight && inGear != 6) {
				ToggleFlightMode (true);
			} else if (wasInFlight && inGear == 6) {
				ToggleFlightMode (false);
			}

			// Update following camera to keep up with jetpack
			if (!characterMotor.grounded) {
				jetpackerCamera.behind = gears[inGear].cameraFollower.behind;
				jetpackerCamera.above = gears[inGear].cameraFollower.above;
				jetpackerCamera.minDistance = gears[inGear].cameraFollower.minDistance;
				jetpackerCamera.maxDistance = gears[inGear].cameraFollower.maxDistance;
				jetpackerCamera.lerpSpeed = gears[inGear].cameraFollower.lerpSpeed;
			}
		}
	}

	private void ToggleFlightMode(bool flightMode) {
		if (wasInFlight == flightMode) {
			return;
		}
		wasInFlight = flightMode;
		extendGroundControls = !flightMode;

		//float behind = jetpackerCamera.behind;
		//jetpackerCamera.behind = jetpackerCamera.above;
		//jetpackerCamera.above = behind;
		jetpackerCamera.negateAbove = flightMode;

		// Disable mouse looks temporarily, to avoid unwanted corrections.
		//characterLook.enabled = false;
		//cameraLook.enabled = false;

		// Retrieve the parent of the camera so that the camera rotation does not conflict with MouseLook later.
		//Transform cameraHolder = cameraLook.transform.parent;

		// Prepare for mode switch.
		if (flightMode) {
			deactivateOnRelease = false;
			readyToRotateBody = false;

			//TODO This might not be needed if the camera look ever works.
			//looks[0].clampY = false;
			looks[0].minimumY = -90;
			looks[0].maximumY = 90;

			// Start using MouseLook attached camera.
			//shifter.look = cameraLook;
			//cameraLook.ResetLook();

			//Rotate towards flight focal point.
			//Quaternion targetRotation = Quaternion.LookRotation(flightFocalPoint.transform.position - cameraHolder.position, -character.transform.forward);
			//StartCoroutine("RotateOverTime", new RotateOverTimeInfo(cameraHolder, targetRotation, cameraSwitchSpeed));
		} else {
			deactivateOnRelease = false;

			//looks[0].clampY = true;
			looks[0].minimumY = 0;
			looks[0].maximumY = 0;

			// Start using MouseLook attached character.
			//shifter.look = characterLook;

			//Rotate towards non-flight focal point and reset crosshairs to center
			//Quaternion targetRotation = Quaternion.LookRotation(normalFocalPoint.transform.position - cameraHolder.position, character.transform.up);
			//StartCoroutine("RotateOverTime", new RotateOverTimeInfo(cameraHolder, targetRotation, cameraSwitchSpeed / 2));
			//StartCoroutine("RotateOverTime", new RotateOverTimeInfo(cameraLook.transform, targetRotation, cameraSwitchSpeed / 2));
		}

		// Actually determine which MouseLook should be used.
		/*if (characterLook != null && cameraLook != null) {
			//characterLook.enabled = !flightMode;
			//cameraLook.enabled = flightMode;
		}*/
	}

	// Rotate to align with given vector overtime.
	public void FlipJetpackToAlign(Vector3 alignTo, Vector3 up, float degreesPerSecond) {
		Quaternion targetRotation = Quaternion.LookRotation(alignTo, up);
		StartCoroutine("RotateOverTime", new RotateOverTimeInfo(transform, targetRotation, degreesPerSecond));
	}

	// Rotate to a target orientation overtime.
	private IEnumerator RotateOverTime(RotateOverTimeInfo info) {
		while(info.rotatee.rotation != info.targetRotation) {
			/*if (info.rotatee.transform == cameraLook.transform.parent) {
				forcingRotate = true;
			}*/
			info.rotatee.rotation = Quaternion.RotateTowards(info.rotatee.rotation, info.targetRotation, info.degreesPerStep);
			yield return new WaitForSeconds(0);
		}

		/*if (info.rotatee.transform == cameraLook.transform.parent) {
			forcingRotate = false;
		}*/
	}

	private class RotateOverTimeInfo {
		public RotateOverTimeInfo(Transform rotatee, Quaternion targetRotation, float degreesPerStep) {
			this.rotatee = rotatee;
			this.targetRotation = targetRotation;
			this.degreesPerStep = degreesPerStep;
		}
		public Transform rotatee;
		public Quaternion targetRotation;
		public float degreesPerStep;
	}
}

[System.Serializable]
public class JetpackGear {
	public float minSpeed;	//should be taken out once 'RPM' is a thing
	public float maxSpeed;	//should be taken out once 'RPM' is a thing
	public float acceleration;
	public float deceleration;
	public CameraFollower cameraFollower;
}

[System.Serializable]
public class CameraFollower {
	public float behind = 5.0f;
	public float above = 3.0f;
	public float minDistance = 5.0f;
	public float maxDistance = 10.0f;
	public float lerpSpeed = 5.0f;
}

