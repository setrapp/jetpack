using UnityEngine;
using System.Collections;

public class Shifter : MonoBehaviour {
	private bool shifting = false;
	public GameObject hpattern = null;
	public GameObject stick = null;
	public Camera camera = null;
	public MouseLook look = null;
	private bool blocked = false;
	private Vector3 oldMouseWorldPos;

	void Update() {
		//ShiftGears
		/*if (Input.GetAxis("ShiftGears") != 0) {
			if (!shifting) {
				hpattern.SetActive(true);
				Vector3 hpatternToStick = stick.transform.position - hpattern.transform.position;
				oldMouseWorldPos = camera.ScreenToWorldPoint(new Vector3 ((float)Input.mousePosition.x, (float)Input.mousePosition.y, stick.transform.position.z - camera.transform.position.z));
				PutStickAtMouse (false);
				hpattern.transform.position = stick.transform.position - hpatternToStick;
				shifting = true;
			} else {
				PutStickAtMouse (true);
			}
		} else if (shifting){
			hpattern.SetActive(false);
			PutStickAtMouse(false);
			shifting = false;
		}

		look.enabled = !shifting;*/
	}

	private void PutStickAtMouse(bool keepInBox) {

		Vector3 mouseWorldPos = camera.ScreenToWorldPoint(new Vector3 ((float)Input.mousePosition.x, (float)Input.mousePosition.y, stick.transform.position.z - camera.transform.position.z));
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
				stickPos = hit.point - (toStickPosDir / 100);
				//stick.transform.position = hit.point;
				tempBlocked = true;
				//Debug.Log ("blocked");
			} else {
				tempBlocked = false;
			}
		}

		if (!blocked) {
			stickPos.z = stick.transform.position.z;
			stick.transform.position = stickPos;
		} else {
			//mouseOffset = 
		}
		blocked = tempBlocked;
		oldMouseWorldPos = mouseWorldPos;
	}
}
