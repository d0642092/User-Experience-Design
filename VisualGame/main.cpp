#include "GameController.h"

bool finish_menu(VideoCapture, GameControl);
int main(int argc, char** argv){
	VideoCapture cap(0);
	Mat video, validVisual, gameVisual;
	GameControl gameControl = GameControl();
	cap >> video;
	// talk video size and type;
	gameControl.init_game(video);
	while (waitKey(1) == -1) {
		cap >> video;
		if (video.empty())
			break;
		// detect color position and combine game img
		gameVisual = gameControl.get_now_status(&video, &validVisual);
		imshow("Valid Area", validVisual);
		
		// game rule
		if (gameControl.isFinish(&gameVisual)) {
			waitKey(300);
			gameControl.reset_detect_center();
			destroyAllWindows();
			if (!finish_menu(cap, gameControl))
				break;
			destroyAllWindows();
			gameControl.init_game(video);
		};

		// combine with camera
		addWeighted(gameVisual, 10.0, video, 0.5, 1, gameVisual);
		imshow("Game", gameVisual);
	}
	cap.release();
	return 0;
}

bool finish_menu(VideoCapture cap, GameControl gameControl) {
	Mat video, validVisual, finishVisual;
	int choose, buffer=0;
	while (waitKey(1) == -1) {
		cap >> video;
		if (video.empty())
			break;
		finishVisual = gameControl.get_now_status(&video, &validVisual);
		imshow("Valid Area", validVisual);;
		
		addWeighted(finishVisual, 10.0, video, 0.5, 1, finishVisual);
		imshow("Game", finishVisual);
		
		choose = gameControl.isRestart();
		buffer += 1;
		if (choose == 0 && buffer > 10)
			return false;
		else if (choose == 1 && buffer > 10)
			return true;
	}
	cap.release();
	return false;
}