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

#include "../include/response.h"

#define INDENT_PRINT(level, ...) do { for(int i = 0; i < level; i++) { printf("%s", "\t");} printf(__VA_ARGS__); } while(0);
#define is_xml(raw) (strstr(raw, "<?xml") == NULL)

static void
print_node(xmlNodePtr node, int* level)
{
    int now = *level;
    const char* name = (const char*)node->name;

    if (!str_is_equal(name, "multistatus"))
    {
        if (node->type == XML_ELEMENT_NODE)
        {
            if (node->children != NULL)
            {
                if (node->children->type == XML_TEXT_NODE)
                    INDENT_PRINT(*level, "%s = ", node->name)
                else
                {
                    if (!str_is_equal(name, "propstat"))
                    {
                        if (str_is_equal(name, "response"))
                        {
                            INDENT_PRINT(*level, "\e[1m\e[92m%s\e[0m\n", "RESPONSE")
                            INDENT_PRINT(*level, "\t|\n\n")
                            (*level)++;
                        }
                        else
                        {
                            if (str_is_equal(name, "prop"))
                            {
                                INDENT_PRINT(*level, "\e[1m\e[34m%s\e[0m\n", "PROP")
                                INDENT_PRINT(*level, "\t|\n\n")
                                
                                (*level)++;
                            }else
                            {
                                INDENT_PRINT(*level, "%s\n", node->name)
                                INDENT_PRINT(*level, "\t|\n\n")
                                (*level)++;
                            }
                        }
                    }
                }
            }
            else
                INDENT_PRINT(*level, "%s\n\n", node->name)
        }

        if (node->type == XML_TEXT_NODE)
            printf("%s\n\n", node->content);
    }

    xmlNodePtr children = node->children;

    while (children != NULL)
    {
        print_node(children, level);

        children = children->next;
    }

    *level = now;
}

void
print_sanitized_response(char* raw)
{
    if (is_xml(raw))
    {
        OUT_INFO("%s\n", raw)
        return;
    }

    xmlDocPtr doc = xmlParseDoc((const xmlChar *)raw);

    if (doc == NULL)
    {
        OUT_INFO("%s\n\n%s\n", "Parsing not possible. Printing raw response anyway...", raw)
        xmlFreeDoc(doc);

        return;
    }

    xmlNodePtr node = NULL;

    node = xmlDocGetRootElement(doc);

    if (node == NULL)
        ERROR_EXIT("%s\n", "Unable to create XML root node from response.")

    OUT_INFO("%s\n", "-- Sanitized response --")

    int level = 0;

    print_node(node, &level);

    xmlFreeDoc(doc);
}