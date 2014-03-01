using UnityEngine;
using System.Collections;

public class ID_AudioDialogue : MonoBehaviour {

	// Use this for initialization
	void Start () {
	
	}
	
	// Update is called once per frame
	void Update () {
	
	}

	public void UpdateDialogueVolumeLevel(float percentage){
		gameObject.audio.volume = percentage;
	}
}
