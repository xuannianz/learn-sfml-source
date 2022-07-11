#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <sstream>

#include "SFML/Graphics/CircleShape.hpp"
#include "SFML/Graphics/Color.hpp"
#include "SFML/Graphics/Rect.hpp"
#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/Graphics/RenderStates.hpp"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/Sprite.hpp"
#include "SFML/Graphics/Texture.hpp"
#include "SFML/Graphics/Font.hpp"
#include "SFML/Graphics/Text.hpp"
#include "SFML/System/Vector2.hpp"
#include "SFML/Window.hpp"
#include "SFML/Window/Event.hpp"
#include "SFML/Window/Keyboard.hpp"
#include "SFML/Window/VideoMode.hpp"
#include "SFML/Window/WindowStyle.hpp"

using namespace std;
using namespace sf;
namespace g1{
    void test(){
        // Style::Fullscreen 有点危险啊, 退不出来
        const float window_height = 1080;
        const float window_width = 1920;
        sf::RenderWindow window(sf::VideoMode(window_width, window_height), "My window");
        Texture texture_bg;
        // 加载图像到 gpu, 比较慢, 不应该放在循环体中
        texture_bg.loadFromFile("01/graphics/background.png");
        // sprite 要和 texture 关联才能显示
        Sprite sprite_bg;
        sprite_bg.setTexture(texture_bg);
        sprite_bg.setPosition(0, 0);
        Texture texture_tree;
        texture_tree.loadFromFile("01/graphics/tree.png");
        Sprite sprite_tree;
        sprite_tree.setTexture(texture_tree);
        sprite_tree.setPosition(810, 0);
        Texture texture_bee;
        texture_bee.loadFromFile("01/graphics/bee.png");
        Sprite sprite_bee;
        bool bee_isalive= false;
        float bee_height = 0;
        float bee_speed = 0;
        sprite_bee.setTexture(texture_bee);
        // 刚开始隐藏起来
        sprite_bee.setPosition(window_width, 0);
        Texture texture_cloud;
        texture_cloud.loadFromFile("01/graphics/cloud.png");
        Sprite sprite_cloud;
        float cloud_height = 0;
        bool cloud_isalive = false;
        float cloud_speed = 0;
        sprite_cloud.setTexture(texture_cloud);
        sprite_cloud.setPosition(window_width, 0);
        Font font_msg, font_score;
        font_msg.loadFromFile("01/fonts/KOMIKAP_.ttf");
        font_score.loadFromFile("01/fonts/KOMIKAP_.ttf");
        int score = 0;
        Text text_msg("Press Enter to start!", font_msg, 75);
        FloatRect bbox = text_msg.getLocalBounds();
        // window 居中的位置
        text_msg.setOrigin(bbox.left + bbox.width / 2.0f, bbox.top + bbox.height / 2.0f);
        text_msg.setPosition(window_width / 2.0f, window_height / 2.0f);
        Text text_score("Score: 0", font_score, 100);
        text_score.setPosition(20, 20);
        // time bar
        float time_bar_width = 400;
        float time_bar_height = 80;
        RectangleShape time_bar{Vector2f(time_bar_width, time_bar_height)};
        time_bar.setFillColor(Color::Red);
        // window 最下方居中的位置
        time_bar.setPosition((window_width - time_bar_width) / 2.0f, (window_height - time_bar_height) + 5.f);
        float time_remaining = 10;
        float pixel_per_second = time_bar_width / time_remaining;
        Clock clock;
        bool paused = true;
        while (window.isOpen())
        {
            // check all the window's events that were triggered since the last iteration of the loop
            sf::Event event;
            while (window.pollEvent(event))
            {
                // "close requested" event: we close the window
                if (event.type == sf::Event::Closed)
                    window.close();
                
                if (event.type == sf::Event::KeyPressed){
                    if (Keyboard::isKeyPressed(Keyboard::Escape)){
                        window.close();
                    // 回车键, 开始游戏
                    } else if (Keyboard::isKeyPressed(Keyboard::Return)){
                        paused = false;
                        clock.restart();
                        score = 0;
                        time_remaining = 10;
                    }
                }
            }
            if (!paused){
                // 根据两帧的时间差计算频率, 根据频率设置速度
                Time dt = clock.restart();
                if (!bee_isalive){
                    srand(time(NULL));
                    bee_height = (rand() % 350 + 500.0);
                    // 尽量和上面的 seed 不一样
                    srand(time(NULL) * 10);
                    // 每秒走多少个像素
                    bee_speed = ((rand() % 200) + 200.0);
                    sprite_bee.setPosition(2000, bee_height);
                    bee_isalive = true;
                } else {
                    float x = sprite_bee.getPosition().x;
                    float y = sprite_bee.getPosition().y;
                    x -= bee_speed * dt.asSeconds();
                    sprite_bee.setPosition(x, y);
                    if (x < -100){
                        bee_isalive = false;
                    }
                }
                if (!cloud_isalive){
                    srand(time(NULL) * 20);
                    cloud_height = rand() % 150;
                    srand(time(NULL) * 30);
                    cloud_speed = 10 + rand() % 100;
                    sprite_cloud.setPosition(-200, cloud_height);
                    cloud_isalive = true;
                } else {
                    float x = sprite_cloud.getPosition().x;
                    float y = sprite_cloud.getPosition().y;
                    x += cloud_speed * dt.asSeconds();
                    sprite_cloud.setPosition(x, y);
                    if (x > 2000){
                        cloud_isalive = false;
                    }
                }
                score = rand() % 100;
                stringstream ss;
                ss << "Score: " << score;
                text_score.setString(ss.str());
                time_remaining -= dt.asSeconds();
                time_bar_width = pixel_per_second * time_remaining;
                if (time_remaining < 0){
                    paused = true;
                    text_msg.setString("Time out!!! Press Enter to restart!");
                    // 因为内容变了, 要重新计算它的 bounding box, 找到 bounding box 的中心点
                    FloatRect bbox = text_msg.getLocalBounds();
                    text_msg.setOrigin(bbox.left + bbox.width / 2.f, bbox.top + bbox.height / 2.f);
                    // 和 window 的中心点对齐
                    text_msg.setPosition(window_width / 2.f, window_height / 2.f);
                }
                time_bar.setSize(Vector2f(time_bar_width, time_bar_height));
            }
            window.clear();
            window.draw(sprite_bg);
            window.draw(sprite_cloud);
            window.draw(sprite_tree);
            window.draw(sprite_bee);
            window.draw(text_score);
            window.draw(time_bar);
            if (paused){
                window.draw(text_msg);
            }
            window.display();
        }
    }
}
namespace doc{
    void test(){
        sf::RenderWindow window(sf::VideoMode(800, 600), "My window", Style::Fullscreen);
        sf::CircleShape cs(50);
        cs.setFillColor(sf::Color::Green);
        cs.setPosition(100, 300);
        sf::VideoMode desk_mode = sf::VideoMode::getDesktopMode();
        cout << desk_mode.width << "*" << desk_mode.height << endl;
        // run the program as long as the window is open
        while (window.isOpen())
        {
            // check all the window's events that were triggered since the last iteration of the loop
            sf::Event event;
            while (window.pollEvent(event))
            {
                // "close requested" event: we close the window
                if (event.type == sf::Event::Closed)
                    window.close();
            }
            window.clear();
            window.draw(cs);
            window.display();
        }
    }
}
int main()
{
    g1::test();
    return 0;
}
