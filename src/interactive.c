/*
 * Copyright (C) 2020 Lukas Pfeifer
 * for cdav version 1.0
 *      https://github.com/luv4bytes/cdav
 *
 * This file is part of cdav.
 *
 * cdav is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * cdav is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with cdav. If not, see <http://www.gnu.org/licenses/>.
 *
*/

#include "../include/interactive.h"

static void
rmnl(char* s1)
{
    if (!s1)
        return;

    char* end = strchr(s1, '\n');
    *end = 0;
}

static char*
_fgets(char* into, int sz, int* empty)
{
    if (!into)
        return NULL;

    char* check = fgets(into, sz, stdin);
    rmnl(into);
    
    if (into[0] == 0)
        *empty = 1;

    return check;
}

void
intac_print_help()
{
    // TODO: Print help

    printf("This will be the help text!\n");
}

void
intac_add_cmd(const char* cmd, intacFunc fnc)
{
    ec_dict_node* new = (ec_dict_node*) malloc(sizeof(ec_dict_node));
    ec_dict_node_init(new);

    new->key = (char*)cmd;
    new->data = fnc;

    ec_dict_add(&COMMANDS, new);
}

void
intac_init()
{
    ec_dict_init(&COMMANDS, 10);
    
    intac_add_cmd("exit", intac_exit);
    intac_add_cmd("quit", intac_exit);
    intac_add_cmd("help", intac_print_help);
    intac_add_cmd("run", intac_run);
    intac_add_cmd("test", intac_test_connect);
}

void
intac_run()
{
    FILE* p = NULL;
    char* check = NULL;

    char cmd[INPUT_SZ];
    memset(cmd, 0, INPUT_SZ);

    int no_command = 0;
    printf("Command > ");
    check = _fgets(cmd, INPUT_SZ, &no_command);

    if (no_command)
        return;

    if (!check)
        ERROR_EXIT("%s\n", strerror(ferror(stdin)))
        
    p = popen(cmd, "r");

    if (!p)
        ERROR_EXIT("%s\n", strerror(errno))
 
    char* line = NULL;
    ssize_t read = 0;
    size_t n = 0;

    while((read = getline(&line, &n, p)) != -1)
        printf("%s", line);
 
    if (line)
        free(line);

    pclose(p);
}

void
intac_test_connect()
{
    CURL* curl = curl_easy_init();

    char target[INPUT_SZ];
    memset(target, 0, INPUT_SZ);

    char* check = NULL;

    int no_command = 0;
    printf("Target > ");
    check = _fgets(target, INPUT_SZ, &no_command);

    if (no_command)
        return;

    if (!check)
        ERROR_EXIT("%s\n", strerror(ferror(stdin)))

    curl_easy_setopt(curl, CURLOPT_URL, target);
    curl_easy_setopt(curl, CURLOPT_CONNECT_ONLY, 1L);

    printf("Trying to connect to %s ...\n", target);

    CURLcode ret = curl_easy_perform(curl);

    if (ret != CURLE_OK)
    {
        INTAC_ERROR("%s\n", curl_easy_strerror(ret))
        return;
    }

    char* ip = NULL;
    curl_easy_getinfo(curl, CURLINFO_PRIMARY_IP, &ip);

    printf("%s [%s] available for communication\n", target, ip);

    curl_easy_cleanup(curl);
}

void
intac_exit()
{
    printf("Goodbye!\n");
    exit(0);
}

void
intac_session()
{
    intac_init();

    printf("This is an interactive session of cdav. Please type \"help\" to see further information about interactive mode.\n\n");

    char cmd[INPUT_SZ];

    while (TRUE)
    {
        memset(cmd, 0, INPUT_SZ);
        char* check = NULL;

        int no_command = 0;
        printf("> ");
        check = _fgets(cmd, INPUT_SZ, &no_command);

        if (check == NULL)
            ERROR_EXIT("%s\n", "Error getting input!")

        if (no_command)
            continue;

        ec_dict_node* found = ec_dict_get(&COMMANDS, cmd);

        if (found == NULL)
            INTAC_INVALID

        intacFunc f = (intacFunc)found->data;
        f();
    }

    exit(0);
}
