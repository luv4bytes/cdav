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
#include "ec_dictionary.h"
#include "curl/curl.h"

#define TRUE 1
#define FALSE 0

#define INPUT_SZ 512

#define INTAC_INVALID {printf("%s\n", "Please provide valid command. For an overview of commands type 'help'."); continue;}
#define INTAC_ERROR(...) {printf("ERROR:\n"); printf(__VA_ARGS__);}

/// Interactive function pointer.
typedef void(*intacFunc)();

/// Defines IDs for commands.
typedef enum
{
    EXIT,
    HELP,
    TEST_CONNECTION,
    RUN

} COMMAND_IDS;

/// Defines commands used by cdav interactive.
ec_dictionary COMMANDS;

/// Adds a new command with given name and function.
void
intac_add_cmd(const char* cmd, intacFunc fnc);

/// Initializes structures for interactive mode.
void
intac_init();

/// Prints the help text for interactive mode.
void
intac_print_help();

/// Prompts the user to issue a command. ping, make... etc.
void
intac_run();

/// Performs a test of a connection.
void
intac_test_connect();

/// Exits the program.
void
intac_exit();

/// Starts an interactive session of cdav.
void
__attribute__((noreturn)) intac_session();

#endif // INTERACTIVE_H