using UnityEngine;
using System.Collections;

public class FlappyJetpack : MonoBehaviour {

	GameObject flappyCamera, flappyJetpack;

	float gravityConstant = 9.8f;
	float timeSinceThrust = 0.0f;
	bool gameStarted= false;
	bool gameOver= false;
	JetpackScript j;
	// Use this for initialization
	void Start () {
		flappyCamera= GameObject.Find("flappyCamera");
		flappyJetpack= GameObject.Find("flappyJetpack");
		j=flappyJetpack.GetComponent<JetpackScript>();
	}
	
	// Update is called once per frame
	void Update () {
		if(gameStarted){
			Vector3 currentPos= this.transform.position;
			currentPos.z += Time.deltaTime * 50.0f;
			timeSinceThrust+=Time.deltaTime*3.0f;
			this.transform.position= currentPos;
			bool usingJetpack= false;

			Vector3 jetpackPos=flappyJetpack.transform.localPosition;

			if(Input.GetKey(KeyCode.D)){
				if(jetpackPos.x<=100.0f){
					jetpackPos.x+=Time.deltaTime * 50.0f;
				}
				else{
					jetpackPos.x= 100.0f;
				}
			}
			else if(Input.GetKey(KeyCode.A)){
				if(jetpackPos.x>=-100.0f){
					jetpackPos.x-=Time.deltaTime * 50.0f;
				}
				else{
					jetpackPos.x= -100.0f;
				}
			}
			 
			if(Input.GetKey(KeyCode.Space)){
				if(jetpackPos.y<=50.0f){
					timeSinceThrust=0.0f;
					jetpackPos.y+=Time.deltaTime * 70.0f;
				}
			}

			if(jetpackPos.y<=-50.0f){
				gameStarted=false;
				gameOver=true;
			}

			if(j.collided==true){
				gameStarted=false;
				gameOver=true;
			}

			jetpackPos.y-=(20.0f*Time.deltaTime) + (Time.deltaTime * 40.0f * timeSinceThrust);
			flappyJetpack.transform.localPosition= jetpackPos;
		}
		else{
			if(Input.GetKey(KeyCode.Space) && gameStarted==false){
				j.collided=false;
				this.transform.position= new Vector3(1000,60,200);
				flappyJetpack.transform.localPosition= new Vector3(0,30,100);
				gameStarted=true;

			}

		}

	}

	void OnGUI(){
		GUIStyle myStyle = new GUIStyle();
		myStyle.normal.textColor= Color.red;
		myStyle.fontSize=40;
		if(!gameStarted){
			if(!gameOver)
			GUI.Label (new Rect (Screen.width/2-350, Screen.height/2-50, 100, 20), "Press your jetpack button (Space) to start", myStyle);
			else{
				GUI.Label (new Rect (Screen.width/2-250, Screen.height/2-50, 100, 20), "Game over! Press Space to try again", myStyle);
			}

		}


	}

	void OnCollisionEnter(Collision collision) { 
		
	}
}
