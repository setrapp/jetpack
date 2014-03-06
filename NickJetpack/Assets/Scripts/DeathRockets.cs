using UnityEngine;
using System.Collections;

public class DeathRockets : MonoBehaviour {

	// Use this for initialization
	void Start () {
	
	}
	
	// Update is called once per frame
	void Update () {
		if(transform.position.y <= -8){
			Destroy(gameObject);
		}
	}
}
