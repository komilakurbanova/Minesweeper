#pragma once

#include <string>
#include <vector>
#include <tuple>
#include <queue>
#include <set>
#include <algorithm>
#include <random>

class Minesweeper {
private:
    void FindNumberOfNear(std::vector<std::vector<int>>& field, const size_t width, const size_t height);
    void PrepareNewField(const size_t width, const size_t height);
    void CheckIfVictory();

public:
    enum CellType {
        CLOSED = 11,
        FLAG = 10,
        MINE = 9,
    };

    struct Cell {
        size_t x = 0;
        size_t y = 0;
    };

    enum class GameStatus {
        NOT_STARTED,
        IN_PROGRESS,
        VICTORY,
        DEFEAT,
    };

    using RenderedField = std::vector<std::string>;

    Minesweeper(size_t width, size_t height, size_t mines_count);
    Minesweeper(size_t width, size_t height, const std::vector<Cell>& cells_with_mines);

    void NewGame(size_t width, size_t height, size_t mines_count);
    void NewGame(size_t width, size_t height, const std::vector<Cell>& cells_with_mines);

    void OpenCell(const Cell& cell);
    void MarkCell(const Cell& cell);

    GameStatus GetGameStatus() const;
    time_t GetGameTime() const;

    RenderedField RenderField() const;

private:
    size_t height_;
    size_t width_;
    std::vector<std::vector<int>> field_start_;
    std::vector<std::vector<int>> field_in_progress_;
    GameStatus status_;
    time_t start_game_;
    time_t end_game_;
    std::set<std::pair<size_t, size_t>> was_in_queue_;
};
