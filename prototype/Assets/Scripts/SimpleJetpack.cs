using UnityEngine;
using System.Collections;

public class SimpleJetpack : MonoBehaviour
{
	private CharacterMotor characterMotor = null;
	private bool nextJumpActivates = false;
	public bool activated = false;
	private bool wasActivated = false;
	private Vector3 oldUp = Vector3.up;
	private float ignoredGravity = 0;
	public float speedScale = 1;
	public float jetpackSpeed;
	public float acceleration = 0;
	public float minSpeed = 0;
	public float maxSpeed = 0;
	private bool initComplete = false;
	protected bool deactivateOnRelease = true;
	protected bool extendGroundControls = true;
	public float airFriction;

	void Update () {
		EnterJetpackFrame();
		ExitJetpackFrame();
	}

	// Ensure that jetpack ready to use.
	private void InitJetpack() {
		characterMotor = gameObject.GetComponent<CharacterMotor>();
		oldUp = transform.up;
		initComplete = true;
	}

	// Run checks that jetpacks commonly make before updating.
	protected void EnterJetpackFrame() {
		if (!initComplete) {
			InitJetpack();
		}

		if (characterMotor != null) {
			// Remove jetpack speed from character to fiddle with in isolation.
			characterMotor.movement.velocity -= transform.up * jetpackSpeed * speedScale;

			if (characterMotor.grounded) {
				// NO JETPACKS ON THE GROUND.
				nextJumpActivates = false;
				ForceStopJetpack (true);
			} else {
				if (Input.GetAxis ("Jump") > 0) {
					// 'Second Jump' activates jetpack.
					if (nextJumpActivates) {
						activated = true;
					}
				} else {
					// The next jump made while in the air will activate jetpack.
					nextJumpActivates = true;
					activated = false;

					// Determine how to slow jetpack when deactivated
					if (activated) {
						if (deactivateOnRelease) {
							ForceStopJetpack();
						} else {
							jetpackSpeed *= airFriction;
						}
					}
				}
			}
		}
	}

	// Apply operations that jetpacks commonly make after updating.
	protected void ExitJetpackFrame() {
		if (characterMotor != null) {
			if (!characterMotor.grounded) {
				if (activated && Input.GetAxis ("Jump") > 0) {
					ApplyJetPackAcceleration (acceleration);
				}
			}
		}

		if (activated) {
			if (extendGroundControls) {
				if (Input.GetAxis("Vertical") != 0) {
					float extendedAccleration = (characterMotor.movement.maxGroundAcceleration / 2) * Input.GetAxis("Vertical") * Time.deltaTime;
					if (characterMotor.movement.velocity.z + extendedAccleration > characterMotor.movement.maxForwardSpeed) {
						extendedAccleration = Mathf.Max(characterMotor.movement.maxForwardSpeed - characterMotor.movement.velocity.z, 0);
					} else if (characterMotor.movement.velocity.z + extendedAccleration < -characterMotor.movement.maxForwardSpeed) {
						extendedAccleration = Mathf.Min(-characterMotor.movement.maxForwardSpeed - characterMotor.movement.velocity.z, 0);
					}
					characterMotor.movement.velocity += transform.TransformDirection(new Vector3(0, 0, extendedAccleration));
				}
				if (Input.GetAxis("Horizontal") != 0) {
					float extendedAccleration = (characterMotor.movement.maxGroundAcceleration / 2) * Input.GetAxis("Horizontal") * Time.deltaTime;
					if (characterMotor.movement.velocity.x + extendedAccleration > characterMotor.movement.maxForwardSpeed) {
						extendedAccleration = Mathf.Max(characterMotor.movement.maxForwardSpeed - characterMotor.movement.velocity.x, 0);
					} else if (characterMotor.movement.velocity.x + extendedAccleration < -characterMotor.movement.maxForwardSpeed) {
						extendedAccleration = Mathf.Min(-characterMotor.movement.maxForwardSpeed - characterMotor.movement.velocity.x, 0);
					}
					characterMotor.movement.velocity += transform.TransformDirection(new Vector3(extendedAccleration, 0, 0));
				}
			}
		}

		if (!wasActivated && activated) {
			// Store current gravity and then ignore it.
			ignoredGravity = characterMotor.movement.gravity;
			characterMotor.movement.gravity = 0;
		} else if (wasActivated && !activated) {
			// Stop ignoring gravity.
			characterMotor.movement.gravity = ignoredGravity;
			ignoredGravity = 0;
		}
		wasActivated = activated;

		// Add jetpack speed to character velocity to actually apply speed to character.
		if (characterMotor != null) {
			characterMotor.movement.velocity += transform.up * jetpackSpeed * speedScale;
			oldUp = transform.up;
		}
	}

	// Change jetpack speed and clamp it between min and max speeds.
	// This DOES NOT actually affect the character's own speed. 
	private void ApplyJetPackAcceleration(float jetpackAcceleration) {					
		float curAcceleration = jetpackAcceleration * Time.deltaTime;
		if(jetpackSpeed + curAcceleration > maxSpeed) {
			curAcceleration = Mathf.Max(maxSpeed - jetpackSpeed, 0);
		} else if (jetpackSpeed + acceleration < 0) {
			curAcceleration = Mathf.Min(-jetpackSpeed, 0);
		}
		jetpackSpeed += curAcceleration;
	}

	// Stop the jetpack immediately.
	// Hard stopping causes the jetpack to stall.
	protected void ForceStopJetpack(bool hardStop = false) {
		// Transfer all of the jetpack's speed into the character
		characterMotor.movement.velocity += transform.up * jetpackSpeed;
		jetpackSpeed = 0;
		activated = false;
		if (hardStop) {
			nextJumpActivates = false;
		}
	}
}