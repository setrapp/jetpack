using UnityEngine;
using System.Collections;

public class JetPack : MonoBehaviour {
	private bool Left_Up_Engine = false;
	private bool Right_Up_Engine = false;
	private bool Left_Forward_Engine = false;
	private bool Right_Forward_Engine = false;
	private bool Left_Strafe_Engine = false;
	private bool Right_Strafe_Engine = false;
	private bool Left_Rocket_Launcher = false;
	private bool Right_Rocket_Launcher = false;


	//unity help
	public Rigidbody rocket;
	public float speed = 100f;

	// Use this for initialization
	void Start () {
		
	}
	
	// Update is called once per frame
	void Update () {
		CheckInput();
		//The UP Thrusters
		if(Left_Up_Engine){
			rigidbody.AddRelativeForce(Vector3.up * 10);
			rigidbody.AddRelativeTorque(-Vector3.forward);
			GameObject.FindGameObjectWithTag("Left_Up_Engine_fire").renderer.enabled = true;
		}else if(!Left_Up_Engine){
			GameObject.FindGameObjectWithTag("Left_Up_Engine_fire").renderer.enabled = false;
		}
		if(Right_Up_Engine){
			rigidbody.AddRelativeForce(Vector3.up * 10);
			rigidbody.AddRelativeTorque(Vector3.forward);
			GameObject.FindGameObjectWithTag("Right_Up_Engine_fire").renderer.enabled = true;
		}else if(!Left_Up_Engine){
			GameObject.FindGameObjectWithTag("Right_Up_Engine_fire").renderer.enabled = false;
		}
		//The Forward Thrusters
		if(Left_Forward_Engine){
			rigidbody.AddRelativeForce(Vector3.forward * 10);
			rigidbody.AddRelativeTorque(Vector3.up * 6);
			GameObject.FindGameObjectWithTag("Left_Forward_Engine_fire").renderer.enabled = true;
		}else if(!Left_Forward_Engine){
			GameObject.FindGameObjectWithTag("Left_Forward_Engine_fire").renderer.enabled = false;
		}
		if(Right_Forward_Engine){
			rigidbody.AddRelativeForce(Vector3.forward * 10);
			rigidbody.AddRelativeTorque(Vector3.down * 6);
			GameObject.FindGameObjectWithTag("Right_Forward_Engine_fire").renderer.enabled = true;
		}else if(!Right_Forward_Engine){
			GameObject.FindGameObjectWithTag("Right_Forward_Engine_fire").renderer.enabled = false;
		}
		//The Strafe Thrusters
		if(Left_Strafe_Engine){
			rigidbody.AddRelativeForce(Vector3.right * 15);
			//rigidbody.AddRelativeTorque(Vector3.up * 5);
			GameObject.FindGameObjectWithTag("Left_Strafe_Engine_fire").renderer.enabled = true;
		}else if(!Left_Forward_Engine){
			GameObject.FindGameObjectWithTag("Left_Strafe_Engine_fire").renderer.enabled = false;
		}
		if(Right_Strafe_Engine){
			rigidbody.AddRelativeForce(Vector3.left * 15);
			//rigidbody.AddRelativeTorque(Vector3.down * 5);
			GameObject.FindGameObjectWithTag("Right_Strafe_Engine_fire").renderer.enabled = true;
		}else if(!Right_Forward_Engine){
			GameObject.FindGameObjectWithTag("Right_Strafe_Engine_fire").renderer.enabled = false;
		}
		//The Rocket Launcher
		if(Left_Rocket_Launcher){
			Rigidbody rocketClone = (Rigidbody) Instantiate(rocket, new Vector3(transform.position.x-1,transform.position.y,transform.position.z), transform.rotation);
			rocketClone.velocity = transform.forward * speed;
		}else if(!Left_Forward_Engine){

		}
		if(Right_Rocket_Launcher){
			Rigidbody rocketClone = (Rigidbody) Instantiate(rocket, new Vector3(transform.position.x+1,transform.position.y,transform.position.z), transform.rotation);
			rocketClone.velocity = transform.forward * speed;
		}else if(!Right_Forward_Engine){

		}
		
		
		//check loss
		if(transform.position.y <= -8){
			rigidbody.velocity = new Vector3(0, 0, 0);
			rigidbody.angularVelocity = new Vector3(0,0,0);
			transform.position = new Vector3(0f,2.5f,0f);
			transform.rotation = Quaternion.identity;
		}
	}
	
	void CheckInput(){
		//Left_Up_Engine
		if (Input.GetKeyDown ("f")){
			Left_Up_Engine = true;
			GameObject.FindGameObjectWithTag("Left_Up_Engine_fire").audio.Play();
		}else if (Input.GetKeyUp("f")){
			Left_Up_Engine = false;
			GameObject.FindGameObjectWithTag("Left_Up_Engine_fire").audio.Stop();
		}
		//Right_Up_Engine
		if (Input.GetKeyDown ("j")){
			Right_Up_Engine = true;
			GameObject.FindGameObjectWithTag("Right_Up_Engine_fire").audio.Play();
		}else if (Input.GetKeyUp("j")){
			Right_Up_Engine = false;
			GameObject.FindGameObjectWithTag("Right_Up_Engine_fire").audio.Stop();
		}
		//Left_Forward
		if (Input.GetKeyDown ("d")){
			Left_Forward_Engine = true;
			GameObject.FindGameObjectWithTag("Left_Forward_Engine_fire").audio.Play();
		}else if (Input.GetKeyUp("d")){
			Left_Forward_Engine = false;
			GameObject.FindGameObjectWithTag("Left_Forward_Engine_fire").audio.Stop();
		}
		//Right_Forward
		if (Input.GetKeyDown ("k")){
			Right_Forward_Engine = true;
			GameObject.FindGameObjectWithTag("Right_Forward_Engine_fire").audio.Play();
		}else if (Input.GetKeyUp("k")){
			Right_Forward_Engine = false;
			GameObject.FindGameObjectWithTag("Right_Forward_Engine_fire").audio.Stop();
		}
		//Left_Strafe
		if (Input.GetKeyDown ("s")){
			Left_Strafe_Engine = true;
			GameObject.FindGameObjectWithTag("Left_Strafe_Engine_fire").audio.Play();
		}else if (Input.GetKeyUp("s")){
			Left_Strafe_Engine = false;
			GameObject.FindGameObjectWithTag("Left_Strafe_Engine_fire").audio.Stop();
		}
		//Right_Strafe
		if (Input.GetKeyDown ("l")){
			Right_Strafe_Engine = true;
			GameObject.FindGameObjectWithTag("Right_Strafe_Engine_fire").audio.Play();
		}else if (Input.GetKeyUp("l")){
			Right_Strafe_Engine = false;
			GameObject.FindGameObjectWithTag("Right_Strafe_Engine_fire").audio.Stop();
		}
//		//Left_Rocket
//		if (Input.GetKeyDown ("a")){
//			Left_Rocket_Launcher = true;
//		}else if (Input.GetKeyUp("a")){
//			Left_Rocket_Launcher = false;
//		}
//		//Right_Rocket
//		if (Input.GetKeyDown (";")){
//			Right_Rocket_Launcher = true;
//		}else if (Input.GetKeyUp(";")){
//			Right_Rocket_Launcher = false;
//		}
	}
}
