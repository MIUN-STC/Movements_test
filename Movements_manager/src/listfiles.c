#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include "tcol.h"


/*
struct dirent {
ino_t          d_ino;        inode number 
off_t          d_off;        offset to the next dirent 
unsigned short d_reclen;     length of this record 
unsigned char  d_type;       type of file; not supported by all file system types 
char           d_name[256];  filename 
};
*/


int main(void)
{
	char bigname [1000];
	// Pointer for directory entry
	struct dirent * de;

	// opendir() returns a pointer of DIR type.
	char const * dirname = "/run/user/1000/gvfs/smb-share:server=eaglewatchnas02.local,share=skagen/camera_c2/";
	DIR * dr = opendir (dirname);

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
		char * w = ".mp4";
		char * e = strstr (de->d_name, w);
		char * c = TCOL_RESET;
		if (e != NULL)
		{
			c = TCOL (TCOL_BOLD, TCOL_DEFAULT, TCOL_YELLOW);
		}
		else
		{
			c = TCOL_RESET;
		}
		strncat (bigname, dirname, sizeof (bigname));
		strncat (bigname, de->d_name, sizeof (bigname));
		printf ("%s%s" TCOL_RESET "\n", c, bigname);
		bigname [0] = 0;
	}
	
	closedir (dr);
	return 0;
}
