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

#define UNKNOWN_OPERATION "Unknown operation! - Exiting."
#define NO_ARGS "No arguments given!"

#define PRINT_VERSION printf("cdav %s\n", VERSION); // VERSION is defined by Makefile

/// Prints help text.
void
print_help()
{
	// TODO: Print help
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
     *          --no-overwrite              - Specify no overwrite
     *
     *      MOVE:
     *          -a  --address               - Specify address
     *          -u  --user                  - Specify user
     *          -pw --password              - Specify password
     *          -da --destination-address   - Specify destination address
     *          --no-overwrite		    - Specify no overwrite
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

	char* file = NULL;		// -f 	--file
	char* operation = NULL; 	// -o 	--operation
	char* address = NULL;		// -a 	--address
	char* user = NULL;		// -u 	--user
	char* password = NULL;		// -pw 	--password
	char* props = NULL;		// -p	--props
	char* set_props = NULL;		// -sp 	--set-props
	char* rm_props = NULL;		// -rp 	--rm-props
	char* destination = NULL;	// -da 	--destination-address
	int overwrite = 1;		// --no-overwrite
	char* upload_file = NULL;	// -uf	--upload-file
	char* save_as = NULL;		// -s 	--save-as
	char* depth = NULL;		// -d	--depth
	int help = 0;			// -h	--help
	int version = 0;		// -v	--version

	char arg_f_short[] = "-f";
	char arg_f_long[] = "--file";

	char arg_o_short[] = "-o";
	char arg_o_long[] = "--operation";

	char arg_a_short[] = "-a";
	char arg_a_long[] = "--address";

	char arg_u_short[] = "-u";
	char arg_u_long[] = "--user";

	char arg_pw_short[] = "-pw";
	char arg_pw_long[] = "--password";

	char arg_p_short[] = "-p";
	char arg_p_long[] = "--props";

	char arg_sp_short[] = "-sp";
	char arg_sp_long[] = "--set-props";

	char arg_rp_short[] = "-rp";
	char arg_rp_long[] = "--rp-props";

	char arg_da_short[] = "-da";
	char arg_da_long[] = "--destination-address";

	char arg_no_ow[] = "--no-overwrite";

	char arg_uf_short[] = "-uf";
	char arg_uf_long[] = "--upload-file";

	char arg_s_short[] = "-s";
	char arg_s_long[] = "--save-as";

	char arg_d_short[] = "-d";
	char arg_d_long[] = "--depth";

	char arg_h_short[] = "-h";
	char arg_h_long[] = "--help";

	char arg_v_short[] = "-v";
	char arg_v_long[] = "--version";

	for(int i = 1; i < argc; i++)
	{
		if ( (i + 1) > argc)
			break;

		if (eval_arg(argv[i], arg_f_short, arg_f_long) == 1)
		{
			// TODO: Eval command file and execute instructions
			file = argv[i + 1];
			break;
		}

		if (eval_arg(argv[i], arg_o_short, arg_o_long))
			operation = argv[i + 1];

		if (eval_arg(argv[i], arg_a_short, arg_a_long))
			address = argv[i + 1];

		if (eval_arg(argv[i], arg_u_short, arg_u_long))
			user = argv[i + 1];

		if (eval_arg(argv[i], arg_pw_short, arg_pw_long))
			password = argv[i + 1];

		if (eval_arg(argv[i], arg_p_short, arg_p_long))
			props = argv[i + 1];

		if (eval_arg(argv[i], arg_sp_short, arg_sp_long))
			set_props = argv[i + 1];

		if (eval_arg(argv[i], arg_rp_short, arg_rp_long))
			rm_props = argv[i + 1];

		if (eval_arg(argv[i], arg_da_short, arg_da_long))
			destination = argv[i + 1];

		if (eval_arg(argv[i], arg_no_ow, NULL))
			 overwrite = 0;

		if (eval_arg(argv[i], arg_uf_short, arg_uf_long))
			upload_file = argv[i + 1];

		if (eval_arg(argv[i], arg_s_short, arg_s_long))
			save_as = argv[i + 1];

		if (eval_arg(argv[i], arg_d_short, arg_d_long))
			depth = argv[i + 1];

		if (eval_arg(argv[i], arg_h_short, arg_h_long))
			help = 1;

		if (eval_arg(argv[i], arg_v_short, arg_v_long))
			version = 1;
	}

	if (help == 1)
	{
		print_help();
		return 0;
	}

	if (version == 1)
	{
		PRINT_VERSION
		return 0;
	}

	if (depth == 0)
		depth = "0";

	int init = curl_global_init(CURL_GLOBAL_ALL);

	if (init != CURLE_OK)
	{
		const char* err = curl_easy_strerror(init);
		fprintf(stderr, "CURL ERR: %d - %s\n", init, err);

		error_exit("CURL ERR: Exiting\n");
	}

	CDAV_BASIC_PARAMS params;

	switch(eval_op(operation))
	{
		case GET:

			params.url = address;
			params.user = user;
			params.passwd = password;

			cdav_get(&params, save_as);

			break;

		case PUT:

			params.url = address;
			params.user = user;
			params.passwd = password;

			cdav_put(&params, upload_file);

			break;

		case PROPFIND:
		{
			CDAV_PROP** prop_list = NULL;

			params.url = address;
			params.user = user;
			params.passwd = password;

			int count = 0;
			prop_list = cdav_parse_props(props, &count);

			cdav_propfind(&params, prop_list, count, atoi(depth));

			break;
		}

		case PROPPATCH:
		{
			params.url = address;
			params.user = user;
			params.passwd = password;

			CDAV_PROP** sprops_list = NULL;
			int sprop_count = 0;

			CDAV_PROP** rprops_list = NULL;
			int rprop_count = 0;

			sprops_list = cdav_parse_set_props(set_props, &sprop_count);
			rprops_list = cdav_parse_props(rm_props, &rprop_count);

			cdav_proppatch(&params, sprops_list, sprop_count, rprops_list, rprop_count);

			break;
		}

		case MKCOL:

			params.url = address;
			params.user = user;
			params.passwd = password;

			cdav_mkcol(&params);

			break;

		case DELETE:

			params.url = address;
			params.user = user;
			params.passwd = password;

			cdav_delete(&params);

			break;

		case COPY:

			params.url = address;
			params.user = user;
			params.passwd = password;

			cdav_copy(&params, destination, overwrite);

			break;

		case MOVE:

			params.url = address;
			params.user = user;
			params.passwd = password;

			cdav_move(&params, destination, overwrite);

			break;

		case UNKNOWN:
			error_exit(UNKNOWN_OPERATION);

		default:
			error_exit(UNKNOWN_OPERATION);
	}

	curl_global_cleanup();

	return EXIT_SUCCESS;
}
