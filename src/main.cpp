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

DWORD BASSChannel;
BOOL RestartChannel = FALSE;
std::string FileName;
float Volume = 1.0;

static void PlayMusic (GtkWidget *widget, gpointer data, DWORD bassChannel)
{
    if (BASS_ChannelIsActive(BASSChannel) == BASS_ACTIVE_STOPPED
		|| BASS_ChannelIsActive(BASSChannel) == BASS_ACTIVE_PAUSED)
	{
		if(BASS_ChannelPlay(BASSChannel, RestartChannel))
        {
			printf("Playing Audio...\n");
			RestartChannel = FALSE;
		}
    	else
        	BASSHelpers::BASSError("Couldn't play file.", FALSE);
	}
	else if(BASS_ChannelIsActive(BASSChannel) == BASS_ACTIVE_PLAYING)
	{
		if(BASS_ChannelPause(BASSChannel))
        {
			printf("Pausing Playback.\n");
		}
    	else
        	BASSHelpers::BASSError("Couldn't pause.", FALSE);
	}
}

static void StopMusic (GtkWidget *widget, gpointer data, DWORD bassChannel)
{
	if(BASS_ChannelStop(BASSChannel))
	{
		printf("Stopping Playback.\n");
		RestartChannel = TRUE;
	}
	else
		BASSHelpers::BASSError("Couldn't stop.", FALSE);
}

static void SetMusicFile(char *name)
{
	FileName = name;
	BASS_ChannelStop(BASSChannel);
	BASSChannel = BASS_StreamCreateFile(FALSE, name, 0, 0, BASS_SAMPLE_FLOAT);
	BASS_SetVolume(Volume);
}

static void ChangeVolume (GtkRange *range)
{
	double value = gtk_range_get_value(range);
	printf("Volume: %f\n", value);
	BASS_SetVolume((float)value);
	Volume = (float)value;
}

static void SetFile (GFile *file, gpointer data)
{
	char *name;	
	name = g_file_get_path (file);

	printf("Set path to: %s\n", name);

	gtk_label_set_label (GTK_LABEL (data), name);
	SetMusicFile(name);
}

static void FileOpened (GObject *source, GAsyncResult *result, void *data)
{
	GFile *file;
	GError *error = NULL;

	file = gtk_file_dialog_open_finish (GTK_FILE_DIALOG (source), result, &error);

	SetFile (file, data);
}

static void GTKOpenFile(GtkButton *picker, GtkLabel  *label)
{
	GtkWindow *parent = GTK_WINDOW (gtk_widget_get_root (GTK_WIDGET (picker)));
	GtkFileDialog *dialog;
	GCancellable *cancellable;

	dialog = gtk_file_dialog_new ();

	cancellable = g_cancellable_new ();

	gtk_file_dialog_open (dialog, parent, cancellable, FileOpened, label);

	g_object_unref (cancellable);
	g_object_unref (dialog);
}

static void activate (GtkApplication *app, gpointer user_data, DWORD bassChannel)
{
	GtkWidget *window;
	GtkIconTheme *icon_theme = gtk_icon_theme_get_for_display(gdk_display_get_default());

	window = gtk_application_window_new (app);
	gtk_window_set_title (GTK_WINDOW (window), "The music player");
	gtk_window_set_default_size (GTK_WINDOW (window), 200, 200);
	gtk_icon_theme_add_search_path(icon_theme, "assets");
	gtk_window_set_default_icon_name("icon");
	gtk_window_set_icon_name(GTK_WINDOW(window), "icon");

	GtkWidget *table;

	table = gtk_grid_new ();
    gtk_widget_set_margin_start (table, 20);
    gtk_widget_set_margin_end (table, 20);
    gtk_widget_set_margin_top (table, 20);
    gtk_widget_set_margin_bottom (table, 20);
    gtk_grid_set_row_spacing (GTK_GRID(table), 6);
    gtk_grid_set_column_spacing (GTK_GRID(table), 6);
	gtk_grid_set_column_homogeneous (GTK_GRID(table), true);
	gtk_widget_set_hexpand(GTK_WIDGET(table), true);
    gtk_window_set_child (GTK_WINDOW (window), table);
	
	GtkWidget *playButton;
	playButton = gtk_button_new_with_label("Play/Pause");
	g_signal_connect (playButton, "clicked", G_CALLBACK (PlayMusic), NULL);
	gtk_grid_attach (GTK_GRID (table), playButton, 0, 0, 2, 1);
	
	GtkWidget *stopButton;
	stopButton = gtk_button_new_with_label("Stop");
	g_signal_connect (stopButton, "clicked", G_CALLBACK (StopMusic), NULL);
	gtk_grid_attach (GTK_GRID (table), stopButton, 2, 0, 2, 1);
	
	GtkWidget *volume;
	volume = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0.0, 1.0, 0.01);
	gtk_scale_set_draw_value (GTK_SCALE (volume), FALSE);
	gtk_range_set_value (GTK_RANGE (volume), 1.0);
	g_signal_connect (volume, "value-changed", G_CALLBACK (ChangeVolume), NULL);
	gtk_grid_attach (GTK_GRID (table), volume, 1, 1, 3, 1);
	
	GtkWidget *labelvol;
	labelvol = gtk_label_new("Volume: ");
	gtk_label_set_xalign(GTK_LABEL(labelvol), 0.0);
	gtk_grid_attach (GTK_GRID (table), labelvol, 0, 1, 2, 1);

	GtkWidget *labelFile;
	labelFile = gtk_label_new("File: ");
	gtk_label_set_xalign(GTK_LABEL(labelFile), 0.0);
	gtk_grid_attach (GTK_GRID (table), labelFile, 0, 2, 1, 1);

	GtkWidget *labelFilename;
	labelFilename = gtk_label_new(FileName.c_str());
	gtk_label_set_xalign(GTK_LABEL(labelFilename), 0.0);
	gtk_label_set_ellipsize(GTK_LABEL(labelFilename), PANGO_ELLIPSIZE_MIDDLE);
	gtk_grid_attach (GTK_GRID (table), labelFilename, 1, 2, 2, 1);

	GtkWidget *buttonFile;
    buttonFile = gtk_button_new_from_icon_name ("document-open-symbolic");
    gtk_accessible_update_property (GTK_ACCESSIBLE (buttonFile),
                                    GTK_ACCESSIBLE_PROPERTY_LABEL, "Select File",
                                    -1);
    g_signal_connect (buttonFile, "clicked", G_CALLBACK (GTKOpenFile), labelFilename);
	gtk_grid_attach (GTK_GRID (table), buttonFile, 3, 2, 1, 1);

	gtk_window_present (GTK_WINDOW (window));
}

int main(int argc, char *argv[])
{
    //basstest code
    printf("Using BASS Version %s\n", BASSHelpers::GetVersionStr().c_str());

    // Check the correct BASS version was loaded.
	if (HIWORD(BASS_GetVersion()) != BASSVERSION) {
		printf("An incorrect version of BASS was loaded\n");
		return 0;
	}

	int deviceIdx = -1;

    printf("Path: %s\n", argv[1]);

    if(argc < 2)
    {
        printf("No path specified.\n");
		FileName = "No file.";
        //return 0;
    } else {
		FileName = argv[1];
	}

    // Attempts initialisation on default device. 
    if(!BASS_Init(deviceIdx, 48000, 0, 0, NULL))
        BASSHelpers::BASSError("Couldn't init device.");

    BASSChannel = BASS_StreamCreateFile(FALSE, argv[1], 0, 0, BASS_SAMPLE_FLOAT);

    //gtk code
    GtkApplication *app;
	GFile *file;
    int status;

    app = gtk_application_new ("vektor451.vekamp", G_APPLICATION_HANDLES_OPEN);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
	g_signal_connect(app, "open", G_CALLBACK(activate), NULL);
    status = g_application_run (G_APPLICATION (app), argc, argv);
	
	file = g_file_new_for_path(argv[1]);
    
	g_object_unref (app);
    BASS_Free();
    return 0;
}