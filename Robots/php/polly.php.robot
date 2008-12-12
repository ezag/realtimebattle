#!/usr/bin/env php
<?php
/**
 * This robot moves along a polygon with as many vertices as there are robots left
 *
 * @author Falko Menge <fakko at users dot sourceforge dot net>
 * @license GNU General Public License, version 2, or any later version
 */
 
require_once 'RTB.php';
$rtb = new RTB();
//$rtb = new RTB(__FILE__ . '.log'); // use this line to enable logging

$rtb->send('Name phpPolly2');
$rtb->send('Colour 6633FF');

$steps = 0;
$state = 2;

while ($message = $rtb->receive()) { // receive a message from RealTimeBattle server
	
	if ($rtb->gameStarted()) {
        if ($message[0] == 'RobotsLeft') {
            $robots = $message[1];
        }
        
        if ($message[0] == 'Energy') {
            ++$steps;
            if ($steps >= 48) { // RealTimeBattle makes 24 steps per second
                $steps = 0;
                
                // this code will be executed once a second
                switch ($state) {
                    case 0:
                        $rtb->accelerate();
                        $state = 1;
                        break;
                    case 1:
                        $rtb->brake(); // full brake
                        //$rtb->send('Accelerate', $rtb->getOption(ROBOT_MIN_ACCELERATION)); // drive backwards
                        $state = 2;
                        break;
                    case 2:
                        $rtb->send(
                            'RotateAmount',
                            ROTATE_ROBOT,
                            $rtb->getOption(ROBOT_MAX_ROTATE),
                            2 * M_PI / $robots
                        );
                        $state = 0;
                        break;
                }
            }
        }
        
        if ($message[0] == 'Radar'){
            switch ($message[2]) {
                case ROBOT:
                    if ($rtb->getRobotEnergy() > 10) {
                        $rtb->shoot();
                    }
                    break;
                case MINE:
                    $rtb->shootMin();
                    break;
            }
        }
	}
}
?>
