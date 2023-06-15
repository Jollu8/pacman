#pragma once

#include <stack>
#include <iostream>
#include "Entity.h"
#include "Pacman.h"
#include "np.h"

/**
 * @brief Перечисление типов призраков.
 */
enum class eGhostType {
    e_Blinky, ///< Блинки.
    e_Pinky, ///< Пинки.
    e_Inky, ///< Инки.
    e_Clyde ///< Клайд.
};

/**
 * @brief Перечисление состояний призраков.
 */
enum class eGhostState {
    e_Chase, ///< Преследование.
    e_Scatter, ///< Разбегание.
    e_Frightened ///< Испуг.
};


/**
 * @brief Класс Ghost представляет собой призрака в игре Pac-Man.
 */
class Ghost final : public Entity {
public:
    /**
     * @brief Конструктор класса Ghost.
     * @param type Тип призрака.
     * @param grid Двумерный массив тайлов игрового поля.
     * @param pacMan Ссылка на объект класса PacMan.
     */
    explicit Ghost(eGhostType type, std::vector<std::vector<Tile>> &grid, PacMan &pacMan) :
            Entity(sf::Vector2i(),
                   cnp::k_gridCellSize,
                   eDirection::e_None,
                   sf::Color::White),
            m_pacMan(pacMan),
            m_type(type),
            m_state(eGhostState::e_Chase),
            m_homeTimer(0.f),
            m_updateTicks(0),
            m_grid(grid),
            m_currentCorner(0) {
        // Определение углов карты для патрулирования и разбегания
        switch (m_type) {
            case eGhostType::e_Blinky:
                m_colour = sf::Color::Red;
                break;
            case eGhostType::e_Pinky:
                m_colour = sf::Color::Magenta;
                break;
            case eGhostType::e_Inky:
                m_colour = sf::Color::Cyan;
                break;
            case eGhostType::e_Clyde:
                m_colour = sf::Color(255, 165, 0);
                break;
            default:;
        }

        m_position = cnp::k_cornerPositions[static_cast<int>(m_type)];
    }

    /**
     * @brief Обновляет состояние призрака.
     */
    void Update() {
        if (m_state == eGhostState::e_Frightened && m_position == cnp::k_homePositions[static_cast<int>(m_type)]) {
            m_homeTimer += m_clock.getElapsedTime().asSeconds();
            if (m_homeTimer >= cnp::k_ghostHomeTime) {
                m_state = eGhostState::e_Chase;
                m_homeTimer = 0.f;
            }
        } else {
            Move();
            CheckPacManCollisions();
        }

        m_clock.restart();
    }

    /**
     * @brief Отрисовывает призрака на экране.
     * @param window Окно для отрисовки.
     */

    void Render(sf::RenderWindow &window) {
        std::stack<Tile *> temp = m_path;

        while (!temp.empty()) {
            auto *node = temp.top();
            temp.pop();

            m_shape.setFillColor({m_colour.r, m_colour.g, m_colour.b, 80});
            m_shape.setPosition(static_cast<sf::Vector2f>(node->m_position));
            window.draw(m_shape);
        }

        if (m_state != eGhostState::e_Frightened) m_shape.setFillColor(m_colour);
        else {
            const sf::Color frightenedColour = {0, 19, 142};
            if (m_position == cnp::k_homePositions[static_cast<int>(m_type)]) {
                // Blend from blue to the normal ghost colour
                const float normalisedTimer = m_homeTimer / static_cast<float>(cnp::k_ghostHomeTime);

                const sf::Uint32 lerpedColour = hnp::interpolate(frightenedColour.toInteger(), m_colour.toInteger(),
                                                                 normalisedTimer);

                m_shape.setFillColor(sf::Color(lerpedColour));
            } else m_shape.setFillColor(frightenedColour);
        }

        m_shape.setPosition(static_cast<sf::Vector2f>(m_position));

        window.draw(m_shape);
    }

    /**
   * @brief Сбрасывает состояние призрака.
   */
    void Reset() {
        m_state = eGhostState::e_Chase;
        m_position = cnp::k_cornerPositions[static_cast<int>(m_type)];

        // Очищает путь, если он существует
        while (!m_path.empty()) {
            m_path.pop();
        }
    }

    /**
  * @brief Возвращает текущее состояние призрака.
  * @return Текущее состояние призрака.
  */
    const eGhostState &GetGhostState() const {
        return m_state;
    }

/**
 * @brief Устанавливает состояние призрака.
 * @param state Новое состояние призрака.
 */
    void SetGhostState(eGhostState state) {
        m_state = state;
    }

private:
    PacMan &m_pacMan; ///< Ссылка на объект PacMan.
    eGhostType m_type; ///< Тип призрака.
    eGhostState m_state; ///< Состояние призрака.
    float m_homeTimer; ///< Таймер для отслеживания времени нахождения призрака дома.

    // Поиск пути будет обновляться каждые 10 игровых тиков (раз в секунду)
    int m_updateTicks;
    std::vector<std::vector<Tile>> &m_grid; ///< Ссылка на двумерный массив тайлов игрового поля.
    int m_currentCorner; ///< Текущий угол карты для патрулирования.

    std::stack<Tile *> m_path; ///< Стек тайлов, представляющий путь призрака.
    std::vector<Tile *> m_openList; ///< Список открытых тайлов для поиска пути.
    std::vector<Tile *> m_closedList; ///< Список закрытых тайлов для поиска пути.

    /**
     * @brief Возвращает узел с наименьшей стоимостью F из списка узлов.
     * @param list Список узлов для поиска.
     * @return Узел с наименьшей стоимостью F или nullptr, если список пуст.
     */
    static Tile *GetLowestFCostNode(std::vector<Tile *> &list) {
        if (!list.empty()) {
            Tile *lowestFCostNode = list[0];

            for (auto &node: list) {
                // перезаписываем наименьший узел, если стоимость F меньше
                if (node->m_fCost < lowestFCostNode->m_fCost) {
                    lowestFCostNode = node;
                }
            }

            return lowestFCostNode;
        }
        return nullptr;
    }

    /**
 * @brief Вычисляет расстояние между двумя тайлами.
 * @param a Первый тайл.
 * @param b Второй тайл.
 * @return Расстояние между тайлами.
 */
    static int CalculateDistanceCost(Tile *a, Tile *b) {
        const int deltaX = abs(a->m_position.x - b->m_position.x);
        const int deltaY = abs(a->m_position.y - b->m_position.y);

        const int remaining = abs(deltaX - deltaY);

        return 15 * (deltaX < deltaY ? deltaX : deltaY) + cnp::k_gridMovementCost * remaining;
    }

/**
 * @brief Вычисляет путь от начального узла до конечного узла.
 * @param endNode Конечный узел пути.
 */
    void CalculatePath(Tile *endNode) {
        // Очищаем стек
        while (!m_path.empty()) {
            m_path.pop();
        }

        m_path.push(endNode);

        Tile *currentNode = endNode;

        // Проходим через родительские узлы, пока не найдем узел без родителя
        // Этот узел является начальным узлом
        while (currentNode->m_cameFromNode != nullptr) {
            m_path.push(currentNode->m_cameFromNode);
            currentNode = currentNode->m_cameFromNode;
        }

        if (m_path.empty()) {
            std::cout << "No path to X: " << endNode->m_position.x
                      << " Y : " << endNode->m_position.y << " found" << std::endl;
        }
    }


    /**
  * @brief Возвращает список соседних узлов для текущего узла.
  * @param currentNode Текущий узел.
  * @return Список соседних узлов.
  */
    std::vector<Tile *> GetNeighbourNodes(Tile *currentNode) const {
        std::vector<Tile *> neighbours;

        const int xIndex = hnp::world_coord_to_array_index(currentNode->m_position.x);
        const int yIndex = hnp::world_coord_to_array_index(currentNode->m_position.y);

        // Находим 4 соседние позиции, если они допустимы
        if (xIndex - 1 >= 0) {
            // Слева
            neighbours.push_back(&m_grid[yIndex][xIndex - 1]);
        }

        if (xIndex + 1 < static_cast<int>(m_grid[0].size())) {
            // Справа
            neighbours.push_back(&m_grid[yIndex][xIndex + 1]);
        }

        // Сверху
        if (yIndex - 1 >= 0) {
            neighbours.push_back(&m_grid[yIndex - 1][xIndex]);
        }

        // Снизу
        if (yIndex + 1 < static_cast<int>(m_grid.size())) {
            neighbours.push_back(&m_grid[yIndex + 1][xIndex]);
        }

        return neighbours;
    }

    /**
 * @brief Выполняет поиск пути между начальной и конечной позициями с помощью алгоритма A*.
 * @param startPosition Начальная позиция.
 * @param endPosition Конечная позиция.
 */
    void AStarPathFinding(sf::Vector2i startPosition, sf::Vector2i endPosition) {
        // Вычисляем индексы массива для начальной и конечной позиций
        const sf::Vector2i startNodeIndices(
                startPosition.x / cnp::k_gridCellSize,
                startPosition.y / cnp::k_gridCellSize
        );

        const sf::Vector2i endNodeIndices(
                endPosition.x / cnp::k_gridCellSize,
                endPosition.y / cnp::k_gridCellSize
        );

        Tile *startNode = &m_grid[startNodeIndices.y][startNodeIndices.x];
        Tile *endNode = &m_grid[endNodeIndices.y][endNodeIndices.x];

        m_openList.clear();
        m_closedList.clear();

        // Проходим через сетку, устанавливаем стоимость g в бесконечность и вычисляем стоимость f
        for (auto &tile: m_grid) {
            for (auto &pathNode: tile) {
                // получаем ссылку на объект
                // устанавливаем стоимость g в бесконечность
                pathNode.m_gCost = INT_MAX;

                // вычисляем стоимость f
                pathNode.CalculateFCost();
                pathNode.m_cameFromNode = nullptr;
            }
        }

        // Вычисляем стоимости для начального узла
        startNode->m_gCost = 0;
        startNode->m_hCost = CalculateDistanceCost(startNode, endNode);
        startNode->CalculateFCost();
        m_openList.push_back(startNode);
        int loops = 0;
        // Проходим через все узлы и находим путь
        while (!m_openList.empty()) {
            ++loops;
            // Текущий узел является узлом в открытом списке с наименьшей стоимостью F
            Tile *currentNode = GetLowestFCostNode(m_openList);

            m_openList.erase(std::remove(m_openList.begin(), m_openList.end(), currentNode), m_openList.end());
            m_closedList.push_back(currentNode);

            // Если текущий узел является конечным
            if (currentNode == endNode) {
                CalculatePath(endNode);
            }

            for (auto &neighbour: GetNeighbourNodes(currentNode)) {
                // Убедитесь, что текущий соседний узел не находится в закрытом списке
                if (hnp::is_in_vector(m_closedList, neighbour)) {
                    continue;
                }

                // Если сосед блокирует путь
                if (neighbour->m_type == eTileType::e_Wall) {
                    // добавляем в закрытый список
                    m_closedList.push_back(neighbour);
                    // продолжаем с начала цикла For
                    continue;
                }

                // Обновляем узел
                neighbour->m_cameFromNode = currentNode;
                neighbour->m_gCost = currentNode->m_gCost + CalculateDistanceCost(currentNode, neighbour);

                // Вычисляем новую стоимость H для узла
                neighbour->m_hCost = CalculateDistanceCost(neighbour, endNode);
                // neighbour.CalculateFCost();
                neighbour->m_fCost = neighbour->m_gCost + neighbour->m_hCost;

                // если открытый список не содержит соседа, добавляем его
                if (hnp::is_in_vector(m_openList, neighbour)) {
                    // проверяем, является ли потенциальная стоимость меньше стоимости узла
                    if (currentNode->m_gCost > neighbour->m_gCost) {
                        continue;
                    }
                }

                m_openList.push_back(neighbour);
            }
        }
    }

    /**
 * @brief Обновляет поиск пути в зависимости от текущего состояния призрака.
 */
    void UpdatePathFinding() {
        switch (m_state) {
            case eGhostState::e_Chase:
                ChaseModePathFinding();
                break;
            case eGhostState::e_Scatter:
                AStarPathFinding(m_position, cnp::k_cornerPositions[static_cast<int>(m_type)]);
                break;
            case eGhostState::e_Frightened:
                AStarPathFinding(m_position, cnp::k_homePositions[static_cast<int>(m_type)]);
                break;
            default:;
        }
    }

    /**
 * @brief Выполняет поиск пути в режиме преследования.
 */
    void ChaseModePathFinding() {
        const int pacManIndexX = hnp::world_coord_to_array_index(m_pacMan.GetPosition().x);
        const int pacManIndexY = hnp::world_coord_to_array_index(m_pacMan.GetPosition().y);

        switch (m_type) {
            case eGhostType::e_Blinky:
                // Если индексы X и Y допустимы в массиве
                if (hnp::is_in_range(pacManIndexX, 0, static_cast<int>(m_grid[0].size() - 1)) &&
                    hnp::is_in_range(pacManIndexY, 0, static_cast<int>(m_grid.size() - 1))) {
                    // Преследуем Pacman и пытаемся оказаться за ним
                    switch (m_pacMan.GetDirection()) {
                        case eDirection::e_None:
                            AStarPathFinding(m_position, m_pacMan.GetPosition());
                            break;
                        case eDirection::e_Up:
                            MoveTowardsBlockBelow(pacManIndexX, pacManIndexY);
                            break;
                        case eDirection::e_Down:
                            MoveTowardsBlockAbove(pacManIndexX, pacManIndexY);
                            break;
                        case eDirection::e_Left:
                            MoveTowardsBlockRight(pacManIndexX, pacManIndexY);
                            break;
                        case eDirection::e_Right:
                            MoveTowardsBlockLeft(pacManIndexX, pacManIndexY);
                            break;
                    }
                } else {
                    // если не допустимы, перемещаемся в свой угол
                    AStarPathFinding(
                            m_position,
                            cnp::k_cornerPositions[static_cast<int>(m_type)]
                    );
                }
                break;
            case eGhostType::e_Pinky:
                if (hnp::is_in_range(pacManIndexX, 0, static_cast<int>(m_grid[0].size() - 1)) &&
                    hnp::is_in_range(pacManIndexY, 0, static_cast<int>(m_grid.size() - 1))) {
                    // Преследуем Pacman и пытаемся оказаться перед ним
                    switch (m_pacMan.GetDirection()) {
                        case eDirection::e_None:
                            AStarPathFinding(m_position, m_pacMan.GetPosition());
                            break;
                        case eDirection::e_Up:
                            MoveTowardsBlockAbove(pacManIndexX, pacManIndexY);
                            break;
                        case eDirection::e_Down:
                            MoveTowardsBlockBelow(pacManIndexX, pacManIndexY);
                            break;
                        case eDirection::e_Left:
                            MoveTowardsBlockLeft(pacManIndexX, pacManIndexY);
                            break;
                        case eDirection::e_Right:
                            MoveTowardsBlockRight(pacManIndexX, pacManIndexY);
                            break;
                    }
                } else {
                    // если не допустимы, перемещаемся в свой угол
                    AStarPathFinding(
                            m_position,
                            cnp::k_cornerPositions[static_cast<int>(m_type)]
                    );
                }
                break;

            case eGhostType::e_Inky:
                // Патрулируем область
                if (m_path.empty()) {
                    m_currentCorner++;

                    if (m_currentCorner > 3) m_currentCorner = 0;

                    AStarPathFinding(m_position, cnp::k_cornerPositions[m_currentCorner]);
                }

                break;
            case eGhostType::e_Clyde:
                // Перемещаемся в случайную позицию
                if (m_path.empty()) {
                    Tile &randomTile = m_grid[1][1];
                    do {
                        const sf::Vector2i random(hnp::rand_range(25, 750), hnp::rand_range(50, 725));
                        randomTile = m_grid[hnp::world_coord_to_array_index(random.y)][hnp::world_coord_to_array_index(
                                random.y)];
                    } while (randomTile.m_type != eTileType::e_Path);
                    AStarPathFinding(m_position, randomTile.m_position);
                }

                break;
            default:;
        }
    }


    /**
  * @brief Выполняет поиск пути к блоку над PacMan.
  * @param pacManIndexX Индекс X позиции PacMan в массиве.
  * @param pacManIndexY Индекс Y позиции PacMan в массиве.
  */
    void MoveTowardsBlockAbove(int pacManIndexX, int pacManIndexY) {
        if (m_position.y - m_pacMan.GetPosition().y < 2 * cnp::k_gridCellSize &&
            m_position.x == m_pacMan.GetPosition().x) {
            AStarPathFinding(m_position, m_pacMan.GetPosition());
        } else {
            // Проверяем, является ли блок над PacMan допустимым
            if (hnp::is_in_range(pacManIndexY - 1, 0, static_cast<int>(m_grid.size() - 1))) {
                if (m_grid[pacManIndexY - 1][pacManIndexX].m_type == eTileType::e_Path) {
                    AStarPathFinding(m_position, m_grid[pacManIndexY - 1][pacManIndexX].m_position);
                }
            } else {
                AStarPathFinding(m_position, m_pacMan.GetPosition());
            }
        }
    }

    /**
   * @brief Выполняет поиск пути к блоку под PacMan.
   * @param pacManIndexX Индекс X позиции PacMan в массиве.
   * @param pacManIndexY Индекс Y позиции PacMan в массиве.
   */
    void MoveTowardsBlockBelow(int pacManIndexX, int pacManIndexY) {
        // Если расстояние короткое, двигаемся к нему
        if (m_pacMan.GetPosition().y - m_position.y < 2 * cnp::k_gridCellSize &&
            m_position.x == m_pacMan.GetPosition().x) {
            AStarPathFinding(m_position, m_pacMan.GetPosition());
        } else {
            // Проверяем, является ли блок под PacMan допустимым
            if (hnp::is_in_range(pacManIndexY + 1, 0, static_cast<int>(m_grid.size() - 1))) {
                if (m_grid[pacManIndexY + 1][pacManIndexX].m_type == eTileType::e_Path) {
                    AStarPathFinding(m_position, m_grid[pacManIndexY + 1][pacManIndexX].m_position);
                }
            } else {
                AStarPathFinding(m_position, m_pacMan.GetPosition());
            }
        }
    }

/**
 * @brief Выполняет поиск пути к блоку слева от PacMan.
 * @param pacManIndexX Индекс X позиции PacMan в массиве.
 * @param pacManIndexY Индекс Y позиции PacMan в массиве.
 */
    void MoveTowardsBlockLeft(int pacManIndexX, int pacManIndexY) {
        // Если расстояние короткое, двигаемся к нему
        if (m_position.x - m_pacMan.GetPosition().x < 2 * cnp::k_gridCellSize &&
            m_position.y == m_pacMan.GetPosition().y) {
            AStarPathFinding(m_position, m_pacMan.GetPosition());
        } else {
            // Проверяем, является ли блок слева от PacMan допустимым
            if (hnp::is_in_range(pacManIndexX - 1, 0, static_cast<int>(m_grid.size() - 1))) {
                if (m_grid[pacManIndexY][pacManIndexX - 1].m_type == eTileType::e_Path) {
                    AStarPathFinding(m_position, m_grid[pacManIndexY][pacManIndexX - 1].m_position);
                }
            } else {
                AStarPathFinding(m_position, m_pacMan.GetPosition());
            }
        }
    }


    /**
 * @brief Выполняет поиск пути к блоку справа от PacMan.
 * @param pacManIndexX Индекс X позиции PacMan в массиве.
 * @param pacManIndexY Индекс Y позиции PacMan в массиве.
 */
    void MoveTowardsBlockRight(int pacManIndexX, int pacManIndexY) {
        // Если расстояние короткое, двигаемся к нему
        if (m_pacMan.GetPosition().x - m_position.x < 2 * cnp::k_gridCellSize &&
            m_position.y == m_pacMan.GetPosition().y) {
            AStarPathFinding(m_position, m_pacMan.GetPosition());
        } else {
            // Проверяем, является ли блок справа от PacMan допустимым
            if (hnp::is_in_range(pacManIndexX + 1, 0, static_cast<int>(m_grid.size() - 1))) {
                if (m_grid[pacManIndexY][pacManIndexX + 1].m_type == eTileType::e_Path) {
                    AStarPathFinding(m_position, m_grid[pacManIndexY][pacManIndexX + 1].m_position);
                }
            } else {
                AStarPathFinding(m_position, m_pacMan.GetPosition());
            }
        }
    }


    /**
 * @brief Выполняет перемещение призрака.
 */
    void Move() {
        m_updateTicks++;
        if (m_updateTicks == 10 || m_path.empty()) {
            UpdatePathFinding();
            m_updateTicks = 0;
        }

        // Извлекаем первый элемент пути
        if (!m_path.empty()) {
            Tile *destination = m_path.top();
            m_path.pop();

            m_position = destination->m_position;
        }
    }

/**
 * @brief Проверяет столкновения с PacMan.
 */
    void CheckPacManCollisions() {
        if (m_state != eGhostState::e_Frightened) {
            if (m_position == m_pacMan.GetPosition()) {
                if (m_pacMan.GetPacManState() == ePacManState::e_PowerUp) {
                    m_state = eGhostState::e_Frightened;
                    m_pacMan.AddPoints(1000);
                } else {
                    m_pacMan.SetIsAlive(false);
                }
                std::cout << "I hit PacMan!" << std::endl;
            }
        }
    }

};
