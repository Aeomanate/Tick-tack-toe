#include <iostream>
#include <vector>
#include <string>
#include <numeric>
#include <functional>
#include <conio.h>
#include <windows.h>

class Field {
  public:
    struct Pos {
        int row, col;
    };
  
  public:
    Field(size_t field_size, size_t abreast_count)
        : cells(field_size, std::vector<char>(field_size, ' '))
        , cur_field_pos { int(field_size/2), int(field_size/2) }
        , field_size(field_size)
        , abreast_count(abreast_count)
    {}
    
    void moveCursor(Field::Pos move_dir) {
        cur_field_pos.row += move_dir.row;
        cur_field_pos.col += move_dir.col;
        
        int fs = int(field_size);
        if(cur_field_pos.row < 0)   cur_field_pos.row = fs - 1;
        if(cur_field_pos.row >= fs) cur_field_pos.row = 0;
        if(cur_field_pos.col < 0)   cur_field_pos.col = fs - 1;
        if(cur_field_pos.col >= fs) cur_field_pos.col = 0;
    }
    void toggleWhoMove() {
        crosses_turn = !crosses_turn;
    }
    bool makeMove() {
        bool is_available_turn = isAvailableTurn();
        if(is_available_turn) {
            getCurrentCell() = whoTurn();
        }
        
        return is_available_turn;
    }
    
    // Info functions
    size_t fieldSize() const {
        return field_size;
    }
    char getCell(Pos pos) const {
        return char(((Field*)(this))->getCell(pos));
    }
    char whoTurn() const {
        return crosses_turn ? 'X' : 'O';
    }
    Pos currentCursorPos() const {
        return cur_field_pos;
    }
    size_t abreastCount() const {
        return abreast_count;
    }
  
  private:
    // Technical functions
    bool isAvailableTurn() {
        return getCurrentCell() == ' ';
    }
    char& getCurrentCell() {
        return getCell(currentCursorPos());
    }
    char& getCell(Pos pos) {
        return cells[size_t(pos.row)][size_t(pos.col)];
    }
  
  private:
    std::vector<std::vector<char>> cells;
    Pos cur_field_pos;
    size_t const field_size;
    size_t abreast_count;
    bool crosses_turn = false;
};

class WinChecker {
  public:
    WinChecker(Field const& field)
        : field(field)
        , cur_player(field.whoTurn())
    { }
    
    bool isWin() {
        return
            check_main_diagonal() ||
            check_side_diagonal() ||
            check_horizontal() ||
            check_vertical();
    }
  
  private:
    // Technical functions
    bool isCurPlayer() {
        return field.getCell(seek) == field.whoTurn();
    }
    bool isInfield() {
        return
            seek.row >= 0 && seek.col >= 0 &&
            seek.row < int(field.fieldSize()) &&
            seek.col < int(field.fieldSize());
    }
    bool isContinueCheck() {
        return isInfield() && isCurPlayer();
    }
    bool mainCheck(std::function<void()> burhan, std::function<void()> nastya) {
        size_t counter = 1;
        
        // First check direction
        seek = field.currentCursorPos();
        while(true) {
            burhan();
            if(isContinueCheck()) {
                ++counter;
            } else {
                break;
            }
        }
        
        // second check_direction
        seek = field.currentCursorPos();
        while(true) {
            nastya();
            if(isContinueCheck()) {
                ++counter;
            } else {
                break;
            }
        }
        
        return counter >= field.abreastCount();
    }
    
    // Check functions
    bool check_main_diagonal() {
        return mainCheck(
            [&] { --seek.row; --seek.col; },
            [&] { ++seek.row; ++seek.col; }
        );
    }
    bool check_side_diagonal() {
        return mainCheck(
            [&] { --seek.row; ++seek.col; },
            [&] { ++seek.row; --seek.col; }
        );
    }
    bool check_horizontal() {
        return mainCheck(
            [&] { --seek.col; },
            [&] { ++seek.col; }
        );
    }
    bool check_vertical() {
        return mainCheck(
            [&] { --seek.row; },
            [&] { ++seek.row; }
        );
    }
  
  private:
    Field const& field;
    Field::Pos seek;
    char cur_player; // X O
};

class Drawer {
  public:
    Drawer(size_t field_size)
        : field_size(field_size)
        , field_screen_size {
            short(field_size * 4 + 1), // X
            short(field_size * 2)      // Y
        }
    { }
    
    void printHint(std::string hint, int y_offset) {
        short x = short(field_screen_size.X - int(hint.size()));
        if(x > 0) {
            x /= 2;
        }
        
        short y = short(field_screen_size.Y + 1 + y_offset);
        
        setCursor({ x, y });
        std::cout << hint;
        
        if(y_offset != 0) {
            waitForAnyKey();
            setCursor({ x, y });
            std::cout << std::string(unsigned(field_screen_size.X), ' ');
        }
    }
    void setCursorToField(Field field) {
        setCursor(convertFieldToScreen(field.currentCursorPos()));
    }
    void draw(Field field) {
        setCursor({0, 0});
        std::cout << drawFrame();
        drawFieldContent(field);
        printHint(field.whoTurn() + std::string(" are walking now"), +0);
    }
  
  private:
    // Technical  functions
    COORD convertFieldToScreen(Field::Pos pos) {
        int x = 2 + pos.col * 4;
        int y = 1 + pos.row * 2;
        return COORD { static_cast<short>(x), static_cast<short>(y) };
    }
    void setCursor(COORD coord) {
        SetConsoleCursorPosition(console_handle, coord);
    }
    void waitForAnyKey() {
        if(_getch() == 224) _getch();
    }
    
    // Draw functions
    std::string make4Elements(std::string element) {
        std::string r = "";
        for(size_t i = 0; i != field_size; ++i) {
            r += element;
        }
        r.pop_back();
        return r;
    }
    std::string drawFrame() {
        std::vector<std::string> r;
        
        /*
         ÚÄÄÄÂÄÄÄÂÄÄÄ¿
         ³ F ³ F ³ F ³
         ÃÄÄÄÅÄÄÄÅÄÄÄ´
         ³ F ³ F ³ F ³
         ÃÄÄÄÅÄÄÄÅÄÄÄ´
         ³ F ³ F ³ F ³
         ÀÄÄÄÁÄÄÄÁÄÄÄÙ
         */
        
        r.push_back("Ú" + make4Elements("ÄÄÄÂ") + "¿\n");
        
        for(size_t i = 0; i != field_size; ++i) {
            r.push_back("³" + make4Elements("   ³") + "³\n");
            r.push_back("Ã" + make4Elements("ÄÄÄÅ") + "´\n");
        }
        r.pop_back();
        r.push_back("À" + make4Elements("ÄÄÄÁ") + "Ù\n");
        
        return std::accumulate(r.begin(), r.end(), std::string());
    }
    void drawFieldContent(Field const& field) {
        for(size_t row = 0; row != field_size; ++row) {
            for(size_t col = 0; col != field_size; ++col) {
                Field::Pos pos = {int(row), int(col)};
                setCursor(convertFieldToScreen(pos));
                std::cout << field.getCell(pos);
            }
        }
    }
  
  private:
    size_t field_size;
    COORD const field_screen_size;
    HANDLE const console_handle = GetStdHandle(STD_OUTPUT_HANDLE);
};

class Game {
  public:
    Game(size_t field_size, size_t abreast_count)
        : field(field_size, abreast_count)
        , drawer(field_size)
        , win_checker(field)
    { }
    
    void run() {
        while(is_game_work) {
            drawer.draw(field);
            handleUserInput();
        }
        
        drawer.draw(field);
        drawer.printHint(field.whoTurn() + std::string(" ARE WIN!"), +1);
    }
  
  
  private:
    bool handleArrows(int arrow) {
        enum Arrows { UP = 72, LEFT = 75, RIGHT = 77, DOWN = 80 };
        switch(arrow) {                           // ROW  COL
            case UP   : field.moveCursor(Field::Pos { -1, 0 }); break;
            case DOWN : field.moveCursor(Field::Pos { +1, 0 }); break;
            case LEFT : field.moveCursor(Field::Pos { 0, -1 }); break;
            case RIGHT: field.moveCursor(Field::Pos { 0, +1 }); break;
            default:
                return false;
        }
        return true;
    }
    
    void handleUserInput() {
        drawer.setCursorToField(field);
        
        int first_input = _getch();
        bool is_special = first_input == 224;
        bool is_space = !is_special && first_input == 32;
        
        bool is_handle_success = (is_special && handleArrows(_getch())) || is_space;
        if(!is_handle_success) {
            drawer.printHint("Press arrows or space", +1);
        }
        
        if(is_space) {
            if(field.makeMove()) {
                is_game_work = !win_checker.isWin();
                if(is_game_work) {
                    field.toggleWhoMove();
                }
            } else {
                drawer.printHint("Impossible to make this move", +1);
            }
        }
    }
  
  private:
    bool is_game_work = true;
    Field field;
    Drawer drawer;
    WinChecker win_checker;
};

int main() {
    Game(11, 5).run();
}