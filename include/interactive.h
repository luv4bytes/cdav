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

#ifndef INTERACTIVE_H
#define INTERACTIVE_H

#include "cdav.h"
#include "args.h"
#include "err.h"
#include "helper.h"
#include "requests.h"
#include "dav.h"
#include "curl/curl.h"

#define TRUE 1
#define FALSE 0

#define INPUT_SZ 512

#define INTAC_INVALID {printf("%s\n", "Please provide valid command. For an overview of commands type 'help'."); continue;}
#define INTAC_ERROR(...) {printf("ERROR:\n"); printf(__VA_ARGS__);}

#define INTAC_CHECK_CURL { if (session.curlHandle == NULL) { printf("No CURL handle found. Make sure you have a running connection.\n"); return; }}
#define INTAC_CHECK_URL { if (str_null_empty(session.url)) { printf("No URL defined, please assign a URL via 'url'.\n"); return; }}

/// Interactive function pointer.
typedef void(*intacFunc)();

/// Defines IDs for commands.
typedef enum
{
    EXIT,
    HELP,
    TEST_CONNECTION,
    RUN

} INTAC_COMMAND_IDS;

/// Struct to define a command and associated function.
typedef struct intac_cmd_st
{
    const char* longCmd;
    const char* shortCmd;
    const char* description;
    intacFunc function;

} INTAC_CMD;

/// Defines session-wide values.
typedef struct intac_session_st
{
    char* url;
    char* rootDir;
    char* currentDir;
    char** contents;
    char* user;
    char* password;
    
    CURL* curlHandle;
        
} INTAC_SESSION;

INTAC_SESSION session;

#define CMD_COUNT 16
/// Defines commands used by cdav interactive.
extern INTAC_CMD INTAC_COMMANDS[CMD_COUNT];

/// Adds a new command with given name and function.
void
intac_add_cmd(const char* cmd, intacFunc fnc);

/// Gets the intacFunc associated with the given command.
intacFunc
intac_get_cmd(const char* cmd);

/// Prints the help text for interactive mode.
void
intac_print_help();

/// Initializes a new session.
void
intac_new_session();

/// Prints session information.
void
intac_print_session_info();

/// Deletes all values stored in the current session.
void
intac_clear_session();

/// Clears the screen.
void
intac_clear_screen();

/// Prompts the user to issue a command. ping, make... etc.
void
intac_run();

/// Performs a test of a connection.
void
intac_test_connect();

/// Prompts the user to enter the session URL.
void
intac_set_url();

/// Prompts the user to enter the session user.
void
intac_set_user();

/// Prompts the user to enter the session password.
void
intac_set_password();

/// Prompts the user to enter a directory that will be used as root directory.
void
intac_set_root_directory();

/// Connect to a target.
void
intac_connect();

/// List contents of current directory.
void
intac_list_dir();

/// Disconnect from a target.
void
intac_disconnect();

/// Exits the program.
void
intac_exit();

/// Starts an interactive session of cdav.
void
__attribute__((noreturn)) intac_start_session();

#endif // INTERACTIVE_H