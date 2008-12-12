#!/usr/bin/env php
<?php
/**
 * @author Falko Menge <fakko at users dot sourceforge dot net>
 * @license GNU General Public License, version 2, or any later version
 */
require_once 'RTB.php';
$rtb = new RTB();
//$rtb = new RTB(__FILE__ . '.log'); // use this line to enable logging

$rtb->send('Name phpCain2');
$rtb->send('Colour 990000');

$berserker = 0;
$mine = 0;

while ($message = $rtb->receive()) { // receive a message from RealTimeBattle server
	
	if ($rtb->gameStarted()) {
		if ($message[0] == 'Radar') {
			// draw a circle arround each point reported by radar (requires RealTimeBattle to be started with: realtimebattle -D 5)
			$rtb->send('DebugCircle', $message[3], $message[1], 0.2);
			
			if ($message[2] == MINE) {
				$mine = time();
			}
			if ($message[2] == ROBOT) {
				$berserker = time();
                $shotEnergy = $rtb->getShotEnergyMax() / $message[1];
                if ($shotEnergy > $rtb->getShotEnergyMax()) {
                    $shotEnergy = $rtb->getShotEnergyMax();
                }
                if ($shotEnergy < $rtb->getShotEnergyMin()) {
                    $shotEnergy = $rtb->getShotEnergyMin();
                }
                $lastRobotRadarMessage = $message;
			}

            if ( time() < $berserker + 1){
                if ($rtb->getRobotEnergy() > 10) {
    				$rtb->brake();
    				$rtb->send('Rotate 7 0');
                    $rtb->shoot($shotEnergy);
                }
			} else {
    			if ( time() < $mine + 1){
    				//echo "Print entschärft\n";
    				$rtb->shootMin();
    			}
				$rtb->accelerate();
				$rtb->send('Rotate 7 9');
			}
		}
	}
}
?>
