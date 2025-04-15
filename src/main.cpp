// c++
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>

// vekamp
#include "utils.hpp"

// libs and lib helpers
#include "bassplayer.hpp"
#include <gtk/gtk.h>

// globals, move this to it's own class eventually. 
std::string FileName;
GtkWidget *progLabel;
GtkWidget *progressScale;
bool holdingScroll = false;
bool keepUpdating = true;

static gboolean UpdateProgress(gpointer data)
{
	double progSecs = BASS::BASSPlayer::GetTrackProgressSecs();
	std::string progString = BASS::BASSPlayer::GetTrackProgressStr(progSecs);
	
	gtk_label_set_text(GTK_LABEL(progLabel), progString.c_str());
	gtk_range_set_value(GTK_RANGE(progressScale), progSecs);

	return keepUpdating;
}

static void StartProgressTimeout()
{
	g_timeout_add(25, UpdateProgress, NULL);
}

static void PlayMusic (GtkWidget *widget, gpointer data)
{
	BASS::BASSPlayer::StartPausePlayback();
	keepUpdating = BASS::BASSPlayer::IsPlaying();
	
	if(keepUpdating)
		StartProgressTimeout();
}

static void StopMusic (GtkWidget *widget, gpointer data)
{
	BASS::BASSPlayer::StopPlayback();
	keepUpdating = false;
}

static void ProgScrollBegin (GtkWidget *widget, gpointer data)
{
	printf("Holding scroll.\n");
	BASS::BASSPlayer::StartScroll();
	keepUpdating = false;
	holdingScroll = true;
}

static void ProgScrollEnd (GtkWidget *widget, gpointer data)
{
	printf("No longer holding scroll.\n");
	
	double scrollPos = gtk_range_get_value(GTK_RANGE(progressScale));
	BASS::BASSPlayer::SetPos(scrollPos);

	BASS::BASSPlayer::EndScroll();
	
	keepUpdating = BASS::BASSPlayer::IsPlaying();
	holdingScroll = false;
	
	if(keepUpdating)
		StartProgressTimeout();
}

static void ProgScrollChange(GtkRange *range)
{
	if(holdingScroll)
	{
		double value = gtk_range_get_value(range);
		//BASS::BASSPlayer::SetPos(value);
		std::string progString = BASS::BASSPlayer::GetTrackProgressStr(value);
		gtk_label_set_text(GTK_LABEL(progLabel), progString.c_str());
	}
}

static void SetMusicFile(char *name)
{
	BASS::BASSPlayer::StartFilePlayback(name);
	
	gtk_range_set_range(GTK_RANGE(progressScale), 0.0, BASS::BASSPlayer::GetTrackLenSecs());
	gtk_range_set_value(GTK_RANGE(progressScale), 0.0);
	
	const char *lenStr = BASS::BASSPlayer::GetTrackLenStr();
	char finalLenStr[20];

	std::snprintf(finalLenStr, sizeof(finalLenStr), "0:00 / %s", lenStr);
	
	gtk_label_set_text(GTK_LABEL(progLabel), finalLenStr);

	keepUpdating = false;
}

static void ChangeVolume(GtkRange *range)
{
	double value = gtk_range_get_value(range);
	BASS::BASSPlayer::SetVolume((float)value);
}

static void SetFile(GFile *file, gpointer data)
{
	char *name;	
	name = g_file_get_path (file);

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

static gboolean abort_mission (gpointer data)
{
	//idk how to make this work? guess cancellable is never gonna get freed lol.
	//GCancellable *cancellable = data;
  	//g_cancellable_cancel (cancellable);
	
	return true;
}

static void GTKOpenFile(GtkButton *picker, GtkLabel  *label)
{
	GtkWindow *parent = GTK_WINDOW (gtk_widget_get_root (GTK_WIDGET (picker)));
	GtkFileDialog *dialog;
	GCancellable *cancellable;

	dialog = gtk_file_dialog_new ();

	cancellable = g_cancellable_new();
	
	g_timeout_add_seconds_full (G_PRIORITY_DEFAULT,
		20,
		abort_mission, g_object_ref (cancellable), g_object_unref);

	cancellable = g_cancellable_new();

	gtk_file_dialog_open (dialog, parent, cancellable, FileOpened, label);

	g_object_unref (cancellable);
	g_object_unref (dialog);
}

static void activate (GtkApplication *app, gpointer user_data, DWORD bassChannel)
{
	GtkWidget *window;
	window = gtk_application_window_new (app);

	GtkIconTheme *icon_theme = gtk_icon_theme_get_for_display(gdk_display_get_default());
	gtk_window_set_default_icon_name("icon");
	gtk_window_set_icon_name(GTK_WINDOW(window), "icon");
	gtk_icon_theme_add_search_path(icon_theme, "assets");

	gtk_window_set_title (GTK_WINDOW (window), "VekAmp");
	gtk_window_set_default_size (GTK_WINDOW (window), 800, 500);
	
	GtkCssProvider *cssStyle = gtk_css_provider_new();
	gtk_css_provider_load_from_path(cssStyle, "assets/css/style.css");

	gtk_style_context_add_provider_for_display(
		gdk_display_get_default(),
		GTK_STYLE_PROVIDER(cssStyle),
		GTK_STYLE_PROVIDER_PRIORITY_APPLICATION
	);

	GtkWidget *windowTable;
	windowTable = gtk_grid_new();
	
	gtk_widget_set_margin_start (windowTable, 8);
    gtk_widget_set_margin_end (windowTable, 8);
    gtk_widget_set_margin_top (windowTable, 8);
    gtk_widget_set_margin_bottom (windowTable, 8);
	gtk_widget_set_valign(windowTable, GTK_ALIGN_END);
	gtk_window_set_child(GTK_WINDOW(window), windowTable);

	GtkWidget *controlFrame;
	controlFrame = gtk_frame_new(NULL);
	
    gtk_widget_set_margin_top (controlFrame, 8);
	gtk_widget_set_valign(controlFrame, GTK_ALIGN_END);
	gtk_grid_attach(GTK_GRID(windowTable), controlFrame, 0, 1, 3, 1);

	GtkWidget *controlTable;
	controlTable = gtk_grid_new();
    gtk_grid_set_row_spacing (GTK_GRID(controlTable), 6);
    gtk_grid_set_column_spacing (GTK_GRID(controlTable), 6);
	gtk_widget_set_margin_start (controlTable, 8);
    gtk_widget_set_margin_end (controlTable, 8);
    gtk_widget_set_margin_top (controlTable, 8);
    gtk_widget_set_margin_bottom (controlTable, 8);
	//gtk_grid_set_column_homogeneous (GTK_GRID(table), true);
    gtk_frame_set_child (GTK_FRAME(controlFrame), controlTable);

	GtkWidget *playbackTable;
	playbackTable = gtk_grid_new();
	
	gtk_grid_set_column_homogeneous (GTK_GRID(playbackTable), true);
	gtk_grid_set_baseline_row(GTK_GRID(playbackTable), 1);
    gtk_grid_set_column_spacing (GTK_GRID(playbackTable), 4);
	gtk_widget_set_valign(playbackTable, GTK_ALIGN_CENTER);
    gtk_widget_set_margin_end (playbackTable, 8);
	gtk_grid_attach(GTK_GRID(controlTable), playbackTable, 0, 0, 1, 1);
	
	GtkWidget *prevButton;
	prevButton = gtk_button_new_from_icon_name("media-skip-backward");

	g_signal_connect (prevButton, "clicked", G_CALLBACK (StopMusic), NULL);
	gtk_grid_attach (GTK_GRID (playbackTable), prevButton, 0, 0, 1, 1);

	GtkWidget *playButton;
	playButton = gtk_button_new_from_icon_name("media-playback-start");

	g_signal_connect (playButton, "clicked", G_CALLBACK (PlayMusic), NULL);
	gtk_grid_attach (GTK_GRID (playbackTable), playButton, 1, 0, 1, 1);

	GtkWidget *nextButton;
	nextButton = gtk_button_new_from_icon_name("media-skip-forward");

	g_signal_connect (nextButton, "clicked", G_CALLBACK (PlayMusic), NULL);
	gtk_grid_attach (GTK_GRID (playbackTable), nextButton, 2, 0, 1, 1);

	GtkWidget *progressTable;
	progressTable = gtk_grid_new();
	
	gtk_widget_set_hexpand(GTK_WIDGET(progressTable), true);
	gtk_grid_set_column_homogeneous (GTK_GRID(progressTable), true);
	gtk_grid_set_baseline_row(GTK_GRID(progressTable), 1);
	gtk_grid_attach(GTK_GRID(controlTable), progressTable, 1, 0, 4, 1);

	// thank you zero upvote stackoverflow answer for this ingenious solution.
	// i will memorise you here because i cannot make an account
	// https://stackoverflow.com/a/79108304
	GtkWidget *scaleBox;
	scaleBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_grid_attach (GTK_GRID (progressTable), scaleBox, 0, 1, 4, 1);

	progressScale = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0.0, 1.0, 0.01);
	
	gtk_scale_set_draw_value (GTK_SCALE (progressScale), FALSE);
	gtk_widget_set_hexpand(GTK_WIDGET(progressScale), true);
	gtk_range_set_value (GTK_RANGE (progressScale), 1.0);
	gtk_box_append(GTK_BOX(scaleBox), progressScale);

	GtkGesture *progressScaleGesture;
	progressScaleGesture = gtk_gesture_click_new();
	
	//gtk_gesture_long_press_set_delay_factor(GTK_GESTURE_LONG_PRESS(progressScaleGesture), 0.5);
	gtk_gesture_single_set_button(GTK_GESTURE_SINGLE(progressScaleGesture), 0);
	gtk_widget_add_controller(scaleBox, GTK_EVENT_CONTROLLER(progressScaleGesture));
	gtk_event_controller_set_propagation_phase(GTK_EVENT_CONTROLLER(progressScaleGesture), GTK_PHASE_CAPTURE);

	g_signal_connect(progressScaleGesture, "pressed", G_CALLBACK (ProgScrollBegin), NULL);
	g_signal_connect(progressScaleGesture, "released", G_CALLBACK (ProgScrollEnd), NULL);
	g_signal_connect(progressScale, "value-changed", G_CALLBACK(ProgScrollChange), NULL);

	GtkWidget *trackLabel;
	trackLabel = gtk_label_new("Artist - Track");
	
	gtk_label_set_xalign(GTK_LABEL(trackLabel), 0.0);
	gtk_grid_attach (GTK_GRID (progressTable), trackLabel, 0, 0, 3, 1);

	progLabel = gtk_label_new("00:00/99:99");
	
	gtk_label_set_xalign(GTK_LABEL(progLabel), 1.0);
	gtk_grid_attach (GTK_GRID (progressTable), progLabel, 3, 0, 1, 1);
	
	GtkWidget *volTable;
	volTable = gtk_grid_new();
	
	gtk_widget_set_hexpand_set(GTK_WIDGET(volTable), true);
	gtk_grid_set_baseline_row(GTK_GRID(volTable), 1);
	gtk_widget_set_valign(volTable, GTK_ALIGN_CENTER);
    gtk_widget_set_margin_start(volTable, 8);
	gtk_grid_attach(GTK_GRID(controlTable), volTable, 5, 0, 1, 1);

	GtkWidget *volumeIcon;
	volumeIcon = gtk_image_new_from_icon_name("audio-volume-high");

	gtk_grid_attach (GTK_GRID (volTable), volumeIcon, 0, 0, 1, 1);

	GtkWidget *volume;
	volume = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0.0, 1.0, 0.01);
	
	gtk_widget_set_hexpand(GTK_WIDGET(volume), true);
	gtk_scale_set_draw_value (GTK_SCALE (volume), FALSE);
	gtk_range_set_value (GTK_RANGE (volume), 1.0);
	g_signal_connect (volume, "value-changed", G_CALLBACK(ChangeVolume), NULL);
	gtk_grid_attach (GTK_GRID (volTable), volume, 1, 0, 1, 1);
	
	const char* classes[]{"controlPanelVolume", nullptr};
	gtk_widget_set_css_classes(volume, classes);

	GtkWidget *fileTable;
	fileTable = gtk_grid_new();
	gtk_widget_set_hexpand(GTK_WIDGET(fileTable), true);
	gtk_grid_attach(GTK_GRID(windowTable), fileTable, 0, 0, 3, 1);

	GtkWidget *labelFile;
	labelFile = gtk_label_new("File: ");
	gtk_label_set_xalign(GTK_LABEL(labelFile), 0.0);
	gtk_grid_attach (GTK_GRID (fileTable), labelFile, 0, 0, 1, 1);

	GtkWidget *labelFilename;
	labelFilename = gtk_label_new(FileName.c_str());
	gtk_widget_set_hexpand(GTK_WIDGET(labelFilename), true);
	gtk_label_set_xalign(GTK_LABEL(labelFilename), 0.5);
	gtk_label_set_ellipsize(GTK_LABEL(labelFilename), PANGO_ELLIPSIZE_MIDDLE);
	gtk_grid_attach (GTK_GRID (fileTable), labelFilename, 1, 0, 2, 1);

	GtkWidget *buttonFile;
    buttonFile = gtk_button_new_from_icon_name ("document-open-symbolic");
	gtk_widget_set_halign(buttonFile, GTK_ALIGN_END);
    gtk_accessible_update_property (GTK_ACCESSIBLE (buttonFile),
                                    GTK_ACCESSIBLE_PROPERTY_LABEL, "Select File",
                                    -1);
    g_signal_connect (buttonFile, "clicked", G_CALLBACK (GTKOpenFile), labelFilename);
	gtk_grid_attach (GTK_GRID (fileTable), buttonFile, 3, 0, 1, 1);

	StartProgressTimeout();

	if(FileName != "No file.")
	{
		//printf("Track Len: %f\n", BASS::BASSPlayer::GetTrackLenSecs());
		gtk_range_set_range(GTK_RANGE(progressScale), 0.0, BASS::BASSPlayer::GetTrackLenSecs());
		gtk_range_set_value(GTK_RANGE(progressScale), 0.0);
		
		const char *lenStr = BASS::BASSPlayer::GetTrackLenStr();
		char finalLenStr[20];
	
		std::snprintf(finalLenStr, sizeof(finalLenStr), "0:00 / %s", lenStr);
		
		gtk_label_set_text(GTK_LABEL(progLabel), finalLenStr);
	}
	
	gtk_window_present (GTK_WINDOW (window));
}

int main(int argc, char *argv[])
{
    BASS::BASSPlayer::Init();

    printf("Path: %s\n", argv[1]);

    if(argc < 2)
    {
        printf("No path specified.\n");
		FileName = "No file.";
        //return 0;
    } else {
		FileName = argv[1];
	}

	BASS::BASSPlayer::StartFilePlayback(FileName.c_str());

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
    BASS::BASSPlayer::Destroy();
    return 0;
}