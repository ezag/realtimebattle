#!/usr/bin/env php
<?php
/**
 * @author Falko Menge <fakko at users dot sourceforge dot net>
 * @license GNU General Public License, version 2, or any later version
 */
echo "RobotOption 3 0\n";
echo "RobotOption 1 1\n";
echo "Name phpCain\n";
echo "Colour 990000\n";

// define constants for easier handling of object types 
define('OBJECT_TYPE_NOOBJECT', -1); 
define('OBJECT_TYPE_ROBOT', 0); 
define('OBJECT_TYPE_SHOT', 1); 
define('OBJECT_TYPE_WALL', 2); 
define('OBJECT_TYPE_COOKIE', 3); 
define('OBJECT_TYPE_MINE', 4);
define('OBJECT_TYPE_LAST_OBJECT_TYPE', 5);

// create a new log file
//file_put_contents(__FILE__ . '.log', '');

$gameStarted = false;
$normal = false;
$berserker = 0;
$mine = 0;

while ($message = fgets(STDIN)) { // receive message from RealTimeBattle server
	// remove newline from message
	$message = trim($message);
	// split message into its parts
	$messageparts = explode(' ', $message);
	
	// write message to a log file
	//file_put_contents(__FILE__ . '.log', $message . "\n", FILE_APPEND);
	
	// write message to console
	//fwrite(STDERR, $message . "\n");
	
	if ($message == 'GameStarts') {
		$gameStarted = true;
}
	if ($message == 'Dead' or $message == 'GameFinishes') {
	    print "see you in hell\n";
		$gameStarted = false;
	}
	if ($message == 'ExitRobot') {
		exit; // quit the program
	}
	if ($gameStarted) {
		if ($messageparts[0] == 'Radar') {
			// draw a circle arround each point reported by radar (requires RealTimeBattle to be started with: realtimebattle -D 5)
			echo "DebugCircle {$messageparts[3]} {$messageparts[1]} 0.2";
			
			if ($messageparts[2] == OBJECT_TYPE_MINE) {
				$mine = time();
				print "du kriegst mich nicht!\n";
			}
			if ($messageparts[2] == OBJECT_TYPE_ROBOT) {
				$berserker = time();
			}
			
			//if ($messageparts[2] != OBJECT_TYPE_ROBOT) {
			//	$normal = true;
			//}
		}
		if ( time() < $berserker + 1){
			print "STIRB!\n";
			echo "Brake 1\n";
			echo "Accelerate 0.5\n";
			echo "Rotate 7 9\n";
			echo "Shoot 30\n";
		} else {
			echo "Accelerate 0.5\n";
			echo "Rotate 7 9\n";
			//echo "Shoot 5\n";
		}
		if ( time() < $mine + 1){
			echo "Shoot 0.5\n";
		}
	}
}
?>
