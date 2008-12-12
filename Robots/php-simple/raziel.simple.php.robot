#!/usr/bin/env php
<?php
/**
 * This robot moves in a circle while heavily shooting
 *
 * @author Falko Menge <fakko at users dot sourceforge dot net>
 * @license GNU General Public License, version 2, or any later version
 */
 
// set options, name and color
echo "RobotOption 3 0\n";
echo "RobotOption 1 1\n";
echo "Name phpRazzielSimple\n";
echo "Colour ee299 aaffaa\n";

$gameStarted = false;

while ($message = trim(fgets(STDIN))) { // receive message from RealTimeBattle server
		
	if ($message == 'GameStarts') {
		$gameStarted = true;
        // move forward
		echo "Accelerate 0.5\n";
        // rotate robot, cannon and rada with maximum speed
		echo "Rotate 7 3\n";
	}

	if ($gameStarted) {
        // fire constantly
		echo "Shoot 8\n";
	}

	if ($message == 'Dead' or $message == 'GameFinishes') {
		$gameStarted = false;
	}

	if ($message == 'ExitRobot') {
		// quit the program
        exit;
	}
}
?>
