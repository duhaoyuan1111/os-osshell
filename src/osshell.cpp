#include <iostream>
#include <cstdlib>
#include <string>
#include <cstring>
#include <sstream>
#include <vector>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <stdio.h>

void allocateArrayOfCharArrays(char ***array_ptr, size_t array_length, size_t item_size);
void freeArrayOfCharArrays(char **array, size_t array_length);
void splitString(std::string text, char d, char **result);

int main (int argc, char **argv)
{
    // Get list of paths to binary executables
    // `os_path_list` supports up to 16 directories in PATH, 
    //     each with a directory name length of up to 64 characters
    char **os_path_list;
    allocateArrayOfCharArrays(&os_path_list, 16, 64);
    char* os_path = getenv("PATH");
    splitString(os_path, ':', os_path_list);
    char path_array[16][64];
    
    // Welcome message
    printf("Welcome to OSShell! Please enter your commands ('exit' to quit).\n");

    // Allocate space for input command lists
    // `command_list` supports up to 32 command line parameters, 
    //     each with a parameter string length of up to 128 characters
    char **command_list;
    allocateArrayOfCharArrays(&command_list, 32, 128);

    // Repeat:
    while (true){
        std::cout << "osshell> ";
        char user_input_char[128];
        std::cin.getline(user_input_char, 128);
        
        if (strcmp(user_input_char,"exit")==0){
            exit(-1);
        } else if (strcmp(user_input_char,"history")==0){
            printf("History!\n");
        } else if (user_input_char[0] == '.' || user_input_char[0] == '/'){
            printf("Start with a . or /!\n");
            
        } else if (user_input_char[0] == '\n'){

        } else {
            // The <os_path_list> prepared by professor has flaws.
            // Each path has several '\t' padded, so I create a new list
            // The new list has "path"+"/"+"user_command" style.
            for (int i=0; i< 16; i++){
                memset(path_array[i],'\0',sizeof(path_array[i]));
            }
            int flag = 0;
            char tail[128];
            for (int b=0;b<strlen(tail);b++){
                memset(tail,'\0',sizeof(tail));
            }
            for (int j=0;j<strlen(user_input_char);j++){
                if (user_input_char[j] != ' ' && flag == 0){
                    tail[j] = user_input_char[j];
                } else {
                    flag = 1;
                }
            }
            for (int i=0;i<sizeof(os_path_list);i++){
                int count = 0;
                for (int k=0;k<strlen(os_path_list[i]);k++){
                    if (os_path_list[i][k] != '\t'){
                        count++;
                        path_array[i][k] = os_path_list[i][k];
                    }
                }
                path_array[i][count] = '/';
                for (int h=1;h<strlen(tail)+1;h++){
                    
                    path_array[i][count+h] = tail[h-1];
                }
                
            }
            // To find if a path is valid and can be executed
            int counter = 0;
            while (counter<8){// if ==8, no valid command found
                if (access(path_array[counter],X_OK)==0){
                    break;
                }
                counter++;
            }
            // prepare the 2nd param for execv()
            char **cmd;
            std::string user_input_string = user_input_char;
            allocateArrayOfCharArrays(&cmd,16,64);
            
            splitString(user_input_string,' ',cmd);
            
            if (counter < 8){ // found command, execv!
                int pid;
                pid = fork();
                if (pid == 0) { // child
                    execv(path_array[counter],cmd);
                } else {// parent
                    int status;
                    waitpid(pid, &status, 0);
                }
               
            }
            

        }
        







    }
    //  Print prompt for user input: "osshell> " (no newline)
    //  Get user input for next command
    //  If command is `exit` exit loop / quit program
    //  If command is `history` print previous N commands
    //  For all other commands, check if an executable by that name is in one of the PATH directories
    //   If yes, execute it
    //   If no, print error statement: "<command_name>: Error command not found" (do include newline)

    // Free allocated memory
    freeArrayOfCharArrays(os_path_list, 16);
    freeArrayOfCharArrays(command_list, 32);

    return 0;
}

/*
   array_ptr: pointer to list of strings to be allocated
   array_length: number of strings to allocate space for in the list
   item_size: length of each string to allocate space for
*/
void allocateArrayOfCharArrays(char ***array_ptr, size_t array_length, size_t item_size)
{
    int i;
    *array_ptr = new char*[array_length];
    for (i = 0; i < array_length; i++)
    {
        (*array_ptr)[i] = new char[item_size];
    }
}

/*
   array: list of strings to be freed
   array_length: number of strings in the list to free
*/
void freeArrayOfCharArrays(char **array, size_t array_length)
{
    int i;
    for (i = 0; i < array_length; i++)
    {
        delete[] array[i];
    }
    delete[] array;
}

/*
   text: string to split
   d: character delimiter to split `text` on
   result: NULL terminated list of strings (char **) - result will be stored here
*/
void splitString(std::string text, char d, char **result)
{
    int i;
    std::vector<std::string> list;
    std::stringstream ss(text);
    std::string token;
    
    while (std::getline(ss, token, d))
    {
        list.push_back(token);
    }

    for (i = 0; i < list.size(); i++)
    {
        
        strcpy(result[i], list[i].c_str());
        
    }
    result[list.size()] = NULL;
}
