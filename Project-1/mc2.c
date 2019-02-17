//Kenneth Desrosiers - kdesrosiers
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <stdio.h>

//helper function to print statistics
int printStats(double time, long min, long maj){
  printf("\n-- Stats!!! -- \n");
  printf("Elapsed time: %f milliseconds\n", time);
  printf("Number of reclaimed page faults: %ld\n",min);
  printf("Number of page faults: %ld\n",maj);
  printf("\n\n");
  return 0;
}
//helper function that does terminal command for 'whoami' calculates statistics
void runWhoAmI(){
  struct timeval start, end;
  //capturing time at the start
  gettimeofday(&start, NULL);
  //struct for the resource usage for 'whoami'
  struct rusage whoamiUsage;
  //creates a new child process
  pid_t proc = fork();
  //if the PID is less than 0, something is wrong
  if (proc < 0) {
    perror("\nan error has occurred");
    exit(-1);
  }
  //if it's greater than zero, its the parent process
  //must wait for child to finish
  if (proc > 0) {
    wait(NULL);
  }
  //if it is zero, its the child process, and execute 'whoami'
  if (proc == 0) {
    //finds location of command 'whoami'
    printf("\n");
    execl("/usr/bin/whoami", "whoami", NULL);
    printf("\n");
  }
  //captures the end time
  gettimeofday(&end, NULL);
  /*calculates the seconds it took to run,
  in milliseconds. calculates the total amount in seconds,
  then divides by 1000 to get the time in milliseconds!*/
  double totalSec = end.tv_sec - start.tv_sec;
  double totalMicro = end.tv_usec - start.tv_usec;
  double total = ((totalSec) * 1000) + ((totalMicro) / 1000); //in milliseconds
  //calculates the actual resource usage
  getrusage(RUSAGE_SELF, &whoamiUsage);
  //reclaimed page faults
  long minor = whoamiUsage.ru_minflt;
  //un-reclaimed page faults
  long major = whoamiUsage.ru_majflt;
  printStats(total, minor, major);
}


//helper function that does terminal command for 'last' calculates statistics
void runLast(){
  //the entire process for runLast() is exactly the same as for runWhoAmI()!
  struct timeval start, end;
  gettimeofday(&start, NULL);
  struct rusage lastUsage;
  pid_t proc = fork();
  if (proc < 0) {
    perror("\nan error has occurred");
    exit(-1);
  }
  if (proc > 0){
    wait(NULL);
  }
  if (proc == 0) {
    printf("\n");
    //finds locaiton of command 'last'
    execl("/usr/bin/last", "last", NULL);
  }
  gettimeofday(&end, NULL);
  double totalSec = end.tv_sec - start.tv_sec;
  double totalMicro = end.tv_usec - start.tv_usec;
  double total = ((totalSec) * 1000) + ((totalMicro) / 1000); //in milliseconds
  getrusage(RUSAGE_SELF, &lastUsage);
  long minor = lastUsage.ru_minflt;
  long major = lastUsage.ru_majflt;
  printStats(total, minor, major);
}


//helper function that does terminal command for 'ls' on a user-specified path calculates statistics
void runLS(){
  //most of the process for runLS() is the same as for runWhoAmI() and runLast()
  struct timeval start, end;
  gettimeofday(&start, NULL);
  struct rusage lsUsage;
  pid_t proc = fork();
  if (proc < 0) {
    perror("an error has occurred");
    exit(-1);
  }
  if (proc > 0){
    wait(NULL);
  }
  if (proc == 0){
    //makes places to store the user input
    char arg[129];
    char path[129];
    printf("arguments?: ");
    //gets the user input for arguments
    fgets(arg, 129, stdin);
    if(strlen(arg) > 129){
      printf("\nToo long!!!\n");
      exit(-1);
    }
    printf("path?: ");
    //gets user input for path
    fgets(path, 129, stdin);
    if(strlen(path) > 129){
      printf("\nToo long!!!\n");
      exit(-1);
    }
    char *arrayOfArgs[33];
    int i = 1;
    int count = 0;
    //if the user doesn't input arg or path
    if(arg[0] == '\n' && path[0] == '\n'){
      execl("/bin/ls", "ls", NULL);
    }
    //if the user inputs arg but not path
    else if(arg[0] != '\n' && path[0] == '\n'){
      int length = strlen(arg);
      arg[length-1] = '\0';
      //storing the first argument into the array of arguments
      arrayOfArgs[0] = "ls";
      arrayOfArgs[i] = strtok(arg," \n");
      while(arrayOfArgs[i] != NULL){
        arrayOfArgs[++i] = strtok(NULL, " \n");
        count++;
      }
      if(count > 32){
        printf("\nToo many arguments!!\n");
        exit(-1);
      }
      execvp(arrayOfArgs[0], arrayOfArgs);
      printf("Couldn't execute!!\n");
    }
    //if the user inputs arg and path
    else if(arg[0] != '\n' && path[0] != '\n'){
      int arglen = strlen(arg);
      //the purpose of doing this now and in every other use is to get rid of the newline character added to the end of user input from fgets()
      arg[arglen-1] = '\0';
      int pathlen = strlen(path);
      path[pathlen-1] ='\0';
      //concatenating a space with arg so it will make concatenating arg with path easier
      strcat(arg," ");
      strcat(arg,path);
      arrayOfArgs[0] = "ls";
      arrayOfArgs[i] = strtok(arg," \n");
      while(arrayOfArgs[i] != NULL){
        arrayOfArgs[++i] = strtok(NULL, " \n");
        count++;
      }
      if(count > 32){
        printf("\nToo many arguments!!\n");
        exit(-1);
      }
      execvp(arrayOfArgs[0], arrayOfArgs);
      printf("Couldn't execute!!\n");
    }
    //if the user doesnt input arg, but they input path
    else if(arg[0] == '\n' && path[0] != '\n'){
      int length = strlen(path);
      path[length-1] = '\0';
      //storing the first argument into the array of arguments
      arrayOfArgs[0] = "ls";
      arrayOfArgs[i] = strtok(path," \n");
      while(arrayOfArgs[i] != NULL){
        arrayOfArgs[++i] = strtok(NULL, " \n");
        count++;
      }
      if(count > 32){
        printf("\nToo many arguments!!\n");
        exit(-1);
      }
      execvp(arrayOfArgs[0], arrayOfArgs);
      printf("Couldn't execute!!\n");
    }
  }
  gettimeofday(&end, NULL);
  double totalSec = end.tv_sec - start.tv_sec;
  double totalMicro = end.tv_usec - start.tv_usec;
  double total = ((totalSec) * 1000) + ((totalMicro) / 1000); //in milliseconds
  getrusage(RUSAGE_SELF, &lsUsage);
  long minor = lsUsage.ru_minflt;
  long major = lsUsage.ru_majflt;
  printStats(total, minor, major);
}

//helper function to log out
int logOut(){
  printf("Logging you out, Commander!\n");
  exit(0);
  return 0;
}

//helper function to change directories
int changeDir(){
  char location[129];
  printf("\nWhere would you like to go?: ");
  fgets(location, 129, stdin);
  int leng = strlen(location);
  location[leng-1] = '\0';
  //do not exec(), if you do, the child process directory will change, not the main one (which is what we want)
  chdir(location);
  return 0;
}

//helper function to print the current working Directory
int printDir(){
  pid_t newProc = fork();
  if(newProc < 0){
    perror("an error has occurred");
    exit(-1);
  }
  else if(newProc > 0){
    wait(NULL);
  }
  else{
    printf("-- Current Directory --\n\n");
    execl("/bin/pwd", "pwd", NULL);
    printf("Couldn't execute!!\n");
  }
  printf("\n\n");
  return 0;
}

//helper function to print menu introduction
void printIntro(){
  printf("===== Mid-Day Commander, v1 =====\n");
  printf("G'day, Commander! What command would you like run?\n");
}


//making a struct to make a linked list for the command list
struct String{
  int number;
  char data[129];
  int background;
  struct String *next;
};
//a pointer to the head of the command list
struct String *head = NULL;

//making a struct to create background commands
struct BackgroundPID{
  pid_t pidNum;
  int done;
  int order;
  struct BackgroundPID *next;
};
//a pointer to the head of the PID list
struct BackgroundPID *firstID = NULL;

//helper function that adds a new command to the menu
int addACommand(){
  //make a new node
  struct String *current = head;
  struct String *new = NULL;
  new = (struct String*)malloc(sizeof(struct String));
  char str[129];
  fgets(str, 129, stdin);
  if(strlen(str) > 129){
    perror("too many characters!");
    exit(-1);
  }
  //tokenize the input so exec() can read it
  int len = strlen(str);
  //gets rid of newline character that fgets() adds in
  str[len-1] = '\0';
  while(current->next != NULL){
    current = current->next;
  }
  current->next = new;
  new->number = (current->number) + 1;
  strncpy(new->data, str, 129);
  new->next = NULL;
  return 0;
}

//helper function to add a new pid to the list
int addAPID(int pidVal){
  //create a new node
  struct BackgroundPID *current = firstID;
  struct BackgroundPID *new = NULL;
  new = (struct BackgroundPID*)malloc(sizeof(struct BackgroundPID));
  while(current->next != NULL){
    current = current->next;
  }
  current->next = new;
  new->pidNum = pidVal;
  new->next = NULL;
  new->done = 0;
  new->order = (current->order) + 1;
  return 0;
}

//helper to print out all current background commands
int printBackgroundCommands(){
  struct BackgroundPID *now = firstID;
  printf("\n\n-- Background Processes! --\n\n");
  while(now != NULL){
    if(now->done == 0){
      printf("\n[%d]  Process ID: %d\n",now->order, now->pidNum);
    }
    now = now->next;
  }
  return 0;
}

//helper function to process background processes
int processCheck(){
  struct BackgroundPID *now = firstID;
  struct rusage newUsage;
  //struct timeval end;
  //the WNOHANG option finds any child that has finished
  pid_t pid = wait3(NULL, WNOHANG, &newUsage);
  while(now != NULL){
    if(pid > 0 && now->pidNum == pid){
      printf("[%d]  Process %d is done!\n",now->order, pid);
      //reset the done parameter and the numbering system
      now->done = 1;
      now->order = 1;
    }
    else if(pid == 0){
      printf("No process has finished!\n");
    }
    now = now->next;
  }
  return 0;
}

//helper to handle everything regarding newly added commands (including background processes)
int customRunner(char str[]){
  struct String *current = head;
  int length = 0;
  char *arrayOfArgs[33];
  char temp[128];
  int i = 0;
  pid_t newProc = fork();
  while(current != NULL){
    //checks to see if the added command is a background command
    if(current->data[strlen(current->data) - 1] == '&'){
      //sets the background member equal to 1
      current->background = 1;
    }
    else{
      //else it is 0
      current->background = 0;
    }
    //if the user input is equal to any number on the list
    if(atoi(str) == current->number){
      //if the process is not a background command, process it normally
      if(current->background == 0){
        if(newProc == 0){
          //tokenize the command
          arrayOfArgs[i] = strtok(current->data," \n");
          while(arrayOfArgs[i] != NULL){
            arrayOfArgs[++i] = strtok(NULL, " \n");
            length++; //keeps tracks of the length of the argument array (not counting the name of executable)
          }
          //don't allow more than 32 arguments
          if(length > 32){
            printf("\nToo many arguments!!!!\n");
          }
          else{
            execvp(arrayOfArgs[0], arrayOfArgs);
            //should never print out if everything worked
            printf("Couldn't execute!!\n");
          }
        }
        if(newProc > 0){
          wait(NULL);
        }
        else{
          perror("something went wrong!\n");
        }
      }
      //now if the user input IS a bacground command
      else if(current->background == 1){
        if(newProc == 0){
          //getting rid of the '&' and tokenizing the command
          strcpy(temp, current->data);
          temp[strlen(temp) - 1] = '\0';
          arrayOfArgs[i] = strtok(temp," \n");
          while(arrayOfArgs[i] != NULL){
            arrayOfArgs[++i] = strtok(NULL, " \n");
            length++;
          }
          //can't allow more than 32 arguments
          if(length > 32){
            printf("\nToo many arguments!!!!\n");
          }
          else{
            printf("\n");
            execvp(arrayOfArgs[0], arrayOfArgs);
            printf("Couldn't execute!!\n");
          }
        }
        else if(newProc > 0){
          printf("The PID is %d.\n\n",newProc);
          addAPID(newProc);
        }
        else{
          printf("fork did not work!\n");
        }
      }
    }
    current = current->next;
  }
  return 0;
}
//helper function to print the list
int printNumberedList(){
  //prints out the entire command linked list
  struct String *current = head;
  while(current != NULL){
    printf("\t%d. %s\n",current->number,current->data);
    current = current->next;
  }
  return 0;
}

//helper function to print the rest of the menu
int printRestOfMenu(){
  printf("\ta. add command: adds a new command!\n");
  printf("\tc. change directory: Changes process working directory.\n");
  printf("\te. exit: Leave Midday Commander.\n");
  printf("\tp. pwd: Prints working directory.\n");
  printf("\tr. running processes: Print list of running processes\n");
  return 0;
}
//helper function that runs the menu
int runTheMenu(){
  //where all of the magic happens!
  //runs the whole simulation
  while(1){
    printf("\n\n");
    printIntro();
    printNumberedList();
    printRestOfMenu();
    printf("Option?:");
    char str[129];
    fgets(str,129, stdin);
    processCheck();
    str[strlen(str)-1]='\0';
    if(strlen(str) > 129){
    printf("\nToo long!!!\n");
  }
  //for reading in from a file
  if(feof(stdin)){
    exit(0);
  }
  //conditions that test the user input, and acts accordingly
  if(strcmp(str, "0") == 0){
    runWhoAmI();
  }
  else if(strcmp(str, "1") == 0){
    runLast();
  }
  else if(strcmp(str, "2") == 0){
    runLS();
  }
  else if(strcmp(str, "a") == 0){
    printf("What command do you want to add? ");
    //run the add a command function
    addACommand();
  }
  else if(strcmp(str, "c") == 0){
    changeDir();
  }
  else if(strcmp(str, "e") == 0){
    //can't allow the user to exit if the background processes aren't done
    struct BackgroundPID *currentPID = firstID;
    int l = 0;
    while(currentPID != NULL){
      if(currentPID->done == 0){
        l++;
      }
      currentPID = currentPID->next;
    }
    if(l > 0){
      printf("You can't exit yet, commands are still running!!\n");
    }
    else{
      logOut();
    }
  }
  else if(strcmp(str, "p") == 0){
    printDir();
  }
  else if(strcmp(str, "r") == 0){
    printBackgroundCommands();
  }
  else{
    customRunner(str);
  }
}
return 0;
}


int main(){
  //manually making the first three items in the command linked list,
  //the first three existing numbered commands
  struct String *first = NULL;
  first = (struct String*)malloc(sizeof(struct String));
  struct String *second = NULL;
  second = (struct String*)malloc(sizeof(struct String));
  struct String *third = NULL;
  third = (struct String*)malloc(sizeof(struct String));
  strncpy(first->data, "whoami :Prints out the result of whoami", 129);
  first->number = 0;
  first->next = second;
  strncpy(second->data, "last :Prints out the result of last", 129);
  second->number = 1;
  second->next = third;
  strncpy(third->data, "ls :Prints out the result of a listing on a user-specified path", 129);
  third->number = 2;
  third->next = NULL;
  head = first;
  //intialing a filler node to act as the head of the PID linked list
  struct BackgroundPID *theHead = NULL;
  theHead = (struct BackgroundPID*)malloc(sizeof(struct BackgroundPID));
  theHead->next = NULL;
  //setting filler node's 'done' to a value that isn't 0 or 1, so it won't interfere with the 'e' command
  theHead->done = 2;
  theHead->order = 0;
  firstID = theHead;
  //run the program
  runTheMenu();
  return 0;
}
