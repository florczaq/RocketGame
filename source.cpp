#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <ctime>
#include <fstream>

#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

#define movement_speed 7
#define bullet_speed -6
#define enemy_speed 5
const int w_w = sf::VideoMode::getDesktopMode().width / 2;
const int w_h = sf::VideoMode::getDesktopMode().height - sf::VideoMode::getDesktopMode().height / 20;

class Player : public sf::Drawable
{
public:
	sf::Image img;
	sf::Texture texture;
	sf::Sprite player;

	int width = 50, height = 50;

	Player()
	{
		img.loadFromFile("rocket.png");
		texture.loadFromImage(img);
		player.setTexture(texture);

		player.scale(1.9, 1.9);
		player.setPosition(w_w / 2, w_h - w_h / 5);
		player.setOrigin(player.getLocalBounds().width / 2, player.getLocalBounds().height / 2);
	}

	void move(float velocity)
	{
		player.move(velocity, 0.f);
		if (player.getPosition().x > w_w - width / 2)
			player.setPosition(w_w - width / 2, player.getPosition().y);
		if (player.getPosition().x < 0 + width / 2)
			player.setPosition(width / 2, player.getPosition().y);
	}

	virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const
	{
		target.draw(player);
	}
};

class Shot : public sf::Drawable
{
public:
	sf::RectangleShape rectangle;
	int width = 2, height = 10;
	bool if_move = false;

	Shot()
	{
		rectangle.setSize(sf::Vector2f(width, height));
		rectangle.setOrigin(width / 2, height / 2);
		rectangle.setPosition(-15, -15);
		rectangle.setOutlineThickness(1);
		rectangle.setOutlineColor(sf::Color::Red);
	}

	void set_color()
	{
		rectangle.setOutlineColor(sf::Color(rand() % 255 + 100, rand() % 255 + 100, rand() % 255 + 100));
		rectangle.setOutlineThickness(2);
	}

	void set_position()
	{
		if (!if_move)
			rectangle.setPosition(-15, 15);
	}

	bool if_colision(sf::FloatRect bounds, bool &hitted)
	{
		if (rectangle.getPosition().y < 0 - height)
		{
			if_move = false;
			return true;
		}
		else if (rectangle.getGlobalBounds().intersects(bounds))
		{
			if_move = false;
			hitted = true;
			return true;
		}
		else
		{
			return false;
		}
	}

	void move_bull()
	{
		if_move = true;
	}

	void move()
	{
		if (if_move)
			rectangle.move(0.f, bullet_speed);
	}

	void shot(sf::Vector2f position)
	{
		rectangle.setPosition(position);
	}

	virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const
	{
		target.draw(rectangle);
	}
};

class Shots : public sf::Drawable
{
public:
	Shot shot[20];

	void set_pos()
	{
		for (int i = 0; i < 20; i++)
		{
			shot[i].set_position();
		}
	}

	virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const
	{
		for (int i = 0; i < 20; i++)
			target.draw(shot[i]);
	}
};

class Enemy : public sf::Drawable
{
public:
	sf::CircleShape circle;

	int r = 10, hp = 1;
	bool in_move = false;

	Enemy()
	{
		circle.setRadius(r);
		circle.setOrigin(r, r);
		circle.setPosition(w_w / 2, -2 * r);
		circle.setFillColor(sf::Color(183, 183, 183));
		circle.setOutlineColor(sf::Color(100, 100, 100));
		circle.setOutlineThickness(6);
	}

	void pick_hp()
	{
		hp = rand() % 5 + 2;
	}

	void set_size()
	{
		r = 10;
		circle.setRadius(r *= hp);
	}

	void set_position()
	{
		int pos = (rand() % (w_w - 150)) + 50;
		circle.setPosition(pos, -2 * r);
	}

	void get_shoted(bool &hitted)
	{
		hitted = false;
		hp--;
	}

	bool if_collision()
	{
		if (circle.getPosition().y > w_h + (2 * r))
		{
			in_move = false;
			return true;
		}
		else
			return false;
	}

	void move(float velocityy)
	{
		in_move = true;
		circle.move(0.f, velocityy);
	}

	bool if_null(int &score)
	{
		if (hp == 0)
		{
			score += r / 2;
			return true;
		}
		return false;
	}

	void hit()
	{
		pick_hp();
		set_size();
		set_position();
	}

	virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const
	{
		target.draw(circle);
	}
};

class Background : public sf::Drawable
{
public:
	sf::Image img;
	sf::Texture texture;
	sf::Sprite sprite;

	Background()
	{
		img.loadFromFile("stars.png");
		texture.loadFromImage(img);
		sprite.setTexture(texture);
		sprite.setOrigin(sprite.getLocalBounds().width / 2, sprite.getLocalBounds().height / 2);
		sprite.setPosition(w_w / 2, w_h / 4);
	}

	void move(float add)
	{
		sprite.move(0, (3 + add));
	}

	virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const
	{
		target.draw(sprite);
	}
};

class Backgrounds : public sf::Drawable
{
public:
	Background stars[2];

	sf::Vector2f position_0 = sf::Vector2f(w_w / 2, w_h / 2), position_1 = sf::Vector2f(w_w / 2, w_h / 2 - stars[0].sprite.getLocalBounds().height);

	Backgrounds()
	{
		stars[0].sprite.setPosition(position_0);
		stars[1].sprite.setPosition(position_1);
	}

	void if_ending()
	{
		if (stars[0].sprite.getPosition().y > w_h + stars[0].sprite.getLocalBounds().height / 2)
			stars[0].sprite.setPosition(position_1);

		if (stars[1].sprite.getPosition().y > w_h + stars[1].sprite.getLocalBounds().height / 2)
			stars[1].sprite.setPosition(position_1);
	}

	void movement(float add)
	{
		for (int i = 0; i < 2; i++)
			stars[i].move(add);

		if_ending();
	}

	virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const
	{
		for (int i = 0; i < 2; i++)
			target.draw(stars[i]);
	}
};

class Score : public sf::Drawable
{
public:
	sf::Text text;
	sf::Font pixel;

	Score()
	{
		if (!pixel.loadFromFile("pixel7.ttf"))
			std::cout << "Error: Font" << std::endl;
		text.setFont(pixel);
		text.setCharacterSize(100);
		text.setString("Score: 0");
		text.setOrigin(text.getLocalBounds().width / 2, text.getLocalBounds().height);
		text.setPosition(w_w / 3, w_h / 30);
		text.setOutlineColor(sf::Color::Black);
		text.setOutlineThickness(1);
	}

	void set_string(int &score)
	{
		text.setString("Score: " + std::to_string(score));
		text.setOrigin(text.getLocalBounds().width / 2, text.getLocalBounds().height);
	}

	void high_score(int score, bool &if_change)
	{
		std::fstream file;

		file.open("hscore.txt", std::ios::in);
		if (!file.good())
		{
			if_change = true;
		}

		if (!if_change)
		{
			std::string line;
			getline(file, line);
			int hscore = atoi(line.c_str());
			score > hscore ? if_change = true : if_change = false;
		}
		file.close();

		if (if_change)
		{
			file.open("hscore.txt", std::ios::out);
			file << std::to_string(score) << std::endl;
			file.close();
		}
	}

	virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const
	{
		target.draw(text);
	}
};

class Heart : public sf::Drawable
{
public:
	sf::Image image;
	sf::Texture texture;
	sf::Sprite heart;

	Heart()
	{
		image.loadFromFile("heart.png");
		texture.loadFromImage(image);
		heart.setTexture(texture);
		heart.setOrigin(heart.getLocalBounds().width / 2, heart.getLocalBounds().height / 2);
		heart.setPosition(w_w / 2, w_h / 2);
		heart.scale(1.5, 1.5);
	}

	virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const
	{
		target.draw(heart);
	}
};

class Hearts : public sf::Drawable
{
public:
	Heart hearts[3];

	int pos_x = w_w - w_w / 7, padding = 20, h_many = 3;

	Hearts()
	{
		for (int i = 0; i < 3; i++)
		{
			hearts[i].heart.setPosition(pos_x, w_w / 20);
			pos_x -= (hearts[i].heart.getLocalBounds().width + padding);
		}
	}

	void hitted()
	{
		h_many--;
	}

	virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const
	{
		for (int i = 0; i < h_many; i++)
			target.draw(hearts[i]);
	}
};

class Border : public sf::Drawable
{
public:
	sf::RectangleShape rectangle;

	int width = w_w, height = w_h / 10;

	Border()
	{
		rectangle.setSize(sf::Vector2f(width, height));
		rectangle.setOrigin(width / 2, height / 2);
		rectangle.setPosition(w_w / 2, height / 2);
		rectangle.setFillColor(sf::Color(22, 20, 35, 255));
		rectangle.setOutlineColor(sf::Color(22, 10, 45, 255));
		rectangle.setOutlineThickness(20);
	}

	virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const
	{
		target.draw(rectangle);
	}
};

class Close_button : public sf::Drawable
{
public:
	sf::Image img;
	sf::Texture texture;
	sf::Sprite closing;

	int width = 30, height = 30;

	Close_button()
	{
		img.loadFromFile("close.png");
		texture.loadFromImage(img);
		closing.setTexture(texture);
		closing.setOrigin(closing.getLocalBounds().width, 0.f);
		closing.setPosition(w_w, 0.f);
	}

	virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const
	{
		target.draw(closing);
	}
};

class Circle_mouse : public sf::Drawable
{
public:
	sf::CircleShape circle;

	Circle_mouse()
	{
		circle.setRadius(0.1);
	}

	void set_pos(sf::Vector2f pos)
	{
		circle.setPosition(pos);
	}

	virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const
	{
		target.draw(circle);
	}
};

class Body_player : public sf::Drawable
{
public:
	sf::RectangleShape rectangle;

	int width = 45, height = 50;

	Body_player()
	{
		rectangle.setSize(sf::Vector2f(width, height));
		rectangle.setOrigin(width / 2, height / 2);
	}

	bool if_hit(sf::FloatRect rects)
	{
		return rectangle.getGlobalBounds().intersects(rects) ? true : false;
	}

	void set_position(sf::Vector2f position)
	{
		rectangle.setPosition(position);
	}

	virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const
	{
		target.draw(rectangle);
	}
};

class End_screen : public sf::Drawable
{
public:
	sf::Text text;
	sf::Text text2;
	sf::Font font;
	std::string endline = "Game Over\n\nScore : ";

	bool high = false;

	End_screen()
	{
		if (!font.loadFromFile("pixel2.ttf"))
			exit(1);
		text.setFont(font);
		text.setCharacterSize(80);
		text.setString("Game Over\n\nScore: 0");
		text.setOrigin(text.getLocalBounds().width / 2, text.getLocalBounds().height / 2);
		text.setFillColor(sf::Color::Cyan);
		text.setPosition(w_w / 2, w_h / 3);

		text2.setFont(font);
		text2.setCharacterSize(80);
		text2.setString("High Score!");
		text2.setOrigin(text2.getLocalBounds().width / 2, text2.getLocalBounds().height / 2);
		text2.setFillColor(sf::Color::Red);
		text2.setPosition(w_w / 2, w_h - w_h / 3);
	}

	void set_string(int score, bool if_higher)
	{
		text.setString("Game Over\n\nScore: " + std::to_string(score));
		text.setOrigin(text.getLocalBounds().width / 2, text.getLocalBounds().height / 2);

		if_higher ? high = true : high = false;
	}

	virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const
	{
		target.draw(text);
		if (high)
			target.draw(text2);
	}
};

class End_button : public sf::Drawable
{
public:
	sf::RectangleShape rectangle;

	sf::Text text;
	sf::Font font;

	int width = w_w / 3, height = w_h / 10;

	End_button()
	{
		rectangle.setSize(sf::Vector2f(width, height));
		rectangle.setPosition(w_w / 2, w_h - w_h / 7);
		rectangle.setOrigin(width / 2, height / 2);
		rectangle.setFillColor(sf::Color(143, 143, 143, 255));
		rectangle.setOutlineColor(sf::Color(50, 50, 50));
		rectangle.setOutlineThickness(6);

		if (!font.loadFromFile("pixel8.ttf"))
			exit(1);

		text.setFont(font);
		text.setCharacterSize(100);
		text.setString("Again?");
		text.setOrigin(text.getLocalBounds().width / 2, text.getLocalBounds().height / 2);
		text.setPosition(w_w / 2, w_h - w_h / 5.4);
		text.setFillColor(sf::Color(220, 220, 220, 255));
		text.setOutlineColor(sf::Color::Black);
		text.setOutlineThickness(2);
	}

	virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const
	{
		target.draw(rectangle);
		target.draw(text);
	}
};

int main()
{
again:
	srand(time(NULL));
	sf::RenderWindow window(sf::VideoMode(w_w, w_h), "StartShot", sf::Style::None);

	window.setPosition(sf::Vector2i(window.getPosition().x, w_h / 100));
	window.setFramerateLimit(60);

	Player player;
	Shots shots;
	Enemy enemy;
	Backgrounds background;
	Score text_score;
	Border border;
	Hearts hearts;
	Close_button button_close;
	Circle_mouse mousec;
	Body_player body;
	End_screen endlines;
	End_button button;

	bool up = false, down = false, left = false, right = false, hitted = false, if_end = false, highs = false;
	float velocity = 0, velocity2 = 0, add_speed = 0;
	int id = 0, dp = 0, score = 0;

	sf::FloatRect enemy_rect;

	enemy.pick_hp();
	enemy.set_size();
	enemy.set_position();

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed || event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
				window.close();

			if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
				if (mousec.circle.getGlobalBounds().intersects(button_close.closing.getGlobalBounds()))
					window.close();
				else if (mousec.circle.getGlobalBounds().intersects(button.rectangle.getGlobalBounds()) && if_end)
					goto again;

			if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && !if_end)
				if (mousec.circle.getGlobalBounds().intersects(border.rectangle.getGlobalBounds()))
					window.setPosition(sf::Vector2i(sf::Mouse::getPosition().x - w_w / 2, sf::Mouse::getPosition().y - w_h / 20));

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
				right = true;
			else
				right = false;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
				left = true;
			else
				left = false;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
				up = true;
			else
				up = false;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
				down = true;
			else
				down = false;

			if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Space)
			{
				id == 0 ? dp = 19 : dp = id - 1;

				if (shots.shot[dp].rectangle.getPosition().y < w_h - w_h / 3.8)
				{
					shots.shot[id].set_color();
					shots.shot[id].move_bull();
					shots.shot[id].shot(player.player.getPosition() - sf::Vector2f(0, player.height / 1.5));

					if (id < 21)
						id++;
				}
			}
		}

		{
			if (id > 19)
				id = 0;
		}

		mousec.set_pos(sf::Vector2f(sf::Mouse::getPosition(window)));

		shots.set_pos();

		for (int i = 0; i < 20; i++)
		{
			shots.shot[i].move();
			shots.shot[i].if_colision(enemy.circle.getGlobalBounds(), hitted);
		}

		enemy.move(enemy_speed + add_speed);

		if (hitted)
			enemy.get_shoted(hitted);

		if (enemy.if_null(score) || enemy.if_collision())
			enemy.hit();

		text_score.set_string(score);

		velocity = 0;

		if (right)
			velocity = movement_speed;
		if (left)
			velocity = -movement_speed;

		player.move(velocity);

		body.set_position(player.player.getPosition());

		enemy_rect = enemy.circle.getGlobalBounds();

		if (body.if_hit(enemy_rect))
		{
			hearts.hitted();
			enemy.hit();
		}

		background.movement(add_speed);

		if (hearts.h_many == 0)
		{
			if_end = true;
			text_score.high_score(score, highs);
			endlines.set_string(score, highs);
		}

		add_speed += 0.0001;
		std::cout << add_speed << std::endl;

		window.clear(sf::Color(12, 10, 25));
		window.draw(background);
		if (!if_end)
		{
			window.draw(player);
			window.draw(shots);
			window.draw(enemy);
			window.draw(border);
			window.draw(text_score);
			window.draw(button_close);
			window.draw(hearts);
		}
		else
		{
			window.draw(endlines);
			window.draw(button);
		}
		window.draw(button_close);
		window.draw(mousec);
		window.display();
	}

	return 0;
}
