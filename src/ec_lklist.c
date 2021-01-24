/*
 * Copyright 2020 Lukas Pfeifer
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation the rights to use, 
 * copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "../include/ec_lklist.h"

ec_bool
ec_lklist_node_init(ec_lklist_node* node)
{
    if (node == NULL)
        return EC_FALSE;

    node->data = NULL;
    node->next = NULL;

    return EC_TRUE;
}

ec_bool
ec_lklist_init(ec_lklist* list)
{
    if (list == NULL)
        return EC_FALSE;

    list->size = 0;
    list->nodes = NULL;

    return EC_FALSE;
}

ec_bool
ec_lklist_add(ec_lklist* list, ec_lklist_node* node)
{
    if (list == NULL)
        return EC_FALSE;

    ec_lklist_node* nd = list->nodes;

    if (nd == NULL)
    {
        list->nodes = node;
        list->size++;

        return EC_TRUE;
    }

    while(nd != NULL)
    {
        if (nd->next == NULL)
        {
            nd->next = node;
            list->size++;

            return EC_TRUE;
        }

        nd = nd->next;
    }

    return EC_FALSE;
}

ec_lklist_node**
ec_lklist_get_where(ec_lklist* list, size_t* count_out, predicate where)
{
    ec_lklist_node** result = NULL;
    size_t size = 0;
    *count_out = 0;

    if (list == NULL)
        return NULL;

    ec_lklist_node* node = list->nodes;

    while(node != NULL)
    {
        if (where(node) == EC_TRUE)
        {
            result = (ec_lklist_node**) realloc(result, sizeof(ec_lklist_node*) * ++size);
            result[size - 1] = node;
        }

        node = node->next;
    }

    *count_out = size;
    return result;
}

ec_bool
ec_lklist_remove(ec_lklist* list, ec_lklist_node* node)
{
    if (list == NULL)
        return EC_FALSE;

    ec_lklist_node* nd = list->nodes;
    ec_lklist_node* last_nd = NULL;

    while(nd != NULL)
    {
        if (nd == node)
        {
            if (last_nd == NULL)
            {
                list->nodes = node->next;
                list->size--;

                return EC_TRUE;
            }

            last_nd->next = nd->next;
            list->size--;

            return EC_TRUE;
        }

        last_nd = nd;
        nd = nd->next;
    }

    return EC_FALSE;
}

ec_bool
ec_lklist_remove_where(ec_lklist* list, size_t* count_out, predicate where)
{
    if (list == NULL)
        return EC_FALSE;

    *count_out = 0;

    ec_lklist_node* node = list->nodes;

    while(node != NULL)
    {
        if (where(node) == EC_TRUE)
        {
            if (ec_lklist_remove(list, node) == EC_TRUE)
                (*count_out)++;
        }

        node = node->next;
    }

    return EC_TRUE;
}