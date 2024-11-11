#include "GamesEngineeringBase.h"
#include <fstream>
#include <iostream>

using namespace std;
using namespace GamesEngineeringBase;

double checkMagnitude(int x1, int x2, int y1, int y2) {
	int xa = abs(x1 - x2);
	int ya = abs(y1 - y2);

	return sqrt(xa*xa + ya*ya);
}

double checkAngle(int x1, int x2, int y1, int y2) {
	if (x1 == x2 && y1 == y2) return 0.f;
	if (y1 < y2) {
		return asin(abs(y1 - y2) / checkMagnitude(x1, x2, y1, y2));
	}
	else {
		return asin(abs(x1 - x2) / checkMagnitude(x1, x2, y1, y2));
	}
}


class Tile {
private:
	Image appearance;
public:
	bool collidable;
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

const unsigned int tileNum = 4;
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
		ifstream infile("Resources/impassable.txt");
		unsigned int index = 0;
		string s;
		while (getline(infile, s)) {
			int l = s.length();
			for (unsigned int i = 0; i < l; i += 2) {
				int x = (int)s[i];
				x = x - 48;
				passable[x] = false;
				index++;
			}
		}
		for (unsigned int i = 0; i < size; i++) {
			string filename;
			filename = "Resources/" + to_string(i) + ".png";
			tiles[i].load(filename,passable[i]);
		}
	}
	Tile& operator[](unsigned int index) { return tiles[index]; }
};

class EnemyProjectile {
public:
	int x;
	int y;
	Image* sprite;
	int targetX;
	int targetY;
	int speed = 5;
	int damage = 3;
	bool friendly;
	bool xPositive = false;
	bool yPositive = false;
	bool active = true;

	EnemyProjectile() {};
	EnemyProjectile(int _x, int _y, int _targetX, int _targetY, bool _friendly, Image* _sprite) {
		x = _x;
		y = _y;
		targetX = _targetX;
		targetY = _targetY;
		friendly = _friendly;
		sprite = _sprite;
		if (targetX > x) xPositive = true;
		if (targetY > y) yPositive = true;
	}
	void draw(Window& canvas, int px, int py) {
		if (active) {
			int targetX;
			int targetY;
			for (int i = 0; i < sprite->width; i++) {
				targetX = (464 + ((px - x) + i));
				for (int j = 0; j < sprite->height; j++) {
					targetY = (336 + ((py - y) + j));

					if (targetX >= 0 && targetX < 1024) {
						if (targetY >= 0 && targetY < 768) {
							canvas.draw(targetX, targetY, sprite->at(i, j));
						}
					}
				}
			}
		}
	}
	void update(int px, int py) {
		if (active) {
			double angleDif;
			double bestAngleDif = 10.f;
			int candidateX;
			int candidateY;
			int bestX = 0;
			int bestY = 0;
			double candidateAngle;
			double correctAngle = checkAngle(x, targetX, y, targetY);

			for (int i = 0; i <= speed; i++) {
				candidateX = i;
				if (!xPositive) candidateX = -candidateX;
				candidateY = speed - i;
				if (!yPositive) candidateY = -candidateY;
				candidateAngle = checkAngle(x + candidateX, targetX, y + candidateY, targetY);
				if (candidateAngle > correctAngle) {
					angleDif = abs(candidateAngle - correctAngle);
				}
				else {
					angleDif = abs(correctAngle - candidateAngle);
				}

				if (angleDif < bestAngleDif) {
					bestAngleDif = angleDif;
					bestX = candidateX;
					bestY = candidateY;
				}
			}
			x = x + bestX;
			y = y + bestY;

			if ((xPositive && x >= targetX - speed) || (!xPositive && x <= targetX + speed)) {
				if ((yPositive && y >= targetY - speed) || (!yPositive && y <= targetY + speed)) {
					active = false;
				}
			}
		}
	}
};

class Enemy {
public:
	Image* sprite;
	int currentHealth;
	int maxHealth;
	int speed;
	int x;
	int y;
	void draw(Window& canvas, int px, int py) {
		if (currentHealth > 0) {
			int targetX;
			int targetY;
			for (int i = 0; i < sprite->width; i++) {
				targetX = (464 + ((px - x) + i));
				for (int j = 0; j < sprite->height; j++) {
					targetY = (336 + ((py - y) + j));

					if (targetX >= 0 && targetX < 1024) {
						if (targetY >= 0 && targetY < 768) {
							canvas.draw(targetX, targetY, sprite->at(i, j));
						}
					}
				}
			}
		}
	}
	virtual void drawChildren(Window& canvas, int px, int py) = 0;
	virtual void update(Image* ammoSprite, int pX, int pY) = 0;
	virtual bool childrenCollision(int pX, int pY) = 0;

};

class Meleer : public Enemy {
public:
	Meleer(Image* _sprite, int _x, int _y, int _speed, int _hp) {
		speed = _speed;
		x = _x;
		y = _y;
		maxHealth = _hp;
		currentHealth = _hp;
		sprite = _sprite;
	}
	void update(Image* ammoSprite, int pX, int pY) {
		if (currentHealth > 0) {
			if (pX > x) {
				x = x + speed;
			}
			if (pX < x) {
				x = x - speed;
			}
			if (pY > y) {
				y = y + speed;
			}
			if (pY < y) {
				y = y - speed;
			}
		}
	}
	void drawChildren(Window& canvas, int px, int py) {
	}
	bool childrenCollision(int pX, int pY) { 
		return false; 
	}
};

class Sentry : public Enemy {
public:
	int projectileCooldown = 50;
	int frameCounter = 0;
	int projectilesFired = 0;
	EnemyProjectile* projectiles[1000];
	Sentry(Image* _sprite, int _x, int _y, int _hp) {
		x = _x;
		y = _y;
		maxHealth = _hp;
		currentHealth = _hp;
		sprite = _sprite;
	}
	void update(Image* ammoSprite, int pX, int pY) {
		if (currentHealth > 0) {
			frameCounter++;
			if (frameCounter == projectileCooldown) {
				frameCounter = 0;
				projectiles[projectilesFired] = new EnemyProjectile(x - (sprite->width/2), y-(sprite->height/2), pX - 48, pY - 48, false, ammoSprite);
				projectilesFired++;
			}
		}
		for (int i = 0; i < projectilesFired; i++) {
			projectiles[i]->update(pX, pY);
		}
	}
	void draw(Window& canvas, int px, int py) {
		if (currentHealth > 0) {
			int targetX;
			int targetY;
			for (int i = 0; i < sprite->width; i++) {
				targetX = (464 + ((px - x) + i));
				for (int j = 0; j < sprite->height; j++) {
					targetY = (336 + ((py - y) + j));

					if (targetX >= 0 && targetX < 1024) {
						if (targetY >= 0 && targetY < 768) {
							canvas.draw(targetX, targetY, sprite->at(i, j));
						}
					}
				}
			}
		}
	}
	void drawChildren(Window& canvas, int px, int py) {
		for (int i = 0; i < projectilesFired; i++) {
			projectiles[i]->draw(canvas, px, py);
		}
	}
	bool childrenCollision(int pX, int pY) {
		for (int i = 0; i < projectilesFired; i++) {
			if (projectiles[i]->active) {
				if (pX - 20 >= projectiles[i]->x && pX - 76 <= projectiles[i]->x) {
					if (pY - 20 >= projectiles[i]->y && pY - 76 <= projectiles[i]->y) {

						return true;
					}
				}
			}
		}
		return false;
	}


};

enum PType {
	BANANA,
	APPLE
};

class Powerup {
public: 
	int x;
	int y;
	Image* sprite;
	PType type;
	bool consumed = false;
	Powerup(int _x, int _y, Image* _sprite, PType _type) {
		x = _x;
		y = _y;
		sprite = _sprite;
		type = _type;
	}
	void draw(Window& canvas, int px, int py) {
		if (!consumed) {
			int targetX;
			int targetY;
			for (int i = 0; i < sprite->width; i++) {
				targetX = (464 + ((px - x) + i));
				for (int j = 0; j < sprite->height; j++) {
					targetY = (336 + ((py - y) + j));

					if (targetX >= 0 && targetX < 1024) {
						if (targetY >= 0 && targetY < 768) {
							canvas.draw(targetX, targetY, sprite->at(i, j));
						}
					}
				}
			}
		}
	}
};



const int maxWidth = 48;
const int maxHeight = 36;

class World {
private:
	TileSet tiles;
	unsigned int a[maxWidth][maxHeight];
	int spawnRate;
	int spawnRateAcceleration;
	int spawnRateMinimum;
	int lastEnemySpawned;
	Image meleerSpriteA;
	Image meleerSpriteB;
	Image meleerSpriteC;
	Image sentrySprite;
	

public:
	Image appleSprite;
	Image bananaSprite;
	Image healthSprite;
	Image projectileSprite;
	Image projectileSpriteB;
	bool infinite;
	int enemiesSpawned;
	int score = 0;
	int powerupsSpawned = 0;
	Enemy* enemies[1000];
	Powerup* powerups[250];

	World(string filename, bool _infinite) {
		meleerSpriteA.load("Resources/Enemies/0.png");
		meleerSpriteB.load("Resources/Enemies/1.png");
		meleerSpriteC.load("Resources/Enemies/2.png");
		projectileSprite.load("Resources/attack.png");
		projectileSpriteB.load("Resources/attack2.png");
		healthSprite.load("Resources/heart.png");
		sentrySprite.load("Resources/Enemies/3.png");
		bananaSprite.load("Resources/banana.png");
		appleSprite.load("Resources/apple.png");
		infinite = _infinite;
		enemiesSpawned = 0;
		spawnRate = 300;
		spawnRateMinimum = 80;
		lastEnemySpawned = 0;
		spawnRateAcceleration = 10;
		tiles.load();
		ifstream infile(filename);
		string s;
		for (int i = 0; i < maxHeight; i++) {
			getline(infile, s);
			for (int j = 0; j < maxWidth*2; j=j+2) {
				a[j/2][i] = s[j]-48;
			}

		}
		infile.close();
	}
	void draw(Window& canvas, int wx, int wy) {
		int tilesize = 32;
		int yTile = wy / tilesize;
		int xTile = wx / tilesize;
		int yOffset = wy % tilesize;
		int xOffset = wx % tilesize;
		int infX;
		int infY;

		if (!infinite) {
			for (int x = -20; x < 20; x++) {
				for (int y = -20; y < 20; y++) {
					if (x + xTile >= 0 && x + xTile < 48 && y + yTile >= 0 && y + yTile < 36) {
						tiles[a[x + xTile][y + yTile]].draw(canvas, (xOffset - tilesize * x) + 512, (yOffset - tilesize * y) + 384);
					}
				}
			}
		}
		else {
			for (int x = -20; x < 20; x++) {
				for (int y = -20; y < 20; y++) {
					infX = (x + xTile) % 48;
					if (infX < 0) infX += 48;
					infY = (y + yTile) % 36;
					if (infY < 0) infY += 36;
					tiles[a[infX][infY]].draw(canvas, (xOffset - tilesize * x) + 512, (yOffset - tilesize * y) + 384);
				}
			}
		}

		


		for (unsigned int i = 0; i < enemiesSpawned; i++)
		{
			enemies[i]->draw(canvas, wx, wy);
			enemies[i]->drawChildren(canvas, wx, wy);
		}
		for (unsigned int i = 0; i < powerupsSpawned; i++)
		{
			powerups[i]->draw(canvas, wx, wy);
		}
	}
	bool checkWall(int x, int y) {
		/*cout << "x: " << x << " y: " << y;*/
		if (infinite) {
			int infX = (x / 32) % 48;
			if (infX < 0) infX += 48;
			int infY = (y / 32) % 36;
			if (infY < 0) infY += 36;
			return tiles[a[infX][infY]].collidable;
		}
		return tiles[a[(x)/32][(y)/32]].collidable;
	}

	void spawnEnemy(Window& canvas, int x, int y) {
		int evilx = 0;
		int evily = 0;
		if (!infinite) {
			evilx = rand() % 1480;
			while (abs(evilx - x) < 500) {
				evilx = rand() % 1480;
			}
			evily = rand() % 1100;
			while (abs(evily - y) < 380) {
				evily = rand() % 1100;
			}
		}

		if (infinite) {
			evilx = x + (rand() % 1480) - 740;
			while (abs(evilx - x) < 500) {
				evilx = x + (rand() % 1480) - 740;
			}
			evily = y + (rand() % 1100) - 740;
			while (abs(evily - y) < 380) {
				evily = y + (rand() % 1100) - 740;
			}
		}
		
		int enemyType = rand() % 4;
		switch(enemyType) {
		case(0):
			enemies[enemiesSpawned] = new Meleer(&meleerSpriteA, evilx, evily, 1, 9);
			break;
		case(1):
			enemies[enemiesSpawned] = new Meleer(&meleerSpriteB, evilx, evily, 2, 12);
			break;
		case(2):
			enemies[enemiesSpawned] = new Meleer(&meleerSpriteC, evilx, evily, 4, 3);
			break;
		case(3):
			enemies[enemiesSpawned] = new Sentry(&sentrySprite, evilx, evily, 9);
			break;
			
		}
		enemiesSpawned++;

	}

	void update(Window& canvas, int x, int y, int fc) {
		if (fc - lastEnemySpawned == spawnRate) {
			lastEnemySpawned = fc;
			if (spawnRate > spawnRateMinimum) {
				spawnRate = spawnRate - spawnRateAcceleration;
			}
			spawnEnemy(canvas, x, y);
		}
		for (unsigned int i = 0; i < enemiesSpawned; i++)
		{
			enemies[i]->update(&projectileSpriteB,x,y);
		}
		score++;
	}

};


int countTrues(bool going[4]) {
	int trues = 0;
	for (int i = 0; i < 4; i++) {
		if (going[i]) trues++;
	}
	return trues;
}

enum Direction {
	UP,
	DOWN,
	LEFT,
	RIGHT
};


class Projectile {
public:
	int x;
	int y;
	Image* sprite;
	int targetX;
	int targetY;
	int speed = 5;
	int damage = 3;
	bool friendly;
	bool xPositive = false;
	bool yPositive = false;
	bool active = true;

	Projectile() {};
	Projectile(int _x, int _y, int _targetX, int _targetY, bool _friendly, Image* _sprite) {
		x = _x;
		y = _y;
		targetX = _targetX;
		targetY = _targetY;
		friendly = _friendly;
		sprite = _sprite;
		if (targetX > x) xPositive = true;
		if (targetY > y) yPositive = true;
	}
	void draw(Window& canvas, int px, int py) {
		if (active) {
			int targetX;
			int targetY;
			for (int i = 0; i < sprite->width; i++) {
				targetX = (464 + ((px - x) + i));
				for (int j = 0; j < sprite->height; j++) {
					targetY = (336 + ((py - y) + j));

					if (targetX >= 0 && targetX < 1024) {
						if (targetY >= 0 && targetY < 768) {
							canvas.draw(targetX, targetY, sprite->at(i, j));
						}
					}
				}
			}
		}
	}
	void update(World& w) {
		if (active) {
			double angleDif;
			double bestAngleDif = 10.f;
			int candidateX;
			int candidateY;
			int bestX = 0;
			int bestY = 0;
			double candidateAngle;
			double correctAngle = checkAngle(x, targetX, y, targetY);
			
			for (int i = 0; i <= speed; i++) {
				candidateX = i;
				if (!xPositive) candidateX = -candidateX;
				candidateY = speed - i;
				if (!yPositive) candidateY = -candidateY;
				candidateAngle = checkAngle(x + candidateX, targetX, y + candidateY, targetY);
				if (candidateAngle > correctAngle) {
					angleDif = abs(candidateAngle - correctAngle);
				}
				else {
					angleDif = abs(correctAngle - candidateAngle);
				}

				if (angleDif < bestAngleDif) {
					bestAngleDif = angleDif;
					bestX = candidateX;
					bestY = candidateY;
				}
			}
			x = x + bestX;
			y = y + bestY;

			for (int i = 0; i < w.enemiesSpawned; i++) {
				if (w.enemies[i]->currentHealth > 0) {
					if (x-4 <= w.enemies[i]->x && x-4 >= w.enemies[i]->x - w.enemies[i]->sprite->width && y-4 <= w.enemies[i]->y && y-4 >= w.enemies[i]->y - w.enemies[i]->sprite->height) {
						active = false;
						w.enemies[i]->currentHealth -= 3;
						if (w.enemies[i]->currentHealth <= 0) {
							w.score += 100;
							int pCheck = rand() % 10;
							if (pCheck == 0) {
								w.powerups[w.powerupsSpawned] = new Powerup(w.enemies[i]->x, w.enemies[i]->y, &w.appleSprite, APPLE);
								w.powerupsSpawned++;
							}
							if (pCheck == 1) {
								w.powerups[w.powerupsSpawned] = new Powerup(w.enemies[i]->x, w.enemies[i]->y, &w.bananaSprite, BANANA);
								w.powerupsSpawned++;
							}
						}
					}
				}
			}
			if ((xPositive && x >= targetX-speed) || (!xPositive && x <= targetX+speed)) {
				if ((yPositive && y >= targetY-speed) || (!yPositive && y <= targetY+speed)) {
					active = false;
				}
			}
		}
	}
};

class Player {
public:
	Image sprite;
	int x;
	int y;
	int speed;
	bool moving[4]{ false };
	int timeSinceShot = 0;
	int shotFrequency = 30;
	int bulletsFired = 0;
	int health = 3;
	int frameCount = 0;
	int lastFrameDamaged = 0;
	int iFrames = 60;
	int lastSpec = 0;
	int specCooldown = 300;
	int specRange = 100;
	int specDamage = 9;
	int specTargets = 2;
	Projectile* bullets[1000];
	Player() {
		sprite.load("Resources/RobotDown.png");
		x = 484;
		y = 366;
		speed = 5;
	}
	void update(World& w, Window& canvas) {
		frameCount++;
		if (timeSinceShot >= shotFrequency) {
			if (w.enemiesSpawned > 0) {

				timeSinceShot = 0;
				Enemy* closestEnemy = NULL;
				double closestDistance = 800.0f;
				for (int i = 0; i < w.enemiesSpawned; i++) {
					if (w.enemies[i]->currentHealth > 0) {
						if (closestDistance > checkMagnitude(x, w.enemies[i]->x, y, w.enemies[i]->y) && w.enemies[i]->currentHealth > 0) {
							closestDistance = checkMagnitude(x, w.enemies[i]->x, y, w.enemies[i]->y);
							closestEnemy = w.enemies[i];
						}
					}

				}
				if (closestEnemy != NULL) {
					bullets[bulletsFired] = new Projectile(x - (sprite.width / 2), y - (sprite.height / 2), closestEnemy->x - (closestEnemy->sprite->width/2), closestEnemy->y-(closestEnemy->sprite->height/2), true, &w.projectileSprite);
					bulletsFired++;
				}
			}
		}
		for (int i = 0; i < bulletsFired; i++) {
			bullets[i]->update(w);
			bullets[i]->draw(canvas, x, y);
		}
		if (timeSinceShot < shotFrequency) {
			timeSinceShot++;
		}
		bool damaged = false;

		if (frameCount - lastFrameDamaged > iFrames) {
			for (int i = 0; i < w.enemiesSpawned; i++) {
				if (w.enemies[i]->currentHealth > 0) {
					if (x + sprite.width >= w.enemies[i]->x && x <= w.enemies[i]->x + w.enemies[i]->sprite->width) {
						if (y + sprite.height >= w.enemies[i]->y && y <= w.enemies[i]->y + w.enemies[i]->sprite->height) {
							damaged = true;
						}
					}

				}
				if (w.enemies[i]->childrenCollision(x, y)) damaged = true;
			}
		}

		if (damaged) {
			lastFrameDamaged = frameCount;
			health--;
		}

		for (int i = 0; i < w.powerupsSpawned; i++) {
			if (!w.powerups[i]->consumed) {
				if (x + sprite.width >= w.powerups[i]->x && x <= w.powerups[i]->x + w.powerups[i]->sprite->width) {
					if (y + sprite.height >= w.powerups[i]->y && y <= w.powerups[i]->y + w.powerups[i]->sprite->height) {
						switch (w.powerups[i]->type) {
						case APPLE:
							if (shotFrequency > 10)
							{
								shotFrequency -= 5;
							}
							break;
						case BANANA:
							specTargets++;
							break;
						}
						w.powerups[i]->consumed = true;
					}
				}
			}
		}

	}
	void move(Direction d, World& w) {
		switch (d) {
		case UP:
			moving[0] = true;
			if (countTrues(moving) == 1) {
				sprite.load("Resources/RobotUp.png");
			}
			if (!w.checkWall(x + 31, y + 64 + speed) && !w.checkWall(x + 48, y + 64 + speed) && !w.checkWall(x + 65, y + 64 + speed)) y += speed;

			break;
		case DOWN:
			moving[1] = true;
			if (countTrues(moving) == 1) {
				sprite.load("Resources/RobotDown.png");
			}
			if (!w.checkWall(x + 31, y - speed) && !w.checkWall(x + 48, y - speed) && !w.checkWall(x + 65, y - speed)) y -= speed;
			break;
		case LEFT:
			moving[2] = true;
			if (countTrues(moving) == 1) {
				sprite.load("Resources/RobotLeft.png");
			}
			if (!w.checkWall(x + 64 + speed, y + 31) && !w.checkWall(x + 64 + speed, y + 48) && !w.checkWall(x + 64 + speed, y + 65)) x += speed;

			break;
		case RIGHT:

			moving[3] = true;
			if (countTrues(moving) == 1) {
				sprite.load("Resources/RobotRight.png");
			}
			if (!w.checkWall(x - speed, y + 31) && !w.checkWall(x - speed, y + 48) && !w.checkWall(x - speed, y + 65)) x -= speed;

			break;

		}
	}
	void spec(World& w, int fc) {
		if (fc - lastSpec > specCooldown) {
			lastSpec = fc;
			Enemy* chosenEnemy = NULL;
			int highestHealth = 0;
			for (int j = 0; j < specTargets; j++) {
				for (int i = 0; i < w.enemiesSpawned; i++) {
					if (w.enemies[i]->currentHealth > highestHealth) {
						chosenEnemy = w.enemies[i];
						highestHealth = w.enemies[i]->currentHealth;
					}

				}
				if (highestHealth != 0) {
					for (int i = 0; i < w.enemiesSpawned; i++) {
						if (checkMagnitude(chosenEnemy->y, w.enemies[i]->y, chosenEnemy->y, w.enemies[i]->y) < specRange && w.enemies[i]->currentHealth > 0) {
							w.enemies[i]->currentHealth -= specDamage;
						}
					}
				}
			}
		}
	}
	void draw(World& w, Window& canvas) {
		for (unsigned int i = 0; i < sprite.height; i++) {
			for (unsigned int j = 0; j < sprite.width; j++)
			{
				if (sprite.alphaAt(j, i) > 50)
					canvas.draw(464 + j, 336 + i, sprite.at(j, i));

			}
		}
		if (health >= 1) {
			for (unsigned int i = 0; i < w.healthSprite.height; i++) {
				for (unsigned int j = 0; j < w.healthSprite.width; j++) {
					if (w.healthSprite.alphaAt(j, i) > 50)
						canvas.draw(20 + j, 20 + i, w.healthSprite.at(j, i));
				}
			}
			if (health >= 2) {
				for (unsigned int i = 0; i < w.healthSprite.height; i++) {
					for (unsigned int j = 0; j < w.healthSprite.width; j++) {
						if (w.healthSprite.alphaAt(j, i) > 50)
							canvas.draw(120 + j, 20 + i, w.healthSprite.at(j, i));
					}
				}
				if (health >= 3) {
					for (unsigned int i = 0; i < w.healthSprite.height; i++) {
						for (unsigned int j = 0; j < w.healthSprite.width; j++) {
							if (w.healthSprite.alphaAt(j, i) > 50)
								canvas.draw(220 + j, 20 + i, w.healthSprite.at(j, i));
						}
					}
				}
			}

		}
	}
};




int main() {
	srand(time(nullptr));
	int mode;
	string level;
	bool infinite = false;
	cout << "Type 1 for regular lvl 1, 2 for lvl 2, or 3 for infinite mode" << '\n';
	cin >> mode;
	switch (mode) {
	case 1:
		level = "level1.txt";
		break;
	case 2:
		level = "level2.txt";
		break;
	case 3:
		level = "levelinf.txt";
		infinite = true;
		break;
	}

	Window canvas;
	canvas.create(1024, 768, "Roberto the roboto");
	bool running = true;
	World w("Resources/" + level, infinite);
	Timer tim;
	float timepassed = 0;
	Player player;
	int framecount = 0;
	int lateUpdates = 0;
	bool waitHappened = true;
	while (running)
	{
		float dt = tim.dt();
		dt = dt * 1000;
		timepassed -= dt;
		if (timepassed > 3.3333333333f) {
			if (!waitHappened) {
				lateUpdates++;
			}
			waitHappened = false;
			framecount++;
			timepassed = 0.f;
			canvas.checkInput();
			canvas.clear();
			if (canvas.keyPressed(VK_ESCAPE)) break;
			if (canvas.keyPressed('W')) {
				player.move(UP, w);
			}
			else {
				player.moving[0] = false;
			}
			if (canvas.keyPressed('S')) {
				player.move(DOWN, w);
			}
			else {
				player.moving[1] = false;
			}
			if (canvas.keyPressed('A')) {
				player.move(LEFT, w);
			}
			else {
				player.moving[2] = false;
			}
			if (canvas.keyPressed('D')) {
				player.move(RIGHT, w);
			}
			else {
				player.moving[3] = false;
			}
			if (canvas.keyPressed('R')) {
				player.spec(w, framecount);
			}
			w.update(canvas, player.x, player.y, framecount);
			w.draw(canvas, player.x, player.y);
			player.update(w, canvas);
			player.draw(w, canvas);
			canvas.present();
			if (player.health <= 0) {
				running = false;
			}
		}
		else {
			waitHappened = true;
		}

	}
	cout << "Your score was: " << w.score << '\n';
	cout << lateUpdates << " frames were late" << '\n';
	string end;
	cout << "Press any key to exit" << '\n';
	cin >> end;
}