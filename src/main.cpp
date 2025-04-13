// c++
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>

// platform specific
#ifdef _WIN32
#include <conio.h>
#else // OSX/Linux // Just copied this code from the BASS contest example code. Yeah this is just C code.
#include <sys/time.h>
#include <termios.h>
#include <string.h>
#include <unistd.h>

#define Sleep(x) usleep(x*1000)

int _kbhit()
{
	int r;
	fd_set rfds;
	struct timeval tv = { 0 };
	struct termios term, oterm;
	tcgetattr(0, &oterm);
	memcpy(&term, &oterm, sizeof(term));
	cfmakeraw(&term);
	tcsetattr(0, TCSANOW, &term);
	FD_ZERO(&rfds);
	FD_SET(0, &rfds);
	r = select(1, &rfds, NULL, NULL, &tv);
	tcsetattr(0, TCSANOW, &oterm);
	return r;
}
#endif

// vekamp
#include "utils.hpp"

// libs and lib helpers
#include "basshelpers.hpp"
#include <gtk/gtk.h>

static void activate (GtkApplication *app, gpointer user_data)
{
	GtkWidget *window;
	GtkWidget *label;

	window = gtk_application_window_new (app);
	gtk_window_set_title (GTK_WINDOW (window), "She hello on my world till I vekamp.");
	gtk_window_set_default_size (GTK_WINDOW (window), 200, 200);

	label = gtk_label_new("Hello World");
	gtk_window_set_child (GTK_WINDOW (window), label);

	gtk_window_present (GTK_WINDOW (window));
}

int main(int argc, char *argv[])
{
    //gtk code
    GtkApplication *app;
    int status;

    app = gtk_application_new ("vektor451.vekamp", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
    status = g_application_run (G_APPLICATION (app), argc, argv);
    g_object_unref (app);

    //basstest code
    printf("Using BASS Version %s\n", BASSHelpers::GetVersionStr().c_str());

    // Check the correct BASS version was loaded.
	if (HIWORD(BASS_GetVersion()) != BASSVERSION) {
		printf("An incorrect version of BASS was loaded\n");
		return 0;
	}

	int deviceIdx = -1;
    DWORD BASSChannel;

    printf("Path: %s\n", argv[1]);

    if(argc < 2)
    {
        printf("No path specified.\n");
        return 0;
    }

    // Attempts initialisation on default device. 
    if(!BASS_Init(deviceIdx, 48000, 0, 0, NULL))
        BASSHelpers::BASSError("Couldn't init device.");

    BASSChannel = BASS_StreamCreateFile(FALSE, argv[1], 0, 0, BASS_SAMPLE_FLOAT);

    if(BASS_ChannelPlay(BASSChannel, FALSE))
        printf("Playing Audio...\n");
    else
        BASSHelpers::BASSError("Couldn't play file.");

    while(!_kbhit() && BASS_ChannelIsActive(BASSChannel))
    {
        
    }

    BASS_Free();
    return 0;
}