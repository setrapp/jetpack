using UnityEngine;
using System.Collections;

public class Shifter : MonoBehaviour {
	private bool shifting = false;
	public GameObject hpattern = null;
	public GameObject stick = null;
	public Camera camera = null;
	public MouseLook look = null;
	private bool blocked = false;
	//private Vector3 oldMouseWorldPos;
	private Vector2 oldMousePos;
	public float mouseMoveThreshold;
	public ShifterSpots shifterSpots;

	void Start() {
		shifterSpots.PopulateSpotsArray();
	}

	void Update() {
		//ShiftGears
		if (Input.GetAxis("ShiftGears") != 0) {
			if (!shifting) {
				oldMousePos = Input.mousePosition;
				hpattern.SetActive(true);
				shifterSpots.MoveShifter(ShifterSpots.ShifterDirection.NONE, stick);
				/* Move shift under mouse, do we want this?
				Vector3 hpatternToStick = stick.transform.position - hpattern.transform.position;
				Vector3 cameraToStick = stick.transform.position - camera.transform.position;
				stick.transform.position = camera.ScreenToWorldPoint(new Vector3 ((float)Input.mousePosition.x, (float)Input.mousePosition.y, camera.transform.TransformDirection(cameraToStick).z));
				stick.transform.Translate(cameraToStick);
				//oldMouseWorldPos = camera.ScreenToWorldPoint(new Vector3 ((float)Input.mousePosition.x, (float)Input.mousePosition.y, stick.transform.position.z - camera.transform.position.z));
				//PutStickAtMouse (false);
				hpattern.transform.position = stick.transform.position - hpatternToStick;
				*/
				shifting = true;
			} else {
				//PutStickAtMouse (true);
				if (Input.mousePosition.x - oldMousePos.x < -mouseMoveThreshold) {
					shifterSpots.MoveShifter(ShifterSpots.ShifterDirection.LEFT, stick);
					oldMousePos = Input.mousePosition;
				} else if (Input.mousePosition.x - oldMousePos.x > mouseMoveThreshold) {
					shifterSpots.MoveShifter(ShifterSpots.ShifterDirection.RIGHT, stick);
					oldMousePos = Input.mousePosition;
				}

				if (Input.mousePosition.y - oldMousePos.y < -mouseMoveThreshold) {
					shifterSpots.MoveShifter(ShifterSpots.ShifterDirection.UP, stick);
					oldMousePos = Input.mousePosition;
				} else if (Input.mousePosition.y - oldMousePos.y > mouseMoveThreshold) {
					shifterSpots.MoveShifter(ShifterSpots.ShifterDirection.DOWN, stick);
					oldMousePos = Input.mousePosition;
				}
			}
		} else if (shifting){
			hpattern.SetActive(false);
			//PutStickAtMouse(false);
			shifting = false;
		}

		look.enabled = !shifting;
	}

	private void PutStickAtMouse(/*bool keepInBox*/) {
		//Vector3 stickPos = shifterSpots
		/*Vector3 mouseWorldPos = camera.ScreenToWorldPoint(new Vector3 ((float)Input.mousePosition.x, (float)Input.mousePosition.y, stick.transform.position.z - camera.transform.position.z));
		Vector3 stickPos = stick.transform.position + mouseWorldPos - oldMouseWorldPos;
		//stickPos.z = stick.transform.position.z;

		bool tempBlocked = blocked;
		RaycastHit hit = new RaycastHit();
		Vector3 raycastOrigin = stick.transform.position;
		raycastOrigin.z = hpattern.transform.position.z;
		Vector3 toStickPosDir = stickPos - stick.transform.position;
		float toStickPosDist = toStickPosDir.magnitude;
		if (toStickPosDist > 0) {
			toStickPosDir /= toStickPosDist;
			if (Physics.Raycast(raycastOrigin, toStickPosDir, out hit, toStickPosDist, ~LayerMask.NameToLayer("Shifterbox"))) {
				//stickPos = hit.point - (toStickPosDir / 100);
				//stick.transform.position = hit.point;
				//tempBlocked = true;
				//Debug.Log ("blocked");
			} else {
				tempBlocked = false;
			}
		}

		if (!blocked) {
			//stickPos.z = stick.transform.position.z;
			stick.transform.position = stickPos;
		} else {
			//mouseOffset = 
		}
		blocked = tempBlocked;
		oldMouseWorldPos = mouseWorldPos;*/
	}
}

[System.Serializable]
public class ShifterSpots {
	public GameObject gear1;
	public GameObject gear2;
	public GameObject gear3;
	public GameObject gear4;
	public GameObject gear5;
	public GameObject gearH;
	public GameObject neutralL;
	public GameObject neutralM;
	public GameObject neutralR;
	private GameObject[] spots;
	private int spotIndex = 8;

	public enum ShifterDirection {
		NONE = 0,
		LEFT,
		UP,
		RIGHT,
		DOWN
	}

	public enum ShifterGear
	{
		NEUTRAL = 0,
		FIRST,
		SECOND,
		THIRD,
		FOURTH,
		FIFTH,
		HOVER
	}

	public void PopulateSpotsArray() {
		spots = new GameObject[9];
		spots[0] = gear1;
		spots[1] = neutralL;
		spots[2] = gear2;
		spots[3] = gear3;
		spots[4] = neutralM;
		spots[5] = gear4;
		spots[6] = gear5;
		spots[7] = neutralR;
		spots[8] = gearH;
	}

	public void MoveShifter(ShifterDirection direction, GameObject shifter) {
		switch (direction) {
		case ShifterDirection.LEFT:
			// Only shift left if in neutral and not all the way left.
			if (spotIndex == 4 || spotIndex == 7) {
				spotIndex -= 3;
			}
			break;
		case ShifterDirection.UP:
			// Only shift down if not at the bottom.
			if (spotIndex != 2 && spotIndex != 5 && spotIndex != 8) {
				spotIndex += 1;
			}
			break;
		case ShifterDirection.RIGHT:
			// Only shift right if in neutral and not all the way right.
			if (spotIndex == 1 || spotIndex == 4) {
				spotIndex += 3;
			}
			break;
		case ShifterDirection.DOWN:
			// Only shift up if not at the top.
			if (spotIndex != 0 && spotIndex != 3 && spotIndex != 6) {
				spotIndex -= 1;
			}
			break;
		}

		shifter.transform.position = spots[spotIndex].transform.position;
	}

	public int GetGear() {
		int gear = (int)ShifterGear.NEUTRAL;
		switch (spotIndex) {
		case 0:
			gear = (int)ShifterGear.FIRST;
			break;
		case 2:
			gear = (int)ShifterGear.SECOND;
			break;
		case 3:
			gear = (int)ShifterGear.THIRD;
			break;
		case 5:
			gear = (int)ShifterGear.FOURTH;
			break;
		case 6:
			gear = (int)ShifterGear.FIFTH;
			break;
		case 8:
			gear = (int)ShifterGear.HOVER;
			break;
		}
		return gear;
	}
}
