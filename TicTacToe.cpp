#include <iostream>
#include <windows.h>
#include <ctime>
#include <cstdlib>
#include <conio.h>
#include <cstring>
using namespace std;

// ANSI Color Codes
#define COLOR_RESET "\033[0m"
#define COLOR_PLAYER1 "\033[93m"  // Yellow
#define COLOR_PLAYER2 "\033[91m"  // Red
#define COLOR_BOARD "\033[96m"    // Cyan
#define COLOR_TEXT "\033[97m"     // White
#define COLOR_MENU "\033[95m"     // Purple
#define COLOR_WIN "\033[92m"      // Green
#define COLOR_RULES "\033[94m"    // Blue for rules
#define COLOR_CREDIT "\033[103m\033[30m" // Yellow background with black text

// Game constants
#define MAX_BOARD_SIZE 7
#define MAX_NAME_LENGTH 50
#define PLAYER1_SYMBOL 'X'
#define PLAYER2_SYMBOL 'O'

// Game state
enum GameState { PLAYING, PLAYER1_WON, PLAYER2_WON, DRAW };
enum Difficulty { EASY, MEDIUM, HARD };
enum GridSize { SMALL = 3, MEDIUM_SIZE = 5, LARGE = 7 };

struct HighScores {
    char player1Name[MAX_NAME_LENGTH];
    char player2Name[MAX_NAME_LENGTH];
    int player1Wins;
    int player2Wins;
    int draws;
    GridSize lastGridSize;
};

struct GameSettings {
    Difficulty computerDifficulty;
    bool soundsEnabled;
    bool colorEnabled;
    GridSize gridSize;
};

// Safe string copy function
void safeStringCopy(char* dest, const char* src, size_t destSize) {
    size_t i;
    for (i = 0; i < destSize - 1 && src[i] != '\0'; i++) {
        dest[i] = src[i];
    }
    dest[i] = '\0';
}

// Utility functions
void clearScreen() {
    system("cls");
}

void drawBoxedMenu(const char* title, const char* options[], int numOptions) {
    int maxLength = strlen(title);
    for (int i = 0; i < numOptions; i++) {
        if (strlen(options[i]) > maxLength) {
            maxLength = strlen(options[i]);
        }
    }

    cout << COLOR_RULES << " ";
    for (int i = 0; i < maxLength + 5; i++) cout << "+";
    cout << " \n";

    cout << "|     " << COLOR_MENU << title << COLOR_RULES;
    for (int i = 0; i < maxLength - strlen(title) ; i++) cout << " ";
    cout << "|\n";

    for (int i = 0; i < numOptions; i++) {
        cout << "| " << COLOR_BOARD << (i + 1) << ". " << options[i];
        for (int j = 0; j < maxLength - strlen(options[i]) + 1; j++) cout << " ";
        cout << COLOR_RULES << "|\n";
    }

    cout << " ";
    for (int i = 0; i < maxLength + 5; i++) cout << "+";
    cout << " \n" << COLOR_RESET;
}

void pause(const char* message = "\nPress backSpace key to back menu...") {
    cout << COLOR_PLAYER2 << message<< COLOR_RESET;
    while(_getch()!='\b');
}

void playSound(int frequency, int duration) {
    Beep(frequency, duration);
}

// Board functions
void initializeBoard(char grid[MAX_BOARD_SIZE][MAX_BOARD_SIZE], int size) {
    char num = '1';
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (i * size + j < 9) {
                grid[i][j] = num++;
            }
            else {
                // For larger boards, use letters after 9
                grid[i][j] = 'A' + (i * size + j - 9);
            }
        }
    }
}

void displayBoard(const char grid[MAX_BOARD_SIZE][MAX_BOARD_SIZE], int size, bool useColor = true) {
    if (useColor) cout << COLOR_BOARD;

    // Draw top border
    cout << " ";
    for (int j = 0; j < size; j++) cout << "____";
    cout << endl;

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (useColor) {
                if (grid[i][j] == PLAYER1_SYMBOL) cout << COLOR_PLAYER1;
                else if (grid[i][j] == PLAYER2_SYMBOL) cout << COLOR_PLAYER2;
            }

            cout << "| " << grid[i][j] << " ";

            if (useColor) cout << COLOR_BOARD;
        }
        cout << "|\n ";

        // Draw horizontal line
        for (int j = 0; j < size; j++) cout << "----";
        cout << endl;
    }
    cout << COLOR_RESET;
}

// Game logic
bool checkWin(const char grid[MAX_BOARD_SIZE][MAX_BOARD_SIZE], int size, char symbol) {
    int requiredToWin = (size == 3) ? 3 : (size == 5) ? 4 : 5;

    // Check rows
    for (int i = 0; i < size; i++) {
        for (int j = 0; j <= size - requiredToWin; j++) {
            bool win = true;
            for (int k = 0; k < requiredToWin; k++) {
                if (grid[i][j + k] != symbol) {
                    win = false;
                    break;
                }
            }
            if (win) return true;
        }
    }

    // Check columns
    for (int j = 0; j < size; j++) {
        for (int i = 0; i <= size - requiredToWin; i++) {
            bool win = true;
            for (int k = 0; k < requiredToWin; k++) {
                if (grid[i + k][j] != symbol) {
                    win = false;
                    break;
                }
            }
            if (win) return true;
        }
    }

    // Check diagonals (top-left to bottom-right)
    for (int i = 0; i <= size - requiredToWin; i++) {
        for (int j = 0; j <= size - requiredToWin; j++) {
            bool win = true;
            for (int k = 0; k < requiredToWin; k++) {
                if (grid[i + k][j + k] != symbol) {
                    win = false;
                    break;
                }
            }
            if (win) return true;
        }
    }

    // Check anti-diagonals (top-right to bottom-left)
    for (int i = 0; i <= size - requiredToWin; i++) {
        for (int j = requiredToWin - 1; j < size; j++) {
            bool win = true;
            for (int k = 0; k < requiredToWin; k++) {
                if (grid[i + k][j - k] != symbol) {
                    win = false;
                    break;
                }
            }
            if (win) return true;
        }
    }

    return false;
}

bool isBoardFull(const char grid[MAX_BOARD_SIZE][MAX_BOARD_SIZE], int size) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (grid[i][j] != PLAYER1_SYMBOL && grid[i][j] != PLAYER2_SYMBOL) {
                return false;
            }
        }
    }
    return true;
}

// Input functions
int getMenuChoice(int min, int max) {
    int choice;
    while (true) {
        choice = _getch() - '0';
        if (choice >= min && choice <= max) return choice;
        cout << "\a"; // Beep for invalid input
    }
}

int getPlayerMove(const char grid[MAX_BOARD_SIZE][MAX_BOARD_SIZE], int size, const char* playerName, char symbol) {
    int move;
    while (true) {
        cout << COLOR_TEXT << playerName << "'s turn ("
            << (symbol == PLAYER1_SYMBOL ? COLOR_PLAYER1 : COLOR_PLAYER2)
            << symbol << COLOR_TEXT << "): ";
        cout << COLOR_PLAYER2 << "\nPress Esc key to exit game.....\n"<< COLOR_RESET;
        int input = _getch();
        if (input == 27) {
            return input;
        }
        if (input >= '1' && input <= '9') {
            move = input - '0';
        }
        else if (input >= 'A' && input <= 'Z') {
            move = 10 + (input - 'A'); // Convert A-Z to 10-35
        }
        else if (input >= 'a' && input <= 'z') {
            move = 10 + (input - 'a'); // Convert a-z to 10-35
        }
        else {
            cout << "\a"; // Beep for invalid input
            continue;
        }


        cout << (char)input << endl; // Show the entered character

        if (move < 1 || move > size * size) {
            cout << "Please enter a valid position (1-" << size * size << ")." << endl;
            continue;
        }

        int row = (move - 1) / size;
        int col = (move - 1) % size;

        if (grid[row][col] == PLAYER1_SYMBOL || grid[row][col] == PLAYER2_SYMBOL) {
            cout << "That position is already taken!" << endl;
            continue;
        }

        return move;
    }
}

// Computer AI
int getRandomMove(const char grid[MAX_BOARD_SIZE][MAX_BOARD_SIZE], int size) {
    int available[MAX_BOARD_SIZE * MAX_BOARD_SIZE];
    int count = 0;
    for (int i = 0; i < size * size; i++) {
        int row = i / size;
        int col = i % size;
        if (grid[row][col] != PLAYER1_SYMBOL && grid[row][col] != PLAYER2_SYMBOL) {
            available[count++] = i + 1;
        }
    }
    return count > 0 ? available[rand() % count] : -1;
}

int getSmartMove(const char grid[MAX_BOARD_SIZE][MAX_BOARD_SIZE], int size, char computerSymbol, char playerSymbol) {
    // Check if computer can win
    for (int i = 0; i < size * size; i++) {
        int row = i / size;
        int col = i % size;
        if (grid[row][col] != PLAYER1_SYMBOL && grid[row][col] != PLAYER2_SYMBOL) {
            char tempGrid[MAX_BOARD_SIZE][MAX_BOARD_SIZE];
            memcpy(tempGrid, grid, sizeof(tempGrid));
            tempGrid[row][col] = computerSymbol;
            if (checkWin(tempGrid, size, computerSymbol)) {
                return i + 1;
            }
        }
    }

    // Block player if they can win
    for (int i = 0; i < size * size; i++) {
        int row = i / size;
        int col = i % size;
        if (grid[row][col] != PLAYER1_SYMBOL && grid[row][col] != PLAYER2_SYMBOL) {
            char tempGrid[MAX_BOARD_SIZE][MAX_BOARD_SIZE];
            memcpy(tempGrid, grid, sizeof(tempGrid));
            tempGrid[row][col] = playerSymbol;
            if (checkWin(tempGrid, size, playerSymbol)) {
                return i + 1;
            }
        }
    }

    // Choose center if available (for odd-sized boards)
    if (size % 2 == 1) {
        int center = size / 2;
        if (grid[center][center] != PLAYER1_SYMBOL && grid[center][center] != PLAYER2_SYMBOL) {
            return center * size + center + 1;
        }
    }

    // Choose a corner
    int corners[] = { 1, size, size * size - size + 1, size * size };
    for (int i = 0; i < 4; i++) {
        int row = (corners[i] - 1) / size;
        int col = (corners[i] - 1) % size;
        if (grid[row][col] != PLAYER1_SYMBOL && grid[row][col] != PLAYER2_SYMBOL) {
            return corners[i];
        }
    }

    return getRandomMove(grid, size);
}

int getComputerMove(const char grid[MAX_BOARD_SIZE][MAX_BOARD_SIZE], int size, char computerSymbol, char playerSymbol, Difficulty difficulty) {
    switch (difficulty) {
    case EASY: return getRandomMove(grid, size);
    case MEDIUM: return (rand() % 2) ? getSmartMove(grid, size, computerSymbol, playerSymbol) : getRandomMove(grid, size);
    case HARD: return getSmartMove(grid, size, computerSymbol, playerSymbol);
    }
    return -1;
}

// Game modes
void playPVP(HighScores& scores, const GameSettings& settings) {
    char grid[MAX_BOARD_SIZE][MAX_BOARD_SIZE];
    GameState gameState = PLAYING;
    int currentPlayer = 1;
    int size = settings.gridSize;

    clearScreen();
    const char* title = " Player vs Player ";
    const char* options[] = { "Enter Player 1 name", "Enter Player 2 name" };
    drawBoxedMenu(title, options, 2);

    cin.ignore();
    cout << COLOR_TEXT << "\n1. Enter Player 1 name: ";
    cin.getline(scores.player1Name, MAX_NAME_LENGTH);

    cout << "2. Enter Player 2 name: ";
    cin.getline(scores.player2Name, MAX_NAME_LENGTH);

    initializeBoard(grid, size);

    while (gameState == PLAYING) {
        clearScreen();
        cout << "Playing on " << size << "x" << size << " board\n";
        displayBoard(grid, size, settings.colorEnabled);

        int move;
        if (currentPlayer == 1) {
            move = getPlayerMove(grid, size, scores.player1Name, PLAYER1_SYMBOL);
            if (move == 27) {
                return;
            }
            if (settings.soundsEnabled) playSound(523, 100);
        }
        else {
            move = getPlayerMove(grid, size, scores.player2Name, PLAYER2_SYMBOL);
            if (move == 27) {
                return;
            }
            if (settings.soundsEnabled) playSound(523, 100);

        }

        int row = (move - 1) / size;
        int col = (move - 1) % size;

        if (currentPlayer == 1) {
            grid[row][col] = PLAYER1_SYMBOL;
            if (checkWin(grid, size, PLAYER1_SYMBOL)) {
                gameState = PLAYER1_WON;
                if (settings.soundsEnabled) playSound(784, 200);
            }
        }
        else {
            grid[row][col] = PLAYER2_SYMBOL;
            if (checkWin(grid, size, PLAYER2_SYMBOL)) {
                gameState = PLAYER2_WON;
                if (settings.soundsEnabled) playSound(784, 200);
            }
        }

        if (isBoardFull(grid, size) && gameState == PLAYING) {
            gameState = DRAW;
            if (settings.soundsEnabled) playSound(220, 300);
        }

        currentPlayer = (currentPlayer == 1) ? 2 : 1;
    }

    clearScreen();
    cout << "Final board (" << size << "x" << size << "):\n";
    displayBoard(grid, size, settings.colorEnabled);

    if (gameState == PLAYER1_WON) {
        cout << COLOR_WIN << "\nCongratulations " << scores.player1Name << "! You won!" << COLOR_RESET << endl;
        scores.player1Wins++;
    }
    else if (gameState == PLAYER2_WON) {
        cout << COLOR_WIN << "\nCongratulations " << scores.player2Name << "! You won!" << COLOR_RESET << endl;
        scores.player2Wins++;
    }
    else {
        cout << COLOR_TEXT << "\nThe game is a draw!" << COLOR_RESET << endl;
        scores.draws++;
    }

    scores.lastGridSize = settings.gridSize;
    pause();
}

void playPVC(HighScores& scores, const GameSettings& settings) {
    char grid[MAX_BOARD_SIZE][MAX_BOARD_SIZE];
    GameState gameState = PLAYING;
    int currentPlayer = 1;
    int size = settings.gridSize;

    clearScreen();
    const char* title = " Player vs Computer ";
    const char* options[] = { "Enter your name" };
    drawBoxedMenu(title, options, 1);

    cin.ignore();
    cout << COLOR_TEXT << "\n1. Enter your name: ";
    cin.getline(scores.player1Name, MAX_NAME_LENGTH);
    safeStringCopy(scores.player2Name, "Computer", MAX_NAME_LENGTH);

    initializeBoard(grid, size);

    while (gameState == PLAYING) {
        clearScreen();
        cout << "Playing on " << size << "x" << size << " board\n";
        displayBoard(grid, size, settings.colorEnabled);

        int move;
        if (currentPlayer == 1) {
            move = getPlayerMove(grid, size, scores.player1Name, PLAYER1_SYMBOL);
            if (move == 27) {
                return;
            }
            if (settings.soundsEnabled) playSound(523, 100);
        }
        else {
            move = getComputerMove(grid, size, PLAYER2_SYMBOL, PLAYER1_SYMBOL, settings.computerDifficulty);
            cout << COLOR_TEXT << "Computer chooses: " << move << endl;
            if (settings.soundsEnabled) playSound(523, 100); // Move sound
            Sleep(1000); // Pause so player can see computer's move
        }

        int row = (move - 1) / size;
        int col = (move - 1) % size;

        if (currentPlayer == 1) {
            grid[row][col] = PLAYER1_SYMBOL;
            if (checkWin(grid, size, PLAYER1_SYMBOL)) {
                gameState = PLAYER1_WON;
                if (settings.soundsEnabled) playSound(784, 200); // Win sound
            }
        }
        else {
            grid[row][col] = PLAYER2_SYMBOL;
            if (checkWin(grid, size, PLAYER2_SYMBOL)) {
                gameState = PLAYER2_WON;
                if (settings.soundsEnabled) playSound(784, 200);
            }
        }

        if (isBoardFull(grid, size) && gameState == PLAYING) {
            gameState = DRAW;
            if (settings.soundsEnabled) playSound(220, 300); // Draw sound
        }

        currentPlayer = (currentPlayer == 1) ? 2 : 1;
    }

    clearScreen();
    cout << "Final board (" << size << "x" << size << "):\n";
    displayBoard(grid, size, settings.colorEnabled);

    if (gameState == PLAYER1_WON) {
        cout << COLOR_WIN << "\nCongratulations " << scores.player1Name << "! You won!" << COLOR_RESET << endl;
        scores.player1Wins++;
    }
    else if (gameState == PLAYER2_WON) {
        cout << COLOR_TEXT << "\nComputer wins! Better luck next time!" << COLOR_RESET << endl;
        scores.player2Wins++;
    }
    else {
        cout << COLOR_TEXT << "\nThe game is a draw!" << COLOR_RESET << endl;
        scores.draws++;
    }

    scores.lastGridSize = settings.gridSize;
    pause();
}

// Menu functions
void showRules() {
    clearScreen();
    const char* title = " Game Rules ";
    const char* rules[] = {
        "1. Play on a grid (3x3, 5x5, or 7x7)",
        "2. Player 1 is X, Player 2 is O",
        "3. Take turns marking spaces",
        "4. First with a full row/column/diagonal wins",
        "5. Full board with no winner = draw",
        "6. Press number/letter keys to make moves",
        "7. 3x3:3 in a row,5x5:4 in a row,7x7:5 in a row"
    };

    cout << COLOR_RULES << " ";
    for (int i = 0; i < 50; i++) cout << "+";
    cout << " \n";

    cout << "| " << title;
    for (int i = 0; i < 50 - strlen(title) - 2; i++) cout << " ";
    cout << "|\n";

    for (int i = 0; i < 7; i++) {
        cout << "| " << COLOR_BOARD << rules[i];
        for (int j = 0; j < 50 - strlen(rules[i]) - 2; j++) cout << " ";
        cout << COLOR_RULES << "|\n";
    }

    cout << " ";
    for (int i = 0; i < 50; i++) cout << "+";
    cout << " \n" << COLOR_RESET;

    pause();
}

void showHighScores(const HighScores& scores) {
    clearScreen();
    const char* title = " High Scores ";
    cout << COLOR_MENU << " ";
    for (int i = 0; i < 40; i++) cout << "=";
    cout << " \n";

    cout << "| " << title;
    for (int i = 0; i < 40 - strlen(title) - 2; i++) cout << " ";
    cout << "|\n";

    cout << "| " << COLOR_TEXT << "Player 1: " << scores.player1Name;
    for (int i = 0; i < 40 - strlen(scores.player1Name) - 12; i++) cout << " ";
    cout << COLOR_MENU << "|\n";

    cout << "| " << COLOR_TEXT << "Wins: " << scores.player1Wins;
    for (int i = 0; i < 40 - 8; i++) cout << " ";
    cout << COLOR_MENU << "|\n";

    cout << "| " << COLOR_TEXT << "Player 2: " << scores.player2Name;
    for (int i = 0; i < 40 - strlen(scores.player2Name) - 12; i++) cout << " ";
    cout << COLOR_MENU << "|\n";

    cout << "| " << COLOR_TEXT << "Wins: " << scores.player2Wins;
    for (int i = 0; i < 40 - 8; i++) cout << " ";
    cout << COLOR_MENU << "|\n";

    cout << "| " << COLOR_TEXT << "Draws: " << scores.draws;
    for (int i = 0; i < 40 - 9; i++) cout << " ";
    cout << COLOR_MENU << "|\n";

    cout << "| " << COLOR_TEXT << "Last Grid Size: " << scores.lastGridSize << "x" << scores.lastGridSize;
    for (int i = 0; i < 40 - 20; i++) cout << " ";
    cout << COLOR_MENU << "|\n";

    cout << " ";
    for (int i = 0; i < 40; i++) cout << "=";
    cout << " \n" << COLOR_RESET;

    pause();
}

void showSettings(GameSettings& settings) {
    int choice;
    do {
        clearScreen();
        const char* title = " Game Settings ";
        const char* options[] = {
            "Computer Difficulty",
            "Sound Effects",
            "Color Display",
            "Grid Size (3x3, 5x5, 7x7)",
            "Back to Main Menu"
        };

        drawBoxedMenu(title, options, 5);

        cout << COLOR_TEXT << "\nCurrent Settings:\n";
        cout << "1. Difficulty: "
            << (settings.computerDifficulty == EASY ? "Easy" :
                settings.computerDifficulty == MEDIUM ? "Medium" : "Hard") << endl;
        cout << "2. Sounds: " << (settings.soundsEnabled ? "ON" : "OFF") << endl;
        cout << "3. Colors: " << (settings.colorEnabled ? "ON" : "OFF") << endl;
        cout << "4. Grid Size: " << settings.gridSize << "x" << settings.gridSize << endl;
        cout << "5. Back\n";
        cout << "\nSelect option : ";

        choice = getMenuChoice(1, 5);

        switch (choice) {
        case 1:
            settings.computerDifficulty = static_cast<Difficulty>((settings.computerDifficulty + 1) % 3);
            break;
        case 2:
            settings.soundsEnabled = !settings.soundsEnabled;
            break;
        case 3:
            settings.colorEnabled = !settings.colorEnabled;
            break;
        case 4:
            // Cycle through only 3x3, 5x5, 7x7
            settings.gridSize = static_cast<GridSize>((settings.gridSize == SMALL) ? MEDIUM_SIZE :
                (settings.gridSize == MEDIUM_SIZE) ? LARGE : SMALL);
            break;
        }
    } while (choice != 5);
}

void showCredits() {
    clearScreen();
    cout << COLOR_RULES << " __________________________ \n";
    cout << "|                          |\n";
    cout << "|       "<< COLOR_MENU <<"TIC-TAC-TOE       "<< COLOR_RULES <<" |\n";
    cout << "|                          |\n";
    cout << "|    "<< COLOR_BOARD <<"Created by :  "<< COLOR_RULES <<"        | \n";
    cout << "|    "<< COLOR_BOARD <<"Malik Kamran Ali     " << COLOR_RULES << " |\n";
    cout << "|                          |\n";
    cout << "|__________________________|\n" << COLOR_RESET;
    pause();
}

// Main game loop
int main() {
    srand(static_cast<unsigned>(time(0)));
    HighScores scores = { "", "", 0, 0, 0, SMALL };
    GameSettings settings = { MEDIUM, true, true, SMALL };

    while (true) {
        clearScreen();
        const char* title = " TIC-TAC-TOE ";
        const char* options[] = {
            "Player vs Player",
            "Player vs Computer",
            "Game Rules",
            "High Scores",
            "Settings",
            "Credits",
            "Exit"
        };

        drawBoxedMenu(title, options, 7);
        cout << COLOR_TEXT << "\nSelect option : ";

        int choice = getMenuChoice(1, 7);

        switch (choice) {
        case 1: playPVP(scores, settings); break;
        case 2: playPVC(scores, settings); break;
        case 3: showRules(); break;
        case 4: showHighScores(scores); break;
        case 5: showSettings(settings); break;
        case 6: showCredits(); break;
        case 7:
            cout << COLOR_TEXT << "\nThanks for playing! Goodbye!" << COLOR_RESET << endl;
            return 0;
        }
    }

    return 0;
}