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


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <math.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <stdarg.h>
#include <sys/stat.h>
#include <stdio.h>

using namespace std;

//#include "Gui.h"
#include "ArenaRealTime.h"
#include "ArenaController.h"
#include "ArenaWindow.h"
//#include "MovingObject.h"
//#include "Shape.h"
#include "IntlDefs.h"
#include "Extras.h"
#include "Various.h"
#include "String.h"
#include "Shot.h"
#include "Options.h"
#include "Wall.h"
#include "Robot.h"
#ifndef NO_GRAPHICS
# include "ScoreWindow.h"
# include "ControlWindow.h"
# include "MessageWindow.h"
#endif

#ifndef NO_GRAPHICS
extern class ControlWindow* controlwindow_p;
#endif

ArenaRealTime::ArenaRealTime()
{
  set_game_mode( (ArenaBase::game_mode_t)the_arena_controller.game_mode );
  set_filenames( the_arena_controller.log_filename,
                 the_arena_controller.statistics_filename,
                 the_arena_controller.tournament_filename,
                 the_arena_controller.message_filename,
                 the_arena_controller.option_filename );

  robots_in_sequence = NULL;
}

ArenaRealTime::~ArenaRealTime()
{
  for(int i=0; i < sequences_in_tournament; i++)
    delete [] robots_in_sequence[i];

  if( robots_in_sequence != NULL )
    delete robots_in_sequence;

  if( use_log_file && LOG_FILE ) LOG_FILE.close();
  if( use_message_file && message_file ) message_file.close();
}


void
ArenaRealTime::clear()
{
  delete_lists(true, true, true, true);

  for(int i=0; i < sequences_in_tournament; i++)
    delete [] robots_in_sequence[i];

  sequence_nr = 0;
  sequences_in_tournament = 0;
  
  set_state( NOT_STARTED );
}

void
ArenaRealTime::set_filenames( String& log_fname,
                              const String& statistics_fname, 
                              const String& tournament_fname,
                              String& message_fname,
                              const String& option_fname )
{
  bool log_stdout = false;

  if( log_fname == "" )
    {
      use_log_file = false;
    }
  else if( log_fname == "-" || log_fname == "STDOUT" )  // use stdout as log_file
    {
      //cout << "Currently writing logs to standard out is not possible " << endl;
      use_log_file = false;
      LOG_FILE.ios::rdbuf(cout.rdbuf());
      use_log_file = true;
      log_stdout = true;
    }
  else
    {
       //LOG_FILE.open(log_fname.chars(), ios::out, S_IRUSR | S_IWUSR);
		LOG_FILE.open(log_fname.chars(), ios::out);
		
      use_log_file = true;
      if( !LOG_FILE )
        {
          Error( false, "Couldn't open log file. Contuing without log file",
                 "ArenaRealTime::set_filenames" );
          use_log_file = false;
        }
    }
  if( message_fname == "" )
    {
      use_message_file = false;
    }
  else if( message_fname == "-" || message_fname == "STDOUT" )
    {
      if( !log_stdout )
        {
          use_message_file = true;
          //message_file.attach( STDOUT_FILENO );
	  message_file.ios::rdbuf(cout.rdbuf());
        }
      else
        {
          use_message_file = false;
        }
    }
  else
    {
      use_message_file = true;
      message_file.open( message_fname.chars(), ios::out);
      if( !message_file )
        {
          Error( false, "Couldn't open message file. Message file disabled",
                 "ArenaRealTime::set_filenames" );
          use_message_file = false;
        }
      
    }

  statistics_file_name = statistics_fname;

  tournament_file_name = tournament_fname;

  option_file_name = option_fname;
}

void
ArenaRealTime::parse_arena_file(String& filename)
{
  Vector2D vec1, vec2, vec0;

  ifstream file(filename.chars());
  if( !file ) Error(true, "Couldn't open arena file" + filename, "ArenaBase::parse_arena_file");

  int succession = 1;
  double scale = the_opts.get_d(OPTION_ARENA_SCALE);
  double angle_factor = 1.0;
  do
    {
      parse_arena_line(file, scale, succession, angle_factor);

    } while( !file.eof() );

  file.close();

  
  if( use_log_file )      // copy the arena file to the log file
    {
      char buffer[500];
      
      ifstream file(filename.chars());
      if( !file ) Error(true, "Couldn't open arena file for log file" + filename, "ArenaBase::parse_arena_file");
  
      do
        {
          file >> ws;
          file.get(buffer, 500, '\n');
          if( buffer[0] != '\0' ) print_to_logfile('A', buffer);
        } 
      while( buffer[0] != '\0' );
      
    }

}

void
ArenaRealTime::print_to_logfile(const char first_letter ... )
{
  if( !use_log_file ) return;

  va_list args;
  va_start(args, first_letter);

  // log 'R' only each OPTION_LOG_EVERY_NTH_UPDATE_INTERVAL
  if( update_count_for_logging != 0 && first_letter == 'R' )
    {
      va_end(args);
      return;
    }

  LOG_FILE << first_letter;

  int prec = 2;
  LOG_FILE << setiosflags(ios::fixed) << setprecision(prec);

  switch(first_letter)
    {
    case 'R': // Robot position info
      LOG_FILE << va_arg(args, int   ) << " ";     // robot id;
      LOG_FILE << va_arg(args, double) << " ";  // x
      LOG_FILE << va_arg(args, double) << " ";  // y
      LOG_FILE << va_arg(args, double) << " ";  // robot angle
      LOG_FILE << va_arg(args, double) << " ";  // cannon angle
      LOG_FILE << va_arg(args, double) << " ";  // radar angle
      LOG_FILE << va_arg(args, double);         // energy
      break;

    case 'T': // Time
      LOG_FILE << setprecision(prec+1) << va_arg(args, double);  // time elapsed
      break;

    case 'P': // Print a robot message
      LOG_FILE << va_arg(args, int   ) << " ";  // robot id
      LOG_FILE << va_arg(args, char* );         // message to print
      break;

    case 'C': // Cookie
      LOG_FILE << va_arg(args, int   ) << " ";  // cookie id
      LOG_FILE << va_arg(args, double) << " ";  // x
      LOG_FILE << va_arg(args, double);         // y
      break;

    case 'M': // Mine
      LOG_FILE << va_arg(args, int   ) << " ";  // mine id
      LOG_FILE << va_arg(args, double) << " ";  // x
      LOG_FILE << va_arg(args, double);         // y
      break;

    case 'S': // Shot
      LOG_FILE << va_arg(args, int   ) << " ";  // shot id
      LOG_FILE << va_arg(args, double) << " ";  // x
      LOG_FILE << va_arg(args, double) << " ";  // y
      LOG_FILE << setprecision(prec+1) << va_arg(args, double) << " ";  // dx/dt
      LOG_FILE << setprecision(prec+1) << va_arg(args, double);         // dy/dt
      break;
      
    case 'D': // Die
      {
        int obj_type = va_arg(args, int  );
        LOG_FILE << (char)obj_type    << " ";  // object type to kill
        LOG_FILE << va_arg(args, int) << " ";  // object id
        if( obj_type == 'R' )
          {
            LOG_FILE << setprecision(1) 
                     <<  va_arg(args, double) << " "; // robot points received
            LOG_FILE << va_arg(args, int);            // position this game
          }
      }
      break;

    case 'G': // Game begins
      LOG_FILE << va_arg(args, int  ) << " ";  // sequence number
      LOG_FILE << va_arg(args, int  ) << " ";  // game number
      //      LOG_FILE << va_arg(args, char*);         // arena filename
      break;

    case 'H': // Header
      LOG_FILE << va_arg(args, int  ) << " ";  // number of games per sequence
      LOG_FILE << va_arg(args, int  ) << " ";  // number of robots per sequence
      LOG_FILE << va_arg(args, int  ) << " ";  // number of sequences
      LOG_FILE << va_arg(args, int  ) << " ";  // number of robots
      //      LOG_FILE << va_arg(args, char*);         // name of optionfile
      break;

    case 'L': // List of robot properties
      {
        LOG_FILE << va_arg(args, int  ) << " ";  // robot id
        LOG_FILE << hex2str(va_arg(args, long )) << " ";  // robot colour
       
        String name = va_arg(args, char*);   // robot name
        if( name == "" ) name = "Anonymous";
        LOG_FILE << name;
      }
      break;

    case 'A': // Arena file line
      LOG_FILE << va_arg(args, char*);  // line of arena file
      break;

    case 'O':
      {
        char option_type = (char)va_arg(args, int);
        LOG_FILE << va_arg(args, char*);                        // Option label
        switch( option_type )
          {
          case 'D':
            LOG_FILE << va_arg(args, double) ; // Option value
            break;
          case 'L':
            LOG_FILE << va_arg(args, long);   // Option value
            break;
          case 'H':
            LOG_FILE << hex2str( va_arg(args, long) ).chars();   // Option value
            break;
          case 'S':
            LOG_FILE << va_arg(args, char*);   // Option value
            break;
          }
      }
      break;

    default:
      Error(true, "Unrecognized first letter in logfile", "ArenaRealTime::print_to_logfile");
      break;
    }

  LOG_FILE << endl;
  va_end(args);
}

Vector2D
ArenaRealTime::get_random_position()
{
  return Vector2D( boundary[0][0] + (boundary[1][0] - boundary[0][0])*
                   (double)rand()/(double)RAND_MAX, 
                   boundary[0][1] + (boundary[1][1] - boundary[0][1])*
                   (double)rand()/(double)RAND_MAX );
}

void
ArenaRealTime::broadcast(const message_to_robot_type msg_type ...)
{
  va_list args;
  va_start(args, msg_type);
  String str = (String)message_to_robot[msg_type].msg + ' ';
  for(int i=0; i<message_to_robot[msg_type].number_of_args; i++)
    {
      switch(message_to_robot[msg_type].arg_type[i])
        {
        case NONE: 
          Error(true, "Couldn't send message, no arg_type", "ArenaRealTime::broadcast");
          break;
        case INT:
          str += (String)va_arg(args, int) + ' ';
          break;
        case DOUBLE:
          str += String(va_arg(args, double), 6) + ' ';
          break;
        case STRING:
          str += (String)va_arg(args, char*) + ' ';
          break;   
        case HEX:
          str += hex2str(va_arg(args, int)) + ' ';
          break;
        default:
          Error(true, "Couldn't send message, unknown arg_type", "ArenaRealTime::broadcast");
        }
    }
  str += '\n';

 ListIterator<Shape> li;
 for( object_lists[ROBOT].first(li); li.ok(); li++ )
    *(((Robot*)li())->get_outstreamp()) << str;
}

void
ArenaRealTime::quit_ordered()
{
  set_state( EXITING );
}

bool
ArenaRealTime::timeout_function()
{
#ifndef NO_GRAPHICS
      int old_total = (int)total_time;
#endif 

  if( state != PAUSED )
    {
      update_timer ();
    }

  switch(state)
    {
    case NOT_STARTED:
      if( the_arena_controller.auto_start_and_end )
        parse_tournament_file( tournament_file_name,
                               (StartTournamentFunction)
                               ArenaRealTime::start_tournament_from_tournament_file,
                               this );
      break;

    case FINISHED:
      if( the_arena_controller.auto_start_and_end )
        {
          if( statistics_file_name != "" )
            save_statistics_to_file( statistics_file_name );

          Quit();
        }
      break;
      
    case STARTING_ROBOTS:
      {
        read_robot_messages();

        if( total_time > next_check_time ) start_sequence_follow_up();
      }
      break;
      
    case SHUTTING_DOWN_ROBOTS:
      {     
        ListIterator<Robot> li;
        for( all_robots_in_sequence.first(li); li.ok(); li++ )
          li()->get_messages();
        
        if( total_time > next_check_time ) end_sequence_follow_up();
      }
      break;
      
    case GAME_IN_PROGRESS:
      {
        update();
#ifndef NO_GRAPHICS
        if((int)total_time > old_total && !no_graphics)
          the_gui.get_scorewindow_p()->set_window_title();
#endif
        if( robots_left <= 1 || total_time > the_opts.get_d(OPTION_TIMEOUT) ) 
          {
            end_game();
          }
        else
          {
            if( game_mode == COMPETITION_MODE && total_time > next_check_time ) check_robots();
            
            // Place mines and cookies
            if( ((double)rand()) / (double)RAND_MAX <= timestep*the_opts.get_d(OPTION_COOKIE_FREQUENCY) )
              add_cookie();
            
            if( ((double)rand()) / (double)RAND_MAX <= timestep*the_opts.get_d(OPTION_MINE_FREQUENCY) )
              add_mine();

              if( halt_next )
                {
                  set_state( PAUSED );
                  halt_next = false;
                }
          }
      }
      break;

    case PAUSED:
      break;

    case PAUSING_BETWEEN_GAMES:
      if( !pause_after_next_game ) start_game();
      break;

    case EXITING:
      return false;
      
    case BEFORE_GAME_START:
    case NO_STATE:
      Error(true, "Arena shouldn't be in this state here!!", "ArenaRealTime::timeout_function");
    }

  return true;
}

void
ArenaRealTime::update()
{
  print_to_logfile('T', total_time);

#ifndef NO_GRAPHICS
  if( state == GAME_IN_PROGRESS && !no_graphics)
    the_gui.get_messagewindow_p()->freeze_clist();
#endif

  read_robot_messages();
  move_shots(timestep);
  update_robots();

#ifndef NO_GRAPHICS
  if( state == GAME_IN_PROGRESS && !no_graphics)
    {
      the_gui.get_arenawindow_p()->draw_moving_objects( true );
      the_gui.get_messagewindow_p()->thaw_clist();
    }
#endif

  update_count_for_logging++;
  if( update_count_for_logging == the_opts.get_l(OPTION_LOG_EVERY_NTH_UPDATE_INTERVAL) )
    update_count_for_logging = 0;
}

void
ArenaRealTime::add_cookie()
{
  double en = the_opts.get_d(OPTION_COOKIE_MIN_ENERGY) + 
    (the_opts.get_d(OPTION_COOKIE_MAX_ENERGY) - the_opts.get_d(OPTION_COOKIE_MIN_ENERGY)) * 
    (double)rand() / (double)RAND_MAX;
  bool found_space = false;
  double r = the_opts.get_d(OPTION_COOKIE_RADIUS);
  Vector2D pos;

  for( int i=0; i<100 && !found_space; i++ )
    {
      pos = get_random_position();
      found_space = space_available(pos, r*2.0);
    }
  
  if( !found_space ) return;

  Cookie* cookiep = new Cookie(pos, en);
  object_lists[COOKIE].insert_last( cookiep );

  print_to_logfile('C', cookiep->get_id(), pos[0], pos[1]);
}

void
ArenaRealTime::add_mine()
{
  double en = the_opts.get_d(OPTION_MINE_MIN_ENERGY) + 
    (the_opts.get_d(OPTION_MINE_MAX_ENERGY) - the_opts.get_d(OPTION_MINE_MIN_ENERGY)) * 
    (double)rand() / (double)RAND_MAX;
  bool found_space = false;
  double r = the_opts.get_d(OPTION_MINE_RADIUS);
  Vector2D pos;

  for( int i=0; i<100 && !found_space; i++)
    {
      pos = get_random_position();
      found_space = space_available(pos, r*2.0);
    }
  
  if( !found_space )  return;

  Mine* minep = new Mine(pos, en);
  object_lists[MINE].insert_last( minep );

  print_to_logfile('M', minep->get_id(), pos[0], pos[1]);
}

void
ArenaRealTime::check_robots()
{
  ListIterator<Robot> li;
  for( all_robots_in_sequence.first(li); li.ok(); li++ )
    {
      if( li()->is_process_running() )
        {
          li()->check_process();
        }      
    } 

  next_check_time = total_time + the_opts.get_d(OPTION_CHECK_INTERVAL);
}

void
ArenaRealTime::read_robot_messages()
{
  ListIterator<Robot> li;
  for( all_robots_in_sequence.first(li); li.ok(); li++ )
    {
      if( li()->is_alive() || state != GAME_IN_PROGRESS )  
        li()->get_messages();
    }
}

void
ArenaRealTime::update_robots()
{
  Robot* robotp;

  robots_killed_this_round = 0;

  ListIterator<Shape> li;
  for( object_lists[ROBOT].first(li); li.ok(); li++ )
    {
      robotp = (Robot*)li();
      if( robotp->is_alive() )
        {
          robotp->update_radar_and_cannon(timestep);  
          robotp->change_velocity(timestep);
          robotp->move(timestep);        
        }
      if( robotp->is_alive() ) 
        robotp->get_messages();
    }

  // Check if robots have died and send energy level

  for( object_lists[ROBOT].first(li); li.ok(); li++ )
    {
      robotp = (Robot*)li();
      if( !robotp->is_alive() ) 
        {
          object_lists[ROBOT].remove(li);
          robots_killed_this_round++;
        }
      else
        {
          double lvls = (double)the_opts.get_l(OPTION_ROBOT_ENERGY_LEVELS);
          robotp->send_message( ENERGY, rint( robotp->get_energy() / lvls ) * lvls );
        }
    }

  robots_left -= robots_killed_this_round;

  if( robots_killed_this_round > 0 )
    {
      for( object_lists[ROBOT].first(li); li.ok(); li++ )
        {
          robotp = (Robot*)li();
          //          robotp->add_points(robots_killed_this_round);
#ifndef NO_GRAPHICS
          if( robots_left < 15 && !no_graphics ) 
            robotp->display_score();
#endif
        }

      ListIterator<Robot> li2;
      for( all_robots_in_sequence.first(li2); li2.ok(); li2++ )
        if( li2()->is_dead_but_stats_not_set() )
          li2()->set_stats(robots_killed_this_round);
      
      broadcast(ROBOTS_LEFT, robots_left);
    }

  
  for( object_lists[ROBOT].first(li); li.ok(); li++ )
    ((Robot*)li())->send_signal();
}


// coulor_dist is supposed to be a subjective distance between two colours, normalized
// to be between 0.0 (same colour) and 1.0 (furthest away).
double
ArenaRealTime::colour_dist(const long col1, const long int col2)
{
  double red1   = (col1 & 0x0000ff);
  double green1 = (col1 & 0x00ff00) >> 8;
  double blue1  = (col1 & 0xff0000) >> 16;

  double red2   = (col2 & 0x0000ff);
  double green2 = (col2 & 0x00ff00) >> 8;
  double blue2  = (col2 & 0xff0000) >> 16;

  return  ( fabs(red1  -red2  ) * log(1.0 + sqrt(red1  +red2  )) + 
            fabs(green1-green2) * log(1.0 + sqrt(green1+green2)) +
            fabs(blue1 -blue2 ) * log(1.0 + sqrt(blue1 +blue2 )) ) / 2417.8;


//    return( ((double)abs((col1 & 0xff)*0x101 - col2.blue))*
//            log(1.0 + sqrt((col1 & 0xff)*0x101 + col2.blue))/log(2.0) +
//            ((double)abs(((col1 & 0xff00) >> 8)*0x101 - col2.green))*
//            log(1.0 + sqrt(((col1 & 0xff00) >> 8)*0x101 + col2.green))/log(2.0) +
//            ((double)abs(((col1 & 0xff0000) >> 16)*0x101 - col2.red))*
//            log(1.0 + sqrt(((col1 & 0xff0000) >> 16)*0x101 + col2.red))/log(2.0));
}

bool
ArenaRealTime::is_colour_allowed(const long colour, const double min_dist, const Robot* robotp)
{
  double d;
  ListIterator<Robot> li;

  for( all_robots_in_sequence.first(li); li.ok(); li++ )
    {
      if(li() != robotp)
        {
          d = colour_dist( colour, li()->get_rgb_colour() );
          if( d < min_dist ) return false;          
        }
    }
  
  d = colour_dist( colour, the_opts.get_l( OPTION_BACKGROUND_COLOUR ) );
  if( d < min_dist ) return false;

  return true;
}

long int
ArenaRealTime::find_free_colour(const long int home_colour, 
                                 const long int away_colour, 
                                 const Robot* robotp, const bool random_colours)
{  
  long int tmp_colour;

  for(double min_dist = 0.1; min_dist > 0.01 ; min_dist *= 0.8)
    {
      if( !random_colours )
        {
          if( is_colour_allowed(home_colour, min_dist, robotp) ) return home_colour;
          if( is_colour_allowed(away_colour, min_dist, robotp) ) return away_colour;
        }
      for(int i=0; i<25; i++)
        {
          tmp_colour = rand() & 0xffffff;
          if( is_colour_allowed(tmp_colour, min_dist*2, robotp) ) return tmp_colour;
        }                  
    }
   Error(true, "Impossible to find colour", "ArenaRealTime::find_free_colour");

   return 0;
}

int
ArenaRealTime::set_debug_level( const int new_level)
{
  ArenaBase::set_debug_level(new_level);

  if( GAME_IN_PROGRESS )
    {
      broadcast(GAME_OPTION, DEBUG_LEVEL, (double)debug_level);
    }

  return debug_level;
}

void
ArenaRealTime::start_game()
{
  // put the arena together

  if( pause_after_next_game )
    {
      set_state( PAUSING_BETWEEN_GAMES );
      return;
    }  
  
  current_arena_nr = current_arena_nr % number_of_arenas + 1;
  
  String* filename = arena_filenames.get_nth(current_arena_nr);

  print_to_logfile('G', sequence_nr, game_nr + 1);

  parse_arena_file(*filename);

  int charpos;
  if( (charpos = filename->find('/',0,true)) != -1 )
    current_arena_filename = get_segment(*filename, charpos+1, -1);
  else
    Error(true, "Incomplete arena file path" + *filename, "ArenaRealTime::start_game");

  char msg[128];
  snprintf( msg, 127, _("Game %d of sequence %d begins on arena"),
            game_nr+1, sequence_nr );
  print_message( "RealTimeBattle", String(msg) + " " + current_arena_filename );

  // reset some variables

  shot_count = 0;
  cookie_count = 0;
  mine_count = 0;


  // Place robots on the arena

  Robot* robotp;
  bool found_space;
  double angle;
  Vector2D pos;

  robots_left = 0;
  
  set_state( BEFORE_GAME_START );

  ListIterator<Robot> li;
  for( all_robots_in_sequence.first(li); li.ok(); li++ )
    {
      robotp = li();
      robotp->get_messages();

      found_space = false;
      for( int i=0; i<100 && !found_space; i++)
        {
          pos = get_random_position();
          found_space = space_available(pos, the_opts.get_d(OPTION_ROBOT_RADIUS)*1.2);
        }

      if( !found_space ) Error(true, "Couldn't find space for all robots", "ArenaRealTime::start_game");
      angle = ((double)rand())*2.0*M_PI/RAND_MAX;
      robotp->set_values_before_game(pos, angle);
      object_lists[ROBOT].insert_last(robotp);
      robots_left++;
      robotp->live();
    }

  //  print_to_logfile('G', sequence_nr, games_per_sequence - games_remaining_in_sequence + 1);

  broadcast(GAME_STARTS);
  broadcast(ROBOTS_LEFT, robots_left);
  the_opts.broadcast_opts();


  update_count_for_logging = 0;
  print_to_logfile('T', 0.0);

  ListIterator<Shape> li2;
  for( object_lists[ROBOT].first(li2); li2.ok(); li2++ )
    {
      robotp = (Robot*)li2();
      robotp->send_signal();
      robotp->move(0.0);  // To log current position
    }

  set_state( GAME_IN_PROGRESS );
  game_nr++;

#ifndef NO_GRAPHICS
  if( !no_graphics )
    {
      the_gui.get_arenawindow_p()->clear_area();
      the_gui.get_arenawindow_p()->drawing_area_scale_changed();
      the_gui.get_scorewindow_p()->update_robots();

      reset_timer();  // Time should be zero in score window
      the_gui.get_scorewindow_p()->set_window_title();
      the_gui.get_arenawindow_p()->set_window_title();
    }
#endif

  reset_timer();  // Game starts !
  next_check_time = total_time + the_opts.get_d(OPTION_CHECK_INTERVAL);
}

void
ArenaRealTime::end_game()
{
  Robot* robotp;
  ListIterator<Robot> li;

  int robots_died_by_timeout = robots_left;
  robots_left = 0;

  for( all_robots_in_sequence.first(li); li.ok(); li++ )
    {
      robotp = li();
      if( robotp->is_alive() || robotp->is_dead_but_stats_not_set() )
        {
          robotp->die();
          robotp->set_stats(robots_died_by_timeout, true);
        }
    }

  broadcast(GAME_FINISHES);

  delete_lists(false, false, false, false);
  
  if(game_nr == games_per_sequence) 
    end_sequence();
  else
    start_game();
}


void
ArenaRealTime::start_sequence()
{
  game_nr = 0;
  current_arena_nr = 0;

  // Make list of robots in this sequence

  for(int i=0; i<robots_per_game; i++)
    {
      all_robots_in_sequence.
        insert_last( all_robots_in_tournament.
                     get_nth(robots_in_sequence[sequence_nr][i]));
    }

  // execute robot processes


  Robot* robotp;
  ListIterator<Robot> li;
  for( all_robots_in_sequence.first(li); li.ok(); li++ )
    {
      robotp = li();
      if( robotp->is_networked() )
        {
          *robotp->get_outstreamp() << "@R" << endl;
          robotp->set_values_at_process_start_up();
        }      
      else
        robotp->start_process();
    }
  
  // wait a second before checking

  set_state( STARTING_ROBOTS );
  sequence_nr++;
  reset_timer();
  next_check_time = total_time + the_opts.get_d(OPTION_ROBOT_STARTUP_TIME);
}

void
ArenaRealTime::start_sequence_follow_up()
{
  // check if the process have started correctly
  Robot* robotp = NULL;

  ListIterator<Robot> li;
  for( all_robots_in_sequence.first(li); li.ok(); li++ )
    {
      robotp = li();
      if( !(robotp->is_networked()) &&
          !(robotp->is_process_running()) ) 
        {
          all_robots_in_sequence.remove(li);
          robots_left--;
        }
      else
        {      
          if( !robotp->set_and_get_has_competed() )
            print_to_logfile('L', robotp->get_id(), robotp->get_rgb_colour(), 
                             robotp->get_robot_name().chars());
          
          if( !robotp->is_name_given() )
            {
              robotp->send_message(WARNING, NAME_NOT_GIVEN, "");
              char msg[128];
              snprintf( msg, 127, _("Robot with filename %s has not given any name"),
                        robotp->get_robot_filename().chars() );
              print_message( "RealTimeBattle", String(msg) );
            }

          if( !robotp->is_colour_given() )
            {
              robotp->send_message(WARNING, COLOUR_NOT_GIVEN, "");
              robotp->set_colour( find_free_colour( 0, 0, robotp, true ) );
              robotp->set_colour_given( true );
            }
        }

    }
  start_game();
}

void
ArenaRealTime::end_sequence()
{
  // kill all robot processes

  ListIterator<Robot> li;
  for( all_robots_in_sequence.first(li); li.ok(); li++ )
    {
      li()->end_process();
    }

  // wait a second before checking

  set_state( SHUTTING_DOWN_ROBOTS);
  next_check_time = total_time + 1.0;
}

void
ArenaRealTime::end_sequence_follow_up()
{
  // check if the process have stopped, otherwise kill
  
  Robot* robotp;

  ListIterator<Robot> li;
  for( all_robots_in_sequence.first(li); li.ok(); li++ )
    {
      robotp = li();
      if( robotp->is_process_running() ) robotp->kill_process_forcefully();
      robotp->delete_pipes();
      all_robots_in_sequence.remove(li);
    }

  if( sequence_nr == sequences_in_tournament ) 
    end_tournament();
  else
    start_sequence();
}

void
ArenaRealTime::start_tournament_from_tournament_file
( const List<start_tournament_info_t>& robotfilename_list, 
  const List<start_tournament_info_t>& arenafilename_list, 
  const int robots_p_game, const int games_p_sequence, 
  const int n_o_sequences, ArenaRealTime* ar_p )
{
  ar_p->start_tournament( robotfilename_list, arenafilename_list, robots_p_game,
                          games_p_sequence, n_o_sequences );
}

void
ArenaRealTime::
start_tournament(const List<start_tournament_info_t>& robotfilename_list, 
                 const List<start_tournament_info_t>& arenafilename_list, 
                 const int robots_p_game, 
                 const int games_p_sequence, 
                 const int n_o_sequences)
{
  clear();

  // Open windows if they were closed, else clear them 

#ifndef NO_GRAPHICS
  if( !no_graphics )
    {
      if( the_gui.is_messagewindow_up() )
        MessageWindow::clear_clist( NULL, the_gui.get_messagewindow_p() );
      else //if( !use_message_file )
        the_gui.open_messagewindow();

      if( !the_gui.is_scorewindow_up() ) the_gui.open_scorewindow();
      if( !the_gui.is_arenawindow_up() ) the_gui.open_arenawindow();
    }
#endif

  // Create robot classes and put them into the list all_robots_in_tournament

  number_of_robots = 0;
  robot_count = 0;
  Robot* robotp;
  start_tournament_info_t* infop = NULL;
  String* stringp;

  ListIterator<start_tournament_info_t> li;
  for( robotfilename_list.first(li); li.ok(); li++ )
    {
      infop = li();
      robotp = new Robot(infop->filename);
      all_robots_in_tournament.insert_last( robotp );
      number_of_robots++;
    }

  // Create list of arena filenames
  number_of_arenas = 0;
  
  for( arenafilename_list.first(li); li.ok(); li++ )
    {
      stringp = new String(li()->filename);
      arena_filenames.insert_last( stringp );
      number_of_arenas++;
    }

  robots_per_game = robots_p_game;
  games_per_sequence = games_p_sequence;
  sequences_in_tournament = n_o_sequences;

  // make list of robots to compete in the sequences
  int games_per_round = binomial(number_of_robots, robots_per_game);
  int complete_rounds = n_o_sequences / games_per_round;
  int rem_games = n_o_sequences % games_per_round;

  robots_in_sequence = new int*[n_o_sequences];
  for(int i=0; i<n_o_sequences; i++) robots_in_sequence[i] = new int[robots_per_game];
  
  int current_sequence[robots_per_game];
  int current_nr = 0;
  //  for(int i=0; i<robots_per_game; i++) current_sequence[i] = i+1;
  
  // set complete rounds first

  for(int round_nr=0; round_nr < complete_rounds; round_nr++)
    {
      int k, i, j;

      for(i=0; i<robots_per_game; i++) current_sequence[i] = i+1;
      current_sequence[robots_per_game-1]--;   // To be increased first time

      
      for(i=0; i< games_per_round; i++)
        {
          
          k = robots_per_game - 1;
          while( current_sequence[k] == number_of_robots + 1 - robots_per_game + k )
            k--;

          if( k < 0 ) Error(true, "Problem generating list of participants, k < 0", 
                            "ArenaRealTime::start_tournament");

          current_sequence[k]++;
          for(j=k+1; j<robots_per_game; j++) current_sequence[j] = current_sequence[j-1]+1;

          for(j=0; j<robots_per_game; j++) 
            robots_in_sequence[current_nr][j] = current_sequence[j];

          current_nr++;
        }
      reorder_pointer_array((void**)robots_in_sequence, games_per_round);
    }

  // the remaining game will be randomly chosen

  int robot_matches_played[number_of_robots];
  for(int i=0; i<number_of_robots; i++) robot_matches_played[i] = 0;

  bool robot_playing_this_match[number_of_robots];
  int min_matches_played = 0;
  int number_of_robots_on_min_matches = number_of_robots;
  int nr;

  for(int i=0; i< rem_games; i++)
    {
      for(int i2=0; i2<number_of_robots; i2++) robot_playing_this_match[i2] = false;

      for(int j=0; j<robots_per_game; j++)
        {
          do 
            nr = (int)floor(number_of_robots*((double)rand() / (double)RAND_MAX));
          while( robot_playing_this_match[nr] || robot_matches_played[nr] != min_matches_played );

          robot_playing_this_match[nr] = true;
          robot_matches_played[nr]++;
          number_of_robots_on_min_matches--;
          if( number_of_robots_on_min_matches == 0 ) 
            {
              min_matches_played++;
              number_of_robots_on_min_matches = number_of_robots;
            }

          robots_in_sequence[current_nr][j] = nr + 1;   // robot count will start from 1
        }      
      current_nr++;
    }

  // set random seed


  srand(timer.get_random_seed());

  // start first sequence

  print_to_logfile('H', games_per_sequence, robots_per_game, 
                   sequences_in_tournament, number_of_robots);

  the_opts.log_all_options();

  sequence_nr = 0;
  start_sequence();
}

void
ArenaRealTime::end_tournament()
{
  set_state( FINISHED );

#ifndef NO_GRAPHICS
  if( !no_graphics )
    {
      //      if( !use_message_file )
      //        the_gui.close_messagewindow();
      //      the_gui.close_scorewindow();
      the_gui.close_arenawindow();
    }
#endif
}
