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

#ifndef EC_DICTIONARY_H
#define EC_DICTIONARY_H

#include <stdlib.h>

#ifndef EC_BOOL

    #define EC_BOOL
    #define EC_TRUE 1
    #define EC_FALSE !EC_TRUE

    typedef int ec_bool;
    
#endif

/// Defines a node in a dictionary.
typedef struct ec_dict_node_st
{
    char* key;
    void* data;

} ec_dict_node;

/// Defines a dictionary.
typedef struct ec_dictionary_st
{
    size_t increase_by;
    size_t capacity;
    size_t used;
    ec_dict_node** table;

} ec_dictionary;

/// Initializes the given dictionary.
ec_bool
ec_dict_init(ec_dictionary* dict, size_t initialCapacity);

/// Initializes the given node.
ec_bool
ec_dict_node_init(ec_dict_node* node);

/// Adds the given node to the dictionary.
ec_bool
ec_dict_add(ec_dictionary* dict, ec_dict_node* node);

/// Returns the ec_dict_node* with the given key or NULL if not found.
ec_dict_node*
ec_dict_get(ec_dictionary* dict, char* key);

/// Removes the node with given key from the given from the dictionary.
ec_bool
ec_dict_remove(ec_dictionary* dict, char* key);

/// Frees the memory used by the internal table. The passed dictionary needs to be freed manually if neccessary.
void
ec_dict_free(ec_dictionary* dict);

#endif // EC_DICTIONARY_H