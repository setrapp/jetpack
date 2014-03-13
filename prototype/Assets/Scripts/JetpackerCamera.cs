using UnityEngine;
using System.Collections;

public class JetpackerCamera : MonoBehaviour {
	public Transform character;
	public MouseLook look;
	public bool negateBehind = false;
	public float behind = 10.0f;
	public float minBehind = 5.0f;
	public float maxBehind = 20.0f;
	public bool negateAbove = false;
	public float above = 5.0f;
	public float minAbove = 2.5f;
	public float maxAbove = 10.0f;
	public float minDistance = 5.0f;
	public float maxDistance = 20.0f;
	public float lerpSpeed = 5.0f;
	public bool snapAbove = false;

	// Update is called once per frame
	void Update () {
		if (character == null) {
			return;
		}

		float lerpSpeedDT = lerpSpeed * Time.deltaTime;

		Vector3 behindDir = -character.forward;
		if (negateBehind) {
			behindDir *= -1;
		}
		Vector3 aboveDir = character.up;
		if (negateAbove) {
			aboveDir *= -1;
		}

		// Stay directly above and behind the character, within a distance range.
		Vector3 characterSpacePos = character.InverseTransformPoint(transform.position);
		characterSpacePos.x = 0;
		if (snapAbove) {
			//characterSpacePos.y = above;
		}
		// TODO Make above and behind limits work.
		/*if (Mathf.Abs(characterSpacePos.z) > Mathf.Abs(maxBehind)) {
			characterSpacePos.z = maxBehind * (negateBehind ? 1 : -1);
		} else if (Mathf.Abs(characterSpacePos.z) < Mathf.Abs(minBehind)){
			characterSpacePos.z = minBehind * (negateBehind ? 1 : -1);
		}
		if (Mathf.Abs(characterSpacePos.y) > Mathf.Abs(maxAbove)) {
			characterSpacePos.y = maxAbove * (negateBehind ? -1 : 1);
		} else if (Mathf.Abs(characterSpacePos.y) < Mathf.Abs(minAbove)) {
			characterSpacePos.y = minAbove * (negateBehind ? -1 : 1);
		}*/
		transform.position = character.TransformPoint(characterSpacePos);

		// Vector from character to current position.
		Vector3 fromCharacter = (transform.position - character.position);
		float fromCharacterDist = fromCharacter.magnitude;
		Vector3 fromCharacterDir = fromCharacter / fromCharacterDist;
	
		// Vector from character to desired follow position.
		Vector3 follow = (behindDir * behind) + (aboveDir * above);
		float followDist = follow.magnitude;
		Vector3 followDir = follow / followDist;

		// Vector from current position to desired follow position
		Vector3 toDesired = ((character.position + follow) - transform.position);
		float toDesiredDist = toDesired.magnitude;
		Vector3 toDesiredDir = toDesired / toDesiredDist;

		// Stay within a distance range from the character.
		// TODO Take this out when behind and above limits work.
		/*if (fromCharacterDist < minDistance) {
			transform.position = character.position + (fromCharacterDir * minDistance);
		}
		if (fromCharacterDist > maxDistance) {
			transform.position = character.position + (fromCharacterDir * maxDistance);
		}*/

		// Move towards desired follow position.
		if (lerpSpeed > 0 && (fromCharacterDist * fromCharacterDist) < (behind * behind) + (above * above) && toDesiredDist > lerpSpeedDT) {
			transform.position += toDesiredDir * lerpSpeedDT;
			Debug.Log("lerp");
		} else {
			transform.position = character.position + follow;
			lerpSpeed = 0;
			Debug.Log("no lerp");
		}

		// Look at focal point.
		transform.LookAt(character);

		// TODO Figure out how to add in control of vertical rotation.
		if (look != null) {
			//look.ForceRotation(false, true);
		}
	}
}
