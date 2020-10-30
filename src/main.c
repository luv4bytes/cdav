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

#include "../include/dav.h"

int main(int argc, char* argv[])
{
	// TODO: Argument parsing

	/*
	 * COMMAND FILE:
     *
     *  Options:
     *      -f --file       - specify command file path
     *      - pipe in command file
     *
     * OPERATIONS:
     *
     *  Options:
     *      -o --operation  - Specify operation
     *
     *      GET:
     *          -a  --address    - Specify address
     *          -u  --user       - Specify user
     *          -pw --password   - Specify password
     *          -s  --save-as    - Specify saving destination
     *
     *      PUT:
     *          -a  --address     - Specify address
     *          -u  --user        - Specify user
     *          -pw --password    - Specify password
     *          -uf --upload-file - Specify file for upload
     *
     *      PROPFIND:
     *          -a  --address    - Specify address
     *          -u  --user       - Specify user
     *          -pw --password   - Specify password
     *          -p  --props      - Specify properties, separated by comma
     *          -d  --depth      - Specify PROPFIND depth
     *
     *      PROPPATCH:
     *          -a  --address    - Specify address
     *          -u  --user       - Specify user
     *          -pw --password   - Specify password
     *          -sp --setprops   - Specify properties to be set, separated by comma (prop1=test,prop2=21)
     *          -rp --rmprops    - Specify properties to be removed, separated by comma (prop1,prop2)
     *
     *      MKCOL:
     *          -a  --address    - Specify address
     *          -u  --user       - Specify user
     *          -pw --password   - Specify password
     *
     *      DELETE:
     *          -a  --address    - Specify address
     *          -u  --user       - Specify user
     *          -pw --password   - Specify password
     *
     *      COPY:
     *          -a  --address               - Specify address
     *          -u  --user                  - Specify user
     *          -pw --password              - Specify password
     *          -da --destination-address   - Specify destination address
     *          -ow --overwrite             - Specify overwrite
     *
     *      MOVE:
     *          -a  --address               - Specify address
     *          -u  --user                  - Specify user
     *          -pw --password              - Specify password
     *          -da --destination-address   - Specify destination address
     *          -ow --overwrite             - Specify overwrite
     *
    */

	int init = curl_global_init(CURL_GLOBAL_ALL);

	if (init != CURLE_OK)
	{
		const char* err = curl_easy_strerror(init);
		fprintf(stderr, "CURL ERR: %d - %s\n", init, err);

		error_exit("CURL ERR: Exiting\n");
	}

#ifdef TEST
	printf("##### Running tests ##### \n");

	printf("##### Tests done #####\n");
#endif

	printf("Returned: %d\n", EXIT_SUCCESS);

	curl_global_cleanup();

	return EXIT_SUCCESS;
}
