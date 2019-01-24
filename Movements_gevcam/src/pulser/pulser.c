#include "debug.h"
#include "option.h"

#include <stdio.h>
#include <stdarg.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <inttypes.h>
#include <pthread.h>
#include <sys/epoll.h>
#include <sys/timerfd.h>


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
	
	int f;
	{
		char buf [128];
		int l = snprintf (buf, sizeof (buf), "/sys/class/gpio/gpio%i/value", opt.gpio);
		ASSERT_F (l > 0, "%s", "")
		f = open (buf, O_WRONLY);
		ASSERT_F (f >= 0, "%s", "");
	}

	int timer = timerfd_create (CLOCK_MONOTONIC, 0);
	
	{
		struct itimerspec t;
		t.it_interval.tv_sec = 0;
		t.it_interval.tv_nsec = 1000 * opt.duration_period_usec;
		t.it_value.tv_sec = t.it_interval.tv_sec;
		t.it_value.tv_nsec = t.it_interval.tv_nsec;
		timerfd_settime (timer, 0, &t, NULL);
	}
	
	uint32_t i = 0;
	while (1)
	{
		//typedef long unsigned lu;
		int r;
		if (opt.mode & OPT_VERBOSITY) {printf ("0\n");}
		r = write (f, "0\n", 2);
		ASSERT_F (r == 2, "%s", "");
		{
			uint64_t n;
			r = read (timer, &n, sizeof (n));
			ASSERT_F (r == sizeof (n), "%s", "");
		}
		if (opt.mode & OPT_VERBOSITY) {printf ("1\n");}
		r = write (f, "1", 2);
		ASSERT_F (r == 2, "%s", "");
		usleep (opt.duration_on_usec);
		//print_time (stdout, time (NULL));
		//printf ("i: %lu\n", (lu)i);
		i ++;
	}
	
	return 1;
}



