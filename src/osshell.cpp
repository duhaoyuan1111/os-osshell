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
#include <fstream>


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
    int start_of_writing_hist = 0;
    char user_input_history[128][128];
    for (int i=0; i< 128; i++){
        memset(user_input_history[i],'\0',sizeof(user_input_history[i]));
    }
    // Read history file in
    std::ifstream readFile("history.txt");
    if (readFile.is_open()){
        while (readFile.peek() != EOF){
            readFile.getline(user_input_history[start_of_writing_hist], 128, '\n');
            start_of_writing_hist++;
        }
    }
    readFile.close();
    // Repeat:
    while (true){
        std::cout << "osshell> ";
        
        char user_input_char[128];
        std::cin.getline(user_input_char, 128);
        
        if (strcmp(user_input_char,"exit")==0){
            // save history
            if (start_of_writing_hist<128){// not full yet
                for (int p=0;p<strlen(user_input_char);p++){
                    user_input_history[start_of_writing_hist][p] = user_input_char[p];
                }
            } else {
                // shift
                for (int q=0;q<127;q++){
                    for (int w=0;w<128;w++){
                        user_input_history[q][w] = user_input_history[q+1][w];
                    }
                }
                memset(user_input_history[127],'\0',sizeof(user_input_history[127]));
                for (int p=0;p<strlen(user_input_char);p++){
                    user_input_history[127][p] = user_input_char[p];
                }
            }
            start_of_writing_hist++;
            // pending history to a file
            std::ofstream outFile("history.txt");
            for (int i=0;i<start_of_writing_hist;i++){
                outFile << user_input_history[i] << '\n';
            }
            outFile.close();
            printf("\n");
            exit(-1);
        } else if ((strcmp(user_input_char,"history")==0) || (strncmp(user_input_char,"history ",8)==0)){
            if (strcmp(user_input_char,"history")==0){
                int i = 0;
                while(user_input_history[i][0] != '\0'){
                    printf("  %d: %s\n",i+1,user_input_history[i]);
                    i++;
                }
                // save history
                if (start_of_writing_hist<128){// not full yet
                    for (int p=0;p<strlen(user_input_char);p++){
                        user_input_history[start_of_writing_hist][p] = user_input_char[p];
                    }
                } else {
                    // shift
                    for (int q=0;q<127;q++){
                        for (int w=0;w<128;w++){
                            user_input_history[q][w] = user_input_history[q+1][w];
                        }
                    }
                    memset(user_input_history[127],'\0',sizeof(user_input_history[127]));
                    for (int p=0;p<strlen(user_input_char);p++){
                        user_input_history[127][p] = user_input_char[p];
                    }
                }
                start_of_writing_hist++;
            } else if (strcmp(user_input_char,"history clear")==0){ // history clear
                for (int i=0; i< 128; i++){
                    memset(user_input_history[i],'\0',sizeof(user_input_history[i]));
                }
                start_of_writing_hist = 0;
                // don't save this command to history!
            } else if (user_input_char[8]<58 && user_input_char[8]>48){
                // here, is <history 1...>
                int examine = 9;
                int flag = 0;
                // examine if it's a number after 'history' forever
                while (user_input_char[examine] != '\0'){
                    if (user_input_char[examine]>57 || user_input_char[examine]<48) {
                        flag = 1;
                        break;
                    }
                    examine++;
                }
                if (flag == 1) {
                    printf("Error: history expects an integer > 0 (or 'clear')\n");
                } else {
                    int howManyHist;
                    int ott = 0;
                    char num[3];
                    memset(num,'\0',sizeof(num));
                    for (int b=0;b<strlen(user_input_char);b++){
                        if (user_input_char[b]<58 && user_input_char[b]>47){
                            num[ott] = user_input_char[b];
                            ott++;
                        }
                    }
                    howManyHist = atoi(num);
                    if (howManyHist>128){
                        howManyHist = 128;
                    }
                    if (howManyHist>start_of_writing_hist){
                        howManyHist = start_of_writing_hist;
                    }
                    for (int i = start_of_writing_hist-howManyHist;i<start_of_writing_hist;i++){
                        printf("  %d: %s\n",i+1,user_input_history[i]);
                    }
                }
                // save history
                if (start_of_writing_hist<128){// not full yet
                    for (int p=0;p<strlen(user_input_char);p++){
                        user_input_history[start_of_writing_hist][p] = user_input_char[p];
                    }
                } else {
                    // shift
                    for (int q=0;q<127;q++){
                        for (int w=0;w<128;w++){
                            user_input_history[q][w] = user_input_history[q+1][w];
                        }
                    }
                    memset(user_input_history[127],'\0',sizeof(user_input_history[127]));
                    for (int p=0;p<strlen(user_input_char);p++){
                        user_input_history[127][p] = user_input_char[p];
                    }
                }
                start_of_writing_hist++;

            } else {
                //error message
                printf("Error: history expects an integer > 0 (or 'clear')\n");
                // save history
                if (start_of_writing_hist<128){// not full yet
                    for (int p=0;p<strlen(user_input_char);p++){
                        user_input_history[start_of_writing_hist][p] = user_input_char[p];
                    }
                } else {
                    // shift
                    for (int q=0;q<127;q++){
                        for (int w=0;w<128;w++){
                            user_input_history[q][w] = user_input_history[q+1][w];
                        }
                    }
                    memset(user_input_history[127],'\0',sizeof(user_input_history[127]));
                    for (int p=0;p<strlen(user_input_char);p++){
                        user_input_history[127][p] = user_input_char[p];
                    }
                }
                start_of_writing_hist++;
            }
            
        } else if (user_input_char[0] == '.' || user_input_char[0] == '/'){
            // start with . or /
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
            // To find if a path is valid and can be executed
            int ok = 0;
            if (access(tail,X_OK)==0){
                ok = 1;
            }
            // prepare the 2nd param for execv()
            char **cmd_2;
            std::string user_input_string = user_input_char;
            allocateArrayOfCharArrays(&cmd_2,16,64);
            splitString(user_input_string,' ',cmd_2);
            if (ok == 1){ // found command, execv!
                int pid;
                pid = fork();
                if (pid == 0) { // child
                    execv(tail,cmd_2);
                } else {// parent
                    int status;
                    waitpid(pid, &status, 0);
                }
               
            } else {
                printf("%s: Error command not found\n", user_input_char);
            }
            // save history
            if (start_of_writing_hist<128){// not full yet
                for (int p=0;p<strlen(user_input_char);p++){
                    user_input_history[start_of_writing_hist][p] = user_input_char[p];
                }
            } else {
                // shift
                for (int q=0;q<127;q++){
                    for (int w=0;w<128;w++){
                        user_input_history[q][w] = user_input_history[q+1][w];
                    }
                }
                memset(user_input_history[127],'\0',sizeof(user_input_history[127]));
                for (int p=0;p<strlen(user_input_char);p++){
                    user_input_history[127][p] = user_input_char[p];
                }
            }
            start_of_writing_hist++;
        } else if (user_input_char[0] == '\n'){
            // do nothing
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
               
            } else {
                printf("%s: Error command not found\n", user_input_char);
            }
            // save history
            if (start_of_writing_hist<128){// not full yet
                for (int p=0;p<strlen(user_input_char);p++){
                    user_input_history[start_of_writing_hist][p] = user_input_char[p];
                }
            } else {
                // shift
                for (int q=0;q<127;q++){
                    for (int w=0;w<128;w++){
                        user_input_history[q][w] = user_input_history[q+1][w];
                    }
                }
                memset(user_input_history[127],'\0',sizeof(user_input_history[127]));
                for (int p=0;p<strlen(user_input_char);p++){
                    user_input_history[127][p] = user_input_char[p];
                }
            }
            start_of_writing_hist++;
        }
        
    }
    
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
    enum states { NONE, IN_WORD, IN_STRING } state = NONE;

    int i;
    std::vector<std::string> list;
    std::string token;
    for (i = 0; i < text.length(); i++)
    {
        char c = text[i];
        switch (state) {
            case NONE:
                if (c != d)
                {
                    if (c == '\"')
                    {
                        state = IN_STRING;
                        token = "";
                    }
                    else
                    {
                        state = IN_WORD;
                        token = c;
                    }
                }
                break;
            case IN_WORD:
                if (c == d)
                {
                    list.push_back(token);
                    state = NONE;
                }
                else
                {
                    token += c;
                }
                break;
            case IN_STRING:
                if (c == '\"')
                {
                    list.push_back(token);
                    state = NONE;
                }
                else
                {
                    token += c;
                }
                break;
        }
    }
    if (state != NONE)
    {
        list.push_back(token);
    }

    for (i = 0; i < list.size(); i++)
    {
        strcpy(result[i], list[i].c_str());
    }
    result[list.size()] = NULL;
}
