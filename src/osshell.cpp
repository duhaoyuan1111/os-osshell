#include <iostream>
#include <cstdlib>
#include <string>
#include <cstring>
#include <sstream>
#include <vector>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/wait.h>

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


    // Example code for how to loop over NULL terminated list of strings
    int i = 0;
    while (os_path_list[i] != NULL)
    {
        printf("PATH[%2d]: %s\n", i, os_path_list[i]);
        i++;
    }

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
        char *user_input_char;
        std::string user_input;
        std::cin.getline(user_input_char, 128);
        user_input = user_input_char;

        //std::cout << user_input << "!!\n";

        if (user_input == "exit"){
            exit(-1);
        } else if (user_input == "history"){
            printf("History!\n");
        } else if (user_input[0] == '.' || user_input[0] == '/'){
            printf("Start with a . or /!\n");
            
        } else {
            
            // run system command
            char *arguments[128];
            char *execvFirstParamList[128];
            
            char slash = {'/'};
            char *slash_ptr = &slash;
            int j = 0;
            
            //std::cout << user_input << "\n";

            splitString(user_input,' ',arguments);
            
            while(os_path_list[j] != NULL){
                int k = 0;
                char *execvFirstParamHalf;
                char *execvFirstParamAll;
                std::string temp;
                execvFirstParamHalf = strcat(os_path_list[j],slash_ptr);
                temp = execvFirstParamHalf;
                temp.erase(temp.find_last_not_of("\t")+1); // delete annoying '\t'
                char *execvFirstParamHalfNew = (char*) temp.c_str();
                execvFirstParamAll = strcat(execvFirstParamHalfNew,arguments[0]);
                
                printf("3333333333333333\n");
                //std::cout << "\n execvFirstParamAll is: "<< execvFirstParamAll << " | arguments is: " << arguments[0] << " then: " << arguments[1] << "\n";
                
                j++;
            }
            /*int pid;
            pid = fork();
            if (pid == 0){ // child
                
                execv(execvFirstParamAll, arguments);
                
            } else { // parent
                int status;
                waitpid(pid, &status, 0);
            }*/


        }
        //break;







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
