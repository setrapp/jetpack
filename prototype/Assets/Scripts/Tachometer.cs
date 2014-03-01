using UnityEngine;
using System.Collections;

public class Tachometer : MonoBehaviour
{
	public ManualJetpack jetpack = null;
	public float criticalRatio;
	public Texture background = null;
	public Texture foreground = null;
	public Texture dangerForeground = null;
	public float dangerRatio;

	void Update() {
		criticalRatio = (jetpack.jetpackSpeed - jetpack.gears[jetpack.inGear].minSpeed) / (jetpack.gears[jetpack.inGear].maxSpeed - jetpack.gears[jetpack.inGear].minSpeed);
	}

	void OnGUI() {
		string gear = "" + jetpack.inGear;
		if (jetpack.inGear == 0) {
			gear = "Neutral";
		} else if (jetpack.inGear == 6) {
			gear = "Hover";
		}
		//GUI.Box(new Rect (150, 10, 200, 20));
		GUI.DrawTexture (new Rect (15, 10, 20, 200), background);
		float foregroundHeight = 200 * Mathf.Clamp (criticalRatio, 0.0f, 1.0f);
		Texture renderedForeground = (jetpack.inGear != 0 && (criticalRatio < dangerRatio || criticalRatio > 1.0f - dangerRatio)) ? dangerForeground : foreground;
		//Debug.Log ((criticalRatio < dangerRatio || criticalRatio > 1.0f - dangerRatio));
		GUI.DrawTexture (new Rect (15, 10 + (200 - foregroundHeight), 20, foregroundHeight), renderedForeground);
		GUI.Label (new Rect (10, 220, 200, 20), "RPM?");
		/*GUI.Label (new Rect (150, 40, 200, 20), "Min Speed: " + (int)jetpack.gears[jetpack.inGear].minSpeed);
		GUI.Label (new Rect (150, 70, 200, 20), "Cur Speed: " + (int)jetpack.jetpackSpeed);
		GUI.Label (new Rect (150, 100, 200, 20), "Max Speed: " + (int)jetpack.gears[jetpack.inGear].maxSpeed);
		*/
	}
}

