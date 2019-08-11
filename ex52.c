/************************
* Student name: Daniel Kaganovich
* Student: 211939293
* Course Exercise Group: 08
* Exercise name: ex52
************************/ 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>
#include <signal.h>

#define STDIN 0
#define STDOUT 1
#define STDERR 2

#define ERROR_MSG "Error in system call\n"

#define UP 'w'
#define DOWN 's'
#define LEFT 'a'
#define RIGHT 'd'
#define QUIT 'q'

#define BOARD_SIZE 20

#define HORIZENTAL 0
#define VERTICAL 1

//int checkShapeBounds();
void deleteShape();
void resetShape();
void initializeGame();
void printGame();
void moveShape(int posChanged);
void changePose();

typedef struct GameEnvironment{
    char mainBoard[BOARD_SIZE][BOARD_SIZE];
    int i;
    int j;
    int state;
}GameEnvironment;

GameEnvironment game;

/******************
* Function Name: printError
* Function Operation: print the error msg
******************/
void printError() {
    write(STDERR, ERROR_MSG, sizeof(ERROR_MSG));
}

/******************
* Function Name: deleteShape
* Function Operation: delete the shape
******************/
void deleteShape() {
    game.mainBoard[game.i][game.j] = ' ';
    
    if (game.state == HORIZENTAL) {
        game.mainBoard[game.i][game.j + 1] = ' ';
        game.mainBoard[game.i][game.j - 1] = ' ';
        
    } else {
        game.mainBoard[game.i + 1][game.j] = ' ';
        game.mainBoard[game.i - 1][game.j] = ' ';
    }
}

/******************
* Function Name: resetShape
* Function Operation: reset the shape
******************/
void resetShape() {
    deleteShape();
    game.i = 1;
    game.j = 10;
}

/******************
* Function Name: initializeGame
* Function Operation: initialize the game
******************/
void initializeGame() {
    int i,j;
    // create board
    for (j = 0 ; j < BOARD_SIZE; j++) {
        game.mainBoard[BOARD_SIZE - 1][j] = '*';
    }
    for (i = 0; i < BOARD_SIZE - 1; i++) {
        for (j = 0; j < BOARD_SIZE; j++) {
            if ((j==0) || (j == BOARD_SIZE -1)){
                game.mainBoard[i][j] = '*';
            } else {
                game.mainBoard[i][j] = ' ';
            }
        }
    }
    game.state = HORIZENTAL;
    game.i = 0;
    game.j = BOARD_SIZE / 2;
    
    game.mainBoard[game.i][game.j] = '-';
    game.mainBoard[game.i][game.j+1] = '-';
    game.mainBoard[game.i][game.j-1] = '-';
}

/******************
* Function Name: printGame
* Function Operation: print the board
******************/
void printGame() {
    system("clear");
    int i,j ;
    for (i = 0; i < BOARD_SIZE; i++) {
        for (j = 0; j < BOARD_SIZE; j++) {
            char str[2] = {'\0'};
            str[0] = game.mainBoard[i][j];
            write(STDOUT, str, sizeof(str));
        }
        write(STDOUT, "\n", sizeof("\n"));
    }
}

/******************
* Function Name: changePose
* Function Operation: change the pose of the shape
******************/
void changePose() {
    int currI = game.i;
    int currJ = game.j;
    deleteShape();
    if (game.state == VERTICAL) {
        if (game.j > 1 && game.j < BOARD_SIZE - 1 - 1) {
            game.state = !game.state;
        }
    } else {
        game.state = !game.state;
    }
    moveShape(1);
}

/******************
* Function Name: moveShape
* Input: int (the posChanged)
* Function Operation: move the shape (0 is to go down, 1 is to change pos, 2 for others)
******************/
void moveShape(int posChanged) {
    if (!posChanged) {
        deleteShape();
        game.i = game.i + 1;
    } else if (posChanged == 2) {
        deleteShape();
    }
    if(game.state == VERTICAL) {
        if (game.j >= BOARD_SIZE - 1 || game.j <= 0) {
            return;
        }
        //put the shape in new location in gameEnvironment.
        game.mainBoard[game.i][game.j] = '-';
        game.mainBoard[game.i + 1][game.j] = '-';
        game.mainBoard[game.i - 1][game.j] = '-';
        if (game.i + 1 >= BOARD_SIZE - 1) {
            initializeGame();
        }
    } else if(game.state == HORIZENTAL){
        if (game.j + 1 >= BOARD_SIZE || game.j <= 1) {
            return;
        }
        game.mainBoard[game.i][game.j] = '-';
        game.mainBoard[game.i][game.j + 1] = '-';
        game.mainBoard[game.i][game.j - 1] = '-';
        if (game.i >= BOARD_SIZE -1) {
            initializeGame();
        }
    }
}


/******************
* Function Name: doMove
* Input: int (the signal)
* Function Operation: handle the signal from the process
******************/
void doMove(int sig) {
    signal(SIGUSR2, doMove);
    //if we're not out the bounds
    char c;
    //= (char)getchar();
    read(STDIN, &c, 1);
    if (c == QUIT) {
        exit(1);
    }
    
    // check the char
    switch (c) {
        case QUIT:  // if 'q'
            exit(1);
            return;
            break;
            
        case RIGHT:  // if 'd'
            deleteShape();
            // if in the range
            if (!(game.j + 1 >= BOARD_SIZE - 1 && game.state == VERTICAL || game.j + 2 >= BOARD_SIZE - 1 && game.state == HORIZENTAL)) {
                game.j = game.j + 1;
            }
            moveShape(2);
            break;
            
        case LEFT:  // if 'a'
            deleteShape();
            // if in the range
            if (!(game.j <= 1 && game.state == VERTICAL || game.j <= 2 && game.state == HORIZENTAL)) {
                game.j = game.j - 1;
            }
            moveShape(2);
            break;
            
        case DOWN:  // if 's'
            deleteShape();
            // set the shape go down
            game.i = game.i + 1;
            moveShape(2);
            break;
            
        case UP:  // if 'w'
            // change the pose
            changePose();
            break;
            
        default:
            //deleteShape();
            moveShape(2);
    }
    printGame();
}

/******************
* Function Name: moveOneStep
* Input: int (the signal)
* Function Operation: handle the SIGALRM
******************/
void moveOneStep (int sig) {
    signal(SIGALRM, moveOneStep);
    alarm(1);
    moveShape(0);
    printGame();
    alarm(1);
}

/******************
* Function Name: main
* Function Operation: run the draw board
******************/
int main() {
    initializeGame();
    printGame();
    alarm(1);
    signal(SIGALRM, moveOneStep);
    signal(SIGUSR2, doMove);
    while (1) {
        pause();
    }
    return 0;
}

