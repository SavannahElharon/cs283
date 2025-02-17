#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "dshlib.h"
#include <errno.h>
//global int for rc
int lastReturn = 0;
//implementation of prototypes
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
    int argc = 0;
    char *p = cmd_line;
    int quotes = 0;
    while (*p && argc < CMD_ARGV_MAX - 1) {
        while (*p == ' ' && !quotes){
        p++;
        }
        if (*p == '\0'){
        break;
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
        return ERR_TOO_MANY_COMMANDS;
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
        return BI_RC;
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
                return BI_RC;
            } else if (cmd->argc == 2) {
                if (chdir(cmd->argv[1]) != 0) {
                    perror("cd");
                    return BI_RC;
                }
            } else {
                fprintf(stderr, "cd: too many arguments\n");
                return BI_RC;
            }
            break;
        case BI_CMD_EXIT:
            return BI_CMD_EXIT;
        case BI_CMD_DRAGON:
            print_dragon();
            return BI_RC;
        case BI_RC:
            printf("%d\n", lastReturn);
            return BI_RC;
        default:
            return BI_NOT_BI;
    }
    return BI_RC;
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
//ec
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
int exec_local_cmd_loop()
{
    char *cmd_buff= malloc(SH_CMD_MAX * sizeof(char));
    if (cmd_buff == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return ERR_MEMORY;
    };
    int rc = 0;
    cmd_buff_t cmd;
    if (alloc_cmd_buff(&cmd) != OK) {
        free(cmd_buff);
        return ERR_MEMORY;
    }
	while(1){
    	printf("%s", SH_PROMPT);
    	if (fgets(cmd_buff, ARG_MAX, stdin) == NULL){
	    	printf("\n");
 			break;
 	}
	//remove the trailing \n from cmd_buff
	cmd_buff[strcspn(cmd_buff,"\n")] = '\0';
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
    // TODO IMPLEMENT if built-in command, execute builtin logic for exit, cd (extra credit: dragon)
    // the cd command should chdir to the provided directory;
    //check cmd_type and exec
	if(cmd_type== BI_CMD_CD){
		exec_built_in_cmd(&cmd);
	}
	else if(cmd_type == BI_CMD_EXIT){
		exit(0);
	}else if(cmd_type == BI_CMD_DRAGON){
		 exec_built_in_cmd(&cmd);
	} else if (cmd_type == BI_RC) {
		printf("%d\n", lastReturn);
	}else if (cmd_type == BI_NOT_BI) {//if not built in fork/exec
		pid_t pid = fork();
        if (pid < 0){
            fprintf(stderr, "Fork failed\n");
            lastReturn = ERR_MEMORY;
        } else if (pid == 0){
            execvp(cmd.argv[0], cmd.argv);
            int err = errno;
            printError(err);
            exit(err);
        } else {
        	int status;
            waitpid(pid, &status, 0);
            if (WIFEXITED(status)) {
                lastReturn = WEXITSTATUS(status);
    		}else{
    			lastReturn = ERR_EXEC_CMD;
        		}
        }
    }
}
//free mem
free_cmd_buff(&cmd);
free(cmd_buff);
return OK;
}
