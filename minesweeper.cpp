#include "minesweeper.h"

bool Exists(const size_t x, const size_t y, const size_t i, const size_t j, const size_t height, const size_t width) {
    return !(x == 0 && y == 0) && !(x == -1 && i == 0) && !(x == 1 && i == height - 1) && !(y == -1 && j == 0) &&
           !(y == 1 && j == width - 1);
}

void Minesweeper::FindNumberOfNear(std::vector<std::vector<int>>& field, const size_t width, const size_t height) {
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            int count_mines = 0;
            for (int x = -1; x <= 1; ++x) {
                for (int y = -1; y <= 1; ++y) {
                    if (Exists(x, y, i, j, height, width) && field[i + x][j + y] == CellType::MINE) {
                        ++count_mines;
                    }
                }
            }
            if (field[i][j] != CellType::MINE) {
                field[i][j] = count_mines;
            }
        }
    }
}

void Minesweeper::PrepareNewField(const size_t width, const size_t height) {
    field_start_.clear();
    field_in_progress_.clear();
    was_in_queue_.clear();
    std::vector<std::vector<int>> fill_flag(height, std::vector<int>(width, CellType::CLOSED));
    field_in_progress_ = fill_flag;
    status_ = GameStatus::NOT_STARTED;
    start_game_ = time(nullptr);
    end_game_ = time(nullptr);
    width_ = width;
    height_ = height;
    std::vector<std::vector<int>> games(height, std::vector<int>(width, 0));
    field_start_ = games;
}

void Minesweeper::CheckIfVictory() {
    bool all_opened = true;
    for (size_t i = 0; i < height_; ++i) {
        for (size_t j = 0; j < width_; ++j) {
            if ((field_in_progress_[i][j] == CellType::CLOSED || field_in_progress_[i][j] == CellType::FLAG) &&
                field_start_[i][j] != CellType::MINE) {
                all_opened = false;
            }
        }
    }
    if (all_opened && status_ == GameStatus::IN_PROGRESS) {
        status_ = GameStatus::VICTORY;
        end_game_ = time(NULL);
    }
}

Minesweeper::Minesweeper(size_t width, size_t height, size_t mines_count) {
    NewGame(width, height, mines_count);
}

Minesweeper::Minesweeper(size_t width, size_t height, const std::vector<Cell>& cells_with_mines) {
    NewGame(width, height, cells_with_mines);
}

void Minesweeper::NewGame(size_t width, size_t height, size_t mines_count) {
    std::vector<Cell> cells;
    int height_int = static_cast<int>(height);
    int width_int = static_cast<int>(width);
    for (int i = 0; i < height_int; ++i) {
        Cell cell_add;
        for (int j = 0; j < width_int; ++j) {
            cell_add.y = i;
            cell_add.x = j;
            cells.push_back(cell_add);
        }
    }
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(cells.begin(), cells.end(), g);
    cells.resize(mines_count);
    NewGame(width, height, cells);
}

void Minesweeper::NewGame(size_t width, size_t height, const std::vector<Cell>& cells_with_mines) {
    PrepareNewField(width, height);
    for (const auto& cell_el : cells_with_mines) {
        field_start_[cell_el.y][cell_el.x] = CellType::MINE;
    }
    FindNumberOfNear(field_start_, width, height);
}

void Minesweeper::OpenCell(const Cell& cell) {
    bool game_is_over;
    std::queue<Cell> recursion_queue;
    recursion_queue.push(cell);
    was_in_queue_.insert(std::make_pair(cell.y, cell.x));
    while (!recursion_queue.empty()) {
        Cell current_cell = recursion_queue.front();
        switch (status_) {
            case GameStatus::VICTORY:
                game_is_over = true;
                break;
            case GameStatus::DEFEAT:
                game_is_over = true;
                break;
            case GameStatus::NOT_STARTED:
                status_ = GameStatus::IN_PROGRESS;
                start_game_ = time(NULL);
            case GameStatus::IN_PROGRESS:
                if (field_in_progress_[current_cell.y][current_cell.x] == CellType::FLAG) {
                    recursion_queue.pop();
                } else {
                    if (field_start_[current_cell.y][current_cell.x] == CellType::MINE) {
                        field_in_progress_ = field_start_;
                        status_ = GameStatus::DEFEAT;
                        end_game_ = time(NULL);
                        game_is_over = true;
                    } else {
                        bool mine_is_near = false;
                        for (int x = -1; x <= 1; ++x) {
                            for (int y = -1; y <= 1; ++y) {
                                if (Exists(x, y, current_cell.y, current_cell.x, height_, width_) &&
                                    field_start_[current_cell.y + x][current_cell.x + y] == CellType::MINE) {
                                    mine_is_near = true;
                                }
                            }
                        }
                        if (mine_is_near) {
                            field_in_progress_[current_cell.y][current_cell.x] =
                                field_start_[current_cell.y][current_cell.x];
                            recursion_queue.pop();
                        } else {
                            field_in_progress_[current_cell.y][current_cell.x] =
                                field_start_[current_cell.y][current_cell.x];
                            recursion_queue.pop();
                            for (int x = -1; x <= 1; ++x) {
                                for (int y = -1; y <= 1; ++y) {
                                    if (Exists(x, y, current_cell.y, current_cell.x, height_, width_) &&
                                        field_start_[current_cell.y + x][current_cell.x + y] != CellType::FLAG) {
                                        if (was_in_queue_.find(std::make_pair(
                                                current_cell.x + y, current_cell.y + x)) == was_in_queue_.end()) {
                                            Cell to_add;
                                            to_add.x = current_cell.x + y;
                                            to_add.y = current_cell.y + x;
                                            recursion_queue.push(to_add);
                                            was_in_queue_.insert(
                                                std::make_pair(current_cell.x + y, current_cell.y + x));
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                break;
        }
        if (game_is_over) {
            break;
        }
    }
    if (status_ != GameStatus::VICTORY && status_ != GameStatus::DEFEAT) {
        CheckIfVictory();
    }
}
void Minesweeper::MarkCell(const Cell& cell) {
    if (status_ == GameStatus::NOT_STARTED) {
        status_ = GameStatus::IN_PROGRESS;
        start_game_ = time(NULL);
    }
    if (status_ == GameStatus::IN_PROGRESS) {
        if (field_in_progress_[cell.y][cell.x] == CellType::FLAG) {
            field_in_progress_[cell.y][cell.x] = field_start_[cell.y][cell.x];
        } else {
            if (was_in_queue_.find(std::make_pair(cell.y, cell.x)) == was_in_queue_.end()) {
                field_in_progress_[cell.y][cell.x] = CellType::FLAG;
            }
        }
    }
    if (status_ != GameStatus::VICTORY && status_ != GameStatus::DEFEAT) {
        CheckIfVictory();
    }
}

Minesweeper::GameStatus Minesweeper::GetGameStatus() const {
    return status_;
}

time_t Minesweeper::GetGameTime() const {
    return end_game_ - start_game_;
}

Minesweeper::RenderedField Minesweeper::RenderField() const {
    RenderedField answer;
    std::string to_add;
    for (const auto& line : field_in_progress_) {
        to_add.clear();
        for (const auto& el : line) {
            if (el == 0) {
                to_add += ".";
            } else if (el >= 1 && el <= 8) {
                to_add += std::to_string(el);
            } else if (el == CellType::MINE) {
                to_add += "*";
            } else if (el == CellType::FLAG) {
                to_add += "?";
            } else if (el == CellType::CLOSED) {
                to_add += "-";
            }
        }
        answer.push_back(to_add);
    }
    return answer;
}