#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFPhysics.h>
#include <string>
#include <sstream>
using namespace std;
using namespace sf;
using namespace sfp;


void removeTree(vector<Sprite>* trees) {
	//Use iterator for erase()
	trees->erase(trees->begin());
}

void addTree(vector<Sprite> *trees, Texture* treeTex) {
	//Make the sprite and add to vector
	if (trees->size() > 12) {
		removeTree(trees);
	}

	Sprite tree(*treeTex);
	tree.move(200, 0);
	trees->push_back(tree);

}

void moveTrees(vector<Sprite>* trees) {
	for (int i = 0; i < trees->size(); i++) {
		trees->at(i).move(-.1, 0);
	}
}

Vector2f GetTextSize(Text text) {
	FloatRect r = text.getGlobalBounds();
	return Vector2f(r.width, r.height);
}


int main()
{
	int score, min, secs;
	Text scoretext;


	Font fnt;
	if (!fnt.loadFromFile("arial.ttf")) {
		cout << "Could not load font." << endl;
		exit(3);
	}

	// Create our window and world with gravity 0,1
	vector<Sprite> trees;
	//Initialize textures
	Texture treeTex;
	treeTex.loadFromFile("images/Tree_1.png");
	addTree(&trees, &treeTex);

	Texture logTex;
	logTex.loadFromFile("images/log.png");

	RenderWindow window(VideoMode(800, 600), "Bounce");
	Texture back1Tex;
	back1Tex.loadFromFile("images/Background_01.png");
	Sprite back1s(back1Tex);

	Texture back2Tex;
	back2Tex.loadFromFile("images/Ground_01.png");
	Sprite back2s(back2Tex);

	Texture gameOverTex;
	gameOverTex.loadFromFile("images/Gameover.png");
	Sprite gameover(gameOverTex);


	PhysicsShapeList<PhysicsSprite> logs;
	
	//Make trees:
	for (int i = 0; i <= 12000; i++) {
		if (!(i % 1200)) {
			addTree(&trees, &treeTex);
		}
		moveTrees(&trees);
	}
	


	World world(Vector2f(0, 1));
	// Create the ball
	Texture bearTex;
	bearTex.loadFromFile("images/Bear.png");
	PhysicsSprite bear;
	bear.setTexture(bearTex);
	Vector2f bearSize = bear.getSize();
	bear.setCenter(Vector2f(400, 479 - bearSize.y / 2));
	world.AddPhysicsBody(bear);
	// Create the floor
	PhysicsRectangle floor;
	floor.setSize(Vector2f(800, 20));
	floor.setCenter(Vector2f(400, 490));
	floor.setStatic(true);
	world.AddPhysicsBody(floor);

	// Create the back
	PhysicsRectangle back;
	back.setSize(Vector2f(20, 600));
	back.setCenter(Vector2f(-100, 300));
	back.setStatic(true);
	world.AddPhysicsBody(back);

	floor.onCollision = [&bear](PhysicsBodyCollisionResult result) {
			if (result.object2 == bear) {
				if (abs(bear.getVelocity().y) > 0.5) {
					bear.rotate(10.0);
				}
				bear.setVelocity(Vector2f(0, 0));
			}
		};
	Clock clock;
	Time lastTime(clock.getElapsedTime());
	Clock treeClock;
	Clock logClock;
	bool endGame = false;
	while (!endGame) {
		// calculate MS since last frame

		//Check to see if a tree should spawn:
		if (treeClock.getElapsedTime().asSeconds() > 2) {
			addTree(&trees, &treeTex);
			treeClock.restart();
		}

		//Check to see if a log should spawn:
		if (logClock.getElapsedTime().asSeconds() > 3) {
			logClock.restart();
			PhysicsSprite& log = logs.Create();
			log.setTexture(logTex);
			int x = 800;
			Vector2f sizeB = log.getSize();
			log.setCenter(Vector2f(x, 300));
			log.setVelocity(Vector2f(-0.3, 0));
			world.AddPhysicsBody(log);
			log.onCollision = [&floor, &log, &bear, &endGame, &back, &logs, &world]
				(PhysicsBodyCollisionResult result) {
				//End Game Here
					if (result.object2 == floor) {
						Vector2f velo = log.getVelocity();
						log.setVelocity(Vector2f(velo.x, 0));
					}
					else if (result.object2 == back) {
						world.RemovePhysicsBody(log);
						logs.QueueRemove(log);
					}
					else if (result.object2 == bear) {
						endGame = true;
					}
				};
		}

		Time currentTime(clock.getElapsedTime());
		Time deltaTime(currentTime - lastTime);
		int deltaTimeMS(deltaTime.asMilliseconds());
		if (deltaTimeMS > 0) {
			moveTrees(&trees);
			world.UpdatePhysics(deltaTimeMS);
			lastTime = currentTime;


			if (Keyboard::isKeyPressed(Keyboard::Space)&& bear.getVelocity() == Vector2f(0, 0)) {

				bear.setVelocity(Vector2f(0, -.6));
				bear.rotate(-10.0);

			}
			

		}
		logs.DoRemovals();

		window.clear(Color(0, 0, 0));
		window.draw(back1s);

		//Draw the trees behind the background:
		for (int i = 0; i < trees.size(); i++) {
			window.draw(trees[i]);
		}

		window.draw(back2s);
		for (PhysicsShape& log : logs) {
			window.draw((PhysicsSprite&)log);
		}
		window.draw(bear);

		//Display time:
		score = (int)clock.getElapsedTime().asSeconds();
		min = (int)score / 60;
		secs = score % 60;

		ostringstream oss1;
		if (secs < 10) {
			oss1 << min << ":0" << secs;
		}
		else {
			oss1 << min << ":" << secs;
		}
		scoretext.setString(oss1.str());
		scoretext.setFont(fnt);

		Vector2f sz1 = GetTextSize(scoretext);
		scoretext.setPosition(750 - (sz1.x / 2), (sz1.y / 2));
		window.draw(scoretext);

		window.display();
	}

	

	bool exit = false;
	//Game Over Screen:
	while (!exit) {

		if (Keyboard::isKeyPressed(Keyboard::Space) && bear.getVelocity() == Vector2f(0, 0)) {
			exit = true;
		}

		window.draw(gameover);
		Text exitText;
		ostringstream oss2;

		oss2 << "Press Space to Exit";

		scoretext.setFont(fnt);

		exitText.setString(oss2.str());
		exitText.setFont(fnt);

		Vector2f sz1 = GetTextSize(scoretext);
		Vector2f sz2 = GetTextSize(exitText);
		scoretext.setPosition(400 - (sz1.x / 2), 250 - (sz1.y / 2));
		exitText.setPosition(400 - (sz2.x / 2), 290 - (sz2.y / 2));
		window.draw(scoretext);
		window.draw(exitText);
		window.display();
	}
}