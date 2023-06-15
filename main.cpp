#include <iostream>
#include <sstream>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

#include "Game.h"

class FPS
{
public:
    FPS() : m_frame(0), m_fps(0) {}

    [[nodiscard]] unsigned int GetFPS() const { return m_fps; }

    void Update()
    {
        if (m_clock.getElapsedTime().asSeconds() >= 1.f)
        {
            m_fps = m_frame;
            m_frame = 0;
            m_clock.restart();
        }

        ++m_frame;
    }
private:
    unsigned int m_frame;
    unsigned int m_fps;
    sf::Clock m_clock;
};


int main()
{
    sf::RenderWindow window(sf::VideoMode(800, 800), "SFML Pac-Man");

    FPS fps;

    Game game;

    sf::Clock clock;

    // Start the game loop
    while (window.isOpen())
    {
        // Process events
        sf::Event event{};
        while (window.pollEvent(event))
        {
            // Close window: exit
            if (event.type == sf::Event::Closed)
                window.close();
        }

        game.Input();

        fps.Update();
        std::ostringstream ss;
        ss << fps.GetFPS();

        window.setTitle("SFML Pac-Man   FPS: " + ss.str());

        // Game tick every 100 ms
        while (clock.getElapsedTime() >= sf::milliseconds(200))
        {
            game.Update();
            clock.restart();
        }

        window.clear();
        game.Render(window);
        window.display();
    }
    return EXIT_SUCCESS;
}