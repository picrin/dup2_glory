//Plan for action
// 1. understand various differences between exec, subprocess and popen in glibc (for linux)
// 2. write something like
// fork
// check pid
// duplicate file descriptors
// run 
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>

bool write_message(const char *message, int message_length, int file_descriptor){
    unsigned int total_written = 0;
    while(total_written < message_length){
        ssize_t written_this_round = write(file_descriptor, (const char *) &message[total_written], sizeof(char) * (message_length - total_written));
        if (written_this_round == -1){
            perror(NULL);
            exit(1);
            return false;
        }
        total_written += written_this_round;
    }
    return true;
}

int main(){
    int file_descriptor = open("/home/picrin/ssiesz", O_WRONLY|O_CREAT);
    if (file_descriptor == -1){
        //fprintf(stderr, "error while opening ssiesz. Error no: %d", errno);
        perror(NULL);
    }
  
    const char child_message[] = "I'm a child";
    const int message_length = sizeof(child_message)/sizeof(char);
    char *copied_message = (char *) malloc(message_length * sizeof(char));
    strcpy(copied_message, child_message);
    
    pid_t is_parent = fork();
    if (is_parent == false){
        // we are the child
        /********** THAT LINE IS THE GLORY OF DUP2 **********/
        int new_stdout = dup2(file_descriptor, STDOUT_FILENO);
        /********** THAT LINE IS THE END OF GLORY OF DUP2 **********/
        perror("call to dup2");
        write_message(child_message, message_length, STDOUT_FILENO);
        perror("call to write");
    } else if (is_parent == -1){
        perror("dupa");
        // we failed to fork. TODO consider corner cases
        exit(1);
    } else{
        // we are the parent
        write_message("I'm a parent", 12, STDOUT_FILENO);
    } 
    exit(0);
}
