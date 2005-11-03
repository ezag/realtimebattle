//
//
// JBotMain v0.2.0
// Robot for RealTimeBattle in JAVA
// (c) 1999 by Ingo Beckmann
// ingoBeckmann@gmx.de

// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

class JBotMain {
    
    public static void main (String[] argv) {
	// default sleeping time in ms:
	int SLEEP = JBot.DEFAULT_SLEEP;
	double timeShare;

	// get sleeping duration from first command line option:
	if (argv.length > 0) try {
	    SLEEP = Integer.parseInt(argv[0]);
	} catch (NumberFormatException nfe) {
	    System.err.println("Could not parse sleeping duration, taking "+SLEEP+" ms instead !");
	}
	
	// Send RobotOptions:
	/*
	  System.out.println("RobotOption " +JBot.SEND_SIGNAL+ " 0");
	  System.out.println("RobotOption " +JBot.SEND_ROTATION_REACHED+ " 1");
	  System.out.println("RobotOption " +JBot.SIGNAL+ " 0");
	  System.out.println("RobotOption " +JBot.USE_NON_BLOCKING+ " 1");
	*/

	JBot jBot;
	do {
	    
	    // Robot 1...
	    if (true) {
		timeShare = 25.0;   
		if  (argv.length > 1) {
		    jBot = new JBot_C (argv[1], SLEEP, timeShare);
		} else {
		    jBot = new JBot_C (SLEEP, timeShare);
		}
		// ...gets started:
		jBot.start();
		// Wait for Thread to finish:
		try {
		    jBot.join();
		} catch (InterruptedException ie) {
		    System.err.println("JBotMain: Interrupt during join() !");
		}
	    }
	    
	    // Robot 2...
	    if (JBot.handoverBaton) {
		timeShare = JBot.UNTIL_END;
		if  (argv.length > 1) {
		    jBot = new JBot_T (argv[1], SLEEP, timeShare);
		} else {
		    jBot = new JBot_T (SLEEP, timeShare);
		}
		// ...gets started:
		jBot.start();
		// Wait for Thread to finish:
		try {
		    jBot.join();
		} catch (InterruptedException ie) {
		    System.err.println("JBotMain: Interrupt during join() !");
		}
	    }

	} while (!JBot.exitRobot);

    } // end void main
    
}

