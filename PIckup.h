/**
 * @file PickUp.h
 * @brief Определение класса PickUp.
 *
 * Класс PickUp представляет собой игровой объект "подборка".
 */

#pragma once
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include "Pacman.h"
#include "np.h"

/**
 * @enum ePickUpType
 * @brief Типы подборок.
 */
enum class ePickUpType
{
    e_Coin = 1, /**< Монетка. */
    e_PowerUp = 2, /**< Усиление. */
    e_Cherry = 3, /**< Вишня. */
    e_Life = 4
};

/**
 * @class PickUp
 * @brief Класс подборки.
 *
 * Класс PickUp представляет собой игровой объект "подборка".
 */
class PickUp
{
public:
    /**
     * @brief Конструктор по умолчанию.
     *
     * Инициализирует объект класса PickUp с начальными значениями.
     */
    PickUp():
            m_position(),
            m_visible(false),
            m_type(ePickUpType::e_Coin)
    {

    }

    /**
     * @brief Отрисовка подборки.
     *
     * Отрисовывает подборку на экране в зависимости от ее текущего состояния и позиции.
     *
     * @param window Объект класса sf::RenderWindow, представляющий собой окно для отрисовки.
     */
    void Render(sf::RenderWindow& window) const {
        sf::CircleShape circle;
        switch (m_type)
        {
            case ePickUpType::e_Coin:
                circle.setFillColor({ 255, 255, 71 });
                circle.setRadius(5);
                break;
            case ePickUpType::e_PowerUp:
                circle.setFillColor({ 255, 255, 255 });
                circle.setRadius(10);
                break;
            case ePickUpType::e_Cherry: break;
            default:;
        }

        circle.setOrigin(circle.getGlobalBounds().width / 2, circle.getGlobalBounds().height / 2);

        circle.setPosition(
                {
                        static_cast<float>(m_position.x) + static_cast<float>(cnp::k_gridCellSize) / 2.f,
                        static_cast<float>(m_position.y) + static_cast<float>(cnp::k_gridCellSize) / 2.f
                }
        );
        window.draw(circle);
    }

    /**
     * @brief Инициализация подборки.
     *
     * Инициализирует подборку с указанными значениями позиции и типа. Устанавливает видимость подборки в true.
     *
     * @param position Новая позиция подборки (sf::Vector2i).
     * @param type Новый тип подборки (ePickUpType).
     */
    void Initialise(sf::Vector2i position, ePickUpType type){
        m_position = position;
        m_type = type;
        m_visible = true;
    }

    /**
     * @brief Получение видимости подборки.
     *
     * Возвращает видимость подборки (видна или нет).
     *
     * @return Видимость подборки (bool).
     */
    bool Visible() const {
        return m_visible;
    }

    /**
     * @brief Получение типа подборки.
     *
     * Возвращает тип подборки (ePickUpType).
     *
     * @return Тип подборки (ePickUpType).
     */
    ePickUpType GetPickUpType() const {
        return m_type;
    }

    /**
     * @brief Проверка столкновений с Пакманом.
     *
     * Проверяет наличие столкновений с Пакманом и обрабатывает их в зависимости от типа подборки. Если произошло столкновение, то устанавливает видимость подборки в false.
     *
     * @param pacman Ссылка на объект класса PacMan, представляющий собой игрового персонажа Пакмана.
     */
    void CheckPacManCollisions(PacMan& pacman){
        if (m_position == pacman.GetPosition())
        {
            switch (m_type)
            {
                case ePickUpType::e_Coin:
                    pacman.AddPoints(10);
                    break;
                case ePickUpType::e_PowerUp:
                    pacman.AddPoints(500);
                    pacman.PowerUp();
                    break;
                case ePickUpType::e_Life:
                    pacman.AddLives(1);
                    break;
                default:;
            }

            m_visible = false;
        }
    }
    /**
     * @brief Получение позиции подборки.
     *
     * Возвращает текущую позицию подборки.
     *
     * @return Текущая позиция подборки (sf::Vector2i).
     */
    const sf::Vector2i& GetPosition() const {
        return m_position;
    }

private:
    sf::Vector2i m_position; /**< Текущая позиция подборки. */
    bool m_visible; /**< Видимость подборки (видна или нет). */
    ePickUpType m_type; /**< Тип подборки. */
};
