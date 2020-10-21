#ifndef REQUESTS_H
#define REQUESTS_H

#include "cdav.h"
#include "prop.h"

/// Creates a WebDAV PROPFIND request.
char*
cdav_req_propfind(CDAV_PROP** properties);

#endif // REQUESTS_H
