#include <algorithm>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <unistd.h>

#include "SFML/Audio/SoundBuffer.hpp"
#include "SFML/Graphics.hpp"
#include "SFML/System.hpp"
#include "SFML/System/Sleep.hpp"
#include "SFML/Window.hpp"
#include "SFML/Audio.hpp"

using namespace std;
using namespace sf;
namespace g1{
    enum class Side{
        NONE    = 0,
        LEFT    = 1,
        RIGHT   = 2,
    };
    void spawn_branches(Sprite *sprite_branchs, size_t n){
        for (int i = 0; i < n; ++i){
            int direction = rand() % 3;
            int height = i * 150;
            if (direction == static_cast<int>(Side::LEFT)){
                sprite_branchs[i].setPosition(610, height);
                sprite_branchs[i].setRotation(180);
            } else if (direction == static_cast<int>(Side::RIGHT)){
                sprite_branchs[i].setPosition(1330, height);
                sprite_branchs[i].setRotation(0);
            } else {
                sprite_branchs[i].setPosition(3000, height);
                sprite_branchs[i].setRotation(0);
            }
        }
    }
    void update_branchs(Sprite *sprite_branchs, size_t n){
        for (int i = n - 1; i > 0; --i){
            sprite_branchs[i].setPosition(sprite_branchs[i - 1].getPosition().x, i * 150);
            sprite_branchs[i].setRotation(sprite_branchs[i - 1].getRotation());
        }
        spawn_branches(sprite_branchs, 1);
    }
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
        // branch
        Texture texture_branch;
        texture_branch.loadFromFile("01/graphics/branch.png");
        Vector2u texture_branch_size = texture_branch.getSize();
        const int NUM_BRANCHES = 5;
        Sprite sprite_branches[NUM_BRANCHES];
        for (int i = 0; i < NUM_BRANCHES; ++i){
            sprite_branches[i].setTexture(texture_branch);
            sprite_branches[i].setOrigin(texture_branch_size.x / 2.f, texture_branch_size.y / 2.f);
        }
        // player
        Texture texture_player;
        texture_player.loadFromFile("01/graphics/player.png");
        Sprite sprite_player;
        const float player_left_x = 580;
        const float player_right_x = 1200;
        const float player_y = 720;
        Vector2u player_size = texture_player.getSize();
        sprite_player.setTexture(texture_player);
        sprite_player.setPosition(player_right_x, player_y);
        // sprite_player.setOrigin(player_size.x / 2.f, player_size.y / 2.f);
        // sprite_player.setPosition(player_right_x + player_size.x / 2.f, player_y + player_size.y / 2.f);
        // gravestone
        Texture texture_stone;
        texture_stone.loadFromFile("01/graphics/rip.png");
        Sprite sprite_stone;
        sprite_stone.setTexture(texture_stone);
        sprite_stone.setPosition(window_width, 860);
        // axe
        Texture texture_axe;
        texture_axe.loadFromFile("01/graphics/axe.png");
        Sprite sprite_axe;
        const float axe_left_x = 700;
        const float axe_right_x = 1075;
        const float axe_y = 830;
        sprite_axe.setTexture(texture_axe);
        sprite_axe.setPosition(axe_right_x, axe_y);
        // log
        Texture texture_log;
        texture_log.loadFromFile("01/graphics/log.png");
        Sprite sprite_log;
        sprite_log.setTexture(texture_log);
        sprite_log.setPosition(810, 720);
        // audio
        SoundBuffer buf;
        // buf.loadFromFile("/media/adam/18f44327-4098-4d8f-acd3-785433ccc1e1/Music/张信哲-白月光.flac");
        buf.loadFromFile("/media/adam/18f44327-4098-4d8f-acd3-785433ccc1e1/Music/梁朝伟刘德华-无间道国语.wav");
        // buf.loadFromFile("01/sound/chop.wav");
        Sound sound;
        sound.setBuffer(buf);
        sound.play();
        Clock clock;
        bool paused = true;
        bool accept_input = true;
        while (window.isOpen()) {
            // check all the window's events that were triggered since the last iteration of the loop
            sf::Event event;
            while (window.pollEvent(event)) {
                // "close requested" event: we close the window
                if (event.type == sf::Event::Closed)
                    window.close();
                
                if (event.type == sf::Event::KeyPressed && accept_input){
                    if (Keyboard::isKeyPressed(Keyboard::Escape)){
                        window.close();
                    // 回车键, 开始游戏
                    } else if (Keyboard::isKeyPressed(Keyboard::Return)){
                        paused = false;
                        clock.restart();
                        score = 0;
                        time_remaining = 10;
                        spawn_branches(sprite_branches, NUM_BRANCHES);
                    } else if (Keyboard::isKeyPressed(Keyboard::Right) && !paused){
                        ++score;
                        time_remaining += (2.f / score) + 0.15f;
                        sprite_axe.setPosition(axe_right_x, axe_y);
                        sprite_player.setPosition(player_right_x, player_y);
                        // sprite_player.setPosition(player_right_x + player_size.x / 2.f, player_y + player_size.y / 2.f);
                        update_branchs(sprite_branches, NUM_BRANCHES);
                        accept_input = false;
                    } else if (Keyboard::isKeyPressed(Keyboard::Left) && !paused){
                        ++score;
                        time_remaining += (2.f / score) + 0.15f;
                        sprite_axe.setPosition(axe_left_x, axe_y);
                        sprite_player.setPosition(player_left_x, player_y);
                        update_branchs(sprite_branches, NUM_BRANCHES);
                        // sprite_player.setScale(-1, 1);
                        // sprite_player.setPosition(player_left_x + player_size.x / 2.f, player_y + player_size.y / 2.f);
                        // sprite_player.rotate(180);
                        accept_input = false;
                    }
                }
                if (event.type == Event::KeyReleased){
                    // if (event.key.code == Keyboard::Left ||
                        // event.key.code == Keyboard::Right)
                    accept_input = true;
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
            window.draw(sprite_player);
            window.draw(sprite_stone);
            window.draw(sprite_axe);
            window.draw(sprite_log);
            if (paused){
                window.draw(text_msg);
            }
            for (int i = 0; i < NUM_BRANCHES; ++i){
                window.draw(sprite_branches[i]);
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
