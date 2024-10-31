#include "GamesEngineeringBase.h"

using namespace std;
using namespace GamesEngineeringBase;

int countTrues(bool going[4]) {
	int trues = 0;
	for (int i = 0; i < 4; i++) {
		if (going[i]) trues++;
	}
	return trues;
}

int main() {
	Window canvas;
	canvas.create(1024, 768, "Roberto the roboto");
	bool running = true;
	Image image;
	image.load("Resources/RobotDown.png");
	unsigned int robotX = (canvas.getWidth() / 2) - (image.width / 2);
	unsigned int robotY = (canvas.getHeight() / 2) - (image.height / 2);
	bool playerMoving[4]{false};

	while (running)
	{
		canvas.checkInput();
		canvas.clear();
		if (canvas.keyPressed(VK_ESCAPE)) break;
		if (canvas.keyPressed('W')) {
			playerMoving[0] = true;
			if (countTrues(playerMoving) == 1) {
				image.load("Resources/RobotUp.png");
			}
			robotY -= 2;
			
		}
		else {
			playerMoving[0] = false;
		}
		if (canvas.keyPressed('S')) {
			playerMoving[1] = true;
			if (countTrues(playerMoving) == 1) {
				image.load("Resources/RobotDown.png");
			}
			robotY += 2;
			
		}
		else {
			playerMoving[1] = false;
		}
		if (canvas.keyPressed('A')) {
			playerMoving[2] = true;
			if (countTrues(playerMoving) == 1) {
				image.load("Resources/RobotLeft.png");
			}
			robotX -= 2;
			
		}
		else {
			playerMoving[2] = false;
		}
		if (canvas.keyPressed('D')) {
			playerMoving[3] = true;
			if (countTrues(playerMoving) == 1) {
				image.load("Resources/RobotRight.png");
			}
			robotX += 2;
			
		}
		else {
			playerMoving[3] = false;
		}

		for (unsigned int x = 0; x < canvas.getWidth(); x++)
			for (unsigned int y = 0; y < canvas.getHeight(); y++)
				canvas.draw(x, y, 0, 0, 255);
		
		for (unsigned int i = 0; i < image.height; i++)
			if (robotY + i < canvas.getHeight() && robotY + i >= 0) {
				for (unsigned int j = 0; j < image.width; j++)
				{
					if (robotX + j < canvas.getWidth() && robotX + j >= 0) {
						if (image.alphaAt(j, i) > 50)
							canvas.draw(robotX + j, robotY + i, image.at(j, i));
					}
				}
			}
		canvas.present();

	}
}