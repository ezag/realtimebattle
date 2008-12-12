<?php
/**
 * This file defines several constants for easier communication with RealTimeBattle
 *
 * The names and values have been obtained from RealTimeBattle's Messagetypes.h
 * @see http://realtimebattle.cvs.sourceforge.net/realtimebattle/RealTimeBattleNew/include/Messagetypes.h?view=markup 
 *
 * @author Falko Menge <fakko at users dot sourceforge dot net>
 * @license GNU General Public License, version 2, or any later version
 */

// robot options
define('SEND_SIGNAL', 0);             // 0 - false, 1 - true
define('SIGNAL', 2);                  // 0 - no signal, > 1 - signal to send (e.g. SIGUSR1 or SIGUSR2) 
define('SEND_ROTATION_REACHED', 1);   // 0 - no messages
                                      // 1 - messages when RotateTo and RotateAmount finished
                                      // 2 - messages also when sweep direction is changed
define('USE_NON_BLOCKING', 3);        // 0 - false, 1 - true 
                                      // This option should always be sent as soon as possible

// game options
define('ROBOT_MAX_ROTATE', 0);
define('ROBOT_CANNON_MAX_ROTATE', 1);
define('ROBOT_RADAR_MAX_ROTATE', 2);

define('ROBOT_MAX_ACCELERATION', 3);
define('ROBOT_MIN_ACCELERATION', 4);

define('ROBOT_START_ENERGY', 5);
define('ROBOT_MAX_ENERGY', 6);
define('ROBOT_ENERGY_LEVELS', 7);

define('SHOT_SPEED', 8);
define('SHOT_MIN_ENERGY', 9);
define('SHOT_MAX_ENERGY', 10);
define('SHOT_ENERGY_INCREASE_SPEED', 11);

define('TIMEOUT', 12);

define('DEBUG_LEVEL', 13);            // 0 - no debug, 5 - highest debug level

define('SEND_ROBOT_COORDINATES', 14); // 0 - no coordinates, 
                                      // 1 - coordniates are given relative the starting position
                                      // 2 - absolute coordinates

// object types
define('NOOBJECT', -1); 
define('ROBOT', 0); 
define('SHOT', 1); 
define('WALL', 2); 
define('COOKIE', 3); 
define('MINE', 4);
define('LAST_OBJECT_TYPE', 5);

// what to rotate
define('ROTATE_ROBOT', 1);
define('ROTATE_CANON', 2);
define('ROTATE_CANNON', 2);
define('ROTATE_RADAR', 4);
?>
