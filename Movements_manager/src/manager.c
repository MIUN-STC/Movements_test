#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h> // for fork
#include <sys/types.h> // for pid_t
#include <sys/wait.h> // for wait
#include "tcol.h"
#include "debug.h"
#include "option.h"



void run_program (char const * path, char * vid_name)
{
	char vid_path [1000];
	char txt_path [1000];
	char txt_path2 [1000];
	vid_path [0] = 0;
	txt_path [0] = 0;
	txt_path2 [0] = 0;
	
	strncpy (txt_path, vid_name, sizeof (txt_path));
	char * ext = strstr (txt_path, ".mp4");
	//Only look for mp4 files.
	if (ext == NULL) {return;};
	
	ext [0] = 0;
	strncat (txt_path, "b1.txt", sizeof (txt_path));
	strncat (txt_path2, path, sizeof (txt_path2));
	strncat (txt_path2, txt_path, sizeof (txt_path2));
	TRACE_F ("%s", txt_path2);
	
	
	strncat (vid_path, path, sizeof (vid_path));
	strncat (vid_path, vid_name, sizeof (vid_path));
	//TRACE_F ("%s", ext);

	//Fork new process for 
	pid_t pid = fork ();
	ASSERT (pid >= 0);
	if (pid == 0)
	{
		//printf ("Starting blob cap from: %s to %s\n", vid_path, txt_path2);
		FILE * f = fopen (txt_path2, "r");
		if (f != NULL)
		{
			fclose (f);
			printf ("File "TCOL (TCOL_NORMAL, TCOL_BLUE, TCOL_DEFAULT) "%s" TCOL_RESET " already exists\n", txt_path2);
			return;
		}
		//Set the last argument to null to check where the argument list end.
		char const * prog = "~/Documents/Movements_vid2blob/bin/blob1";
		printf ("%s %s %s %s %s\n", prog, "-i", vid_path, "-o", txt_path2);
		//execl (prog, prog, "-i", vid_path, "-o", txt_path2, NULL);
		char * command [] = {"ls", NULL};
		execv ("ls", command);
		abort ();
	}
	else
	{
		TRACE ("waitpid");
		int ret;
		waitpid (pid, &ret, 0);
		ASSERT (WIFEXITED(ret));
	}
}



int main (int argc, char * argv [])
{
	
	struct Option opt;
	opt_init (&opt, argc, argv);
	opt_print (&opt);
	
	if (opt.mode & OPT_HELP)
	{
		opt_help (&opt);
		return 0;
	}
	
	// Pointer for directory entry
	struct dirent * de;

	// opendir() returns a pointer of DIR type. 
	//char const * path = "/run/user/1000/gvfs/smb-share:server=eaglewatchnas02.local,share=skagen/camera_c2/";
	char const * path = opt.directory;
	ASSERT (path != NULL);
	DIR * dr = opendir (path);
	ASSERT (dr != NULL);

	// opendir returns NULL if couldn't open directory
	if (dr == NULL)
	{
		printf("Could not open current directory" );
		return 0;
	}

	// Refer http://pubs.opengroup.org/onlinepubs/7990989775/xsh/readdir.html
	// for readdir()
	while ((de = readdir(dr)) != NULL)
	{
		TRACE_F ("%s", path);
		TRACE_F ("%s", de->d_name);
		run_program (path, de->d_name);
		//break;
	}
	
	closedir (dr);
	return 0;
}

