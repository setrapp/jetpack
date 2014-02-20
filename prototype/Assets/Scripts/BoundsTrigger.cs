using UnityEngine;
using System.Collections;

public class BoundsTrigger : MonoBehaviour {
	public ManualJetpack jetpack;

	void OnTriggerEnter(Collider collider)
	{
		/*if (collider != null && collider.gameObject != gameObject &&  collider.gameObject.tag == "Bounds") {
			jetpack.FlipJetpackToAlign(-collider.transform.forward, -collider.transform.up, jetpack.cameraSwitchSpeed * Time.deltaTime);
		}*/
	}
}
