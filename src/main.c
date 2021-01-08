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
#include "../include/interactive.h"

#define PRINT_VERSION printf("cdav %s\n", VERSION); // Version is defined by Makefile

/// Prints help text.
void
print_help()
{
	PRINT_VERSION
	
	const char* lines[] = {
					"\n",
					"cdav is a scriptable CLI WebDAV client for Linux. It supports all operations defined in RFC 4918.",
					"\n"
					"For further information about WebDAV see \"https://tools.ietf.org/html/rfc4918\".",
					"\n",
					"\n",
					"SYNTAX:",
					"\n",
					"\n",
					"\tcdav -o [OPERATION] ... ",
					"\n",
					"\n",
					"\tGET,",
					"\n",
					"\tPUT,",
					"\n",
					"\tHEAD,",
					"\n",
					"\tPROPFIND,",
					"\n",
					"\tPROPPATCH,",
					"\n",
					"\tMKCOL,",
					"\n",
					"\tDELETE,",
					"\n",
					"\tCOPY,",
					"\n",
					"\tMOVE,",
					"\n",
					"\tLOCK,",
					"\n",
					"\tUNLOCK",
					"\n",
					"\n",
					"\n",
					"ARGUMENTS:",
					"\n",
					"\n",
					"\t-a, --address 		-> Target URL.",
					"\n",
					"\t-u, --user 		-> User for authentication.",
					"\n",
					"\t-pw, --password 	-> Password for authentication.",
					"\n",
					"\t--no-redirect 		-> Specifies that redirects should not be followed.",
					"\n",
					"\t--proxy 		-> Proxy to use.",
					"\n",
					"\t--raw 			-> Indicates if responses should be raw XML output or not.",
					"\n",
					"\t-f, --file 		-> Commandfile to use for execution.",
					"\n",
					"\n",
					"\t-h, --help 		-> Print help text.",
					"\n",
					"\t-v, --version 		-> Print version information.",
					"\n",
					"\n",
					"Specific arguments:",
					"\n",
					"\n",
					"GET:",
					"\n",
					"\t-s, --save-as 		-> Filename for saving.",
					"\n",
					"\n",
					"PUT:",
					"\n",
					"\t-uf, --upload-file 	-> File for upload.",
					"\n",
					"\n",
					"PROPFIND:",
					"\n",
					"\t-p, --props 		-> Properties to look for.",
					"\n",
					"\t-d, --depth 		-> Depth parameter.",
					"\n",
					"\n",
					"PROPPATCH:",
					"\n",
					"\t-sp, --set-props 	-> Properties to set.",
					"\n",
					"\t-rp, --rm-props 	-> Properties to remove.",
					"\n",
					"\n",
					"COPY:",
					"\n",
					"\t-da, --destination-address 	-> Path to copy the resource to.",
					"\n",
					"\t--no-overwrite 			-> Specifies if the destination should be overwritten if it exists.",
					"\n",
					"\n",
					"MOVE:",
					"\n",
					"\t-da, --destination-address 	-> Path to copy the resource to.",
					"\n",
					"\t--no-overwrite 			-> Specifies if the destination should be overwritten if it exists.",
					"\n",
					"\n",
					"LOCK:",
					"\n",
					"\t-ls, --lock-scope 	-> Lock scope according to RFC 4918.",
					"\n",
					"\t-d, --depth 		-> Depth parameter.",
					"\n",
					"\t-lo, --lock-owner 	-> Lock owner.",
					"\n",
					"\n",
					"UNLOCK:",
					"\n",
					"\t-lt, --lock-token 	-> Lock token.",
					"\n",
					"\n",
					"Syntax for setting properties:",
					"\n"
					"\n",
					"\t[PROPERTY, PROPERTY, PROPERTY=VALUE, PROPERTY={CHILD, CHILD=VALUE, CHILD={CHILD_CHILD}}]",
					"\n",
					"\n",
					"Examples:",
					"\n",
					"\n",
					"\t(using command file) cdav -f ./movestuff.cdav",
					"\n",
					"\n",
					"\tcdav -o PROPFIND -a https://test.com/dav/test.jpg -p \"getcontentlength, getcontenttype\" -u somedude -pw verysecret", 
					"\n",
					"\tcdav -o GET -a https://test.com/dav/test.jpg -s ./test.jpg -u somedude -pw verysecret",
					"\n",
					"\tcdav -o PROPPATCH (set) -a https://test.com/dav/test.jpg -sp \"prop=value, parent={child=value, child={prop}}\" -u somedude -pw verysecret", 
					"\n",
					"\tcdav -o PROPPATCH (remove) -a https://test.com/dav/test.jpg -rp \"prop, other_prop\" -u somedude -pw verysecret",
					"\n",
					"\tcdav -o MOVE -a https://test.com/dav/test.jpg -da https://test.com/dav/test2.jpg --no-overwrite -u somedude -pw verysecret",
					"\n",
					"\n",
					"Sources available on https://github.com/luv4bytes/cdav.",
					"\n",
					"Also see https://luv4bytes.github.io/cdav for detailed information.",
					"\n",
					"EOF"
					};

	size_t i = 0;
	while(strcmp(lines[i], "EOF") != 0)
		printf("%s", lines[i++]);

	printf("\n");
}

int
main(int argc, char* argv[])
{
	if (argc == 1)
	{
		print_help();
		return 0;
	}

	ARGS args;
	args_init(&args);

	LIBXML_TEST_VERSION

	for(int i = 1; i < argc; i++)
	{
		if ( (i + 1) > argc)
			break;

		if (args_eval(argv[i], ARG_I_SHORT, ARG_I_LONG))
		{
			interactive_session();
		}

		if (args_eval(argv[i], ARG_F_SHORT, ARG_F_LONG))
		{
			args.file = argv[i + 1];

			exec_cmdfile(args.file);

			return 0;
		}

		if (args_eval(argv[i], ARG_O_SHORT, ARG_O_LONG))
		{
			args.operation = argv[i + 1];
			continue;
		}

		if (args_eval(argv[i], ARG_A_SHORT, ARG_A_LONG))
		{
			args.address = argv[i + 1];
			continue;
		}

		if (args_eval(argv[i], ARG_U_SHORT, ARG_U_LONG))
		{
			args.user = argv[i + 1];
			continue;
		}

		if (args_eval(argv[i], ARG_PW_SHORT, ARG_PW_LONG))
		{
			args.passwd = argv[i + 1];
			continue;
		}

		if (args_eval(argv[i], ARG_P_SHORT, ARG_P_LONG))
		{
			args.props = argv[i + 1];
			continue;
		}

		if (args_eval(argv[i], ARG_SP_SHORT, ARG_SP_LONG))
		{
			args.set_props = argv[i + 1];
			continue;
		}

		if (args_eval(argv[i], ARG_RP_SHORT, ARG_RP_LONG))
		{
			args.rm_props = argv[i + 1];
			continue;
		}

		if (args_eval(argv[i], ARG_DA_SHORT, ARG_DA_LONG))
		{
			args.destination = argv[i + 1];
			continue;
		}

		if (args_eval(argv[i], ARG_NO_OW, NULL))
		{
			args.overwrite = 0;
			continue;
		}

		if (args_eval(argv[i], ARG_UF_SHORT, ARG_UF_LONG))
		{
			args.upload_file = argv[i + 1];
			continue;
		}

		if (args_eval(argv[i], ARG_S_SHORT, ARG_S_LONG))
		{
			args.save_as = argv[i + 1];
			continue;
		}

		if (args_eval(argv[i], ARG_D_SHORT, ARG_D_LONG))
		{
			args.depth = argv[i + 1];
			continue;
		}

		if (args_eval(argv[i], ARG_LS_SHORT, ARG_LS_LONG))
		{
			args.lock_scope = argv[i + 1];
			continue;
		}

		if (args_eval(argv[i], ARG_LT_SHORT, ARG_LT_LONG))
		{
			args.lock_token = argv[i + 1];
			continue;
		}

		if (args_eval(argv[i], ARG_LO_SHORT, ARG_LO_LONG))
		{
			args.lock_owner = argv[i + 1];
			continue;
		}

		if (args_eval(argv[i], ARG_H_SHORT, ARG_H_LONG))
		{
			args.help = 1;
			continue;
		}

		if (args_eval(argv[i], ARG_V_SHORT, ARG_V_LONG))
		{
			args.version = 1;
			continue;
		}

		if (args_eval(argv[i], ARG_NO_REDIRECT, NULL))
		{
			args.follow_redirect = 0;
			continue;
		}

		if (args_eval(argv[i], ARG_PROXY, NULL))
		{
			args.proxy = argv[i + 1];
			continue;
		}

		if (args_eval(argv[i], ARG_RAW, NULL))
		{
			args.raw = 1;
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
	params.raw = args.raw;

	switch(args_eval_op(args.operation))
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
