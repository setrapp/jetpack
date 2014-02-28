using UnityEngine;
using System.Collections;

public class SubtitlesOn : MonoBehaviour {
	// Use this for initialization
	void Start () {
	
	}
	
	// Update is called once per frame
	void Update () {
	
	}

	void OnMouseEnter(){
		//change the color
		renderer.material.color = Color.green;
	}

	void OnMouseExit(){
		bool temp = GameObject.Find("GlobalVariables").GetComponent<GlobalVariables>().displaySubtitles;
		if(temp){
			renderer.material.color = Color.yellow;
		}else{
			renderer.material.color = Color.white;
		}
	}

	void OnMouseUp(){
		GameObject.Find("GlobalVariables").GetComponent<GlobalVariables>().UpdateSubtitles(true);
	}
}
