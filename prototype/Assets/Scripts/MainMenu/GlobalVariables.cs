using UnityEngine;
using System.Collections;

public class GlobalVariables : MonoBehaviour {
	public bool displaySubtitles = false;
	// Use this for initialization
	void Start () {
		GameObject.Find("Subtitles - On").renderer.material.color = Color.white;
		GameObject.Find("Subtitles - Off").renderer.material.color = Color.yellow;
	}
	
	// Update is called once per frame
	void Update () {
	
	}

	public void UpdateSubtitles(bool showSubs){
		displaySubtitles = showSubs;
		if(displaySubtitles){
			GameObject.Find("Subtitles - On").renderer.material.color = Color.yellow;
			GameObject.Find("Subtitles - Off").renderer.material.color = Color.white;
		}else{
			GameObject.Find("Subtitles - On").renderer.material.color = Color.white;
			GameObject.Find("Subtitles - Off").renderer.material.color = Color.yellow;
		}
	}
}
