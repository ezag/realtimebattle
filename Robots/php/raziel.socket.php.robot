#!/usr/bin/env php
<?php
/**
 * This robot moves in a circle while heavily shooting
 *
 * @author Falko Menge <fakko at users dot sourceforge dot net>
 * @license GNU General Public License, version 2, or any later version
 */

require_once 'RTB.php';

// use socket to talk to the RealTimeBattle server
//*
$socket = stream_socket_client("tcp://localhost:2008", $errno, $errstr, 30);
if (!$socket) {
    die("$errstr ($errno)\n");
}
//*/

//$rtb = new RTB();
//$rtb = new RTB(__FILE__ . '.log'); // use this line to enable logging
$rtb = new RTB(__FILE__ . '.log', $socket, $socket);

// set name and color
$rtb->send('Name phpRazziel.netcat');
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
