using UnityEngine;
using System.Collections;

public class JetpackMenu : MonoBehaviour {

	int jetPackType=0;//1:manual 2:qwopjet 3:flappy
	int location= 0; //1:manual 2:flappy
	bool started=false;
	Vector3[] spawnPoints = new Vector3[3];
	public GameObject manualJet;
	public GameObject qwopJet;
	public GameObject flappyJet;
	public GameObject flappyObstacles;

	// Use this for initialization
	void Start () {
		spawnPoints= new Vector3[3];
		spawnPoints[0]= new Vector3(486.0f,126.5f,152.0f);
		spawnPoints[1]= new Vector3(1000.0f,37.0f,230.9f);
		spawnPoints[2]= new Vector3(1007.12f,53.77f, 901.4f);
	}
	
	// Update is called once per frame
	void Update () {
	
	}


	void OnGUI () {
		if(!started){
			if (GUI.Button (new Rect (10,10,150,50), "Manual environment\n")) {
				location=1;
			}
			if (GUI.Button (new Rect (210,10,150,50), "Flappy environment\n")) {
				location=2;
			}
			if (GUI.Button (new Rect (10,110,150,50), "Manual Jetpack ")) {
				jetPackType=1;
			}
			if (GUI.Button (new Rect (210,110,150,50), "Qwopjet ")) {
				jetPackType=2;
			}
			if (GUI.Button (new Rect (410,110,150,50), "Flappy Jetpack ")) {
				jetPackType=3;
			}
			if (GUI.Button (new Rect (410,210,200,200), "Start ")) {
				startGame();
			}
			string s= "";
			if(location==1)
				s+="Environment: Manual ";
			if(location==2)
				s+="Environment: Flappy ";
			if(jetPackType==1)
				s+="Jetpack: Manual";
			if(jetPackType==2)
				s+="Jetpack: QwopJet";
			if(jetPackType==3)
				s+="Jetpack: Flappy";


			GUI.Label (new Rect(10,210,350,50), s);

		}
	}

	void startGame(){
		if(location>0 && jetPackType>0){

			Vector3 spawnLoc= spawnPoints[location-1];
			if(location==2)
			Instantiate(flappyObstacles, spawnPoints[2], transform.rotation);
			switch(jetPackType){
			case 1:
				Instantiate(manualJet, spawnLoc, transform.rotation);
				break;
			case 2:
				Instantiate(qwopJet, spawnLoc, transform.rotation);
				break;
			case 3:
				Instantiate(flappyJet, spawnLoc, transform.rotation);
				break;

			
			}
			started=true;

		}

	}
}
