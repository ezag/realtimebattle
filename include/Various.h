/*
RealTimeBattle, a robot programming game for Unix
Copyright (C) 1998-2000  Erik Ouchterlony and Ragnar Ouchterlony

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software Foundation,
Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#ifndef __VARIOUS__
#define __VARIOUS__

#ifndef NO_GRAPHICS
# include <glib.h>
#include <gtk/gtk.h>
# include <gdk/gdktypes.h>
# include <gtk/gtkwidget.h>
#endif 

#include "List.h"

//#include "String.h"
class String;
struct start_tournament_info_t;

typedef void (*StartTournamentFunction)( const List<start_tournament_info_t>&,
                                         const List<start_tournament_info_t>&,
                                         const int, const int, const int, void* );

#define min(a,b) ((a) < (b) ? (a) : (b))
#define max(a,b) ((a) > (b) ? (a) : (b))
#define abs(a) ((a) > 0 ? (a) : -(a))
#define sgn(a) ((a) > 0 ? 1 : -1)

enum entry_datatype_t
{
  ENTRY_INT,
  ENTRY_DOUBLE,
  ENTRY_HEX,
  ENTRY_CHAR,
  ENTRY_BOOL
};

struct entry_t
{
  entry_t(entry_datatype_t d, bool s) :
    datatype(d), allow_sign(s) {}
  entry_datatype_t datatype;
  bool allow_sign;
};

void Error(const bool fatal, const String& error_msg, const String& function_name);
void Quit(const bool success = true);

int factorial(const int n);

int binomial(const int n, const int k);

void reorder_pointer_array(void** array, int size);

void read_dirs_from_system(List<String>& robotdirs, List<String>& arenadirs);
void split_colonseparated_dirs(String& dirs, List<String>& arenadirs);

bool check_if_filename_is_robot( String& fname );
bool check_if_filename_is_arena( String& fname );
void check_for_robots_and_arenas( String& word, 
                                  List<start_tournament_info_t>& tour_list,
                                  List<String>& dir_list, 
                                  const bool check_robots );
void search_directories( String directory, List<start_tournament_info_t>& tour_list,
                         const bool check_robots );
bool parse_tournament_file( const String& fname,
                            const StartTournamentFunction function,
                            void* data, bool fatal_error_on_file_failure = true );


void create_tmp_rtb_dir();

#ifndef NO_GRAPHICS

void entry_handler( GtkWidget * entry, entry_t * entry_info );

GdkColor make_gdk_colour(const long col);
int long gdk2hex_colour(const GdkColor& col);

gint int_compare(GtkCList *clist, gconstpointer row1, gconstpointer row2);
gint float_compare(GtkCList *clist, gconstpointer row1, gconstpointer row2);
gint string_case_sensitive_compare(GtkCList *clist, gconstpointer row1, gconstpointer row2);
gint string_case_insensitive_compare(GtkCList *clist, gconstpointer row1, gconstpointer row2);

#endif 

#endif
