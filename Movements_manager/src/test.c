#include <stdio.h>
#include <dirent.h>
#include <assert.h>
#include <string.h>
#include <unistd.h> // for fork
#include <sys/types.h> // for pid_t
#include <sys/wait.h> // for wait
#include "tcol.h"
#include "debug.h"
#include "option.h"


#define OPTIONAL_ASSERT_F ASSERT_F

#ifndef OPTIONAL_ASSERT_F
#define OPTIONAL_ASSERT_F(A, F, ...) if (!(A)) {}
#endif


#define ERROR_CODE1 2

int main (int argc, char * argv [])
{
	pid_t pid = fork ();
	ASSERT (pid >= 0);
	if (pid == 0)
	{
		char * command [] = {"ls", "-hhl", NULL};
		int r = execv ("/bin/lsa", command);
		OPTIONAL_ASSERT_F (r != -1, "asd%s", "");
	}
	else
	{
		int ret;
		waitpid (pid, &ret, 0);
		TRACE_F ("waitpid %i", ret);
		OPTIONAL_ASSERT_F (WIFEXITED(ret), "%s", "");
	}
		
	return 0;
}


