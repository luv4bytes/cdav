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

// TODO: Help

void
intac_add_cmd(const char* cmd, COMMAND_IDS id)
{
    ec_dict_node* new = (ec_dict_node*) malloc(sizeof(ec_dict_node));
    ec_dict_node_init(new);

    new->key = (char*)cmd;
    new->data = (COMMAND_IDS*)malloc(sizeof(COMMAND_IDS));
    *((COMMAND_IDS*)new->data) = id;

    ec_dict_add(&COMMANDS, new);
}

void
intac_init()
{
    ec_dict_init(&COMMANDS, 10);
    
    intac_add_cmd("exit", EXIT);
    intac_add_cmd("quit", EXIT);
    intac_add_cmd("help", HELP);
    intac_add_cmd("test", TEST_CONNECTION);
}

void
interactive_session()
{
    intac_init();

    printf("This is an interactive session of cdav. Please type \"help\" to see further information about interactive mode.\n\n");

    while (TRUE)
    {
        char* cmd = NULL;

        printf("> ");
        if (scanf("%ms", &cmd) == 0)
            INVALID_COMMAND

        ec_dict_node* found = ec_dict_get(&COMMANDS, cmd);

        if (found == NULL)
            INVALID_COMMAND

        switch(*((COMMAND_IDS*)found->data))
        {
            case EXIT:
                printf("Goodbye!\n");
                exit(0);

            case HELP:
                break;

            case TEST_CONNECTION:
                break;
        }

        if (cmd != NULL)
            free(cmd);
    }

    exit(0);
}