/************************************************************************/
/*									*/
/*  Project:	VLC Command Line Player					*/
/*  Edit:	04-May-19						*/
/*									*/
/************************************************************************/
/*									*/
/*  Copyright (c) 2018-2019 Jonathan Marten <jjm@keelhaul.me.uk>	*/
/*  Home page:  <http://github.com/martenjj/phononplay>  		*/
/*									*/
/*  This program is free software; you can redistribute it and/or	*/
/*  modify it under the terms of the GNU General Public License as	*/
/*  published by the Free Software Foundation; either version 3 of	*/
/*  the License, or (at your option) any later version.			*/
/*									*/
/*  It is distributed in the hope that it will be useful, but		*/
/*  WITHOUT ANY WARRANTY; without even the implied warranty of		*/
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the	*/
/*  GNU General Public License for more details.			*/
/*									*/
/*  You should have received a copy of the GNU General Public		*/
/*  License along with this program; see the file COPYING for further	*/
/*  details.  If not, see <http://www.gnu.org/licenses>.		*/
/*									*/
/************************************************************************/


/************************************************************************/
/*  Include files							*/
/************************************************************************/

#include <stdio.h>					/* standard input/putput */
#include <string.h>					/* string handling */
#include <stdarg.h>					/* variable argument lists */
#include <stdlib.h>					/* standard routines */
#include <errno.h>					/* system call errors */
#include <getopt.h>					/* argument parsing */

#include <vlc.h>					/* VLC media player */

/************************************************************************/
/*  Message keys for cmderr() - keep ERROR and NONE as these values	*/
/*  backwards compatibility, also sync with 'errstrings'.		*/
/************************************************************************/

enum cemsg
{
	CEERROR		= 0,				/* compatibility - exit */
	CENONE		= 1,				/* compatibility - benign */
	CEWARNING	= 2,				/* continue */
	CENOTICE	= 3,				/* continue */
	CEINFO		= 4,				/* continue */
	CESUCCESS	= 5,				/* continue */
	CEFATAL		= 6,				/* exit */
	CEUNKNOWN	= 7				/* must be last */
};

/************************************************************************/
/*  System error keys for cmderr() - also keep NONE and ERRNO for	*/
/*  backwards compatibility.						*/
/************************************************************************/

enum cesys
{
	CSNONE		= 0,				/* no message */
	CSERRNO		= 1				/* decode 'errno' */
};

/************************************************************************/
/*  Severity strings for cmderr() reports				*/
/************************************************************************/

static const char *errstrings[] =
{
    "ERROR",						/* 0 */
    NULL,						/* 1 */
    "WARNING",						/* 2 */
    "NOTICE",						/* 3 */
    "INFO",						/* 4 */
    "SUCCESS",						/* 5 */
    "FATAL",						/* 6 */
    NULL						/* 7 */
};

/************************************************************************/
/*  MYNAME - The name by which this program was invoked.  Obvious.	*/
/************************************************************************/

char myname[FILENAME_MAX] = "unknown";

/************************************************************************/
/*  Option values passed to VLC						*/
/************************************************************************/

char vlcModule[FILENAME_MAX] = "";
char vlcDevice[FILENAME_MAX] = "";

/************************************************************************/
/*  CMDINIT - Read argv[0] and set the above variables appropriately.	*/
/************************************************************************/

void cmdinit(const char *argv0)
{
    register char *p;					/* filename parsing */

    p = strrchr(argv0,'/');				/* locate the leaf name */
    if (p==NULL) p = (char *) argv0;			/* just a leaf name */
    else ++p;						/* past the last '/' */
    strcpy(myname,p);					/* save as our name */

    p = strchr(myname,'.');				/* now see if it has extension */
    if (p!=NULL) *p = '\0';				/* trim it off if so */
}

/************************************************************************/
/*  CMDERR - Display an error message with parameters, and optionally	*/
/*  with the system error as well.					*/
/************************************************************************/

#if defined(__GNUC__)
__attribute__((format (printf,3,4)))
#endif
void cmderr(enum cesys src, enum cemsg sev, const char *fmt, ...)
{
    va_list args;					/* parameters for format */
    const char *ss;					/* severity string */
    const char *mm = NULL;				/* textual error message*/

    fflush(stdout); fflush(stderr);			/* flush current output */

    if (sev<0 || sev>CEUNKNOWN) sev = CEUNKNOWN;	/* catch bad parameter */
    ss = errstrings[sev];
    if (src==CSERRNO) mm = strerror(errno);		/* system error message */

    fprintf(stderr,"%s",myname);			/* report our program name */
    if (ss!=NULL) fprintf(stderr," (%s)",ss);		/* report severity if there is one */
    fprintf(stderr,": ");

    va_start(args,fmt);					/* locate variable parameters */
    vfprintf(stderr,fmt,args);				/* user message and parameters */
    if (mm!=NULL) fprintf(stderr,", %s",mm);		/* system error message */
    va_end(args);					/* finished with parameters */
    fprintf(stderr,"\n");				/* end of message */
							/* exit from application */
    if (sev==CEERROR || sev==CEFATAL) exit(EXIT_FAILURE);
}

/************************************************************************/
/*  usage -- Display a usage message and exit.				*/
/************************************************************************/

static void usage()
{
	fprintf(stderr, "Usage:   %s [-v] [-m module] [-d device] file...\n", myname);
	fprintf(stderr, "         %s [-v] [-MD]\n", myname);
	fprintf(stderr, "\n");
	fprintf(stderr, "Options: m - Specify VLC output module, default 'alsa'\n");
	fprintf(stderr, "         M - List available VLC output modules\n");
	fprintf(stderr, "         d - Specify VLC output device, default 'default'\n");
	fprintf(stderr, "         D - List available VLC output devices\n");
	fprintf(stderr, "         v - Enable VLC verbose debugging\n");
	fprintf(stderr, "\n");

	exit(EXIT_FAILURE);
}

/************************************************************************/
/*  Main -- Based on https://forum.videolan.org/viewtopic.php?t=98766	*/
/************************************************************************/

int main(int argc, char *argv[])
{
	int c;						/* option character */
	int i;						/* general index */
	int num;					/* how many file arguments */

	libvlc_instance_t *vlcInstance;
	libvlc_media_player_t *vlcMediaPlayer;
	libvlc_media_t *vlcMedia;

	int listModules = 0;
	int listDevices = 0;
	int listMode = 0;
	int vlcVerbose = 0;

	int vlc_arglen = 1;
	const char *vlc_args[2] =
	{
		"--ignore-config"			/* ignore user configuration */
	};

	cmdinit(argv[0]);				/* get our name & location */
    	opterr = 0;					/* we will handle errors */
	while ((c = getopt(argc,argv,":hMm:Dd:v"))!=EOF)
	{						/* look for command options */
		switch (c)
		{
case 'm':		strcpy(vlcModule, optarg);
			break;

case 'M':		listModules = 1;
			break;

case 'd':		strcpy(vlcDevice, optarg);
			break;

case 'D':		listDevices = 1;
			break;

case 'v':		vlcVerbose = 1;
			break;

case ':':		cmderr(CSNONE, CEERROR, "Option '%c' requires a value (use '-h' for help)", optopt);
			break;

case '?':		cmderr(CSNONE, CEERROR, "Unknown option '%c' (use '-h' for help)", optopt);
			break;

case 'h':		usage();
			break;
		}
	}

	num = argc-optind;				/* how many arguments left */

	if (vlcVerbose)
	{
		vlc_args[1] = "-vvv";
		++vlc_arglen;
	}

	vlcInstance = libvlc_new(vlc_arglen, vlc_args);
	vlcMediaPlayer = libvlc_media_player_new(vlcInstance);

	if (listModules || listDevices)
	{
		listMode = 1;

		if (vlcDevice[0]!='\0') cmderr(CSNONE, CEWARNING, "Option 'd' ignored with list mode ('-M'/'-D')");
		if (num>0) cmderr(CSNONE, CEWARNING, "Audio files ignored with list mode ('-M'/'-D')");
	}

	if (vlcModule[0]=='\0') strcpy(vlcModule, "alsa");
	if (vlcDevice[0]=='\0') strcpy(vlcDevice, "default");

	if (listModules)
	{
		printf("\nAvailable VLC output modules:\n");
		i = 0;
		libvlc_audio_output_t *outs = libvlc_audio_output_list_get(vlcInstance);
		while (outs!=NULL)
		{
			printf("%-3d  %s\n     %s\n", i, outs->psz_name, outs->psz_description);
			outs = outs->p_next;
			++i;
		}
	}

	if (listDevices)
	{
		printf("\nAvailable VLC output devices for module '%s':\n", vlcModule);

		i = 0;
		libvlc_audio_output_device_t *devs = libvlc_audio_output_device_list_get(vlcInstance, vlcModule);
		while (devs!=NULL)
		{
			printf("%-3d  %s\n     %s\n", i, devs->psz_device, devs->psz_description);
			devs = devs->p_next;
			++i;
		}
	}

	if (listMode) exit(EXIT_SUCCESS);

	if (num<1) cmderr(CSNONE, CEERROR, "No audio files specified (use '-h' for help)");
	for (i = 0; i<num; ++i)				/* play each file in turn */
	{
		cmderr(CSNONE, CEINFO, "Playing media '%s'", argv[optind+i]);

		vlcMedia = libvlc_media_new_path(vlcInstance, argv[optind+i]);
		libvlc_media_player_set_media(vlcMediaPlayer, vlcMedia); 
		libvlc_media_release(vlcMedia);

		libvlc_audio_output_set(vlcMediaPlayer, vlcModule);
		libvlc_audio_output_device_set(vlcMediaPlayer, NULL, vlcDevice);
		libvlc_media_player_play(vlcMediaPlayer);

		libvlc_state_t vlcMediaPlayerState;
		do
		{
			vlcMediaPlayerState = libvlc_media_player_get_state(vlcMediaPlayer);
		} while (vlcMediaPlayerState!=libvlc_Ended);

		libvlc_media_player_stop(vlcMediaPlayer);
	}

	libvlc_media_player_release(vlcMediaPlayer);
	libvlc_release(vlcInstance);
	cmderr(CSNONE, CESUCCESS, "Finished");

	return (EXIT_SUCCESS);
}
