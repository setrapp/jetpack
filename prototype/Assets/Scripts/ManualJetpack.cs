using UnityEngine;
using System.Collections;

public class ManualJetpack : MonoBehaviour {
	private CharacterMotor characterMotor = null;
	private bool nextJumpActivates = false;
	public bool activated = false;
	private bool wasActivated = false;
	private Vector3 oldUp = Vector3.up;
	private bool readyToShift = false;
	public bool readyToRotateBody = false;
	private bool wasInFlight = false;
	private float ignoredGravity = 0;
	public GameObject normalFocalPoint = null;
	public GameObject flightFocalPoint = null;
	public float cameraSwitchSpeed;
	public JetpackGear[] gears = null;
	public int inGear = 0;	//Gear 0 is hover
	public MouseLook characterLook = null;
	public MouseLook cameraLook = null;
	public float speedScale = 1;
	public float jetpackSpeed;
	public float pitchSpeed, rollSpeed;
	private bool flipping = false;
	
	void Start() {
		characterMotor = gameObject.GetComponent<CharacterMotor>();
		oldUp = transform.up;
		Screen.showCursor = false;
	}
	void Update () {
		if (characterMotor != null) {
			characterMotor.movement.velocity -= oldUp * jetpackSpeed * speedScale;
			if (Input.GetAxis("ShiftGears") > 0) {
				ShiftGears(1);
			} else if (Input.GetAxis("ShiftGears") < 0) {
				ShiftGears(-1);
			} else {
				readyToShift = true;
			}

			if (characterMotor.grounded) {
				nextJumpActivates = false;
				ForceStopJetpack(true);
			} else {
				if (Input.GetAxis("Jump") > 0) {
					if (nextJumpActivates) {
						float acceleration = gears[inGear].acceleration;
						if (Input.GetButton("ChangeSpeed")) {
							acceleration = -gears[inGear].deceleration;
						}
						ApplyJetPackAcceleration(acceleration);
						activated = true;
					}
				} else {
					nextJumpActivates = true;
					if (inGear == 0) {
						ForceStopJetpack();
					} else {
						ApplyJetPackAcceleration(0);
					}
				}

				if (inGear >= 0) {
					float verticalInput = Input.GetAxis("Vertical");
					float horizontalInput = Input.GetAxis("Horizontal");
					if (!Input.GetButton("Vertical") && !Input.GetButton("Horizontal")) {
						readyToRotateBody = true;
					} else if (readyToRotateBody) {
						if(verticalInput != 0) {
							transform.Rotate(-Vector3.right, pitchSpeed * verticalInput * speedScale);
						}
						if(horizontalInput != 0) {
							transform.Rotate(-Vector3.up, rollSpeed * horizontalInput * speedScale);
						}
					}
				}
			}

			if (inGear > 0 && (jetpackSpeed < gears[inGear].minSpeed || jetpackSpeed > gears[inGear].maxSpeed)) {
				ForceStopJetpack(true);
			}

			if (!wasActivated && activated) {
				// Store current gravity and then ignore it.
				ignoredGravity = characterMotor.movement.gravity;
				characterMotor.movement.gravity = 0;
			} else if (wasActivated && !activated) {
				// Stop ignoring gravity
				characterMotor.movement.gravity = ignoredGravity;
				ignoredGravity = 0;
			}
			wasActivated = activated;

			characterMotor.movement.velocity += transform.up * jetpackSpeed * speedScale;
			oldUp = transform.up;
		}
	}

	private void ApplyJetPackAcceleration(float jetpackAcceleration) {					
		float dt = Time.deltaTime;
		float acceleration = jetpackAcceleration * dt;
		if(inGear == 0 && jetpackSpeed + acceleration > gears[inGear].maxSpeed) {
			acceleration = Mathf.Max(gears[inGear].maxSpeed - jetpackSpeed, 0);
		} else if (jetpackSpeed + acceleration < 0) {
			acceleration = Mathf.Min(-jetpackSpeed, 0);
		}
		jetpackSpeed += acceleration;
	}

	private void ShiftGears(int change) {
		int oldGear = inGear;
		if (readyToShift) {
			inGear += change;
			readyToShift = false;
		}
		inGear = (int)Mathf.Clamp(inGear, 0, gears.Length - 1);

		if (oldGear == 0 && inGear > 0) {
			ToggleFlightMode(true);
		} else if (oldGear > 0 && inGear == 0) {
			ToggleFlightMode(false);
		}
	}

	private void ToggleFlightMode(bool flightMode) {
		if (wasInFlight == flightMode) {
			return;
		}
		wasInFlight = flightMode;

		characterLook.enabled = false;
		cameraLook.enabled = false;

		Transform cameraHolder = cameraLook.transform.parent;
		if (flightMode) {
			readyToRotateBody = false;
			cameraLook.ResetLook();
			Quaternion targetRotation = Quaternion.LookRotation(flightFocalPoint.transform.position - cameraHolder.position, -characterLook.transform.forward);
			StartCoroutine("RotateOverTime", new RotateOverTimeInfo(cameraHolder, targetRotation, cameraSwitchSpeed));
		} else {
			Quaternion targetRotation = Quaternion.LookRotation(normalFocalPoint.transform.position - cameraHolder.position, characterLook.transform.up);
			StartCoroutine("RotateOverTime", new RotateOverTimeInfo(cameraHolder, targetRotation, cameraSwitchSpeed / 2));
			StartCoroutine("RotateOverTime", new RotateOverTimeInfo(cameraLook.transform, targetRotation, cameraSwitchSpeed / 2));
		}

		if (characterLook != null && cameraLook != null) {
			characterLook.enabled = !flightMode;
			cameraLook.enabled = flightMode;
		}
	}

	private void ForceStopJetpack(bool hardStop = false) {
		// Transfer all of the jetpack's speed into the character
		characterMotor.movement.velocity += transform.up * jetpackSpeed;
		jetpackSpeed = 0;
		activated = false;
		if (hardStop) {
			nextJumpActivates = false;
		}
	}

	private IEnumerator RotateOverTime(RotateOverTimeInfo info) {
		while(info.rotatee.rotation != info.targetRotation) {
			info.rotatee.rotation = Quaternion.RotateTowards(info.rotatee.rotation, info.targetRotation, info.degreesPerStep);
			yield return new WaitForSeconds(0);
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
