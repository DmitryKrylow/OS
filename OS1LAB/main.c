#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#define delim " \t\r\n\a"

extern char **__environ;
#ifdef __USE_GNU
extern char **environ;
#endif


char *read_command(void)
{
    char *line = NULL;
    size_t buf = 0;
    getline(&line,&buf,stdin);
    return line;
}


char **split_string(char* line){
    int sizebuf = 64, pos = 0;
    char **tks = malloc(sizebuf * sizeof(char*));
    char *tkn = strtok(line,delim);

    while(tkn != NULL){
        tks[pos] = tkn;
        pos++;
        if(pos >= sizebuf){
            sizebuf += 64;
            tks = realloc(tks,sizebuf * sizeof (char*));
        }
        tkn = strtok(NULL, delim);
    }
    tks[pos] = NULL;
    return tks;
}

int exec_command(char** args){
    pid_t pid,wpid;
    int status;

    pid = fork();
    if(pid == 0){
        if(execv(args[0],args) == -1){
            perror("myshell");
        }
        exit(EXIT_FAILURE);
    }else if(pid < 0){
        perror("myshell");
    }else{
        do{
            wpid = waitpid(pid,&status,WUNTRACED);
        }while(!WIFEXITED(status) && !WIFSIGNALED(status));
    }
    return 1;
}


int mycd(char **args);
int myhelp(char **args);
int myquit(char **args);
int mydir(char **args);
int myecho(char **args);
int myclr(char **args);
int myenviron(char **args);
int mypause(char **args);


char *command_str[] = {
        "cd",
        "dir",
        "help",
        "echo",
        "clr",
        "environ",
        "quit",
        "pause",
};

int (*command_func[]) (char **) = {
        &mycd,
        &mydir,
        &myhelp,
        &myecho,
        &myclr,
        &myenviron,
        &myquit,
        &mypause,
};

int num_builtins() {
    return sizeof(command_str) / sizeof(char *);
}

int mypause(char **args){
    int c = getchar();
    while(c != '\n'){
        c = getchar();
    }
    return 1;
}

int myecho(char **args){
    for(int i = 1; args[i] != NULL; i++) {
        printf("%s ", args[i]);
    }
    printf("\n");
    return 1;
}

int myclr(char **args){
    printf ("\033[0d\033[2J");
    return 1;
}

int myenviron(char **args){
    if(args[1] == NULL){
        fprintf(stderr, "myshell: ожидается аргумент для \"environ\"\n");
    }else {
        char *env = getenv(args[1]);
        if(env == NULL){
            fprintf(stderr, "myshell: не существует переменной %s\n",args[1]);
        }else {
            printf("%s\n", env);
        }
    }
    return 1;
}

int mydir(char **args){
    DIR *dir;
    struct dirent *ent;
    if(args[1] == NULL){
        fprintf(stderr, "myshell: ожидается аргумент для \"dir\"\n");
    }else {
        dir = opendir(args[1]);

        while ((ent = readdir(dir)) != 0) {
            printf("%s\n", ent->d_name);
        }
        free(dir);
        free(ent);
    }
    return 1;
}

int mycd(char **args){
    if (args[1] == NULL) {
        fprintf(stderr, "myshell: ожидается аргумент для \"cd\"\n");
    } else {
        if (chdir(args[1]) != 0) {
            perror("myshell");
        }
    }
    return 1;
}


int myhelp(char **args) {
    printf("cd <path> - сменить текущую директорию\ndir <path> - вывести содержимое директории\nenviron <environ name> - вывести значение переменной среды\necho <message> - вывести сообщение в консоль\nclr - очистка консоли\npause - приостановка консоли до ввода символа\nquit - завершение работы\nhelp - вывести помощь по командам\n");
    return 1;
}

int myquit(char **args) {
    return 0;
}
int myshell_execute(char **args) {
    if (args[0] == NULL) {
        return 1;
    }
    for (int i = 0; i < num_builtins(); i++) {
        if (strcmp(args[0], command_str[i]) == 0) {
            return (*command_func[i])(args);
        }
    }
    return exec_command(args);
}

void shell(){
    char* line;
    char **args;
    int status;

    do{
        char dir_now[1024];
        getcwd(dir_now,sizeof (dir_now));
        printf("%s >> ", dir_now);
        line = read_command();
        args = split_string(line);
        status = myshell_execute(args);

        free(line);
        free(args);

    }while(status);

}


int main() {
    shell();
    return 0;
}
