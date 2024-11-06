#include "GamesEngineeringBase.h"
#include <fstream>
#include <iostream>

using namespace std;
using namespace GamesEngineeringBase;


class Tile {
private:
	bool collidable;
	Image appearance;
public:
	Tile() {}
	void load(string filename, bool passable) {
		appearance.load(filename);
		collidable = !passable;
	}
	void draw(Window& canvas, int x, int y) {
		for (unsigned int i = 0; i < appearance.height; i++){
			if (y + i > 0 && (y + i) < canvas.getHeight()) {
				for (unsigned int j = 0; j < appearance.width; j++) {
					if (x + j > 0 && (x + j) < canvas.getWidth()){
						canvas.draw((x + j), (y + i), appearance.at(j, i));
					}
				}
			}
		}
	}

};

const unsigned int tileNum = 10;
class TileSet {
public:
	Tile tiles[tileNum];
	bool passable[tileNum];
	unsigned int size = tileNum;
	TileSet() {}
	void load() {
		for (unsigned int i = 0; i < tileNum; i++) {
			passable[i] = true;
		}
		ifstream infile("impassible.txt");
		unsigned int index = 0;
		string s;
		while (getline(infile, s)) {
			int l = s.length();
			for (unsigned int i = 0; i < l; i += 2) {
				int x = (int)s[i];
				passable[x] = false;
				index++;
			}
		}
		for (unsigned int i = 0; i < size; i++) {
			string filename;
			filename = "Resources/" + s + to_string(i) + ".png";
			tiles[i].load(filename,passable[i]);
		}
	}
};


const int maxWidth = 48;
const int maxHeight = 36;

class World {
private:
	TileSet tiles;
	unsigned int a[maxWidth][maxHeight];
public:
	World(string filename) {
		tiles.load();
		ifstream infile(filename);
		for (int i = 0; i < maxHeight; i++) {
			for (int j = 0; j < maxWidth; j++) {
				infile >> a[j][i];
			}
			infile.ignore(1, '\n');

		}
		infile.close();
	}
	void draw(Window& canvas, int wx, int wy) {
		unsigned 
	}
};


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