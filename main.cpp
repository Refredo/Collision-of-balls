#include <SFML/Graphics.hpp>

#include <vector>

#include <random>

using namespace sf;
using namespace std;

namespace random {
	mt19937 mt{ random_device{}() };

	float randomSpeed() {
		uniform_int_distribution rand{ 1, 9 };
		return float(rand(mt)) / 10;
	}

	Color randomColor() {
		vector<Color> colors{ Color::Red, Color::Green, Color::Yellow, Color::Blue, Color::White, Color::Cyan, Color::Magenta, Color::Color(0, 15, 28, 237) };
		uniform_int_distribution rand{ 0, 7 };
		return colors[rand(mt)];
	}

	Vector2f randomPosition() {
		uniform_int_distribution rand_x{ 1, 100 };
		uniform_int_distribution rand_y{ 10, 40 };

		return Vector2f{ float(rand_x(mt)), float(rand_y(mt)) };
	}
}

struct Ball {
	CircleShape shape;
	float speed;
	Vector2f position;
	Vector2f dir_speed;
	Time lifetime{ seconds(5) };
};

void init(vector<Ball>& balls) {
	float x{ 100 };
	float y{ 100 };
	for (int i = 0; i < balls.size(); i++) {
		balls[i].shape.setFillColor(random::randomColor());
		balls[i].speed = random::randomSpeed();
		balls[i].position = Vector2f{ (x * (i + 1)), ((y * i) + 1) } + random::randomPosition();
		balls[i].shape.setPosition(balls[i].position);
		balls[i].shape.setRadius(40.f);
		balls[i].dir_speed.x = balls[i].speed;
		balls[i].dir_speed.y = balls[i].speed;
	}
}

void initBall(Ball& ball, Vector2f& mousePosition) {
	ball.shape.setFillColor(random::randomColor());
	ball.speed = random::randomSpeed();
	ball.position = mousePosition;
	ball.shape.setPosition(ball.position);
	ball.shape.setRadius(40.f);
	ball.dir_speed.x = ball.speed;
	ball.dir_speed.y = ball.speed;
}

void collision(Ball& first, Ball& second) {
	float Dx = first.position.x - second.position.x;
	float Dy = first.position.y - second.position.y;
	float d = sqrt(Dx * Dx + Dy * Dy); if (d == 0) d = 0.01;
	float s = Dx / d; // sin
	float e = Dy / d; // cos
	if (d < first.shape.getRadius() + second.shape.getRadius()) {
		float Vn1 = second.dir_speed.x * s + second.dir_speed.y * e;
		float Vn2 = first.dir_speed.x * s + first.dir_speed.y * e;
		float dt = (second.shape.getRadius() + first.shape.getRadius() - d) / (Vn1 - Vn2);
		if (dt > 0.6) dt = 0.6;
		if (dt < -0.6) dt = -0.6;
		first.position.x -= first.dir_speed.x * dt;
		first.position.y -= first.dir_speed.y * dt;
		second.position.x -= second.dir_speed.x * dt;
		second.position.y -= second.dir_speed.y * dt;
		Dx = first.position.x - second.position.x;
		Dy = first.position.y - second.position.y;
		d = sqrt(Dx * Dx + Dy * Dy); if (d == 0) d = 0.01;
		s = Dx / d; // sin
		e = Dy / d; // cos
		Vn1 = second.dir_speed.x * s + second.dir_speed.y * e;
		Vn2 = first.dir_speed.x * s + first.dir_speed.y * e;
		float Vt1 = -second.dir_speed.x * e + second.dir_speed.y * s;
		float Vt2 = -first.dir_speed.x * e + first.dir_speed.y * s;


		float o = Vn2;
		Vn2 = Vn1;
		Vn1 = o;


		first.dir_speed.x = Vn2 * s - Vt2 * e;
		first.dir_speed.y = Vn2 * e + Vt2 * s;
		second.dir_speed.x = Vn1 * s - Vt1 * e;
		second.dir_speed.y = Vn1 * e + Vt1 * s;
		first.position.x += first.dir_speed.x * dt;
		first.position.y += first.dir_speed.y * dt;
		second.position.x += second.dir_speed.x * dt;
		second.position.y += second.dir_speed.y * dt;
	}
}

void removeBalls(vector<Ball>& balls, Time time) {
	for (int i = 0; i < balls.size(); i++) {
		balls[i].lifetime -= time;

		if (balls[i].lifetime < seconds(0)) {
			balls.erase(balls.begin() + i);
		}
	}
}

void update(vector<Ball>& balls, float time, Time& Time) {

	removeBalls(balls, Time);

	for (int i = 0; i < balls.size(); i++) {

		if (balls[i].position.x >= 720) {
			balls[i].dir_speed.x = -balls[i].dir_speed.x;
		}
		else if (balls[i].position.x <= 0) {
			balls[i].dir_speed.x = -balls[i].dir_speed.x;
		}

		if (balls[i].position.y < 0) {
			balls[i].dir_speed.y = -balls[i].dir_speed.y;
		}
		else if (balls[i].position.y > 520) {
			balls[i].dir_speed.y = -balls[i].dir_speed.y;
		}

		balls[i].position.x += balls[i].dir_speed.x * time;
		balls[i].position.y += balls[i].dir_speed.y * time;

		for (int fi = 0; fi < balls.size(); ++fi)
		{
			for (int si = fi + 1; si < balls.size(); ++si)
			{
				float Dx = balls[si].position.x - balls[fi].position.x;
				float Dy = balls[si].position.y - balls[fi].position.y;
				float d = sqrt(Dx * Dx + Dy * Dy);
				if (d == 0) d = 0.01;
				float s = Dx / d;
				float e = Dy / d;
				if (d < balls[si].shape.getRadius() + balls[fi].shape.getRadius()) {
					collision(balls[si], balls[fi]);
				}
			}
		}

		balls[i].shape.setPosition(balls[i].position);


	}
}

void MouseMove(Event::MouseMoveEvent& event, Vector2f& mousePosition) {
	mousePosition.x = event.x;
	mousePosition.y = event.y;
}

void pollEvent(RenderWindow& window, vector<Ball>& balls, Vector2f& mousePosition) {

	Event event;

	while (window.pollEvent(event)) {

		if (Keyboard::isKeyPressed(Keyboard::Escape) || event.type == Event::Closed) {
			window.close();
		}

		if (event.type == Event::MouseMoved) {
			MouseMove(event.mouseMove, mousePosition);
		}

		if (event.type == Event::MouseButtonPressed) {
			Ball ball;
			initBall(ball, mousePosition);
			balls.push_back(ball);
		}
	}
}

void redrawFrame(RenderWindow& window, vector<Ball>& balls) {
	window.clear();
	for (int i = 0; i < balls.size(); i++) {
		window.draw(balls[i].shape);
	}
	window.display();
}

int main() {

	RenderWindow window(VideoMode(800, 600), "Shari");

	Clock clock;

	vector<Ball> balls(5);
	init(balls);

	Vector2f mousePosition;

	while (window.isOpen()) {

		float time = clock.getElapsedTime().asMilliseconds();
		Time Time = milliseconds(clock.getElapsedTime().asMilliseconds());
		clock.restart();

		pollEvent(window, balls, mousePosition);
		update(balls, time, Time);
		redrawFrame(window, balls);
	}

	return 0;
}