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

#ifndef EC_LKLIST_H
#define EC_LKLIST_H

#include <stdlib.h>

#ifndef EC_BOOL

    #define EC_BOOL
    #define EC_TRUE 1
    #define EC_FALSE !EC_TRUE

    typedef int ec_bool;
    
#endif

/// Defines a node in a singly linked list.
typedef struct ec_lklist_node_st
{
    struct ec_lklist_node_st* next;
    void* data;

} ec_lklist_node;

/// Defines a linked list.
typedef struct ec_lklist_st
{
    size_t size;
    ec_lklist_node* nodes;

} ec_lklist;

typedef ec_bool (*predicate)(ec_lklist_node* node);

/// Initializes the given node.
ec_bool
ec_lklist_node_init(ec_lklist_node* node);

/// Initializes the given list.
ec_bool
ec_lklist_init(ec_lklist* list);

/// Adds the given node to the list.
ec_bool
ec_lklist_add(ec_lklist* list, ec_lklist_node* node);

/// Gets a list of nodes where given predicate evaluates to EC_TRUE.
/// Returned list needs to be freed manually.
ec_lklist_node**
ec_lklist_get_where(ec_lklist* list, size_t* count_out, predicate where);

/// Removes the given node from the list.
ec_bool
ec_lklist_remove(ec_lklist* list, ec_lklist_node* node);

/// Removes all nodes from the list where the predicate evaluates to EC_TRUE.
ec_bool
ec_lklist_remove_where(ec_lklist* list, size_t* count_out, predicate where);

#endif // EC_LKLIST_H