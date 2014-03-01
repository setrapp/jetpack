using UnityEngine;
using System.Collections;

public class ID_AudioSFX : MonoBehaviour {

	// Use this for initialization
	void Start () {
	
	}
	
	// Update is called once per frame
	void Update () {
	
	}

	public void UpdateSFXVolumeLevel(float percentage){
		gameObject.audio.volume = percentage;
	}
}
