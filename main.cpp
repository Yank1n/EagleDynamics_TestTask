#include "SFML/Graphics.hpp"
#include "MiddleAverageFilter.h"

constexpr int WINDOW_X = 1024;
constexpr int WINDOW_Y = 768;
constexpr int MAX_BALLS = 300;
constexpr int MIN_BALLS = 100;

constexpr float PI = 3.1415926535f;

Math::MiddleAverageFilter<float,100> fpscounter;

float fast_distance(const struct Ball& first, const struct Ball& second);
sf::Vector2f get_nomralized_vector(sf::Vector2f& firstVec, sf::Vector2f& secondVec);
float DotProduct(sf::Vector2f& first, sf::Vector2f& second);

struct Ball
{
    sf::Vector2f p;
    sf::Vector2f dir;
    float r = 0;
    float speed = 0;

    bool checkCollision(Ball& other)
    {
        float distance = fast_distance(*this, other);

        distance -= (this->r + other.r);
        if (distance <= 0)
        {
            collision(other);
            return true;
        }
        return false;
    }
private:
    void collision(Ball& other)
    {
        sf::Vector2f v1 = get_nomralized_vector(p, other.p);
        sf::Vector2f v2 = -v1;

        sf::Vector2f thisSpeedVel = dir * speed;
        sf::Vector2f otherSpeedVel = other.dir * speed;

        sf::Vector2f rv = otherSpeedVel - thisSpeedVel;

        float velAlongNormal = DotProduct(rv, dir);

        dir += v2;
        other.dir += v1;

        if (velAlongNormal > 0)
            return;

        float j = -2 * velAlongNormal;
        float thisMass = r * r * PI;
        float otherMass = other.r * other.r * PI;
        j /= 1 / thisMass + 1 / otherMass;

        speed -= 1 / otherMass * other.speed;
        other.speed += 1 / thisMass * speed;
    }
};

sf::Vector2f get_nomralized_vector(sf::Vector2f& firstVec, sf::Vector2f& secondVec)
{
    float valueX = secondVec.x - firstVec.x;
    float valueY = secondVec.y - secondVec.y;
    float length = sqrtf(valueX * valueX + valueY * valueY);

    return sf::Vector2f(valueX / length, valueY / length);
}

float DotProduct(sf::Vector2f& first, sf::Vector2f& second)
{
    return (first.x * second.x + first.y * second.y);
}

float fast_distance(const Ball& first, const Ball& second)
{
    float x1 = first.p.x;
    float y1 = first.p.y;

    float x2 = second.p.x;
    float y2 = second.p.y;

    float x = abs(x1 - x2);
    float y = abs(y1 - y2);

    int min = (x > y) ? (int)y : (int)x;
    return (x + y - (min >> 1) - (min >> 2) + (min >> 4));
}

void draw_ball(sf::RenderWindow& window, const Ball& ball)
{
    sf::CircleShape gball;
    gball.setRadius(ball.r);
    gball.setPosition(ball.p.x, ball.p.y);
    window.draw(gball);
}

void move_ball(Ball& ball, float deltaTime)
{
    float dx = ball.dir.x * ball.speed * deltaTime;
    float dy = ball.dir.y * ball.speed * deltaTime;
    ball.p.x += dx;
    ball.p.y += dy;

    if (ball.p.x + ball.r * 2 >= WINDOW_X || ball.p.x <= 0)
    {
        ball.dir.x *= -1;
    }
    if (ball.p.y + ball.r * 2 >= WINDOW_Y || ball.p.y <= 0)
    {
        ball.dir.y *= -1;
    }
}

void draw_fps(sf::RenderWindow& window, float fps)
{
    char c[32];
    snprintf(c, 32, "FPS: %f", fps);
    std::string string(c);
    sf::String str(c);
    window.setTitle(str);
}

int main()
{
    sf::RenderWindow window(sf::VideoMode(WINDOW_X, WINDOW_Y), "ball collision demo");
    srand(time(NULL));

    std::vector<Ball> balls;
    int size = rand() % (MAX_BALLS - MIN_BALLS) + MIN_BALLS;
    balls.reserve(size);

    // randomly initialize balls
    Ball newBall{};
    for (int i = 0; i < size; ++i)
    {
        newBall.p.x = rand() % WINDOW_X + 3;
        newBall.p.y = rand() % WINDOW_Y + 3;
        newBall.dir.x = (-5 + (rand() % 10)) / 3.;
        newBall.dir.y = (-5 + (rand() % 10)) / 3.;
        newBall.r = 5 + rand() % 5;
        newBall.speed = 30 + rand() % 30;
        balls.push_back(newBall);
    }

   // window.setFramerateLimit(60);

    sf::Clock clock;
    float lastime = clock.restart().asSeconds();

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
        }

        float current_time = clock.getElapsedTime().asSeconds();
        float deltaTime = current_time - lastime;
        fpscounter.push(1.0f / (current_time - lastime));
        lastime = current_time;

        /// <summary>
        ///  TODO PLACE COLLITION CODE HERE 
        /// объекты создаются в случайном месте на плоскости с случайным вектором скорости, имеют радиус R
        /// Объекты движутся кинетически. Пространство ограниченно границами окна
        /// Напишите обработчик столкновения шаров между собо и краями окна. Как это сделать эфективно?
        /// Масса пропорцианальна площади круга, 
        /// Как можно было-бы улучшить текущую архитектуру кода?
        /// Данный код является макетом, вы можете его модифицировать по своему усмотрению

        for (auto& ball : balls)
        {
            move_ball(ball, deltaTime);
            for (auto& otherBall : balls)
            {
                if (&otherBall != &ball)
                {
                    ball.checkCollision(otherBall);
                }
            }
        }

        window.clear();
        for (const auto &ball : balls)
        {
            draw_ball(window, ball);
        }

		draw_fps(window, fpscounter.getAverage());
		window.display();
    }
    return 0;
}