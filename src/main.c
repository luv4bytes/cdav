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
#include "../include/args.h"
#include "../include/cmdfile.h"

#define PRINT_VERSION printf("cdav %s\n", VERSION); // Version is defined by Makefile

/// Prints help text.
void
print_help()
{
	printf("\n");

	PRINT_VERSION
	printf("\n%s\n", "cdav is currently in development. For further information see https://github.com/luv4bytes/cdav");

	// TODO: Print help

	printf("\n");
}

int
main(int argc, char* argv[])
{
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
     *		--no-redirect	 - Specify if redirects should not be followed
     *		--proxy		 - Specify proxy
     *          -s  --save-as    - Specify saving destination
     *
     *      PUT:
     *          -a  --address     - Specify address
     *          -u  --user        - Specify user
     *          -pw --password    - Specify password
     *		--no-redirect	  - Specify if redirects should not be followed
     *		--proxy		  - Specify proxy
     *          -uf --upload-file - Specify file for upload
     *
     *      PROPFIND:
     *          -a  --address    - Specify address
     *          -u  --user       - Specify user
     *          -pw --password   - Specify password
     *		--no-redirect	 - Specify if redirects should not be followed
     *		--proxy		 - Specify proxy
     *          -p  --props      - Specify properties, separated by comma
     *          -d  --depth      - Specify PROPFIND depth
     *
     *      PROPPATCH:
     *          -a  --address    - Specify address
     *          -u  --user       - Specify user
     *          -pw --password   - Specify password
     *		--no-redirect	 - Specify if redirects should not be followed
     *		--proxy		 - Specify proxy
     *          -sp --set-props   - Specify properties to be set, separated by comma (prop1=test,prop2=21)
     *          -rp --rm-props    - Specify properties to be removed, separated by comma (prop1,prop2)
     *
     *      MKCOL:
     *          -a  --address    - Specify address
     *          -u  --user       - Specify user
     *          -pw --password   - Specify password
     *		--proxy		 - Specify proxy
     *		--no-redirect	 - Specify if redirects should not be followed
     *
     *      DELETE:
     *          -a  --address    - Specify address
     *          -u  --user       - Specify user
     *          -pw --password   - Specify password
     *		--proxy		 - Specify proxy
     *		--no-redirect	 - Specify if redirects should not be followed
     *
     *      COPY:
     *          -a  --address               - Specify address
     *          -u  --user                  - Specify user
     *          -pw --password              - Specify password
     *		--no-redirect	 	    - Specify if redirects should not be followed
     *		--proxy		 	    - Specify proxy
     *          -da --destination-address   - Specify destination address
     *          --no-overwrite              - Specify no overwrite
     *
     *      MOVE:
     *          -a  --address               - Specify address
     *          -u  --user                  - Specify user
     *          -pw --password              - Specify password
     *		--no-redirect	 	    - Specify if redirects should not be followed
     *		--proxy		 	    - Specify proxy
     *          -da --destination-address   - Specify destination address
     *          --no-overwrite		    - Specify no overwrite
     *
     *	    LOCK:
     *          -a  --address               - Specify address
     *          -u  --user                  - Specify user
     *          -pw --password              - Specify password
     *		--no-redirect	 	    - Specify if redirects should not be followed
     *		--proxy		 	    - Specify proxy
     *		-ls --lock-scope	    - Specify lock scope
     *	 	-d  --depth		    - Specify depth
     *		-lo  --lock-owner	    - Specify lock owner
     *
     *	    UNLOCK:
     *          -a  --address               - Specify address
     *          -u  --user                  - Specify user
     *          -pw --password              - Specify password
     *		--proxy		 	    - Specify proxy
     *		--no-redirect	 	    - Specify if redirects should not be followed
     *		-lt --lock-token	    - Specify lock token to unlock
     *
     *	INFORMATION:
     *
     *		-h --help 	- Print help text
     *		-v --version 	- Print version information
    */

	if (argc == 1)
	{
		print_help();
		return 0;
	}

	ARGS args;
	init_args(&args);


	for(int i = 1; i < argc; i++)
	{
		if ( (i + 1) > argc)
			break;

		if (eval_arg(argv[i], ARG_F_SHORT, ARG_F_LONG) == 1)
		{
			args.file = argv[i + 1];

			exec_cmdfile(args.file);

			return 0;
		}

		if (eval_arg(argv[i], ARG_O_SHORT, ARG_O_LONG))
		{
			args.operation = argv[i + 1];
			continue;
		}

		if (eval_arg(argv[i], ARG_A_SHORT, ARG_A_LONG))
		{
			args.address = argv[i + 1];
			continue;
		}

		if (eval_arg(argv[i], ARG_U_SHORT, ARG_U_LONG))
		{
			args.user = argv[i + 1];
			continue;
		}

		if (eval_arg(argv[i], ARG_PW_SHORT, ARG_PW_LONG))
		{
			args.passwd = argv[i + 1];
			continue;
		}

		if (eval_arg(argv[i], ARG_P_SHORT, ARG_P_LONG))
		{
			args.props = argv[i + 1];
			continue;
		}

		if (eval_arg(argv[i], ARG_SP_SHORT, ARG_SP_LONG))
		{
			args.set_props = argv[i + 1];
			continue;
		}

		if (eval_arg(argv[i], ARG_RP_SHORT, ARG_RP_LONG))
		{
			args.rm_props = argv[i + 1];
			continue;
		}

		if (eval_arg(argv[i], ARG_DA_SHORT, ARG_DA_LONG))
		{
			args.destination = argv[i + 1];
			continue;
		}

		if (eval_arg(argv[i], ARG_NO_OW, NULL))
		{
			args.overwrite = 0;
			continue;
		}

		if (eval_arg(argv[i], ARG_UF_SHORT, ARG_UF_LONG))
		{
			args.upload_file = argv[i + 1];
			continue;
		}

		if (eval_arg(argv[i], ARG_S_SHORT, ARG_S_LONG))
		{
			args.save_as = argv[i + 1];
			continue;
		}

		if (eval_arg(argv[i], ARG_D_SHORT, ARG_D_LONG))
		{
			args.depth = argv[i + 1];
			continue;
		}

		if (eval_arg(argv[i], ARG_LS_SHORT, ARG_LS_LONG))
		{
			args.lock_scope = argv[i + 1];
			continue;
		}

		if (eval_arg(argv[i], ARG_LT_SHORT, ARG_LT_LONG))
		{
			args.lock_token = argv[i + 1];
			continue;
		}

		if (eval_arg(argv[i], ARG_LO_SHORT, ARG_LO_LONG))
		{
			args.lock_owner = argv[i + 1];
			continue;
		}

		if (eval_arg(argv[i], ARG_H_SHORT, ARG_H_LONG))
		{
			args.help = 1;
			continue;
		}

		if (eval_arg(argv[i], ARG_V_SHORT, ARG_V_LONG))
		{
			args.version = 1;
			continue;
		}

		if (eval_arg(argv[i], ARG_NO_REDIRECT, NULL))
		{
			args.follow_redirect = 0;
			continue;
		}

		if (eval_arg(argv[i], ARG_PROXY, NULL))
		{
			args.proxy = argv[i + 1];
			continue;
		}
	}

	if (args.help == 1)
	{
		print_help();
		return 0;
	}

	if (args.version == 1)
	{
		PRINT_VERSION
		return 0;
	}

	int init = curl_global_init(CURL_GLOBAL_ALL);

	if (init != CURLE_OK)
	{
		const char* err = curl_easy_strerror(init);
		ERROR_EXIT( "CURL ERR: %d - %s\n", init, err)
	}

	CDAV_BASIC_PARAMS params;

	params.url = args.address;
	params.user = args.user;
	params.passwd = args.passwd;
	params.follow_redirect = args.follow_redirect;
	params.proxy = args.proxy;

	switch(eval_op(args.operation))
	{
		case GET:

			cdav_get(&params, args.save_as);

			break;

		case HEAD:

			cdav_head(&params);

			break;

		case PUT:

			cdav_put(&params, args.upload_file);

			break;

		case PROPFIND:
		{
			CDAV_PROP** prop_list = NULL;

			int count = 0;
			prop_list = cdav_parse_props(args.props, &count);

			cdav_propfind(&params, prop_list, count, args.depth);

			break;
		}

		case PROPPATCH:
		{
			CDAV_PROP** sprops_list = NULL;
			int sprop_count = 0;

			CDAV_PROP** rprops_list = NULL;
			int rprop_count = 0;

			sprops_list = cdav_parse_props(args.set_props, &sprop_count);
			rprops_list = cdav_parse_props(args.rm_props, &rprop_count);

			cdav_proppatch(&params, sprops_list, sprop_count, rprops_list, rprop_count);

			break;
		}

		case MKCOL:

			cdav_mkcol(&params);

			break;

		case DELETE:

			cdav_delete(&params);

			break;

		case COPY:

			cdav_copy(&params, args.destination, args.overwrite);

			break;

		case MOVE:

			cdav_move(&params, args.destination, args.overwrite);

			break;

		case LOCK:

			cdav_lock(&params, args.lock_scope, args.lock_owner, args.depth);

			break;

		case UNLOCK:

			cdav_unlock(&params, args.lock_token);

			break;

		case UNKNOWN:
			ERROR_EXIT("%s\n", UNKNOWN_OPERATION);

		default:
			ERROR_EXIT("%s\n", UNKNOWN_OPERATION);
	}

	curl_global_cleanup();

	return EXIT_SUCCESS;
}