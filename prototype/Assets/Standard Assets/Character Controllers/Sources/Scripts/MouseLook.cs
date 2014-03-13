using UnityEngine;
using System.Collections;

/// MouseLook rotates the transform based on the mouse delta.
/// Minimum and Maximum values can be used to constrain the possible rotation

/// To make an FPS style character:
/// - Create a capsule.
/// - Add the MouseLook script to the capsule.
///   -> Set the mouse look to use LookX. (You want to only turn character but not tilt it)
/// - Add FPSInputController script to the capsule
///   -> A CharacterMotor and a CharacterController component will be automatically added.

/// - Create a camera. Make the camera a child of the capsule. Reset it's transform.
/// - Add a MouseLook script to the camera.
///   -> Set the mouse look to use LookY. (You want the camera to tilt up and down like a head. The character already turns.)
[AddComponentMenu("Camera-Control/Mouse Look")]
public class MouseLook : MonoBehaviour {

	public enum RotationAxes { MouseXAndY = 0, MouseX = 1, MouseY = 2 }
	public RotationAxes axes = RotationAxes.MouseXAndY;
	public float sensitivityX = 15F;
	public float sensitivityY = 15F;

	public bool clampX = false;
	public float minimumX = -360F;
	public float maximumX = 360F;
	public bool clampY = true;
	public float minimumY = -60F;
	public float maximumY = 60F;

	float rotationX = 0F;
	float rotationY = 0F;

	public int ClampingY {
		get { 	
			if (clampY) { 
				if (rotationY <= minimumY) {
					return -1;
				} else if (rotationY >= maximumY) {
					return 1;
				}
			}
				return 0;
		}
	}

	public int ClampingX {
		get { 	
			if (clampX) { 
				if (rotationX <= minimumX) {
					return -1;
				} else if (rotationX >= maximumX) {
					return 1;
				}
			}
			return 0;
		}
	}



	void Update ()
	{
		if (axes == RotationAxes.MouseXAndY)
		{
			rotationX += Input.GetAxis("Mouse X") * sensitivityX;
			if (clampX) {
				rotationX = Mathf.Clamp (rotationX, minimumX, maximumX);
			}
			
			rotationY += Input.GetAxis("Mouse Y") * sensitivityY;
			if (clampY) {
				rotationY = Mathf.Clamp (rotationY, minimumY, maximumY);
			}
			
			transform.localEulerAngles = new Vector3(-rotationY, rotationX, 0);
		}
		else if (axes == RotationAxes.MouseX)
		{
			transform.Rotate(0, Input.GetAxis("Mouse X") * sensitivityX, 0);
		}
		else
		{
			rotationY += Input.GetAxis("Mouse Y") * sensitivityY;
			rotationY = Mathf.Clamp (rotationY, minimumY, maximumY);
			
			transform.localEulerAngles = new Vector3(-rotationY, transform.localEulerAngles.y, 0);
		}
	}

	public void ForceRotation(bool forceX, bool forceY) {
		if (forceX) {
			transform.localEulerAngles = new Vector3(-rotationY, rotationX, 0);
		}
		if (forceY) {
			transform.localEulerAngles = new Vector3(-rotationY, transform.localEulerAngles.y, 0);
		}
	}
	
	void Start ()
	{
		// Make the rigid body not change rotation
		if (rigidbody)
			rigidbody.freezeRotation = true;
	}

	public void ResetLook() {
		rotationX = 0F;
		rotationY = 0F;
	}
}