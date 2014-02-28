using UnityEngine;
using System.Collections;

public class ID_AudioMusic : MonoBehaviour {

	// Use this for initialization
	void Start () {
	
	}
	
	// Update is called once per frame
	void Update () {
	
	}

	public void UpdateMusicVolumeLevel(float percentage){
		gameObject.audio.volume = percentage;
	}
}
