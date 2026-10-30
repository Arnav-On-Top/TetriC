#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>
#include <time.h>
#include <string.h>

#define ROWS 20
#define COLS 10

int board[ROWS][COLS];

int shapes[7][4][4] = {
    {
        {0,0,0,0},
        {1,1,1,1},
        {0,0,0,0},
        {0,0,0,0}
    },
    {
        {1,1,0,0},
        {1,1,0,0},
        {0,0,0,0},
        {0,0,0,0}
    },
    {
        {0,1,0,0},
        {1,1,1,0},
        {0,0,0,0},
        {0,0,0,0}
    },
    {
        {1,0,0,0},
        {1,1,1,0},
        {0,0,0,0},
        {0,0,0,0}
    },
    {
        {0,0,1,0},
        {1,1,1,0},
        {0,0,0,0},
        {0,0,0,0}
    },
    {
        {0,1,1,0},
        {1,1,0,0},
        {0,0,0,0},
        {0,0,0,0}
    },
    {
        {1,1,0,0},
        {0,1,1,0},
        {0,0,0,0},
        {0,0,0,0}
    }
};

int active[4][4];

int piece, x, y, score, gameOver;

char *difficultyNames[] = {
    "Easy", "Medium", "Hard"
};

int speeds[] = {800, 400, 180};

int currentLevel = 0;

int usernameExists(char username[])
{
    FILE *file = fopen("users.txt", "r");

    if (file == NULL)
        return 0;

    char savedUser[50];
    char savedPass[50];

    while (fscanf(file, "%49s %49s",
                  savedUser, savedPass) == 2)
    {
        if (strcmp(username, savedUser) == 0)
        {
            fclose(file);
            return 1;
        }
    }

    fclose(file);
    return 0;
}

void signup()
{
    char username[50];
    char password[50];

    printf("\n SignUp \n");

    while (1)
    {
        printf("choose a username w/o space: ");
        scanf("%49s", username);

        if (usernameExists(username))
        {
            printf("username exists!\n");
        }
        else
        {
            break;
        }
    }

    printf("choose password w/o space: ");
    scanf("%49s", password);

    FILE *file = fopen("users.txt", "a");

    if (file == NULL)
    {
        printf("Could not save account!\n");
        exit(1);
    }

    fprintf(file, "%s %s\n", username, password);
    fclose(file);

    printf("\n Account created successfully! \n");
    printf("now please log in. \n");
}

int login(char username[])
{
    char enteredUser[50];
    char enteredPass[50];
    char savedUser[50];
    char savedPass[50];

    FILE *file;

    while (1)
    {
        printf("\nLogin\n");
        printf("Username: ");
        scanf("%49s", enteredUser);

        printf("Password: ");
        scanf("%49s", enteredPass);

        file = fopen("users.txt", "r");

        if (file == NULL)
        {
            printf("No accounts found. Sign up first.\n");
            return 0;
        }

        while (fscanf(file, "%49s %49s",
                      savedUser, savedPass) == 2)
        {
            if (strcmp(enteredUser, savedUser) == 0 &&
                strcmp(enteredPass, savedPass) == 0)
            {
                strcpy(username, enteredUser);
                fclose(file);
                printf("\nLogin successful\n");
                return 1;
            }
        }

        fclose(file);
        printf("Wrong username or password. try again. \n");
    }
}

int getHighScore(char username[], int level)
{
    FILE *file = fopen("scores.txt", "r");

    if (file == NULL)
        return 0;

    char savedUser[50];
    int savedLevel;
    int savedScore;

    while (fscanf(file, "%49s %d %d",
                  savedUser, &savedLevel, &savedScore) == 3)
    {
        if (strcmp(username, savedUser) == 0 &&
            savedLevel == level)
        {
            fclose(file);
            return savedScore;
        }
    }

    fclose(file);
    return 0;
}

void saveHighScore(char username[], int level, int newScore)
{
    int oldScore = getHighScore(username, level);

    if (newScore <= oldScore)
        return;

    FILE *file = fopen("scores.txt", "a");

    if (file == NULL)
    {
        printf("Could not save high score!\n");
        return;
    }

    fclose(file);

    FILE *input = fopen("scores.txt", "r");
    FILE *output = fopen("scores_temp.txt", "w");

    if (output == NULL)
    {
        if (input != NULL)
            fclose(input);

        return;
    }

    char savedUser[50];
    int savedLevel;
    int savedScore;
    int updated = 0;

    if (input != NULL)
    {
        while (fscanf(input, "%49s %d %d",
                      savedUser, &savedLevel, &savedScore) == 3)
        {
            if (strcmp(username, savedUser) == 0 &&
                savedLevel == level)
            {
                if (updated == 0)
                {
                    fprintf(output, "%s %d %d\n",
                            username, level, newScore);
                    updated = 1;
                }
            }
            else
            {
                fprintf(output, "%s %d %d\n",
                        savedUser, savedLevel, savedScore);
            }
        }

        fclose(input);
    }

    if (updated == 0)
    {
        fprintf(output, "%s %d %d\n",
                username, level, newScore);
    }

    fclose(output);

    remove("scores.txt");
    rename("scores_temp.txt", "scores.txt");

    printf("\nNew High Score\n");
}

void showHighScores(char username[])
{
    printf("\nYour High Scores\n");

    for (int i = 0; i < 3; i++)
    {
        printf("%-8s:%d\n",
               difficultyNames[i],
               getHighScore(username, i));
    }

    printf("Canvas:\n");
}

void draw()
{
    system("cls");

    printf("Tetris\n");
    printf("Difficulty:%s\n", difficultyNames[currentLevel]);
    printf("Score: %d\n\n", score);

    for (int i = 0; i < ROWS; i++)
    {
        printf("|");

        for (int j = 0; j < COLS; j++)
        {
            int filled = board[i][j];

            for (int a = 0; a < 4; a++)
            {
                for (int b = 0; b < 4; b++)
                {
                    if (active[a][b] == 1 &&
                        i == y + a && j == x + b)
                    {
                        filled = 1;
                    }
                }
            }

            if (filled == 1)
                printf("[]");
            else
                printf(".");
        }

        printf("|\n");
    }

    printf("Controls:\n");
    printf("A/D=Move | W=Rotate\n");
    printf("S=Drop | Q=Quit\n");
}

int canMove(int newX, int newY)
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (active[i][j] == 1)
            {
                int boardX = newX + j;
                int boardY = newY + i;

                if (boardX < 0 || boardX >= COLS)
                    return 0;

                if (boardY >= ROWS)
                    return 0;

                if (boardY >= 0 && board[boardY][boardX] == 1)
                    return 0;
            }
        }
    }

    return 1;
}

void lockPiece()
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (active[i][j] == 1)
            {
                if (y + i >= 0)
                {
                    board[y + i][x + j] = 1;
                }
            }
        }
    }
}

void clearLines()
{
    for (int i = ROWS - 1; i >= 0; i--)
    {
        int full = 1;

        for (int j = 0; j < COLS; j++)
        {
            if (board[i][j] == 0)
                full = 0;
        }

        if (full == 1)
        {
            score += 100;

            for (int a = i; a > 0; a--)
            {
                for (int b = 0; b < COLS; b++)
                {
                    board[a][b] = board[a - 1][b];
                }
            }

            for (int b = 0; b < COLS; b++)
                board[0][b] = 0;

            i++;
        }
    }
}

void newPiece()
{
    piece = rand() % 7;

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            active[i][j] = shapes[piece][i][j];
        }
    }

    x = 3;
    y = 0;

    if (canMove(x, y) == 0)
        gameOver = 1;
}

void rotatePiece()
{
    int temp[4][4];
    int oldShape[4][4];

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            oldShape[i][j] = active[i][j];
        }
    }

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            temp[j][3 - i] = active[i][j];
        }
    }

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            active[i][j] = temp[i][j];
        }
    }

    if (canMove(x, y) == 0)
    {
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                active[i][j] = oldShape[i][j];
            }
        }
    }
}
int playGame()
{
    for (int i = 0; i < ROWS; i++)
    {
        for (int j = 0; j < COLS; j++)
        {
            board[i][j] = 0;
        }
    }
    score = 0;
    gameOver = 0;
    int counter = 0;
    newPiece();
    while (gameOver == 0)
    {
        draw();
        if (_kbhit())
        {
            char key = _getch();
            if (key == 'a' || key == 'A')
            {
                if (canMove(x - 1, y))
                    x--;
            }
            if (key == 'd' || key == 'D')
            {
                if (canMove(x + 1, y))
                    x++;
            }
            if (key == 's' || key == 'S')
            {
                if (canMove(x, y + 1))
                {
                    y++;
                    score++;
                }
                else
                {
                    lockPiece();
                    clearLines();
                    newPiece();
                }
            }
            if (key == 'w' || key == 'W')
            {
                rotatePiece();
            }
            if (key == 'q' || key == 'Q')
            {
                gameOver = 1;
                break;
            }
        }
        Sleep(50);
        counter += 50;
        if (counter >= speeds[currentLevel])
        {
            counter = 0;
            if (canMove(x, y + 1))
            {
                y++;
            }
            else
            {
                lockPiece();
                clearLines();
                newPiece();
            }
        }
    }
    draw();
    printf("\nGame Over\n");
    printf("Final score: %d\n", score);
    return score;
}
int chooseDifficulty()
{
    int choice;
    while (1)
    {
        system("cls");
        printf("Welcome To TetriC\n\n");
        printf("1. Easy: Slow Block\n");
        printf("2. Medium: Normal Speed\n");
        printf("3. Hard: Fast Blocks\n");
        printf("\nSelect difficulty(1-3): ");
        if (scanf("%d", &choice) == 1 &&
            choice >= 1 && choice <= 3)
        {
            return choice - 1;
        }
        printf("Invalid choice!\n");
        while (getchar() != '\n')
        {
        }
        Sleep(1000);
    }
}
int main()
{
    srand((unsigned int)time(NULL));
    char username[50] = "";
    int choice;
    int playAgain = 1;
    while (playAgain == 1)
    {
        system("cls");
        printf("Tetris Game \n");
        printf("1. Login\n");
        printf("2. Sign Up\n");
        printf("3. Exit\n");
        printf("Choose: ");
        if (scanf("%d", &choice) != 1)
        {
            while (getchar() != '\n')
            {
            }
            continue;
        }
        if (choice == 3)
        {
            break;
        }
        if (choice == 2)
        {
            signup();
            if (login(username) == 0)
                continue;
        }
        else if (choice == 1)
        {
            if (login(username) == 0)
                continue;
        }
        else
        {
            printf("Invalid choice!\n");
            Sleep(1000);
            continue;
        }
        printf("\n Welcome, %s!\n", username);
        Sleep(1000);
        currentLevel = chooseDifficulty();
        int finalScore = playGame();
        saveHighScore(username, currentLevel, finalScore);
        printf("\n Play another game?\n");
        printf("1. Yes (return to login)\n");
        printf("2. No(show scores and exit)\n");
        printf("Choose:");
        if (scanf("%d", &choice) != 1)
        {
            while (getchar() != '\n')
            {
            }
            choice = 2;
        }
        if (choice == 1)
        {
            playAgain = 1;
        }
        else
        {
            playAgain = 0;
        }
    }
    if (username[0] != '\0')
    {
        showHighScores(username);
    }
    printf("\n Thanks for playing\n");
    system("pause");
    return 0;
}