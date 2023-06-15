
/**
 * @file PacMan.h
 * @brief Определение класса PacMan.
 *
 * Класс PacMan наследуется от класса Entity и представляет собой игрового персонажа Пакмана.
 */

#pragma once

#include "Entity.h"
#include "np.h"

/**
 * @enum ePacManState
 * @brief Состояния Пакмана.
 */
enum class ePacManState {
    e_Normal, /**< Нормальное состояние. */
    e_PowerUp /**< Состояние усиления. */
};

/**
 * @class PacMan
 * @brief Класс Пакмана.
 *
 * Класс PacMan наследуется от класса Entity и представляет собой игрового персонажа Пакмана.
 */
class PacMan final : public Entity {
public:
    /**
     * @brief Конструктор по умолчанию.
     *
     * Инициализирует объект класса PacMan с начальными значениями.
     */
    PacMan() :
            Entity(
                    cnp::k_pacManSpawnPosition,
                    cnp::k_gridCellSize,
                    eDirection::e_None,
                    sf::Color::Yellow
            ),
            m_points(0),
            m_lives(3),
            m_isAlive(true),
            m_state(ePacManState::e_Normal),
            m_powerUpTimer(0.f) {
    }

    /**
     * @brief Обновление состояния Пакмана.
     *
     * Обновляет состояние Пакмана в зависимости от текущего состояния и окружения.
     *
     * @param tiles Вектор векторов объектов класса Tile, представляющий собой игровое поле.
     */
    void Update(const std::vector<std::vector<Tile>> &tiles) {
        CheckForBlockades(tiles);
        Move();
        if (m_state == ePacManState::e_PowerUp) {
            m_powerUpTimer += static_cast<float>(m_clock.getElapsedTime().asSeconds());

            if (m_powerUpTimer >= cnp::k_pacManPowerUpTime) {
                m_state = ePacManState::e_Normal;
                m_powerUpTimer = 0.f;
            }
        }
        m_clock.restart();
    }

    /**
     * @brief Отрисовка Пакмана.
     *
     * Отрисовывает Пакмана на экране в зависимости от его текущего состояния и позиции.
     *
     * @param window Объект класса sf::RenderWindow, представляющий собой окно для отрисовки.
     */
    void Render(sf::RenderWindow &window) {
        if (m_state == ePacManState::e_PowerUp) {
            // Interpolate between pacman's colour and the power-up
            // Colour based on the time
            const float normalisedTimer = m_powerUpTimer / static_cast<float>(cnp::k_pacManPowerUpTime);

            const sf::Uint32 lerpedColour = hnp::interpolate(m_colour.toInteger(), sf::Color::White.toInteger(),
                                                             normalisedTimer);

            m_shape.setFillColor(sf::Color(lerpedColour));
        } else {
            m_shape.setFillColor(sf::Color::Yellow);
        }

        m_shape.setPosition(static_cast<sf::Vector2f>(m_position));
        window.draw(m_shape);
    }

    /**
     * @brief Получение текущего состояния Пакмана.
     *
     * Возвращает текущее состояние Пакмана.
     *
     * @return Текущее состояние Пакмана (ePacManState).
     */
    const ePacManState &GetPacManState() const {
        return m_state;
    }

    /**
     * @brief Усиление Пакмана.
     *
     * Устанавливает состояние Пакмана в режим усиления и обнуляет таймер усиления.
     */
    void PowerUp() {
        m_state = ePacManState::e_PowerUp;
        m_powerUpTimer = 0.f;
    }

    /**
     * @brief Добавление очков.
     *
     * Добавляет указанное количество очков к текущему счету Пакмана.
     *
     * @param amount Количество очков для добавления.
     */
    void AddPoints(int amount) {
        m_points += amount;
    }

    /**
     * @brief Получение текущего счета Пакмана.
     *
     * Возвращает текущий счет Пакмана.
     *
     * @return Текущий счет Пакмана (int).
     */
    int GetPoints() const {
        return m_points;
    }

    /**
     * @brief Получение количества оставшихся жизней Пакмана.
     *
     * Возвращает количество оставшихся жизней Пакмана.
     *
     * @return Количество оставшихся жизней Пакмана (int).
     */
    int GetLivesRemaining() const {
        return m_lives;
    }

    /**
     * @brief Сброс состояния Пакмана.
     *
     * Сбрасывает состояние Пакмана до начальных значений.
     */
    void Reset() {
        {
            m_position = cnp::k_pacManSpawnPosition;
            m_isAlive = true;
        }
    }

    /**
     * @brief Получение состояния жизни Пакмана.
     *
     * Возвращает состояние жизни Пакмана (жив или мертв).
     *
     * @return Состояние жизни Пакмана (bool).
     */
    bool IsAlive() const {
        return m_isAlive;
    }

    /**
     * @brief Установка состояния жизни Пакмана.
     *
     * Устанавливает состояние жизни Пакмана в указанное значение. Если значение равно false, то уменьшает количество жизней на 1 и отнимает 500 очков от текущего счета (если счет меньше 500, то устанавливает его в 0).
     *
     * @param alive Новое состояние жизни Пакмана (bool).
     */
    void SetIsAlive(bool alive) {
        if (!alive) {
            m_lives -= 1;
            m_points -= 500;
            if (m_points < 0) m_points = 0;
        }
        m_isAlive = alive;
    }
    void AddLives(int n) {
        m_lives += n;
    }

private:
    int m_points; /**< Текущий счет Пакмана. */
    int m_lives; /**< Количество оставшихся жизней Пакмана. */
    bool m_isAlive; /**< Состояние жизни Пакмана (жив или мертв). */
    ePacManState m_state; /**< Текущее состояние Пакмана. */
    float m_powerUpTimer; /**< Таймер усиления. */

    /**
     * @brief Движение Пакмана.
     *
     * Осуществляет движение Пакмана в зависимости от текущего направления движения и наличия препятствий.
     */
    void Move() {
        if (!hnp::is_in_vector(m_limitedDirections, m_currentDirection)) {
            switch (m_currentDirection) {
                case eDirection::e_Up:
                    m_position.y -= m_speed;
                    break;
                case eDirection::e_Down:
                    m_position.y += m_speed;
                    break;
                case eDirection::e_Left:
                    m_position.x -= m_speed;
                    break;
                case eDirection::e_Right:
                    m_position.x += m_speed;
                    break;
                case eDirection::e_None:
                    break;
                default:
                    std::cout << "Unknown Movement direction" << std::endl;
                    break;
            }
            WrapAround();
        }
        m_limitedDirections.clear();
    }
};
