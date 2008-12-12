#!/usr/bin/env php
<?php
/**
 * This robot seeks after cookies
 *
 * @author Falko Menge <fakko at users dot sourceforge dot net>
 * @license GNU General Public License, version 2, or any later version
 */
 
require_once 'RTB.php';
$rtb = new RTB();
//$rtb = new RTB(__FILE__ . '.log'); // use this line to enable logging

$rtb->send('Name phpCandy');
$rtb->send('Colour FF66CC');

while ($message = $rtb->receive()) { // receive message from RealTimeBattle server

    if ($message[0] == 'GameStarts') {
        $searchCandy = true;
        $candyFound = false;
		$rtb->send('Print Hello World!');
        $rtb->rotateRadar();
    }

	if ($rtb->gameStarted()) {
        
        // start candy search if velocity is below 0.3
       if (
            !$searchCandy 
            and !$candyFound 
            and $message[0] == 'Info' 
            and $message[2] < 0.3 
        ) {
            $searchCandy = true;
            $rtb->rotateRadar();
        }
        
        // handle radar messages
		if ($message[0] == 'Radar') {
			// draw a circle arround each point reported by radar 
            // (requires RealTimeBattle to be started with: realtimebattle -D 5)
			echo "DebugCircle {$message[3]} {$message[1]} 0.2";

			if ($message[2] == COOKIE){
                if ($searchCandy) {
    				$candyFound = true;
                    $searchCandy = false;
    				$rtb->brake();
                    $rtb->send('DebugLine', 0, 0, $message[3], $message[1]);
                    $rtb->send('Print I saw a cookie!');
    				$rtb->send(
                        "RotateAmount",
                        ROTATE_ROBOT, 
                        $rtb->getOption(ROBOT_MAX_ROTATE),
                        $rtb->calculateRotationAngle($message[3])
                    );
                    $rtb->rotateRadar(0);
                }
            }
		}
      
        // main robot behavior
        if ($candyFound) {
            if ($message[0] == 'RotationReached') {
                $candyRotationReached = true;
                $rtb->accelerate();
                $rtb->send('Print Approaching my cookie');
            }
            // stop the approach if collision with cookie or wall (don't stop on collision with robot)
            if ($message[0] == 'Collision' and ($message[1] == COOKIE or $message[1] == WALL)) {
                $candyFound = false;
                $rtb->brake();
            }
        }
	}

	if ($message[0] == 'Dead') {
	    $rtb->send('Print See you in hell');
	}
}
?>
