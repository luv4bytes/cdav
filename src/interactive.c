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

INTAC_CMD INTAC_COMMANDS[CMD_COUNT] = 
{
    {"exit", "Quit the program.", intac_exit},
    {"quit", "Quit the program.", intac_exit},
    {"help", "Display help text.", intac_print_help},
    {"run", "Run a command on the system.", intac_run},
    {"test", "Test a connection endpoint.", intac_test_connect},
    {"url", "Set the session URL.", intac_set_url},
    {"user", "Set the session user.", intac_set_user},
    {"pw", "Set the session password.", intac_set_password},
    {"si", "Display session information.", intac_print_session_info},
    {"cs", "Clear session values.", intac_clear_session},
    {"cns", "Create a new session.", intac_new_session},
    {"connect", "Connect to a server.", intac_connect},
    {"clear", "Clear the screen.", intac_clear_screen}
};

void
intac_start_session()
{
    printf("This is an interactive session of cdav. Please type \"help\" to see further information about interactive mode.\n\n");

    intac_new_session();

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

        intacFunc found = intac_get_cmd(cmd);

        if (found == NULL)
            INTAC_INVALID

        found();
    }
}

void
intac_new_session()
{ 
    if (session.curlHandle == NULL)
        session.curlHandle = curl_easy_init();

    session.user = NULL;
    session.password = NULL;
    session.url = NULL;
    
    printf("New session created.\n");
}

void
intac_clear_session()
{
    if (session.url != NULL)
        free(session.url), session.url = NULL;

    if (session.user != NULL)
        free(session.user), session.user = NULL;

    if (session.password != NULL)
        free(session.password), session.password = NULL;

    if (session.curlHandle != NULL)
        curl_easy_cleanup(session.curlHandle), session.curlHandle = NULL;

    printf("Session cleared.\n");
}

void
intac_print_help()
{
    printf("Following commands can be issued to cdav when in interactive mode:\n\n");

    for(int i = 0; i < CMD_COUNT; i++)
        printf("%s\t- \t%s\n", INTAC_COMMANDS[i].name, INTAC_COMMANDS[i].description);

    printf("\n");
}

void
intac_print_session_info()
{
    printf("+++++ SESSION INFORMATION +++++\n");

    if (session.url != NULL)
        printf("URL: %s\n", session.url);

    if (session.user != NULL)
        printf("User: %s\n", session.user);

    if (session.password != NULL)
        printf("Password: *****\n");
}

void
intac_clear_screen()
{
    printf("\033[2J");
    printf("\033[H");
}

intacFunc
intac_get_cmd(const char* cmd)
{
    if (cmd == NULL)
       return NULL;

    for(int i = 0; i < CMD_COUNT; i++)
    {
        INTAC_CMD ci = INTAC_COMMANDS[i];
        
        if (strcmp(ci.name, cmd) == 0)
            return ci.function;
    }

    return NULL;
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

    if (!check)
        ERROR_EXIT("%s\n", strerror(ferror(stdin)))

    if (no_command)
        return;
        
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

    if (!check)
        ERROR_EXIT("%s\n", strerror(ferror(stdin)))

    if (no_command)
        return;

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
intac_set_url()
{
    char target[INPUT_SZ];
    memset(target, 0, INPUT_SZ);

    char* check = NULL;

    int no_command = 0;
    printf("URL > ");
    check = _fgets(target, INPUT_SZ, &no_command);

    if (!check)
        ERROR_EXIT("%s\n", strerror(ferror(stdin)))

    if (no_command)
        return;

    if (session.url != NULL)
        free(session.url), session.url = NULL;

    session.url = (char*) calloc(strlen(target), sizeof(char));
    strcpy(session.url, target);
}

void
intac_set_user()
{
    char user[INPUT_SZ];
    memset(user, 0, INPUT_SZ);

    char* check = NULL;

    int no_command = 0;
    printf("User > ");
    check = _fgets(user, INPUT_SZ, &no_command);

    if (!check)
        ERROR_EXIT("%s\n", strerror(ferror(stdin)))

    if (no_command)
        return;

    if (session.user != NULL)
        free(session.user), session.user = NULL;

    session.user = (char*) calloc(strlen(user), sizeof(char));
    strcpy(session.user, user);
}

void
intac_set_password()
{
    char password[INPUT_SZ];
    memset(password, 0, INPUT_SZ);

    char* check = NULL;

    int no_command = 0;
    printf("Password > ");
    check = _fgets(password, INPUT_SZ, &no_command);

    if (!check)
        ERROR_EXIT("%s\n", strerror(ferror(stdin)))

    if (no_command)
        return;

    if (session.password != NULL)
        free(session.password), session.password = NULL;

    session.password = (char*) calloc(strlen(password), sizeof(char));
    strcpy(session.password, password);
}

void
intac_connect()
{
    INTAC_CHECK_CURL
    INTAC_CHECK_URL

    curl_easy_setopt(session.curlHandle, CURLOPT_URL, session.url);

    // TODO:
}

void
intac_exit()
{
    intac_clear_session();
    curl_global_cleanup();

    printf("Goodbye!\n");

    exit(0);
}
