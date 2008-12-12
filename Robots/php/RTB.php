<?php
/**
 * This file declares a helper class for communication with RealTimeBattle 
 *
 * @author Falko Menge <fakko at users dot sourceforge dot net>
 * @license GNU General Public License, version 2, or any later version
 */

// include constant definitions
require_once 'rtb-constants.php';

class RTB {

    protected $logfile = null;
    protected $gameStarted = false;
    protected $gameOptions;
    protected $robotX = 0.0;
    protected $robotY = 0.0;
    protected $robotAngle = 0.0;
    protected $robotEnergy; 
    
    public function __construct($logfile = null, $inputStream = null, $outputStream = null) {
        if (!empty($logfile) and file_put_contents($logfile, '') !== false) {
            $this->logfile = $logfile;
        }
        $this->setInputStream($inputStream);
        $this->setOutputStream($outputStream);
        $this->send('RobotOption', USE_NON_BLOCKING, 0);
        $this->send('RobotOption', SEND_ROTATION_REACHED, 1);
        // set default values for game options
        $this->gameOptions = array(
            ROBOT_MAX_ROTATE           => 0.785398,
            ROBOT_CANNON_MAX_ROTATE    => 1.5708,
            ROBOT_RADAR_MAX_ROTATE     => 2.0944,
            ROBOT_MAX_ACCELERATION     => 2,
            ROBOT_MIN_ACCELERATION     => -0.5,
            ROBOT_START_ENERGY         => 100,
            ROBOT_MAX_ENERGY           => 120,
            ROBOT_ENERGY_LEVELS        => 10,
            SHOT_SPEED                 => 10,
            SHOT_MIN_ENERGY            => 0.5,
            SHOT_MAX_ENERGY            => 30,
            SHOT_ENERGY_INCREASE_SPEED => 10,
            TIMEOUT                    => 120,
            DEBUG_LEVEL                => 0, // 0 - no debug, 5 - highest debug level
            SEND_ROBOT_COORDINATES     => 2, // 0 - no coordinates,
                                             // 1 - coordniates are given relative the starting position
                                             // 2 - absolute coordinates
        ); 
        $this->robotEnergy = $this->getOption(ROBOT_ENERGY_LEVELS);
    }

    /**
     * Set input stream
     *
     * @param resource $handle file pointer
     */
    public function setInputStream($handle = null) {
        if (!is_resource($handle)) {
            $handle = STDIN;
        }
        $this->inputStream = $handle;
    }

    /**
     * Set output stream
     *
     * @param resource $handle file pointer
     */
    public function setOutputStream($handle = null) {
        if (!is_resource($handle)) {
            $handle = STDOUT;
        }
        $this->outputStream = $handle;
    }

    /**
     * Sends a message to the RealTimeBattle server
     * @param $command Command to send
     * @param $arguments,... Arguments for the command (optional)
     * @return void
     */
    public function send() {
        $arguments = func_get_args();
        $command = implode(' ', $arguments);
        
        // check if command is currently allowed
        if ($this->getOption(DEBUG_LEVEL) < 5 and ($arguments[0] == 'DebugLine' or $arguments[0] == 'DebugCircle')) {
            $this->log('Warning: This is only allowed in debug level 5: ' . $command);
        } else {
            $this->sendRawString($command . "\n");
        }
    }

    /**
     * Receives a message from the RealTimeBattle server
     * @return array<integer,string>
     */
    public function receive() {
        // read next message
        $message = $this->receiveRawString();
        
    	// remove newline from message
    	$message = trim($message);
    	
    	// split message into its parts
    	$message = explode(' ', $message);

        $this->handleMessage($message);

        return $message;
    }

    /**
     * Sends a raw string to the RealTimeBattle server
     * @param string
     * @return void
     */
    public function sendRawString($string) {
        if (!empty($string)) {
            $this->log('<< ' . $string);
            fwrite($this->outputStream, $string);
        }
    }
    
    /**
     * Receives a raw string from the RealTimeBattle server
     * @return string
     */
    public function receiveRawString() {
        // read next message
        $message = fgets($this->inputStream);
        
    	// write message to log file
        $this->log('>> ' . $message);

    	// write message to console
    	//fwrite(STDERR, $message . "\n");
        
        return $message;
    }

    /**
     * Extracts game or robot status from a message
     * @return void
     */
    protected function handleMessage($message) {
    	switch ($message[0]) {
            // save game options
            case 'GameOption':
                $this->gameOptions[$message[1]] = $message[2];
                break;
            // save robot state
            case 'Coordinates':
                $this->robotX = $message[1];
                $this->robotY = $message[2];
                $this->robotAngle = $message[3];
            case 'Energy':
                $this->robotEnergy = $message[1];
            // handle game status messages
            case 'GameStarts':
                $this->gameStarted = true;
                break;
            case 'Dead':
            case 'GameFinishes':
                $this->gameStarted = false;
                break;
            case 'ExitRobot':
                exit; // quit the program
    	}
    }

    /**
     * Writes a message to the log file
     * @param string $message Text
     * @return void
     */
    public function log($message) {    	
        if (!empty($this->logfile)) {
            file_put_contents($this->logfile, trim($message) . "\n", FILE_APPEND);
        }
        //fwrite(STDERR, $message . "\n");
    }
    
    /**
     * Checks if a game is in progress
     * @return boolean
     */
    public function gameStarted() {
        return $this->gameStarted;
    }

    /**
     * Returns the value of a game configuration option
     * @param integer $optionName Number of the option to retrieve
     *                            (use the game option constants from above
     *                            to find the correct number)
     * @return mixed
     */
    public function getOption($optionName) {
        return $this->gameOptions[$optionName];
    }

    /**
     * Returns the current x coordinate of the robot
     * @return float
     */
    public function getRobotX() {
        return (float) $this->robotX;
    }

    /**
     * Returns the current y coordinate of the robot
     * @return float
     */
    public function getRobotY() {
        return (float) $this->robotY;
    }

    /**
     * Returns the current angle of the robot
     * @return float
     */
    public function getRobotAngle() {
        return (float) $this->robotAngle;
    }

    /**
     * Returns the current energy of the robot in percent
     * @return float
     */
    public function getRobotEnergy() {
        return (float) $this->robotEnergy;
    }

    /**
     * Sends commands to stop the robot
     * @param float $portion How hard to brake (full brake: 1.0)
     * @return void
     */
    public function brake($portion = 1.0) {
        $this->accelerate(0.0);
        $this->send('Brake', (float) $portion);
    }
    
    /**
     * @param float $acceleration How hard to accelerate
     * @return void
     */
    public function accelerate($acceleration = null) {
        if (is_null($acceleration)) {
            $acceleration = $this->getOption(ROBOT_MAX_ACCELERATION);
        }
        $this->send('Accelerate', (float) $acceleration);
    }
    
    /**
     * Fires the cannon of the robot
     * @param float $shotEnergy How hard to shoot
     * @return void
     */
    public function shoot($shotEnergy = null) {
        if (is_null($shotEnergy)) {
            $shotEnergy = $this->getShotEnergyMax();
        }
        $this->send('Shoot', (float) $shotEnergy); 
    }
    
    /**
     * Shoots with maximum energy
     * @return void
     */
    public function shootMax() {
        $this->shoot();
    }
    
    /**
     * Shoots with minimum energy
     * @return void
     */
    public function shootMin() {
        $this->shoot($this->getShotEnergyMin());
    }
    
    /**
     * Calculates the angle between a radar angle and the front of the robot
     * @param float $radarAngle An angle from a radar message
     * @return float Rotation angle
     */
    public function calculateRotationAngle($radarAngle) {
        $angle = fmod($radarAngle, 2 * M_PI); // rotate counter-clockwise 
        if ($angle > abs($angle - 2* M_PI)) {
            $angle = $angle - 2* M_PI; // rotate clockwise 
        }
        return $angle;
    }
    public function getRobotRotationMax (){
        return $this->getOption(ROBOT_MAX_ROTATE);
    }
    
    public function getCannonRotationMax() {
        return $this->getOption(ROBOT_CANNON_MAX_ROTATE);
    }
    
    public function getRadarRotationMax() {
        return $this->getOption(ROBOT_RADAR_MAX_ROTATE);
    }
    
    public function getAccelerationMax() {
        return $this->getOption(ROBOT_MAX_ACCELERATION);
    }
    
    public function getAccelerationMin() {
        return $this->getOption(ROBOT_MIN_ACCELERATION);
    }
    
    public function getStartEnergy() {
        return $this->getOption(ROBOT_START_ENERGY);
    }
    
    /**
     *
     */
    public function getEnergyMax() {
        return $this->getOption(ROBOT_MAX_ENERGY);
    }
    
    /**
     *
     */
    public function getEnergyLevels() {
        return $this->getOption(ROBOT_ENERGY_LEVELS);
    }
    
    /**
     *
     */
    public function getShotSpeed() {
        return $this->getOption(SHOT_SPEED);
    }
    
    /**
     *
     */
    public function getShotEnergyMin() {
        return $this->getOption(SHOT_MIN_ENERGY);
    }
    
    /**
     *
     */
    public function getShotEnergyMax() {
        return $this->getOption(SHOT_MAX_ENERGY);
    }
    
    /**
     *
     */
    public function getshotEnergyIncreaseSpeed() {
        return $this->getOption(SHOT_ENERGY_INCREASE_SPEED);
    }
    
    /**
     *
     */
    public function getTimeout() {
        return $this->getOption(TIMEOUT);
    }
    
    /**
     *
     */
    public function getDebugLevel() {
       return  $this->getOption(DEBUG_LEVEL);
    }
    
    /**
     *
     */
    public function getsendRobotCoordinates() {
        return $this->getOption(SEND_ROBOT_COORDINATES);
    }
    
    /**
     *
     */
    public function rotate($what = null, $rotationSpeed = null) {
        if (is_null($what)) {
            $what = ROTATE_ROBOT + ROTATE_CANNON + ROTATE_RADAR;
        }
        if (is_null($rotationSpeed)) {
            $rotationSpeed = $this->getRobotRotationMax();
        }
        $this->send('Rotate', $what, $rotationSpeed);
    }
    
    /**
     *
     */
    public function rotateRobot($rotationSpeed = null) {
        if (is_null($rotationSpeed)) {
            $rotationSpeed = $this->getRobotRotationMax();
        }
        $this->rotate(ROTATE_ROBOT, $rotationSpeed);
    }
    
    /**
     *
     */
    public function rotateRadar($rotationSpeed = null) {
        if (is_null($rotationSpeed)) {
            $rotationSpeed = $this->getRadarRotationMax();
        }
        $this->rotate(ROTATE_RADAR, $rotationSpeed);
    }
    /**
     *
     */
    public function rotateCannon($rotationSpeed = null) {
        if (is_null($rotationSpeed)) {
            $rotationSpeed = $this->getCannonRotationMax();
        }
        $this->rotate(ROTATE_CANNON, $rotationSpeed);
    }
}
?>
