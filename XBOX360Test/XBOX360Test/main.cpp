#include "CXBOXController.h"
#include <iostream>

CXBOXController* Player1;
int main(int argc, char* argv[])
{
	Player1 = new CXBOXController(1);

	while(true)
	{
		if(Player1->IsConnected())
		{
			XINPUT_STATE state = Player1->GetState();
			float LX = state.Gamepad.sThumbLX;
			float LY = state.Gamepad.sThumbLY;
			float RX = state.Gamepad.sThumbRX;
			float RY = state.Gamepad.sThumbRY;
			
			float LT = state.Gamepad.bLeftTrigger;
			float RT = state.Gamepad.bRightTrigger;

			 std::system ( "CLS" );

			std::cout << "Left Stick X Value: " << LX << "\n";
			std::cout << "Left Stick Y Value: " << LY << "\n";

			std::cout << "Right Stick X Value: " << RX << "\n";
			std::cout << "Right Stick Y Value: " << RY << "\n";

			std::cout << "Left Trigger Value: " << LT << "\n";
			std::cout << "Right Trigger Value: " << RT << "\n";

			std::cout << "Left Bumper Pushed Down: " << ((state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER)!=0) << "\n";
			std::cout << "Right Bumper Pushed Down: " << ((state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER)!=0) << "\n";

			std::cout << "A Button Pushed Down: " << ((state.Gamepad.wButtons & XINPUT_GAMEPAD_A)!=0) << "\n";
			std::cout << "B Button Pushed Down: " << ((state.Gamepad.wButtons & XINPUT_GAMEPAD_B)!=0) << "\n";
			std::cout << "Y Button Pushed Down: " << ((state.Gamepad.wButtons & XINPUT_GAMEPAD_Y)!=0) << "\n";
			std::cout << "X Button Pushed Down: " << ((state.Gamepad.wButtons & XINPUT_GAMEPAD_X)!=0) << "\n\n";


			std::cout << "Up DPad Pushed Down: " << ((state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP)!=0) << "\n";
			std::cout << "Left DPad Pushed Down: " << ((state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT)!=0) << "\n";
			std::cout << "Down DPad Pushed Down: " << ((state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN)!=0) << "\n";
			std::cout << "Right DPad Pushed Down: " << ((state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT)!=0) << "\n";

			std::cout << "Back Pushed Down: " << ((state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK)!=0) << "\n";
			std::cout << "Start Bumper Pushed Down: " << ((state.Gamepad.wButtons & XINPUT_GAMEPAD_START)!=0) << "\n";



			Player1->Vibrate(LT*256, RT*256);


			/*if(Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_A)
			{
				Player1->Vibrate(65535, 0);
			}

			if(Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_B)
			{
				Player1->Vibrate(0, 65535);
			}

			if(Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_X)
			{
				Player1->Vibrate(65535, 65535);
			}

			if(Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_Y)
			{
				Player1->Vibrate();
			}

			if(Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_BACK)
			{
				break;
			}*/
		}
		else
		{
			std::cout << "\n\tERROR! PLAYER 1 - XBOX 360 Controller Not Found!\n";
			std::cout << "Press Any Key To Exit.";
			std::cin.get();
			break;
		}
	}

	delete(Player1);

	return( 0 );
}