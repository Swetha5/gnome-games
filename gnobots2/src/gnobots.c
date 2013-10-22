/*
 * Gnome Robots II
 * written by Mark Rae <m.rae@inpharmatica.co.uk>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * For more details see the file COPYING.
 */

#include <config.h>

#include <sys/time.h>
#include <string.h>
#include <stdlib.h>
#include <locale.h>

#include <glib/gi18n.h>
#include <glib.h>

#include "gbdefs.h"
#include "statusbar.h"
#include "gameconfig.h"
#include "graphics.h"
#include "menu.h"
#include "sound.h"
#include "properties.h"
#include "game.h"
#include "cursors.h"
#include "games-gridframe.h"
#include "games-scores.h"
#include "games-scores-dialog.h"
#include "games-stock.h"

/* Minimum sizes. */
#define MINIMUM_TILE_WIDTH   8
#define MINIMUM_TILE_HEIGHT  MINIMUM_TILE_WIDTH

#define KEY_GEOMETRY_GROUP "geometry"

/**********************************************************************/
/* Exported Variables                                                 */
/**********************************************************************/
GtkWidget *app = NULL;
static gint window_width = 0, window_height = 0;
static gboolean window_is_fullscreen = FALSE, window_is_maximized = FALSE;
GtkWidget *game_area = NULL;
GamesScores *highscores;
GSettings *settings;
/**********************************************************************/


/**********************************************************************/
/* File Static Variables                                              */
/**********************************************************************/
static gchar *cmdline_scenario = NULL;
static gchar *cmdline_config = NULL;
static gint session_xpos = -1;
static gint session_ypos = -1;
/**********************************************************************/


/**********************************************************************/
/* File Static Variables                                              */
/**********************************************************************/
static const GOptionEntry options[] = {
  {"scenario", 's', 0, G_OPTION_ARG_STRING, &cmdline_scenario,
   N_("Set game scenario"), N_("NAME")},
  {"config", 'c', 0, G_OPTION_ARG_STRING, &cmdline_config,
   N_("Set game configuration"), N_("NAME")},
  {"x", 'x', 0, G_OPTION_ARG_INT, &session_xpos,
   N_("Initial window position"), N_("X")},
  {"y", 'y', 0, G_OPTION_ARG_INT, &session_ypos,
   N_("Initial window position"), N_("Y")},
  {NULL},
};

static const GamesScoresCategory scorecats[] = { 
  {"classic_robots", N_("Classic robots")},
  {"classic_robots-safe", N_("Classic robots with safe moves")},
  {"classic_robots-super-safe", N_("Classic robots with super-safe moves")},
  {"nightmare", N_("Nightmare")},
  {"nightmare-safe", N_("Nightmare with safe moves")},
  {"nightmare-super-safe", N_("Nightmare with super-safe moves")},
  {"robots2", N_("Robots2")},
  {"robots2-safe", N_("Robots2 with safe moves")},
  {"robots2-super-safe", N_("Robots2 with super-safe moves")},
  {"robots2_easy", N_("Robots2 easy")},
  {"robots2_easy-safe", N_("Robots2 easy with safe moves")},
  {"robots2_easy-super-safe", N_("Robots2 easy with super-safe moves")},
  {"robots_with_safe_teleport", N_("Robots with safe teleport")},
  {"robots_with_safe_teleport-safe", N_("Robots with safe teleport with safe moves")},
  {"robots_with_safe_teleport-super-safe", N_("Robots with safe teleport with super-safe moves")}
};

/**********************************************************************/


/**********************************************************************/
/* Function Prototypes                                                */
/**********************************************************************/
/**********************************************************************/


/**********************************************************************/
/* Function Definitions                                               */
/**********************************************************************/

static gboolean
window_configure_event_cb (GtkWidget *widget, GdkEventConfigure *event)
{
  if (!window_is_maximized && !window_is_fullscreen)
  {
    window_width = event->width;
    window_height = event->height;
  }
  
  return FALSE;
}

static gboolean
window_state_event_cb (GtkWidget *widget, GdkEventWindowState *event)
{
  if ((event->changed_mask & GDK_WINDOW_STATE_MAXIMIZED) != 0)
    window_is_maximized = (event->new_window_state & GDK_WINDOW_STATE_MAXIMIZED) != 0;
  if ((event->changed_mask & GDK_WINDOW_STATE_FULLSCREEN) != 0)
    window_is_fullscreen = (event->new_window_state & GDK_WINDOW_STATE_FULLSCREEN) != 0;
  return FALSE;
}

void
quit_game (void)
{
  g_settings_set_int (settings, "window-width", window_width);
  g_settings_set_int (settings, "window-height", window_height);
  g_settings_set_boolean (settings, "window-is-maximized", window_is_maximized);
  g_settings_set_boolean (settings, "window-is-fullscreen", window_is_fullscreen);
  gtk_main_quit ();
}

/**
 * main
 * @argc: number of arguments
 * @argv: arguments
 *
 * Description:
 * main
 *
 * Returns:
 * exit code
 **/
int
main (int argc, char *argv[])
{
  GtkWidget *errordialog;
  GtkWidget *vbox, *menubar, *toolbar, *statusbar, *gridframe;
  GtkUIManager *ui_manager;
  GOptionContext *context;
  struct timeval tv;
  gint i;
  gchar *config;
  gboolean retval;
  GError *error = NULL;

  setlocale (LC_ALL, "");
  bindtextdomain (GETTEXT_PACKAGE, LOCALEDIR);
  bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
  textdomain (GETTEXT_PACKAGE);

  games_scores_startup ();

  gettimeofday (&tv, NULL);
  srand (tv.tv_usec);

  context = g_option_context_new (NULL);
  g_option_context_set_translation_domain (context, GETTEXT_PACKAGE);
  g_option_context_add_group (context, gtk_get_option_group (TRUE));

  g_option_context_add_main_entries (context, options, GETTEXT_PACKAGE);

  retval = g_option_context_parse (context, &argc, &argv, &error);
  g_option_context_free (context);
  if (!retval) {
    g_print ("%s", error->message);
    g_error_free (error);
    exit (1);
  }

  g_set_application_name (_("Robots"));

  highscores = games_scores_new ("gnobots2",
                                 scorecats, G_N_ELEMENTS (scorecats),
                                 NULL, NULL,
                                 0 /* default category */,
                                 GAMES_SCORES_STYLE_PLAIN_DESCENDING);

  settings = g_settings_new ("org.gnome.gnobots2");

  gtk_window_set_default_icon_name ("gnobots2");

  app = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (app), _("Robots"));
  g_signal_connect (GTK_WINDOW (app), "configure-event", G_CALLBACK (window_configure_event_cb), NULL);
  g_signal_connect (GTK_WINDOW (app), "window-state-event", G_CALLBACK (window_state_event_cb), NULL);
  gtk_window_set_default_size (GTK_WINDOW (app), g_settings_get_int (settings, "window-width"), g_settings_get_int (settings, "window-height"));
  if (g_settings_get_boolean (settings, "window-is-fullscreen"))
    gtk_window_fullscreen (GTK_WINDOW (app));
  if (g_settings_get_boolean (settings, "window-is-maximized"))
    gtk_window_maximize (GTK_WINDOW (app));

  g_signal_connect (G_OBJECT (app), "delete_event",
                   G_CALLBACK (quit_game), NULL);

  statusbar = gnobots_statusbar_new ();
  ui_manager = gtk_ui_manager_new ();

  games_stock_prepare_for_statusbar_tooltips (ui_manager, statusbar);
  create_game_menus (ui_manager);
  gtk_window_add_accel_group (GTK_WINDOW (app),
			      gtk_ui_manager_get_accel_group (ui_manager));

  menubar = gtk_ui_manager_get_widget (ui_manager, "/MainMenu");

  toolbar = gtk_ui_manager_get_widget (ui_manager, "/Toolbar");
  gtk_style_context_add_class (gtk_widget_get_style_context (toolbar),
			       GTK_STYLE_CLASS_PRIMARY_TOOLBAR);

  make_cursors ();

  game_area = gtk_drawing_area_new ();
  gtk_widget_add_events (game_area, GDK_BUTTON_PRESS_MASK |
			 GDK_POINTER_MOTION_MASK);
  g_signal_connect (G_OBJECT (game_area), "button-press-event",
		    G_CALLBACK (mouse_cb), NULL);
  g_signal_connect (G_OBJECT (game_area), "motion-notify-event",
		    G_CALLBACK (move_cb), NULL);
  g_signal_connect (G_OBJECT (game_area), "configure-event",
		    G_CALLBACK (resize_cb), NULL);
  g_signal_connect (G_OBJECT (game_area), "draw",
		    G_CALLBACK (draw_cb), NULL);

  gridframe = games_grid_frame_new (GAME_WIDTH, GAME_HEIGHT);
  gtk_container_add (GTK_CONTAINER (gridframe), game_area);

  vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  gtk_box_pack_start (GTK_BOX (vbox), menubar, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX (vbox), toolbar, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX (vbox), gridframe, TRUE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX (vbox), statusbar, FALSE, FALSE, 0);

  gtk_container_add (GTK_CONTAINER (app), vbox);

  gtk_widget_set_size_request (GTK_WIDGET (game_area),
			       MINIMUM_TILE_WIDTH * GAME_WIDTH,
			       MINIMUM_TILE_HEIGHT * GAME_HEIGHT);

  /* Set the window position if it was set by the session manager */
  if (session_xpos >= 0 && session_ypos >= 0) {
    gtk_window_move (GTK_WINDOW (app), session_xpos, session_ypos);
  }

  gtk_widget_show_all (app);

  if (!load_game_configs ()) {
    /* Oops, no configs, we probably haven't been installed properly. */
    errordialog = gtk_message_dialog_new_with_markup (NULL, 0, GTK_MESSAGE_ERROR,
					  GTK_BUTTONS_OK,
					  "<b>%s</b>\n\n%s",
					  _("No game data could be found."),
					  _
					  ("The program Robots was unable to find any valid game configuration files. Please check that the program is installed correctly."));
    gtk_window_set_resizable (GTK_WINDOW (errordialog), FALSE);
    gtk_dialog_run (GTK_DIALOG (errordialog));
    exit (1);
  }

  load_properties ();

  if (!load_game_graphics ()) {
    /* Oops, no graphics, we probably haven't been installed properly. */
    errordialog = gtk_message_dialog_new_with_markup (GTK_WINDOW (app),
					  GTK_DIALOG_MODAL,
					  GTK_MESSAGE_ERROR,
					  GTK_BUTTONS_OK,
					  "<b>%s</b>\n\n%s",
					  _
					  ("Some graphics files are missing or corrupt."),
					  _
					  ("The program Robots was unable to load all the necessary graphics files. Please check that the program is installed correctly."));
    gtk_dialog_run (GTK_DIALOG (errordialog));
    exit (1);
  }

  connect_toolbar_toggle (toolbar);

  init_sound ();

  init_game ();

  if (cmdline_scenario) {
    set_game_graphics (cmdline_scenario);
  }

  if (cmdline_config) {
    for (i = 0; i < num_game_configs (); ++i) {
      config = game_config_name (i);
      if (!strcmp (cmdline_config, config)) {
	properties_set_config (i);
	g_free (config);
	break;
      }
      g_free (config);
    }
  }

  gtk_main ();

  g_settings_sync();

  return 0;
}

/**********************************************************************/
