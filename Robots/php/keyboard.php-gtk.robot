#!/usr/bin/env php
<?php
/**
 * @author Falko Menge <fakko at users dot sourceforge dot net>
 * @license GNU General Public License, version 2, or any later version
 */
if (!extension_loaded('php-gtk')) {
    $prefix = (PHP_SHLIB_SUFFIX === 'dll') ? 'php_' : '';
    dl($prefix . 'php_gtk2.' . PHP_SHLIB_SUFFIX);
}

require_once 'RTB.php';

class KeyboardRobot extends GtkWindow {

    protected $rtb;

    function __construct($parent = null)
    {
        parent::__construct();

        $this->rtb = new RTB();
        $this->rtb->send('Name phpKeyboardBot');
        $this->rtb->send('Colour CCFF66');

        if ($parent) {
            $this->set_screen($parent->get_screen());
        } else {
            $this->connect_simple('destroy', array('Gtk', 'main_quit'));
        }

        $this->set_title('phpKeyboardBot');
        //$this->set_position(Gtk::WIN_POS_CENTER);
        $this->set_default_size(250, 1);
        
        $this->connect('key-press-event', array($this, 'onKeyPress'));
        $this->show_all();
    }

    public function onKeyPress($widget, $event) {
        if ($event->type == Gdk::KEY_PRESS) {
            switch ($event->keyval) {
                case Gdk::KEY_Escape:
                    exit;
                case Gdk::KEY_Up:
                    $this->rtb->accelerate();
                    break;
                case Gdk::KEY_Down:
                    $this->rtb->brake();
                    break;
                case Gdk::KEY_Left:
                    $this->rtb->send(
                        "RotateAmount",
                        ROTATE_ROBOT, 
                        $this->rtb->getOption(ROBOT_MAX_ROTATE),
                        0.3
                    );
                    break;
                case Gdk::KEY_Right:
                    $this->rtb->send(
                        "RotateAmount",
                        ROTATE_ROBOT, 
                        $this->rtb->getOption(ROBOT_MAX_ROTATE),
                        -0.3
                    );
                    break;
                case Gdk::KEY_Control_L:
                    $this->rtb->shoot();
                    break;
            }
        }
    }

}

$application = new KeyboardRobot();

//Start the main loop
Gtk::main();
