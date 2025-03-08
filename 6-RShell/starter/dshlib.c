#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <errno.h>
#include "dshlib.h"
#include "dragon.c"


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

/**** 
 **** FOR REMOTE SHELL USE YOUR SOLUTION FROM SHELL PART 3 HERE
 **** THE MAIN FUNCTION CALLS THIS ONE AS ITS ENTRY POINT TO
 **** EXECUTE THE SHELL LOCALLY
 ****
 */

/*
 * Implement your exec_local_cmd_loop function by building a loop that prompts the 
 * user for input.  Use the SH_PROMPT constant from dshlib.h and then
 * use fgets to accept user input.
 * 
 *      while(1){
 *        printf("%s", SH_PROMPT);
 *        if (fgets(cmd_buff, ARG_MAX, stdin) == NULL){
 *           printf("\n");
 *           break;
 *        }
 *        //remove the trailing \n from cmd_buff
 *        cmd_buff[strcspn(cmd_buff,"\n")] = '\0';
 * 
 *        //IMPLEMENT THE REST OF THE REQUIREMENTS
 *      }
 * 
 *   Also, use the constants in the dshlib.h in this code.  
 *      SH_CMD_MAX              maximum buffer size for user input
 *      EXIT_CMD                constant that terminates the dsh program
 *      SH_PROMPT               the shell prompt
 *      OK                      the command was parsed properly
 *      WARN_NO_CMDS            the user command was empty
 *      ERR_TOO_MANY_COMMANDS   too many pipes used
 *      ERR_MEMORY              dynamic memory management failure
 * 
 *   errors returned
 *      OK                     No error
 *      ERR_MEMORY             Dynamic memory management failure
 *      WARN_NO_CMDS           No commands parsed
 *      ERR_TOO_MANY_COMMANDS  too many pipes used
 *   
 *   console messages
 *      CMD_WARN_NO_CMD        print on WARN_NO_CMDS
 *      CMD_ERR_PIPE_LIMIT     print on ERR_TOO_MANY_COMMANDS
 *      CMD_ERR_EXECUTE        print on execution failure of external command
 * 
 *  Standard Library Functions You Might Want To Consider Using (assignment 1+)
 *      malloc(), free(), strlen(), fgets(), strcspn(), printf()
 * 
 *  Standard Library Functions You Might Want To Consider Using (assignment 2+)
 *      fork(), execvp(), exit(), chdir()
 */
//global var for rc
int lastReturn = 0;
//funcs necessary for exec
int alloc_cmd_buff(cmd_buff_t *cmd_buff) {
    cmd_buff->_cmd_buffer = malloc(SH_CMD_MAX * sizeof(char));
    if (cmd_buff->_cmd_buffer == NULL) {
        return ERR_MEMORY;
    }
    cmd_buff->argc = 0;
    for (int i = 0; i < CMD_ARGV_MAX; i++) {
        cmd_buff->argv[i] = NULL;
    }
    return OK;
}

int free_cmd_buff(cmd_buff_t *cmd_buff) {
    if (cmd_buff->_cmd_buffer != NULL) {
        free(cmd_buff->_cmd_buffer);
    }
    cmd_buff->argc = 0;
    for (int i = 0; i < CMD_ARGV_MAX; i++) {
        cmd_buff->argv[i] = NULL;
    }
    return OK;
}

int build_cmd_buff(char *cmd_line, cmd_buff_t *cmd_buff) {
    if (cmd_line == NULL || cmd_buff == NULL) {
        return ERR_MEMORY;
    }
    clear_cmd_buff(cmd_buff);
    char *input_redir = NULL;
    char *output_redir = NULL;
    int output_mode = 0;
    int argc = 0;
    char *p = cmd_line;
    int quotes = 0;
    int redir_index = -1;

    while (*p && argc < CMD_ARGV_MAX - 1) {
        while (*p == ' ' && !quotes){
            p++;
        }
        if (*p == '\0'){
            break;
        }
        //ec
        if (*p == '<') {
            p++;
            while (*p == ' ') p++;
            char *redir_start = p;
            while (*p && *p != ' ' && *p != '>' && *p != '<') {
                p++;
            }
            if (redir_start != p) {
                input_redir = malloc(p - redir_start + 1);
                strncpy(input_redir, redir_start, p - redir_start);
                input_redir[p - redir_start] = '\0';
            }
            
            continue;
        }
        if (*p == '>') {
            int is_append = 0;
            if (*(p+1) == '>') {
                is_append = 1;
                p++;
            }
            p++;
            while (*p == ' ') p++;
            char *redir_start = p;
            while (*p && *p != ' ' && *p != '<' && *p != '>') {
                p++;
            }
            if (redir_start != p) {
                output_redir = malloc(p - redir_start + 1);
                strncpy(output_redir, redir_start, p - redir_start);
                output_redir[p - redir_start] = '\0';
                output_mode = is_append;
            }
            
            continue;
        }
        if (*p == '"') {
            quotes = 1;
            p++;
            cmd_buff->argv[argc] = p;
            while (*p && (*p != '"' || !quotes)) {
                if (*p == '\\' && *(p + 1) == '"') {
                    p++;
                }
                p++;
            }
            if (*p == '"') {
                *p = '\0';
                quotes = 0;
                p++;
            }
            argc++;
        } else {
            cmd_buff->argv[argc] = p;
            argc++;
            while (*p && (*p != ' ' || quotes)) {
                if (*p == '"') {
                    quotes = !quotes;
                }
                p++;
            }
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
        }
    }
    
    cmd_buff->argc = argc;
    cmd_buff->argv[argc] = NULL;
    
    if (argc >= CMD_ARGV_MAX) {
        if (input_redir){ free(input_redir);}
        if (output_redir){ free(output_redir);}
        return ERR_TOO_MANY_COMMANDS;
    }
    if (input_redir || output_redir) {
        char redir_info[SH_CMD_MAX] = {0};
        if (input_redir) {
            snprintf(redir_info, sizeof(redir_info), "I:%s:", input_redir);
            free(input_redir);
        }
        
        if (output_redir) {
            char mode_str[2];
            snprintf(mode_str, sizeof(mode_str), "%d", output_mode);
            strcat(redir_info, "O:");
            strcat(redir_info, output_redir);
            strcat(redir_info, ":");
            strcat(redir_info, mode_str);
            free(output_redir);
        }
        strncpy(cmd_buff->_cmd_buffer, redir_info, SH_CMD_MAX - 1);
    }
    return OK;
}

Built_In_Cmds match_command(const char *input) {
    if (strcmp(input, "exit") == 0) {
        return BI_CMD_EXIT;
    } else if (strcmp(input, "cd") == 0) {
        return BI_CMD_CD;
    } else if (strcmp(input, "dragon") == 0) {
        return BI_CMD_DRAGON;
    } else if (strcmp(input, "rc") == 0) {
        return BI_EXECUTED;
    } else {
        return BI_NOT_BI;
    }
}

Built_In_Cmds exec_built_in_cmd(cmd_buff_t *cmd) {
    if (cmd->argc == 0) {
        return BI_NOT_BI;
    }
    Built_In_Cmds cmd_type = match_command(cmd->argv[0]);
    switch (cmd_type) {
        case BI_CMD_CD:
            if (cmd->argc == 1) {
                return BI_EXECUTED;
            } else if (cmd->argc == 2) {
                if (chdir(cmd->argv[1]) != 0) {
                    perror("cd");
                    return BI_EXECUTED;
                }
            } else {
                fprintf(stderr, "cd: too many arguments\n");
                return BI_EXECUTED;
            }
            break;
        case BI_CMD_EXIT:
            return BI_CMD_EXIT;
        case BI_CMD_DRAGON:
            print_dragon();
            return BI_EXECUTED;
        case BI_EXECUTED:
            printf("%d\n", lastReturn);
            return BI_EXECUTED;
        default:
            return BI_NOT_BI;
    }
    return BI_EXECUTED;
}
int clear_cmd_buff(cmd_buff_t *cmd_buff) {
    if (cmd_buff == NULL) {
        return ERR_MEMORY;
    }
    if (cmd_buff->_cmd_buffer != NULL) {
        memset(cmd_buff->_cmd_buffer, 0, SH_CMD_MAX);
    }
    cmd_buff->argc = 0;
    for (int i = 0; i < CMD_ARGV_MAX; i++) {
        cmd_buff->argv[i] = NULL;
    }
    return OK;
}
//ec from as5
void printError(int err) {
    switch (err) {
        case ENOENT:
            printf("Command not found in PATH\n");
            break;
        case EACCES:
            printf("Permission denied\n");
            break;
        case ENOEXEC:
            printf("Executable format error\n");
            break;
        default:
            printf("Command execution failed with error: %d\n", err);
            break;
    }
}

int exec_local_cmd_loop()
{
    char *cmd_buff = malloc(SH_CMD_MAX * sizeof(char));
    if (cmd_buff == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return ERR_MEMORY;
    }
    
    int rc = 0;
    cmd_buff_t cmd;
    if (alloc_cmd_buff(&cmd) != OK) {
        free(cmd_buff);
        return ERR_MEMORY;
    }

    while(1) {
        printf("%s", SH_PROMPT);
        if (fgets(cmd_buff, ARG_MAX, stdin) == NULL) {
            printf("\n");
            break;
        }
        cmd_buff[strcspn(cmd_buff, "\n")] = '\0';
        int pipeC = 0;
        for (int i = 0; cmd_buff[i]; i++) {
            if (cmd_buff[i] == '|') {
                pipeC++;
            }
        }
        if (pipeC >= CMD_MAX) {
            printf(CMD_ERR_PIPE_LIMIT, CMD_MAX);
            continue;
        }
        if (pipeC == 0) {
            rc = build_cmd_buff(cmd_buff, &cmd);
            if (rc != OK) {
                if (rc == WARN_NO_CMDS) {
                    printf(CMD_WARN_NO_CMD);
                } else if (rc == ERR_TOO_MANY_COMMANDS) {
                    printf(CMD_ERR_PIPE_LIMIT, CMD_MAX);
                } else {
                    printf("Error parsing command: %d\n", rc);
                }
                continue;
            }
            Built_In_Cmds cmd_type = match_command(cmd.argv[0]);
            if (cmd_type == BI_CMD_CD) {
                exec_built_in_cmd(&cmd);
            } else if (cmd_type == BI_CMD_EXIT) {
                exit(0);
            } else if (cmd_type == BI_CMD_DRAGON) {
                exec_built_in_cmd(&cmd);
            } 
            else if (cmd_type == BI_EXECUTED) {
                printf("%d\n", lastReturn);
            } 
            else if (cmd_type == BI_NOT_BI) {
                pid_t pid = fork();
                if (pid < 0) {
                    fprintf(stderr, "Fork failed\n");
                    lastReturn = ERR_MEMORY;
                } else if (pid == 0) {
                //ec
                    char *input_redir = NULL;
                    char *output_redir = NULL;
                    int output_mode = 0;

                    if (strlen(cmd._cmd_buffer) > 0) {
                        char *token = strtok(cmd._cmd_buffer, ":");
                        while (token != NULL) {
                            if (strcmp(token, "I") == 0) {
                                token = strtok(NULL, ":");
                                if (token) {
                                    input_redir = token;
                                }
                            } else if (strcmp(token, "O") == 0) {
                                token = strtok(NULL, ":");
                                if (token) {
                                    output_redir = token;
                                }
                                token = strtok(NULL, ":");
                                if (token) {
                                    output_mode = atoi(token);
                                }
                            }
                            token = strtok(NULL, ":");
                        }
                    }
                    if (input_redir) {
                        int input_fd = open(input_redir, O_RDONLY);
                        if (input_fd < 0) {
                            perror("input redirection");
                            exit(1);
                        }
                        dup2(input_fd, STDIN_FILENO);
                        close(input_fd);
                    }
                
                    if (output_redir) {
                        int output_fd;
                        if (output_mode == 0) {
                            output_fd = open(output_redir, 
                                             O_WRONLY | O_CREAT | O_TRUNC, 
                                             0644);
                        } else {
                            output_fd = open(output_redir, 
                                             O_WRONLY | O_CREAT | O_APPEND, 
                                             0644);
                        }
                        
                        if (output_fd < 0) {
                            perror("output redirection");
                            exit(1);
                        }
                        dup2(output_fd, STDOUT_FILENO);
                        close(output_fd);
                    }
                    
                    execvp(cmd.argv[0], cmd.argv);
                    int err = errno;
                    printError(err);
                    exit(err);
                } else {
                    int status;
                    waitpid(pid, &status, 0);
                    if (WIFEXITED(status)) {
                        lastReturn = WEXITSTATUS(status);
                    } else {
                        lastReturn = ERR_EXEC_CMD;
                    }
                }
            }
        } 
        //else free
        else {
            int pipes[CMD_MAX - 1][2];
            pid_t child_pids[CMD_MAX];
            char *cmd_parts[CMD_MAX];
            int numCmds = pipeC + 1;
            cmd_parts[0] = cmd_buff;
            int curr = 1;
            for (int i = 0; cmd_buff[i]; i++) {
                if (cmd_buff[i] == '|') {
                    cmd_buff[i] = '\0';
                    cmd_parts[curr++] = &cmd_buff[i + 1];
                }
            }
            for (int i = 0; i < numCmds - 1; i++) {
                if (pipe(pipes[i]) < 0) {
                    perror("pipe");
                    break;
                }
            }
            for (int i = 0; i < numCmds; i++) {
                clear_cmd_buff(&cmd);
                rc = build_cmd_buff(cmd_parts[i], &cmd);
                if (rc != OK) {
                    continue;
                }
                Built_In_Cmds cmd_type = match_command(cmd.argv[0]);
                if (cmd_type == BI_NOT_BI) {
                    child_pids[i] = fork();
                    if (child_pids[i] < 0) {
                        perror("fork");
                        continue;
                    }
                    if (child_pids[i] == 0){
                        if (i > 0) {
                            if (dup2(pipes[i-1][0], STDIN_FILENO) < 0) {
                                perror("dup2 input");
                                exit(1);
                            }
                        }
                        if (i < numCmds - 1) {
                            if (dup2(pipes[i][1], STDOUT_FILENO) < 0) {
                                perror("dup2 output");
                                exit(1);
                            }
                        }
                        for (int j = 0; j < numCmds - 1; j++) {
                            close(pipes[j][0]);
                            close(pipes[j][1]);
                        }
                        execvp(cmd.argv[0], cmd.argv);
                        int err = errno;
                        printError(err);
                        exit(err);
                    }
                } 
                else {
                    rc = exec_built_in_cmd(&cmd);
                    if (rc == BI_CMD_EXIT) {
                        break;
                    }
                }
            }
            for (int i = 0; i < numCmds - 1; i++) {
                close(pipes[i][0]);
                close(pipes[i][1]);
            }
            for (int i = 0; i < numCmds; i++) {
                if (child_pids[i] > 0) {
                    int status;
                    waitpid(child_pids[i], &status, 0);
                    if (WIFEXITED(status)) {
                        lastReturn = WEXITSTATUS(status);
                    }
                }
            }
        }
    }
    //free
    free_cmd_buff(&cmd);
    free(cmd_buff);
    return OK;
}
