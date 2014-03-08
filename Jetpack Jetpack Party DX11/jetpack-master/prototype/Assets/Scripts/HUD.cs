using UnityEngine;
using System.Collections;

public class HUD : MonoBehaviour {
	public ManualJetpack jetpack = null;

	void OnGUI() {
		string gear = "" + jetpack.inGear;
		if (jetpack.inGear == 0) {
			gear = "Hover";
		}
		GUI.Label (new Rect (10, 10, 200, 20), "Gear: " + gear);
		GUI.Label (new Rect (10, 40, 200, 20), "Min Speed: " + (int)jetpack.gears[jetpack.inGear].minSpeed);
		GUI.Label (new Rect (10, 70, 200, 20), "Cur Speed: " + (int)jetpack.jetpackSpeed);
		GUI.Label (new Rect (10, 100, 200, 20), "Max Speed: " + (int)jetpack.gears[jetpack.inGear].maxSpeed);
	}
}
