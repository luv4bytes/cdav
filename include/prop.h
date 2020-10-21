#ifndef PROP_H
#define PROP_H

// Defines a WebDAV property.
typedef struct cdav_prop_t
{
	const char* name;
	const char* text_val;

	struct cdav_prop_t** children;
	int children_size;

} CDAV_PROP;

/// Creates a new property.
CDAV_PROP*
cdav_new_prop();

/// Frees the memory used by the property.
void
cdav_free_prop(CDAV_PROP* prop);

/// Adds the child to the parents children list.
void
cdav_prop_add_child(CDAV_PROP* parent, CDAV_PROP* child);

#endif // PROP_H
