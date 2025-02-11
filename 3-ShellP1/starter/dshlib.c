
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "dshlib.h"

/*
 *  build_cmd_list
 *    cmd_line:     the command line from the user
 *    clist *:      pointer to clist structure to be populated
 *
 *  This function builds the command_list_t structure passed by the caller
 *  It does this by first splitting the cmd_line into commands by spltting
 *  the string based on any pipe characters '|'.  It then traverses each
 *  command.  For each command (a substring of cmd_line), it then parses
 *  that command by taking the first token as the executable name, and
 *  then the remaining tokens as the arguments.
 *
 *  NOTE your implementation should be able to handle properly removing
 *  leading and trailing spaces!
 *
 *  errors returned:
 *
 *    OK:                      No Error
 *    ERR_TOO_MANY_COMMANDS:   There is a limit of CMD_MAX (see dshlib.h)
 *                             commands.
 *    ERR_CMD_OR_ARGS_TOO_BIG: One of the commands provided by the user
 *                             was larger than allowed, either the
 *                             executable name, or the arg string.
 *
 *  Standard Library Functions You Might Want To Consider Using
 *      memset(), strcmp(), strcpy(), strtok(), strlen(), strchr()
 */
int build_cmd_list(char *cmd_line, command_list_t *clist){
	clist->num = 0;
    memset(clist->commands, 0, sizeof(command_t) * CMD_MAX);
    if (strlen(cmd_line) == 0){
        return WARN_NO_CMDS;
    }
    char inputCopy[SH_CMD_MAX];
    strncpy(inputCopy, cmd_line, SH_CMD_MAX - 1);
    inputCopy[SH_CMD_MAX - 1] = '\0';
    char *temp, *curr = strtok_r(inputCopy, "|", &temp);
    while (curr) {
        if (clist->num >= CMD_MAX){
            return ERR_TOO_MANY_COMMANDS;
        }
        while (isspace(*curr)) curr++;
        char *end = curr + strlen(curr) - 1;
        while (end > curr && isspace(*end)) end--;
        *(end + 1) = '\0';
        if (strlen(curr) == 0) {
            curr = strtok_r(NULL, "|", &temp);
            continue;
        }
        char *token, *executable = strtok_r(curr, " ", &token);
        if (!executable){return WARN_NO_CMDS;}
        if (strlen(executable) >= EXE_MAX){
            return ERR_CMD_OR_ARGS_TOO_BIG;
        }
        strcpy(clist->commands[clist->num].exe, executable);
        char args[ARG_MAX] = "";
        while ((executable = strtok_r(NULL, " ", &token))) {
            if (strlen(args) + strlen(executable) + 2 >= ARG_MAX){ return ERR_CMD_OR_ARGS_TOO_BIG;}
            if (strlen(args)){ strcat(args, " ");}
            strcat(args, executable);
        }
        strcpy(clist->commands[clist->num].args, args);
        clist->num++;
        curr = strtok_r(NULL, "|", &temp);
    }

    return clist->num ? OK : WARN_NO_CMDS;    
}
