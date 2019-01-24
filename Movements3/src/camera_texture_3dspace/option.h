#pragma once

struct Option
{
	int help_flag;
	char * config_file;
};


void opt_print (struct Option * opt)
{
	ASSERT_F (opt != NULL, "%s", "");
	printf ("%20s\n", "Options");
	printf ("%20s %i\n", "help_flag", (int)opt->help_flag);
	printf ("%20s %s\n", "config_file", opt->config_file);
	printf ("\n");
}


void opt_help (struct Option * opt)
{
	ASSERT_F (opt != NULL, "%s", "");
	printf ("%10s %s\n", "h", "Shows help");
	printf ("%10s %s\n", "c:", "Config file");
}


void opt_parse (struct Option * opt, int C)
{
	switch (C)
	{
		case 'h':
		opt->help_flag = 1;
		break;
		
		case 'c':
		opt->config_file = optarg;
		break;
	}
}


void opt_init (struct Option * opt, int argc, char * argv [])
{
	ASSERT_F (opt != NULL, "%s", "");
	ASSERT_F (argv != NULL, "%s", "");
	opt->help_flag = 0;
	opt->config_file = "conf.txt";
	while (1)
	{
		int C;
		C = getopt (argc, argv, ":hc:");
		if (C < 0) {break;}
		opt_parse (opt, C);
	}
}
