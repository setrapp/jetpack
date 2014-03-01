using UnityEngine;
using System.Collections;

public class Shifter : MonoBehaviour {
	private bool shifting = false;
	public Camera camera;
	public GameObject hpattern = null;
	public GameObject stick = null;
	public MouseLook look = null;
	private bool blocked = false;
	private Vector3 oldMousePos;
	public float mouseMoveThreshold;
	public ShifterSpots shifterSpots;
	private Vector3 handPos;
	public Texture2D handTexture;

	void Start() {
		shifterSpots.PopulateSpotsArray();
	}

	void Update() {
		//ShiftGears
		if (Input.GetAxis("ShiftGears") != 0) {
			if (!shifting) {
				oldMousePos = Input.mousePosition;
				Vector2 handPos2D = camera.WorldToScreenPoint(stick.transform.position);
				handPos = new Vector3(handPos2D.x, Screen.height - handPos2D.y, 0);
				shifterSpots.UpdateShifter(handPos, stick);
				hpattern.SetActive(true);
				shifting = true;
			} else {
				handPos.x += Input.mousePosition.x - oldMousePos.x;
				handPos.y -= Input.mousePosition.y - oldMousePos.y;
				oldMousePos = Input.mousePosition;
				shifterSpots.UpdateShifter(handPos, stick);
			}
		} else if (shifting){
			hpattern.SetActive(false);
			shifting = false;
		}

		look.enabled = !shifting;
	}

	void OnGUI() {
		GUI.DrawTexture (new Rect (handPos.x - 10, handPos.y - 10, 20, 20), handTexture);
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

	public void UpdateShifter(Vector3 handPos, GameObject shifter) {
		if (handPos.x < Screen.width / 3)
		{	
			if (handPos.y < Screen.height / 3) {
				if (spotIndex != 0 && spotIndex != 3 && spotIndex != 6) {
					spotIndex = 0;
				}
			} else if (handPos.y < Screen.height * 2 / 3) {
				spotIndex = 1;
			} else {
				if (spotIndex != 2 && spotIndex != 5 && spotIndex != 8) {

					spotIndex = 2;
				}
			}
		} else if (handPos.x < Screen.width * 2 / 3) {
			if (handPos.y < Screen.height / 3) {
				if (spotIndex != 0 && spotIndex != 3 && spotIndex != 6) {
					spotIndex = 3;
				}
			} else if (handPos.y < Screen.height * 2 / 3) {
				spotIndex = 4;
			} else {
				if (spotIndex != 2 && spotIndex != 5 && spotIndex != 8) {
					spotIndex = 5;
				}
			}
		} else {
			if (handPos.y < Screen.height / 3) {
				if (spotIndex != 0 && spotIndex != 3 && spotIndex != 6) {
					spotIndex = 6;
				}
			} else if (handPos.y < Screen.height * 2 / 3) {
				spotIndex = 7;
			} else {
				if (spotIndex != 2 && spotIndex != 5 && spotIndex != 8) {
					spotIndex = 8;
				}
			}
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
