#pragma once
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
// chekcing
#include "Tile.h"
#include "np.h"
#include <iostream>

/**
 * @brief Перечисление направлений движения
 */
enum class eDirection {
    e_None,  ///< Нет направления
    e_Up,    ///< Вверх
    e_Down,  ///< Вниз
    e_Left,  ///< Влево
    e_Right  ///< Вправо
};

/**
 * @brief Функция преобразования значения перечисления в строку
 * @param e Значение перечисления eDirection
 * @return Строковое представление значения перечисления
 */
inline const char *to_string(eDirection e) {
    switch (e) {
        case eDirection::e_None:
            return "e_None";
        case eDirection::e_Up:
            return "e_Up";
        case eDirection::e_Down:
            return "e_Down";
        case eDirection::e_Left:
            return "e_Left";
        case eDirection::e_Right:
            return "e_Right";
        default:
            return "unknown";
    }
}

/**
 * @brief Класс Entity представляет собой сущность в игре.
 */
class Entity {
public:
    /**
     * @brief Устанавливает направление движения сущности.
     * @param direction Направление движения.
     */
    void SetDirection(const eDirection direction) {
        switch (direction) {
            case eDirection::e_Up:
                if (m_currentDirection != eDirection::e_Down) {
                    m_currentDirection = direction;
                }
                break;
            case eDirection::e_Down:
                if (m_currentDirection != eDirection::e_Up) {
                    m_currentDirection = direction;
                }
                break;
            case eDirection::e_Left:
                if (m_currentDirection != eDirection::e_Right) {
                    m_currentDirection = direction;
                }
                break;
            case eDirection::e_Right:
                if (m_currentDirection != eDirection::e_Left) {
                    m_currentDirection = direction;
                }
                break;
            case eDirection::e_None:
                m_currentDirection = direction;
                break;
            default:
                std::cout << "Unknown Movement direction" << std::endl;
                break;
        }
    }

    /**
     * @brief Возвращает текущее направление движения сущности.
     * @return Текущее направление движения.
     */
    eDirection GetDirection() const {
        return m_currentDirection;
    }

    /**
     * @brief Возвращает текущую позицию сущности.
     * @return Текущая позиция сущности.
     */
    sf::Vector2i GetPosition() const {
        return m_position;
    }

    /**
     * @brief Устанавливает позицию сущности.
     * @param position Новая позиция сущности.
     */
    void SetPosition(const sf::Vector2i position) {
        m_position = position;
    }

protected:
    sf::Vector2i m_position; ///< Позиция сущности.
    int m_speed; ///< Скорость движения сущности.
    eDirection m_currentDirection; ///< Текущее направление движения сущности.
    std::vector<eDirection> m_limitedDirections; ///< Ограниченные направления движения сущности.
    sf::RectangleShape m_shape; ///< Форма сущности.
    sf::Color m_colour; ///< Цвет сущности.
    sf::Clock m_clock; ///< Таймер для обновления состояния сущности.

    /**
 * @brief Конструктор класса Entity.
 * @param position Начальная позиция сущности.
 * @param speed Скорость движения сущности.
 * @param startingDirection Начальное направление движения сущности.
 * @param colour Цвет сущности.
 */
    Entity(const sf::Vector2i position, const int speed, const eDirection startingDirection, sf::Color colour) :
            m_position(position),
            m_speed(speed),
            m_currentDirection(startingDirection),
            m_shape({cnp::k_gridCellSize, cnp::k_gridCellSize}),
            m_colour(colour),
            m_clock() {
    }

    /**
 * @brief Обрабатывает перемещение сущности за границы игрового поля.
 */
    void WrapAround() {
        if (m_position.x < 0) {
            m_position.x = cnp::k_screenSize + m_position.x;
        } else if (m_position.x > cnp::k_screenSize - cnp::k_gridCellSize) {
            m_position.x = cnp::k_screenSize - m_position.x;
        }
    }

/**
 * @brief Проверяет наличие препятствий на пути движения сущности.
 * @param tiles Двумерный массив тайлов игрового поля.
 */
    void CheckForBlockades(const std::vector<std::vector<Tile>> &tiles) {
        if (hnp::is_in_range(m_position.x, 0, cnp::k_screenSize - cnp::k_gridCellSize) &&
            hnp::is_in_range(m_position.y, 0, cnp::k_screenSize - cnp::k_gridCellSize)) {
            const int entityX = m_position.x / cnp::k_gridCellSize;
            const int entityY = m_position.y / cnp::k_gridCellSize;

            {
                const auto &currentTile = tiles[entityY - 1][entityX];
                if (currentTile.m_canCollide) {
                    if (m_position.y <= currentTile.m_position.y + cnp::k_gridCellSize) {
                        m_position = {m_position.x, currentTile.m_position.y + cnp::k_gridCellSize};
                        m_limitedDirections.push_back(eDirection::e_Up);
                    }
                }
            }

            {
                const auto &currentTile = tiles[entityY + 1][entityX];
                if (currentTile.m_canCollide) {
                    if (m_position.y >= currentTile.m_position.y - cnp::k_gridCellSize) {
                        m_position = {m_position.x, currentTile.m_position.y - cnp::k_gridCellSize};
                        m_limitedDirections.push_back(eDirection::e_Down);
                    }
                }
            }

            {
                const auto &currentTile = tiles[entityY][entityX - 1];
                if (currentTile.m_canCollide) {
                    if (m_position.x >= currentTile.m_position.x + cnp::k_gridCellSize) {
                        m_position = {currentTile.m_position.x + cnp::k_gridCellSize, m_position.y};
                        m_limitedDirections.push_back(eDirection::e_Left);
                    }
                }
            }

            {
                const auto &currentTile = tiles[entityY][entityX + 1];
                if (currentTile.m_canCollide) {
                    if (m_position.x <= currentTile.m_position.x - cnp::k_gridCellSize) {
                        m_position = {currentTile.m_position.x - cnp::k_gridCellSize, m_position.y};
                        m_limitedDirections.push_back(eDirection::e_Right);
                    }
                }
            }
        }
    }


};