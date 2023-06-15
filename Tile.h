/**
 * @file Tile.h
 * @brief Определение структуры Tile.
 *
 * Структура Tile представляет собой игровой объект "плитка".
 */

#pragma once

#include <cfloat>
#include <SFML/System/Vector2.hpp>

#include "np.h"

/**
 * @enum eTileType
 * @brief Типы плиток.
 */
enum class eTileType {
    e_Path = -1, /**< Путь. */
    e_Wall = 0, /**< Стена. */
    e_Coin = 1, /**< Монетка. */
    e_PowerUp = 2, /**< Усиление. */
    e_WrapAroundPath = 3 /**< Путь с оберткой. */
};

/**
 * @struct Tile
 * @brief Структура плитки.
 *
 * Структура Tile представляет собой игровой объект "плитка".
 */
struct Tile {
    /**
     * @brief Конструктор с параметрами.
     *
     * Инициализирует объект структуры Tile с указанными значениями типа, позиции и возможности столкновения.
     *
     * @param type Тип плитки (eTileType).
     * @param position Позиция плитки (sf::Vector2i).
     * @param canCollide Возможность столкновения с плиткой (bool).
     */
    Tile(eTileType type, sf::Vector2i position, bool canCollide)
            : m_type(type), m_position(position), m_canCollide(canCollide),
              m_cameFromNode(nullptr), m_fCost(0), m_gCost(0), m_hCost(0) {
    }

    /**
     * @brief Оператор равенства.
     *
     * Сравнивает два объекта структуры Tile на равенство.
     *
     * @param tile Объект структуры Tile для сравнения.
     * @return Результат сравнения (bool).
     */
    bool operator==(const Tile &tile) const {
        return m_type == tile.m_type && m_position == tile.m_position;
    }

    /**
     * @brief Вычисление значения F-стоимости.
     *
     * Вычисляет значение F-стоимости как сумму значений G-стоимости и H-стоимости.
     */
    void CalculateFCost() {
        m_fCost = m_gCost + m_hCost;
    }

    eTileType m_type; /**< Тип плитки. */
    sf::Vector2i m_position; /**< Позиция плитки. */
    bool m_canCollide; /**< Возможность столкновения с плиткой. */

    Tile *m_cameFromNode; /**< Указатель на предыдущую плитку в пути. */
    int m_fCost; /**< Значение F-стоимости. */
    int m_gCost; /**< Значение G-стоимости. */
    int m_hCost; /**< Значение H-стоимости. */
};
