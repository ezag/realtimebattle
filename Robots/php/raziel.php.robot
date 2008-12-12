#!/usr/bin/env php
<?php
/**
 * This robot moves in a circle while heavily shooting
 *
 * @author Falko Menge <fakko at users dot sourceforge dot net>
 * @license GNU General Public License, version 2, or any later version
 */

require_once 'RTB.php';
$rtb = new RTB();
//$rtb = new RTB(__FILE__ . '.log'); // use this line to enable logging

// set name and color
$rtb->send('Name phpRazziel');
$rtb->send('Colour ee299 aaffaa');

while ($message = $rtb->receive()) { // receive message from RealTimeBattle server

    if ($message[0] == 'GameStarts') {
        // move forward
		$rtb->send('Accelerate', 0.5);
        // rotate robot and cannon with maximum speed
		$rtb->send('Rotate', ROTATE_ROBOT + ROTATE_CANNON, $rtb->getOption(ROBOT_CANNON_MAX_ROTATE));
    }

	if ($rtb->gameStarted()) {
        // fire constantly
        $rtb->send('Shoot', 8);
	}
}
?>
