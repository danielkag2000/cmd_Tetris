/************************
* Student name: Daniel Kaganovich
* Student: 211939293
* Course Exercise Group: 08
* Exercise name: ex51
************************/ 
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
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

#define RUN_FILE "./draw.out"

/******************
* Function Name: printError
* Function Operation: print the error msg
******************/
void printError() {
    write(STDERR, ERROR_MSG, sizeof(ERROR_MSG));
}

/******************
* Function Name: getch
* Function Output: the char from the stdin
* Function Operation: get the char
******************/
char getch() {
    char buf = 0;
    struct termios old = {0};
    if (tcgetattr(0, &old) < 0)
        perror("tcsetattr()");
    old.c_lflag &= ~ICANON;
    old.c_lflag &= ~ECHO;
    old.c_cc[VMIN] = 1;
    old.c_cc[VTIME] = 0;
    if (tcsetattr(0, TCSANOW, &old) < 0)
        perror("tcsetattr ICANON");
    if (read(0, &buf, 1) < 0)
        perror ("read()");
    old.c_lflag |= ICANON;
    old.c_lflag |= ECHO;
    if (tcsetattr(0, TCSADRAIN, &old) < 0)
        perror ("tcsetattr ~ICANON");
    return (buf);
}

/******************
* Function Name: main
* Function Operation: create the process and run the game
******************/
int main() {
    
    pid_t  pid;
    char input = 0;
    
    // define the file descriptors
    int fd[2];
    // do pipe
    pipe(fd);
    
    // do forking
    pid = fork();
    if(pid == 0) {  // child code
        // define the args
        char* args[] = {RUN_FILE, NULL};
        
        // change the stdin to the fd[0]
        dup2(fd[0], STDIN);
        // run the program
        execvp(args[0], args);
        // error happand
        printError();
        exit(-1);
        
    } else if (pid > 0) {  // parent code
        while (input != QUIT) {
            input = getch();  // get input
            // if one of the define inputs
            if (input == QUIT || input == DOWN || input == LEFT || input == RIGHT || input == UP ) {
                // write to the file descriptor
                write(fd[1], &input, 1);
                // send a SIGUSR2 signal
                kill(pid,SIGUSR2);
            }
        }
        return 0;
    }
    // fork error
    printError();
    return -1;
}