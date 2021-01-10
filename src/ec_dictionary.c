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

#include "../include/ec_dictionary.h"

/* Using DJB2 hash function by Dan Bernstein */
static unsigned int
hash_index(char* key, size_t capacity)
{
    if (key == NULL)
        return -1;

    unsigned long hash = 7811500520149435817;
    int c;

    while ((c = *key++) != 0)
        hash = ((hash << 5) + hash) + c;

    return hash % capacity;
}

ec_bool
ec_dict_node_init(ec_dict_node* node)
{
    if (node == NULL)
        return EC_FALSE;

    node->data = NULL;
    node->key = NULL;

    return EC_TRUE;
}

ec_bool
ec_dict_init(ec_dictionary* dict, size_t initialCapacity)
{
    if (dict == NULL)
        return EC_FALSE;

    dict->used = 0;
    dict->increase_by = 25;
    dict->capacity = initialCapacity;

    dict->table = (ec_dict_node**) calloc(initialCapacity, sizeof(ec_dict_node*));

    return EC_TRUE;
}

ec_bool
ec_dict_add(ec_dictionary* dict, ec_dict_node* node)
{
    if (dict == NULL)
        return EC_FALSE;

    if (dict->used == dict->capacity)
    {
        size_t oldcap = dict->capacity;

        dict->capacity = dict->used + dict->increase_by;
        ec_dict_node** new_table = (ec_dict_node**) calloc(dict->capacity, sizeof(ec_dict_node*));

        for(size_t i = 0; i < oldcap; i++)
        {
            if (dict->table[i] != NULL)
                new_table[hash_index(dict->table[i]->key, dict->capacity)] = dict->table[i];
        }
        
        free(dict->table);
        dict->table = new_table;
    }

    dict->used++;
    dict->table[hash_index(node->key, dict->capacity)] = node;

    return EC_TRUE;
}

ec_dict_node*
ec_dict_get(ec_dictionary* dict, char* key)
{   
    if (key == NULL || key[0] == 0)
        return NULL;
    
    return dict->table[hash_index(key, dict->capacity)];
}

ec_bool
ec_dict_remove(ec_dictionary* dict, char* key)
{
    if (dict == NULL)
        return EC_FALSE;

    if (dict->used == dict->capacity - dict->increase_by)
    {
        size_t oldcap = dict->capacity;

        dict->capacity = dict->capacity - dict->increase_by;
        ec_dict_node** new_table = (ec_dict_node**) calloc(dict->capacity, sizeof(ec_dict_node*));

        for(size_t i = 0; i < oldcap; i++)
        {
            if (dict->table[i] != NULL)
                new_table[hash_index(dict->table[i]->key, dict->capacity)] = dict->table[i];
        }
        
        free(dict->table);
        dict->table = new_table;
    }

    dict->table[hash_index(key, dict->capacity)] = NULL;
    dict->used--;

    return EC_FALSE;
}

void
ec_dict_free(ec_dictionary* dict)
{
    if (dict == NULL)
        return;

    if (dict->table != NULL)
        free(dict->table);
}