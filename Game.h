#pragma once
#include <vector>
#include <iostream>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window/Keyboard.hpp>
#include "Entity.h"
#include "Ghost.h"
#include "Pacman.h"
#include "PIckup.h"
#include "Info.h"
#include "Manager.h"
#include "np.h"


class Game
{
public:
    Game():
            m_score(
                    "Score : ",
                    cnp::k_gridCellSize,
                    { 0.f, 0.f }
            ),
            m_lives(
                    "Lives: ",
                    cnp::k_gridCellSize,
                    { cnp::k_screenSize - 5 * cnp::k_gridCellSize, 0.f }
            ),
            m_end(
                    "Game Over",
                    2 * cnp::k_gridCellSize,
                    {
                            (static_cast<float>(cnp::k_screenSize) / 2.f) - 6 * cnp::k_gridCellSize,
                            (static_cast<float>(cnp::k_screenSize) / 2.f) - 2 * cnp::k_gridCellSize
                    },
                    false
            )
    {
        m_font.loadFromFile("../Data/Font.ttf");

        m_score.SetFont(m_font);
        m_lives.SetFont(m_font);
        m_end.SetFont(m_font);


        if (!m_tileManager.LoadLevel("../Data/Level.csv"))
        {
            std::cout << "Error loading level data" << std::endl;
        }

        for (const auto& pickup : m_tileManager.GetPickUpLocations())
        {
            m_pickups.emplace_back();
            m_pickups.back().Initialise(pickup.first, static_cast<ePickUpType>(pickup.second));
        }

        m_ghosts.emplace_back(
                eGhostType::e_Blinky,
                m_tileManager.GetLevelData(),
                m_pacMan
        );

        m_ghosts.emplace_back(
                eGhostType::e_Pinky,
                m_tileManager.GetLevelData(),
                m_pacMan
        );

        m_ghosts.emplace_back(
                eGhostType::e_Inky,
                m_tileManager.GetLevelData(),
                m_pacMan
        );

        m_ghosts.emplace_back(
                eGhostType::e_Clyde,
                m_tileManager.GetLevelData(),
                m_pacMan
        );
    }


    void Input(){
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))
        {
            m_pacMan.SetDirection(eDirection::e_Up);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))
        {
            m_pacMan.SetDirection(eDirection::e_Down);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))
        {
            m_pacMan.SetDirection(eDirection::e_Left);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))
        {
            m_pacMan.SetDirection(eDirection::e_Right);
        }
    }
    void Update(){
        if (m_gameOver)
        {
            m_end.SetString(m_pacMan.GetLivesRemaining() <= 0 ? "Game Over" : "You Win!");

            m_end.SetVisible(true);

            m_score.SetPosition({
                                        m_end.GetPosition().x,
                                        m_end.GetPosition().y + 2 * cnp::k_gridCellSize
                                });
        } else
        {
            if (m_pacMan.IsAlive())
            {
                m_pacMan.Update(m_tileManager.GetLevelData());

                // If all the coins are collected then pacman has won
                int activeCoins = 0;

                for (auto& pickup : m_pickups)
                {
                    switch (pickup.GetPickUpType())
                    {
                        case ePickUpType::e_Coin:
                            if (pickup.Visible())
                            {
                                activeCoins++;
                            }
                        case ePickUpType::e_PowerUp:
                            if (pickup.Visible())
                            {
                                pickup.CheckPacManCollisions(m_pacMan);
                            }
                            break;
                        default:
                            std::cout << "Unknown pickup" << std::endl;
                    }

                }

                if (activeCoins == 0)
                {
                    m_gameOver = true;
                }

                for (auto& ghost : m_ghosts)
                {
                    if (ghost.GetGhostState() != eGhostState::e_Frightened)
                    {
                        switch (m_pacMan.GetPacManState())
                        {
                            case ePacManState::e_Normal:
                                ghost.SetGhostState(eGhostState::e_Chase);
                                break;
                            case ePacManState::e_PowerUp:
                                ghost.SetGhostState(eGhostState::e_Scatter);
                                break;
                            default:;
                        }
                    }
                    ghost.Update();
                }

                if (hnp::rand_range(0, 1000) <= 5)
                {
                    SpawnNewPowerUp();
                }

            } else
            {
                m_pacMan.Reset();
                for (auto& ghost : m_ghosts)
                {
                    ghost.Reset();
                }

                if (m_pacMan.GetLivesRemaining() == 0)
                {
                    m_gameOver = true;
                }
            }
        }
        m_score.SetString("Score: " + std::to_string(m_pacMan.GetPoints()));
        m_lives.SetString("Lives: " + std::to_string(m_pacMan.GetLivesRemaining()));

    }

    void Render(sf::RenderWindow& window){
        m_tileManager.Render(window);

        for (const auto& pickup : m_pickups)
        {
            if (pickup.Visible())
            {
                pickup.Render(window);
            }
        }

        m_pacMan.Render(window);

        for (auto& ghost : m_ghosts)
        {
            ghost.Render(window);
        }

        m_score.Render(window);
        m_lives.Render(window);
        m_end.Render(window);
    }

private:
    bool m_gameOver{};
    PacMan m_pacMan;
    std::vector<PickUp> m_pickups;
    std::vector<Ghost> m_ghosts;
    Manager m_tileManager;

    Info m_score;
    Info m_lives;
    Info m_end;

    sf::Font m_font;

    void SpawnNewPowerUp(){
        // Find an appropriate place to spawn the new power-up
        auto& map = m_tileManager.GetLevelData();
        Tile& randomTile = map[1][1];
        PickUp* firstAvailablePickup = nullptr;

        bool tileTaken = false;
        do
        {
            const sf::Vector2i random(hnp::rand_range(25, 750), hnp::rand_range(50, 725));
            randomTile = map[hnp::world_coord_to_array_index(random.y)][hnp::world_coord_to_array_index(random.y)];

            // See if there is already a coin or pickup at this position
            for (auto& pickup : m_pickups)
            {
                if (pickup.Visible() && pickup.GetPosition() == randomTile.m_position) tileTaken = true;
                else
                {
                    if (!pickup.Visible() && !firstAvailablePickup)
                    {
                        firstAvailablePickup = &pickup;
                    }
                }
            }
        } while (randomTile.m_type != eTileType::e_Path && !tileTaken);

        if (firstAvailablePickup)
        {
            firstAvailablePickup->Initialise(randomTile.m_position, ePickUpType::e_PowerUp);
        }
    }
};