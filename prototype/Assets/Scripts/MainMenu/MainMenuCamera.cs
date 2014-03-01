using UnityEngine;
using System.Collections;

public class MainMenuCamera : MonoBehaviour {
	public Quaternion desiredRot;
	public Vector3 desiredPos;
	public Quaternion SettingsCameraQat;
	public Vector3 SettingsCameraPos;
	public Quaternion HelpCameraQat;
	public Quaternion MainCameraQat;
	public Quaternion VolCameraQat;
	public Vector3 VolCameraPos;
	public Quaternion ConCameraQat;
	public Vector3 ConCameraPos;
	// Use this for initialization
	void Start () {
		desiredRot = new Quaternion();
		desiredPos = new Vector3(0f,4.53f,-8.84f);

		SettingsCameraQat = new Quaternion(0.06f,0.73f,-.06f,0.67f);
		SettingsCameraPos = new Vector3(0f,4.5f,-8.84f);
		HelpCameraQat = new Quaternion(0.05f,-.74f,0.07f,0.67f);
		MainCameraQat = new Quaternion(.09f,.04f,.004f,.995f);
		VolCameraQat = new Quaternion(0f,0.67f,0f,.74f);
		VolCameraPos = new Vector3(1.58f,14.35f,-6.18f);
		ConCameraQat = new Quaternion(0f,0.7f,0f,0.7f);
		ConCameraPos = new Vector3(.85f,13.59f,-18.43f);
	}
	
	// Update is called once per frame
	void Update () {
		//desiredRot = Camera.main.transform.rotation; //for getting info
		//desiredPos = Camera.main.transform.position; //for getting info
	}

	void FixedUpdate(){
		Camera.main.transform.position = Vector3.Slerp(Camera.main.transform.position, desiredPos, .1f);
		Camera.main.transform.rotation = Quaternion.Slerp(Camera.main.transform.rotation, desiredRot, .1f);
	}
}
