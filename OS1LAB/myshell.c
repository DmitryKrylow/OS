#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include "myshell.h"
#define delim " \t\r\n\a"

//объявления функций
int mycd(char **args);
int myhelp(char **args);
int myquit(char **args);
int mydir(char **args);
int myecho(char **args);
int myclr(char **args);
int myenviron(char **args);
int mypause(char **args);

//считывание команды пользователя
char *read_command(void)
{
    char *line = NULL;
    size_t buf = 0;
    getline(&line,&buf,stdin);
    return line;
}

//разделение строки на команду и аргументы
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
//запуск внешней команды
int exec_command(char** args){
    pid_t pid = fork(); // клонирование процесса

    if (pid == 0) {
        // Child процесс
        execvp(args[0], args);
        perror("execvp");
        return(EXIT_FAILURE);
    } else if (pid < 0) {
        perror("fork");
    } else {
        wait(NULL); // Ожидание завершения child процесса
    }
    return 1;
}

//список доступных команд
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
//ссылки на функии для каждой команды
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

//возвращает количество команд
int cnt_command() {
    return sizeof(command_str) / sizeof(char *);
}
//функция паузы - приостановка программы до ввода любого символа
int mypause(char **args){
    getchar();
    return 1;
}
//функция echo - вывод текста в консоль
int myecho(char **args){
    for(int i = 1; args[i] != NULL; i++) {
        printf("%s ", args[i]);
    }
    printf("\n");
    return 1;
}
//функция clr - очистка консоли
int myclr(char **args){
    printf ("\033[0d\033[2J");
    return 1;
}
//функция environ - вывод всех переменных среды
int myenviron(char **args){
    extern char **environ;

    int i = 0;
    while(environ[i]) {
        printf("%s\n", environ[i++]);
    }
    return 1;
}
//функция mydir - вывод файлов директории
int mydir(char **args){
    DIR *dir;
    struct dirent *ent;
    if(args[1] == NULL){//если нет аргумента выводим содержимое текущей директории
        char dir_now[1024];
        getcwd(dir_now,sizeof (dir_now));
        dir = opendir(dir_now);

        while ((ent = readdir(dir)) != 0) {
            printf("%s\n", ent->d_name);
        }
        free(ent);
    }else {
        dir = opendir(args[1]);

        while ((ent = readdir(dir)) != 0) {
            printf("%s\n", ent->d_name);
        }
        free(ent);
    }
    closedir(dir);
    return 1;
}
//функция cd - смена рабочей директории
int mycd(char **args){
    char dir_now[1024];
    getcwd(dir_now,sizeof (dir_now));
    char tryCD[2048];
    snprintf(tryCD, sizeof tryCD, "%s%s", dir_now, args[1]);//получение текущего пути + аргумент
    if (args[1] == NULL) {
        fprintf(stderr, "myshell: ожидается аргумент для \"cd\"\n");
    } else {
        if(chdir(args[1]) != 0) // пытаемся основную директорию
            if(chdir(tryCD) != 0){//если не смогли, то пытаемся менять директорию относительно текущей
                perror("myshell");
        }
    }
    return 1;
}
//фнукция help - вывод списка доступных команд
int myhelp(char **args) {
    printf("cd <path> - сменить текущую директорию\ndir <path> - вывести содержимое директории\nenviron - вывести значения переменных среды\necho <message> - вывести сообщение в консоль\nclr - очистка консоли\npause - приостановка консоли до ввода символа\nquit - завершение работы\nhelp - вывести помощь по командам\n");
    return 1;
}
//функция quit - выход из оболочки
int myquit(char **args) {
    return 0;
}
//проверка списка команд и запуск соответвующей функции
int myshell_execute(char **args) {
    if (args[0] == NULL) {
        return 1;
    }
    for (int i = 0; i < cnt_command(); i++) {
        if (strcmp(args[0], command_str[i]) == 0) {//если есть встроенная функция, то запускаем ее
            return (*command_func[i])(args);
        }
    }
    return exec_command(args); // запускаем внешнюю команду
}
//основная функция myshell
int main() {
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

    return 0;
}
