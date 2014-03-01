using UnityEngine;
using System.Collections;

public class JetpackScript : MonoBehaviour {

	// Use this for initialization
	void Start () {
	
	}
	public bool collided=false;
	public JetpackScript(){}
	
	// Update is called once per frame
	void Update () {
	
	}

	void OnCollisionEnter(Collision collision) { 
		collided=true;
	}
}
