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

#define TRUE 1
#define FALSE 0

#define INVALID_COMMAND {printf("%s\n", "Please provide valid command. For an overview of commands type 'help'."); continue;}

/// Defines IDs for commands.
typedef enum
{
    EXIT,
    HELP,
    TEST_CONNECTION

} COMMAND_IDS;

/// Defines commands used by cdav interactive.
ec_dictionary COMMANDS;

/// Adds a new command with given name and ID to COMMANDS.
void
intac_add_cmd(const char* cmd, COMMAND_IDS id);

/// Initializes structures for interactive mode.
void
intac_init();

/// Starts an interactive session of cdav.
void
__attribute__((noreturn)) interactive_session();

#endif // INTERACTIVE_H