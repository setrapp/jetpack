using UnityEngine;
using System.Collections;

public class ManualJetpack : SimpleJetpack {
	private bool readyToShift = false;
	public bool readyToRotateBody = false;
	private bool wasInFlight = false;
	public GameObject normalFocalPoint = null;
	public GameObject flightFocalPoint = null;
	public float cameraSwitchSpeed;
	public JetpackGear[] gears = null;
	public int inGear = 0;	//Gear 0 is neutral and Gear 6 is hover
	public MouseLook characterLook = null;
	public MouseLook cameraLook = null;
	public float pitchSpeed, yawSpeed, rollSpeed;
	private bool forcingRotate = false;
	public Shifter shifter = null;
	private bool shifting = false;

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
		/*if (Input.GetAxis("ShiftGears") ! 0) {
			ShiftGears(shifter.shifterSpots.GetGear());
		} else if (Input.GetAxis("ShiftGears") < 0) {
			ShiftGears(-1);
		} else {
			readyToShift = true;
		}*/

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
				float pitchChange = 0;
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
				}
			} else if(Input.GetAxis("Horizontal") == 0) {
				readyToRotateBody = !forcingRotate;
			}

			// Hard stop if the jetpack is not moving at the correct speed for the current gear.
			if (jetpackSpeed <= minSpeed || jetpackSpeed >= maxSpeed) {
				ForceStopJetpack(true);
			}
		}

		ExitJetpackFrame();
	}

	public bool InManualGear() {
		return inGear > 0 && inGear < 6;
	}
	
	private void ShiftGears(int newGear) {
		int oldGear = inGear;
		//if (readyToShift) {
			inGear = newGear;
			//readyToShift = false;
		//}
		inGear = (int)Mathf.Clamp(inGear, 0, gears.Length - 1);

		// Toggle flight mode based on change to/from a non-hover gear.
		if (inGear != 0) {
			if (!wasInFlight && inGear != 6) {
				ToggleFlightMode (true);
			} else if (wasInFlight && inGear == 6) {
				ToggleFlightMode (false);
			}
		}
	}

	private void ToggleFlightMode(bool flightMode) {
		if (wasInFlight == flightMode) {
			return;
		}
		wasInFlight = flightMode;

		// Disable mouse looks temporarily, to avoid unwanted corrections.
		characterLook.enabled = false;
		cameraLook.enabled = false;

		// Retrieve the parent of the camera so that the camera rotation does not conflict with MouseLook later.
		Transform cameraHolder = cameraLook.transform.parent;

		// Prepare for mode switch.
		if (flightMode) {
			deactivateOnRelease = false;
			readyToRotateBody = false;

			// Start using MouseLook attached camera.
			shifter.look = cameraLook;
			cameraLook.ResetLook();

			//Rotate towards flight focal point.
			Quaternion targetRotation = Quaternion.LookRotation(flightFocalPoint.transform.position - cameraHolder.position, -characterLook.transform.forward);
			StartCoroutine("RotateOverTime", new RotateOverTimeInfo(cameraHolder, targetRotation, cameraSwitchSpeed));
		} else {
			deactivateOnRelease = false;

			// Start using MouseLook attached character.
			shifter.look = characterLook;

			//Rotate towards non-flight focal point and reset crosshairs to center
			Quaternion targetRotation = Quaternion.LookRotation(normalFocalPoint.transform.position - cameraHolder.position, characterLook.transform.up);
			StartCoroutine("RotateOverTime", new RotateOverTimeInfo(cameraHolder, targetRotation, cameraSwitchSpeed / 2));
			StartCoroutine("RotateOverTime", new RotateOverTimeInfo(cameraLook.transform, targetRotation, cameraSwitchSpeed / 2));
		}

		// Actually determine which MouseLook should be used.
		if (characterLook != null && cameraLook != null) {
			characterLook.enabled = !flightMode;
			cameraLook.enabled = flightMode;
		}
	}

	// Rotate to align with given vector overtime.
	public void FlipJetpackToAlign(Vector3 alignTo, Vector3 up, float degreesPerSecond) {
		Quaternion targetRotation = Quaternion.LookRotation(alignTo, up);
		StartCoroutine("RotateOverTime", new RotateOverTimeInfo(transform, targetRotation, degreesPerSecond));
	}

	// Rotate to a target orientation overtime.
	private IEnumerator RotateOverTime(RotateOverTimeInfo info) {
		while(info.rotatee.rotation != info.targetRotation) {
			if (info.rotatee.transform == cameraLook.transform.parent) {
				forcingRotate = true;
			}
			info.rotatee.rotation = Quaternion.RotateTowards(info.rotatee.rotation, info.targetRotation, info.degreesPerStep);
			yield return new WaitForSeconds(0);
		}

		if (info.rotatee.transform == cameraLook.transform.parent) {
			forcingRotate = false;
		}
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
}
