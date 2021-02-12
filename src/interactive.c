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
    {"exit", NULL, "Quit the program.", intac_exit},
    {"quit", NULL, "Quit the program.", intac_exit},
    {"clear", NULL, "Clear the screen.", intac_clear_screen},
    {"help", "h", "Display help text.", intac_print_help},
    {"run", "r", "Run a command on the system.", intac_run},
    {"test", "t", "Test a connection endpoint.", intac_test_connect},
    {"url", NULL, "Set the session URL.", intac_set_url},
    {"user", "u", "Set the session user.", intac_set_user},
    {"password", "pw", "Set the session password.", intac_set_password},
    {"rootdir", "rd", "Set root directory.", intac_set_root_directory},
    {"sessioninfo", "si", "Display session information.", intac_print_session_info},
    {"clear session", "cs", "Clear session values.", intac_clear_session},
    {"new session", "ns", "Create a new session.", intac_new_session},
    {"connect", "c", "Connect to a server.", intac_connect},
    {"disconnect", "dc", "Disconnect from a server.", intac_disconnect},
    {"list", "ls", "List contents of current directory.", intac_list_dir}

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
    session.user = NULL;
    session.password = NULL;
    session.url = NULL;
    session.currentDir = NULL;
    session.contents = NULL;
    session.contentCount = 0;

    session.rootDir = (char*) calloc(1, sizeof(char));
    strcat(session.rootDir, "/");

    session.curlHandle = NULL;
    
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

    if (session.rootDir != NULL)
        free(session.rootDir), session.rootDir = NULL;

    if (session.curlHandle != NULL)
        curl_easy_cleanup(session.curlHandle), session.curlHandle = NULL;

    if (session.contents != NULL)
    {
        for(size_t i = 0; i < session.contentCount; i++)
            if (session.contents[i] != NULL)
                free(session.contents[i]);
        
        free(session.contents);
    }

    printf("Session cleared.\n");
}

void
intac_print_help()
{
    const char* format = "|  %-15s|  %-15s|  %-30s\n";

    printf("Following commands can be issued to cdav when in interactive mode:\n\n");
    printf(format, "Long command", "Short command", "Description");
    printf("--------------------------------------------------------------\n");

    for(int i = 0; i < CMD_COUNT; i++)
    {   
        INTAC_CMD cmd = INTAC_COMMANDS[i];

        printf(format,  cmd.longCmd,
                                        cmd.shortCmd == NULL ? "" : cmd.shortCmd,
                                        cmd.description);
    }

    printf("\n");
}

static void
printDirectoryContents()
{
    if (session.contents != NULL)
    {
        printf("Current directory content:\n");

        for(size_t i = 0; i < session.contentCount; i++)
            printf("\t%s\n", session.contents[i]);
    }
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

    if (session.rootDir != NULL)
        printf("Root directory: %s\n", session.rootDir);

    if (session.currentDir != NULL)
        printf("Current directory: %s\n", session.currentDir);

    if (session.curlHandle != NULL)
    {
        char* ip = NULL;
        curl_easy_getinfo(session.curlHandle, CURLINFO_PRIMARY_IP, &ip);

        if (ip != NULL)
            printf("Connected to %s [%s]\n", session.url, ip);
    }

    printDirectoryContents();    
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
        
        if (strcmp(ci.longCmd, cmd) == 0)
            return ci.function;

        if (ci.shortCmd != NULL && strcmp(ci.shortCmd, cmd) == 0)
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

    char* input = getpass("Password > ");

    if (!input)
        ERROR_EXIT("%s\n", strerror(ferror(stdin)))

    strncpy(password, input, INPUT_SZ);

    if (session.password != NULL)
        free(session.password), session.password = NULL;

    session.password = (char*) calloc(strlen(password), sizeof(char));
    strcpy(session.password, password);
}

void
intac_set_root_directory()
{
    char rootdir[INPUT_SZ];
    memset(rootdir, 0, INPUT_SZ);

    char* check = NULL;

    int no_command = 0;
    printf("Root directory > ");
    check = _fgets(rootdir, INPUT_SZ, &no_command);

    if (!check)
        ERROR_EXIT("%s\n", strerror(ferror(stdin)))

    if (no_command)
        return;

    if (session.rootDir != NULL)
        free(session.rootDir), session.rootDir = NULL;

    session.rootDir = (char*) calloc(strlen(rootdir), sizeof(char));
    strcpy(session.rootDir, rootdir);
}

void
intac_connect()
{
    INTAC_CHECK_URL

    if (session.curlHandle == NULL)
        session.curlHandle = curl_easy_init();

    curl_easy_setopt(session.curlHandle, CURLOPT_URL, session.url);
    curl_easy_setopt(session.curlHandle, CURLOPT_TCP_KEEPALIVE, 1L);

    if (session.user != NULL && session.password != NULL)
    {
        curl_easy_setopt(session.curlHandle, CURLOPT_HTTPAUTH, CURLAUTH_BASIC | CURLAUTH_DIGEST);
        curl_easy_setopt(session.curlHandle, CURLOPT_PASSWORD, session.password);
        curl_easy_setopt(session.curlHandle, CURLOPT_USERNAME, session.user);
    }

    #ifdef IGNORE_SSL_ERRORS
        curl_easy_setopt(session.curlHandle, CURLOPT_SSL_VERIFYPEER, 0);
    #endif

    CURLcode result = curl_easy_perform(session.curlHandle);

    if (result != CURLE_OK)
    {
        INTAC_ERROR("%s\n", curl_easy_strerror(result))
        curl_easy_cleanup(session.curlHandle);
        session.curlHandle = NULL;
        return;
    }

    char* ip = NULL;
    curl_easy_getinfo(session.curlHandle, CURLINFO_PRIMARY_IP, &ip);

    printf("Connected to %s [%s]\n", session.url, ip);
}

static void
getDirContentFromPropfind(char* response)
{
    if (response == NULL)
        return;

    xmlDocPtr doc = xmlParseDoc((const xmlChar *)response);    

    if (doc == NULL)
        return;

    xmlNodePtr multistatus = doc->children;
    xmlNodePtr responseNode = multistatus->children;

    size_t count = 0;
    char** contents = NULL;
    char* temp = NULL;

    while(responseNode != NULL)
    {
        xmlNodePtr href = responseNode->children;
        xmlChar* path = href->children->content;

        temp = (char*) calloc(strlen((char*) path) + 1, sizeof(char));
        strcat(temp, (char*) path);

        contents = (char**) realloc(contents, sizeof(char*) * ++count);
        contents[count - 1] = temp;

        responseNode = responseNode->next;
    }

    session.contents = contents;
    session.contentCount = count;

    if (doc != NULL)
        xmlFreeDoc(doc);
}

void
intac_list_dir()
{
    INTAC_CHECK_CURL

    CDAV_PROP** list = (CDAV_PROP**) calloc(1, sizeof(CDAV_PROP*));
    CDAV_PROP* prop = cdav_new_prop();

    char resourcetype[] = "resourcetype";
    prop->name = calloc(strlen(resourcetype), sizeof(char));
    strcat((char*) prop->name, resourcetype);

    list[0] = prop;
    char* request = cdav_req_propfind(list, 1);

    char method[] = "PROPFIND";
    curl_easy_setopt(session.curlHandle, CURLOPT_CUSTOMREQUEST, method);
    curl_easy_setopt(session.curlHandle, CURLOPT_POSTFIELDS, request);
    curl_easy_setopt(session.curlHandle, CURLOPT_WRITEFUNCTION, cdav_receive_into_buffer);
    
	CDAV_RECV_BUFFER_PARAMS params;

	params.buffer = NULL;
	params.buffer_sz = 0;
	params.curl = session.curlHandle;

	curl_easy_setopt(session.curlHandle, CURLOPT_WRITEDATA, (void*) &params);
    
    CURLcode result = curl_easy_perform(session.curlHandle);

    if (result != CURLE_OK)
    {
        INTAC_ERROR("%s\n", curl_easy_strerror(result))
        goto free_mem;
    }

    getDirContentFromPropfind(params.buffer);

    printDirectoryContents();

    // TODO: Beautify output

free_mem:
    if (params.buffer != NULL)
        free(params.buffer);    

    free(request);
    free(list);
    cdav_free_prop(prop);
}

void
intac_disconnect()
{
    if (session.curlHandle == NULL)
        return;

    char* ip = NULL;
    curl_easy_getinfo(session.curlHandle, CURLINFO_PRIMARY_IP, &ip);
    printf("Disconnecting from %s [%s]\n", session.url, ip);

    curl_easy_cleanup(session.curlHandle);
    session.curlHandle = NULL;
}

void
intac_exit()
{
    intac_clear_session();
    curl_global_cleanup();

    printf("Goodbye!\n");

    exit(0);
}
