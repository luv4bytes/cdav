#include "../include/cdav_req_creator.h"

CDAV_PROP*
cdav_new_prop()
{
	CDAV_PROP* prop = (CDAV_PROP*) malloc(sizeof(CDAV_PROP));

	prop->name = NULL;
	prop->text_val = NULL;
	prop->children_size = 0;
	prop->children = NULL;

	return prop;
}

void
cdav_free_prop(CDAV_PROP* prop)
{
	if (prop == NULL)
		return;

	for(int i = 0; i < prop->children_size; i++)
	{
		cdav_free_prop(prop->children[i]);
	}

	if (prop->name != NULL)
		free((char*)prop->name);

	if (prop->text_val != NULL)
		free((char*)prop->text_val);

	free(prop);
}

void
cdav_prop_add_child(CDAV_PROP* parent, CDAV_PROP* child)
{
	if (parent == NULL)
		return;

	if (child == NULL)
		return;

	parent->children = (CDAV_PROP**) realloc(parent->children, sizeof(CDAV_PROP) * ++parent->children_size);

	parent->children[parent->children_size - 1] = child;
}

char*
cdav_req_propfind(CDAV_PROP** properties)
{
	// TODO:

	return NULL;
}
