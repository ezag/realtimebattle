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

#if HAVE_DIRENT_H
# include <dirent.h>
# define NAMLEN(dirent) strlen((dirent)->d_name)
#else
# define dirent direct
# define NAMLEN(dirent) (dirent)->d_namlen
# if HAVE_SYS_NDIR_H
#  include <sys/ndir.h>
# endif
# if HAVE_SYS_DIR_H
#  include <sys/dir.h>
# endif
# if HAVE_NDIR_H
#  include <ndir.h>
# endif
#endif

//#include "Gui.h"
#include "ArenaBase.h"
#include "ArenaController.h"
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
# include "ControlWindow.h"
# include "MessageWindow.h"
extern class ControlWindow* controlwindow_p;
#endif 

ArenaBase::ArenaBase()
{
  state = NOT_STARTED;
  game_mode = NORMAL_MODE;

  sequence_nr = 0;
  sequences_in_tournament = 0;
  game_nr = 0;
  games_per_sequence = 0;

  halt_next = false;
  pause_after_next_game = false;

  reset_timer();
  
  object_lists[ROBOT].set_deletion_responsibility(false);
  all_robots_in_sequence.set_deletion_responsibility(false);

  debug_level = 0;
}

ArenaBase::~ArenaBase()
{
  //  if( filep ) delete filep;
  state = EXITING;
  sleep(1);

  delete_lists(true, true, true, true);
}

void
ArenaBase::set_state( const state_t st )
{
  state = st;

#ifndef NO_GRAPHICS
  if( !no_graphics )
    {
      String infotext;
      switch( state )
        {
        case NO_STATE:
        case NOT_STARTED:
          infotext = "RealTimeBattle";
          break;
        case STARTING_ROBOTS:
          infotext = "RTB  " + (String)_("*Starting robots*");
          break;
        case SHUTTING_DOWN_ROBOTS:
          infotext = "RTB  " + (String)_("*Shutting down robots*");
          break;
        case BEFORE_GAME_START:
        case GAME_IN_PROGRESS:
          infotext = "RealTimeBattle  " + (String)_("*Running*");
          if( pause_after_next_game )
            infotext = "RTB  " + (String)_("*Pausing after game*");
          break;
        case PAUSING_BETWEEN_GAMES:
        case PAUSED:
          infotext = "RealTimeBattle  " + (String)_("*Paused*");
          break;
        case EXITING:
          infotext = "RealTimeBattle  " + (String)_("*Exiting*");
          break;
        case FINISHED:
          infotext = "RealTimeBattle  " + (String)_("*Finished*");
          break;
          
        default:
          Error(true, "Unknown state", "ArenaBase::set_state");
        }
      
      controlwindow_p->set_window_title( infotext );
    }
#endif 
}

void 
ArenaBase::interrupt_tournament()
{
  if( state == GAME_IN_PROGRESS || state == PAUSED ||
      state == PAUSING_BETWEEN_GAMES )
    {
      the_arena_controller.auto_start_and_end = false;
#ifndef NO_GRAPHICS
      if( !no_graphics )
        {
          controlwindow_p->remove_replay_widgets();
          the_gui.close_scorewindow();
          the_gui.close_arenawindow();
          the_gui.close_messagewindow();
        }
#endif

      delete_lists(true, true, false, true);
      
      set_state( FINISHED );
    }
}

// This function takes the statistics and saves into a selected file
void
ArenaBase::save_statistics_to_file(String filename)
{
  
  
  ofstream file(filename.chars());

  stat_t* statp;
  Robot* robotp;

  ListIterator<Robot> li;
  ListIterator<stat_t> stat_li;
  for(all_robots_in_tournament.first(li); li.ok() ; li++ )
    {
      robotp = li();
      file << robotp->get_robot_name() << ": " << endl;
      for(robotp->get_statistics()->first(stat_li); stat_li.ok(); stat_li++)
        {
          statp = stat_li();
          file << "Seq: " << statp->sequence_nr 
               << "  Game: " << statp->game_nr 
               << "  Pos: " << statp->position
               << "  Points: " << statp->points 
               << "  Time Survived: " << statp->time_survived
               << "  Total Points: " << statp->total_points << endl;
        }
    }
    
    
}

void
ArenaBase::print_message( const String& messager,
                          const String& text )
{
  if( use_message_file )
    message_file << messager << ": " << text << endl;
#ifndef NO_GRAPHICS
  else if( !no_graphics && the_gui.is_messagewindow_up() )
    the_gui.get_messagewindow_p()->add_message( messager, text );
#endif
}

void
ArenaBase::parse_arena_line(ifstream& file, double& scale, int& succession, double& angle_factor)
{
  char text[20];
  double radius, radius2, bounce_c, hardn, thickness;
  int vertices;

  Vector2D vec1, vec2, vec0, center;
  WallLine* wall_linep;
  WallCircle* wall_circlep;
  WallInnerCircle* wall_inner_circlep;
  WallArc* wall_arcp;


  file >> ws;
  file.get(text, 20, ' ');
  if( strcmp(text, "scale" ) == 0 )
    {
      if( succession != 1 ) Error(true, "Error in arenafile: 'scale' not first", 
                                  "ArenaBase::parse_arena_line");
      succession = 2;
      double scl;
      file >> scl;
      scale *= scl;
    }
  else if( strcmp(text, "angle_unit" ) == 0 )
    {
      char unit[20];
      file >> ws;
      file.get(unit, 20, '\n');
      if( strcmp(unit, "radians" ) == 0)
        angle_factor = 1.0;
      else if( strcmp(unit, "degrees" ) == 0 )
        angle_factor = M_PI / 180.0;
      else
        Error(true, "Error in arenafile: Unknown angle unit: " + String(unit), 
              "ArenaBase::parse_arena_line");
    }
  else if( strcmp(text, "boundary" ) == 0 )
    {
      if( succession > 2 ) 
        Error(true, "Error in arenafile: 'boundary' after wallpieces or duplicate", 
              "ArenaBase::parse_arena_line");
      succession = 3;
      double b1, b2;
      file >> b1;
      file >> b2;
      boundary[0] = Vector2D(scale*b1, scale*b2);
      file >> b1;
      file >> b2;
      boundary[1] = Vector2D(scale*b1, scale*b2);
      if( boundary[1][0] - boundary[0][0] <= 0 || 
          boundary[1][1] - boundary[0][1] <= 0 ) 
        Error(true, "Error in arenafile: 'boundary' negative", 
              "ArenaBase::parse_arena_line");
    }
  else if( strcmp(text, "exclusion_point" ) == 0 )
    {
      if( succession < 3 ) 
        Error(true, "Error in arenafile: 'boundary' after wallpieces or duplicate", 
              "ArenaBase::parse_arena_line");
      file >> vec1;
      exclusion_points.insert_last(new Vector2D(scale*vec1));
    }
  else if( strcmp(text, "inner_circle" ) == 0 )
    {
      if( succession < 3 ) 
        Error(true, "Error in arenafile: 'inner_circle' before boundary", 
              "ArenaBase::parse_arena_line");
      succession = 4;
      file >> bounce_c;
      file >> hardn;
      file >> vec1;
      file >> radius;
      
      wall_inner_circlep = new WallInnerCircle(scale*vec1, scale*radius, bounce_c, hardn);
      object_lists[WALL].insert_first( wall_inner_circlep );
    }
  else if( strcmp(text, "circle" ) == 0 )
    {
      if( succession < 3 ) 
        Error(true, "Error in arenafile: 'circle' before 'boundary'", 
              "ArenaBase::parse_arena_line");
      succession = 4;
      file >> bounce_c;
      file >> hardn;
      file >> vec1;
      file >> radius;
      wall_circlep = new WallCircle(scale*vec1, scale*radius, bounce_c, hardn);
      object_lists[WALL].insert_last(wall_circlep);
    }
  else if( strcmp(text, "arc" ) == 0 )
    {
      if( succession < 3 ) 
        Error(true, "Error in arenafile: 'arc' before 'boundary'", 
              "ArenaBase::parse_arena_line");
      succession = 4;
      double angle1, angle2;
      file >> bounce_c;
      file >> hardn;
      file >> center;
      file >> radius;
      file >> radius2;
      file >> angle1;
      file >> angle2;
      
      wall_arcp = new WallArc(scale*center, scale*radius, scale*radius2, 
                              angle1 * angle_factor, angle2 * angle_factor,
                              bounce_c, hardn);
      object_lists[WALL].insert_last(wall_arcp);
    }
  else if( strcmp(text, "line" ) == 0 )
    {
      if( succession < 3 ) Error(true, "Error in arenafile: 'line' before 'boundary'",
                                 "ArenaBase::parse_arena_line");
      succession = 4;
      file >> bounce_c;
      file >> hardn;
      file >> thickness; 
      thickness *= 0.5;
      file >> vec1;      // start_point
      file >> vec2;      // end_point


      if( length(vec2-vec1) == 0.0 ) 
        Error(true, "Error in arenafile: Zero length line", 
              "ArenaBase::parse_arena_line");

      wall_linep = new WallLine(scale*vec1, unit(vec2-vec1), scale*length(vec2-vec1), 
                                scale*thickness, bounce_c , hardn);      
      object_lists[WALL].insert_last( wall_linep );
    }
  else if( strcmp(text, "polygon" ) == 0 )
    {
      if( succession < 3 ) 
        Error(true, "Error in arenafile: 'polygon' before 'boundary'", 
              "ArenaBase::parse_arena_line");
      succession = 4;
      file >> bounce_c;
      file >> hardn;
      file >> thickness;
      thickness *= 0.5;
      file >> vertices;   // number of vertices
      file >> vec1;      // first point
      wall_circlep = new WallCircle(scale*vec1, scale*thickness, bounce_c, hardn);
      object_lists[WALL].insert_last( wall_circlep );

      for(int i=1; i<vertices; i++)
        {
          vec2 = vec1;
          file >> vec1;      // next point

          if( length(vec2-vec1) == 0.0 ) 
            Error(true, "Error in arenafile: Zero length line in polygon", 
                  "ArenaBase::parse_arena_line");

          wall_linep = new WallLine(scale*vec2, unit(vec1-vec2), 
                                    scale*length(vec1-vec2), 
                                    scale*thickness, bounce_c , hardn);      
          object_lists[WALL].insert_last( wall_linep );
          wall_circlep = new WallCircle(scale*vec1, scale*thickness, bounce_c, hardn);
          object_lists[WALL].insert_last( wall_circlep );
        }
    }
  else if( strcmp(text, "closed_polygon" ) == 0 )
    {
      if( succession < 3 ) 
        Error(true, "Error in arenafile: 'closed_polygon' before 'boundary'", 
              "ArenaBase::parse_arena_line");
      succession = 4;
      file >> bounce_c;
      file >> hardn;
      file >> thickness;
      thickness *= 0.5;
      file >> vertices;   // number of vertices
      file >> vec1;      // first point
      wall_circlep = new WallCircle(scale*vec1, scale*thickness, bounce_c, hardn);
      object_lists[WALL].insert_last( wall_circlep );
      vec0 = vec1;

      for(int i=1; i<vertices; i++)
        {
          vec2 = vec1;
          file >> vec1;      // next point

          if( length(vec2-vec1) == 0.0 )
            Error(true, "Error in arenafile: Line in closed_polygon of zero length", 
                  "ArenaBase::parse_arena_line");
          
          wall_linep = new WallLine(scale*vec2, unit(vec1-vec2), 
                                    scale*length(vec1-vec2), 
                                    scale*thickness, bounce_c , hardn);      
          object_lists[WALL].insert_last( wall_linep );
          wall_circlep = new WallCircle(scale*vec1, scale*thickness, bounce_c, hardn);
          object_lists[WALL].insert_last( wall_circlep );
        }

      if( length(vec0-vec1) == 0.0 ) 
        Error(true, "Error in arenafile: Last line in closed_polygon of zero length", 
              "ArenaBase::parse_arena_line");

      wall_linep = new WallLine(scale*vec1, unit(vec0-vec1), scale*length(vec0-vec1), 
                                scale*thickness, bounce_c , hardn);      
      object_lists[WALL].insert_last( wall_linep );
    }

  else if( strcmp(text, "poly_curve" ) == 0 )
    {
      if( succession < 3 ) 
        Error(true, "Error in arenafile: 'closed_polygon' before 'boundary'", 
              "ArenaBase::parse_arena_line");
      succession = 4;
      file >> bounce_c;
      file >> hardn;
      file >> thickness;
      thickness *= 0.5;

      Vector2D current_pos, direction;
      file >> current_pos;      // first point
      wall_circlep = new WallCircle(scale*current_pos, scale*thickness, bounce_c, hardn);
      object_lists[WALL].insert_last( wall_circlep );

      file >> direction;      // start direction      
      
      direction.normalize();

      if( lengthsqr(direction) < 0.01 ) 
        Error(true, "Error in arenafile: directions must not be zero", 
              "ArenaBase::parse_arena_line");

      vec0 = current_pos;

      char c;
      double len, angle, start_angle, end_angle, tmp;
      bool finish = false;
      do
        {
          file >> c;
          
          switch( c )
            {
            case 'L':  // line
              file >> len;
              if( len <= 0.0 )
                Error(true, "Error in arenafile: Line in poly_curve must be positive", 
                      "ArenaBase::parse_arena_line");
                
              wall_linep = new WallLine(scale*current_pos, direction, 
                                        scale*len, 
                                        scale*thickness, bounce_c , hardn);      
              object_lists[WALL].insert_last( wall_linep );

              current_pos += len * direction;
              wall_circlep = new WallCircle(scale*current_pos, scale*thickness, 
                                            bounce_c, hardn);
              object_lists[WALL].insert_last( wall_circlep );
              
              break;
            case 'A':  // Arc
              file >> angle;
              file >> radius;

              angle *= angle_factor;
              center = current_pos - rotate90( direction ) * radius * sgn( angle );
              start_angle = vec2angle( current_pos - center );
              current_pos = center + radius * angle2vec( start_angle - angle );
              end_angle = vec2angle( current_pos - center );

              if( angle > 0.0 )
                { tmp = start_angle; start_angle = end_angle; end_angle = tmp; }

              wall_arcp = new WallArc(scale*center, scale*(radius - thickness), 
                                      scale*(radius + thickness), 
                                      start_angle, end_angle,
                                      bounce_c, hardn);

              object_lists[WALL].insert_last(wall_arcp);              


              direction = rotate(direction, -angle);

              wall_circlep = new WallCircle(scale*current_pos, scale*thickness, 
                                            bounce_c, hardn);
              object_lists[WALL].insert_last( wall_circlep );
              break;

            case 'T':  // Turn
              file >> angle;              
              direction = rotate( direction, -angle*M_PI/180.0 );

              break;
              
            case 'C':   // connect to start point and quit
              if( length(vec0 - current_pos ) == 0.0 ) 
                Error(true, "Error in arenafile: Last line in poly_curve of zero length", 
                      "ArenaBase::parse_arena_line");
              
              wall_linep = new WallLine(scale*current_pos, unit(vec0-current_pos), 
                                        scale*length(vec0-current_pos), 
                                        scale*thickness, bounce_c , hardn);      
              object_lists[WALL].insert_last( wall_linep );              

              finish = true;
              break;
            case 'Q':   // quit
              finish = true;
              break;
              
            default:
              Error(true, "Incorrect arenafile, unknown command in poly_curve: " 
                    + (String)c, "ArenaBase::parse_arena_line");
              break;
            }
        }
      while( !finish );
    }

  else if( text[0] != '\0' )
    Error(true, "Incorrect arenafile, unknown keyword: " + (String)text, 
          "ArenaBase::parse_arena_line");

}



double
ArenaBase::get_shortest_distance(const Vector2D& pos, const Vector2D& dir, 
                                  const double size, object_type& closest_shape, 
                                  Shape*& colliding_object, const Robot* the_robot)
{
  double dist = infinity;
  double d;
  closest_shape = NOOBJECT;

  ListIterator<Shape> li;

  for( int obj_type = ROBOT; obj_type < LAST_OBJECT_TYPE; obj_type++ )
    {
      for( object_lists[obj_type].first(li); li.ok(); li++)
        {
          if( obj_type != ROBOT || (Robot*)(li()) != the_robot )
            {
              d = li()->get_distance(pos, dir, size);
              if( d < dist)
                {
                  closest_shape = (object_type)obj_type;
                  colliding_object = li();
                  dist = d;
                }
            }
        }
    }

  return dist;
}

bool
ArenaBase::space_available(const Vector2D& pos, const double margin)
{
  ListIterator<Shape> li;

  for( int obj_type = ROBOT; obj_type < LAST_OBJECT_TYPE; obj_type++ )
    {
      for( object_lists[obj_type].first(li); li.ok(); li++)
        if( li()->within_distance(pos, margin) ) return false;
    }

  // Check if it is possible to see any exclusion_points
  
  Vector2D vec;
  double dist;
  object_type obj_t;
  Shape* shapep;

  ListIterator<Vector2D> li_ex;

  for( exclusion_points.first(li_ex); li_ex.ok(); li_ex++)
    {
      vec = *(li_ex());
      dist = length(vec - pos);
      if( dist <= margin || 
          dist <= get_shortest_distance(pos, unit(vec - pos), 0.0, 
                                        obj_t, shapep, (Robot*)NULL) )
        return false;
    }

  return true;
}


double 
ArenaBase::get_shooting_penalty() 
{ 
  return min( the_opts.get_d(OPTION_SHOOTING_PENALTY), 0.5 / (double)robots_left ); 
}


void
ArenaBase::update_timer(const double factor)
{
  double last_timer = current_timer;

  current_timer = timer.get();
  double timescale = 1.0;
  if( state == GAME_IN_PROGRESS ) timescale = the_opts.get_d(OPTION_TIMESCALE);
  timestep = min( (current_timer - last_timer) * timescale, 
                  the_opts.get_d(OPTION_MAX_TIMESTEP) );
  total_time += timestep*factor;

  total_time = max(total_time, 0.0);
}

void
ArenaBase::reset_timer()
{
  total_time = 0.0;
  current_timer = 0.0;
  timer.reset();
  update_timer();
}


void
ArenaBase::move_shots(const double time_period)
{
  Shot* shotp;

  ListIterator<Shape> li;

  for( object_lists[SHOT].first(li); li.ok(); li++)
    {
      shotp = (Shot*)li();

      if( shotp->is_alive() ) shotp->move(time_period);

      if( !shotp->is_alive() ) object_lists[SHOT].remove(li);
    }
}

void
ArenaBase::move_shots_no_check(const double time_period)
{
  ListIterator<Shape> li;

  for( object_lists[SHOT].first(li); li.ok(); li++)
    {
      ((Shot*)li())->move_no_check(time_period);
    }
}

void 
ArenaBase::set_game_mode( const enum game_mode_t gm )
{
  game_mode = gm; 
  if( game_mode == DEBUG_MODE )
    {
      if( debug_level == 0 )
        set_debug_level( the_arena_controller.debug_level );
    }
  else
    {
      debug_level = 0;
    }
}

int
ArenaBase::set_debug_level( const int new_level)
{
  if( new_level > max_debug_level || new_level < 0 || new_level == debug_level )
    return debug_level;
  
  debug_level = new_level;

  return debug_level;
}

void
ArenaBase::pause_game_toggle()
{
  if( game_mode != COMPETITION_MODE )
    {
      if( state == GAME_IN_PROGRESS ) 
        set_state( PAUSED );
      else if( state == PAUSED ) 
        set_state( GAME_IN_PROGRESS );

      halt_next = false; 
    }
  else
    {
      pause_after_next_game = !pause_after_next_game;
      set_state( state ); // to change control window title
    }
}

void
ArenaBase::step_paused_game()
{
  if( game_mode == DEBUG_MODE && state == PAUSED )
    {
      halt_next = true; 
      set_state( GAME_IN_PROGRESS );
    }
}

void
ArenaBase::delete_lists(const bool kill_robots, const bool del_seq_list, 
                        const bool del_tourn_list, const bool del_arena_filename_list, 
                        const bool del_robot_obj_list)
{
  // clear the lists;

  for( int obj_type = ROBOT; obj_type < LAST_OBJECT_TYPE; obj_type++ )
    if( obj_type != ROBOT || del_robot_obj_list )
      object_lists[obj_type].delete_list();

  exclusion_points.delete_list();

  if( del_seq_list )
    {
      if( kill_robots )
        {
          ListIterator<Robot> li;
          for( all_robots_in_sequence.first(li); li.ok(); li++)
            li()->kill_process_forcefully();
        }
      all_robots_in_sequence.delete_list();
    }

  if( del_tourn_list )  all_robots_in_tournament.delete_list();
  if( del_arena_filename_list ) arena_filenames.delete_list();
}

bool
ArenaBase::find_object_by_id( const List<Shape>& obj_list, 
                              ListIterator<Shape>& li,
                              const int obj_id )
{
  for( obj_list.first(li); li.ok(); li++)
    {
      if( li()->get_id() == obj_id ) 
        return true;
    }
  return false;
}

bool
ArenaBase::find_object_by_id( const List<Robot>& obj_list, 
                              ListIterator<Robot>& li,
                              const int obj_id )
{
  for( obj_list.first(li); li.ok(); li++)
    {
      if( li()->get_id() == obj_id ) 
        return true;
    }
  return false;
}
