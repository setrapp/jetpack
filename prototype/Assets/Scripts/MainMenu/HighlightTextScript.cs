using UnityEngine;
using System.Collections;

public class HighlightTextScript : MonoBehaviour {
	public string buttonName = "default";




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
		renderer.material.color = Color.white;
	}

	void OnMouseUp(){
		//Main Buttons
		if(buttonName == "Play"){
			Application.LoadLevel("level_1");
		}
		if(buttonName == "Settings"){
			Camera.main.GetComponent<MainMenuCamera>().desiredRot = Camera.main.GetComponent<MainMenuCamera>().SettingsCameraQat;
			//Camera.main.transform.rotation = Quaternion.Slerp (Camera.main.transform.rotation, new Quaternion(Camera.main.transform.rotation.x,Camera.main.transform.rotation.y+90, Camera.main.transform.rotation.z,Camera.main.transform.rotation.w),0.01f);
		}
		if(buttonName == "Help"){
			Camera.main.GetComponent<MainMenuCamera>().desiredRot = Camera.main.GetComponent<MainMenuCamera>().HelpCameraQat;
		}
		if(buttonName == "Exit"){
			Application.Quit();
		}
		if(buttonName == "Main"){
			Camera.main.GetComponent<MainMenuCamera>().desiredRot = Camera.main.GetComponent<MainMenuCamera>().MainCameraQat;
		}
		//Settings Buttons
		if(buttonName == "Volume"){
			Camera.main.GetComponent<MainMenuCamera>().desiredPos = Camera.main.GetComponent<MainMenuCamera>().VolCameraPos;
			Camera.main.GetComponent<MainMenuCamera>().desiredRot = Camera.main.GetComponent<MainMenuCamera>().VolCameraQat;
		}
		if(buttonName == "Controls"){
			Camera.main.GetComponent<MainMenuCamera>().desiredPos = Camera.main.GetComponent<MainMenuCamera>().ConCameraPos;
			Camera.main.GetComponent<MainMenuCamera>().desiredRot = Camera.main.GetComponent<MainMenuCamera>().ConCameraQat;
		}
		if(buttonName == "SettingsBackVol"){
			Camera.main.GetComponent<MainMenuCamera>().desiredPos = Camera.main.GetComponent<MainMenuCamera>().SettingsCameraPos;
			Camera.main.GetComponent<MainMenuCamera>().desiredRot = Camera.main.GetComponent<MainMenuCamera>().SettingsCameraQat;
		}
		if(buttonName == "SettingsBackCon"){
			Camera.main.GetComponent<MainMenuCamera>().desiredPos = Camera.main.GetComponent<MainMenuCamera>().SettingsCameraPos;
			Camera.main.GetComponent<MainMenuCamera>().desiredRot = Camera.main.GetComponent<MainMenuCamera>().SettingsCameraQat;
		}
	}
}