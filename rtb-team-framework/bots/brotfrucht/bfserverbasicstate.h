/************************************************************************
    $Id: bfserverbasicstate.h,v 1.2 2005/02/24 10:27:53 jonico Exp $
    
    RTB - Team Framework: Framework for RealTime Battle robots to communicate efficiently in a team
    Copyright (C) 2004 The RTB- Team Framework Group: http://rtb-team.sourceforge.net

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    $Log: bfserverbasicstate.h,v $
    Revision 1.2  2005/02/24 10:27:53  jonico
    Updated newest version of the framework

    Revision 1.2  2005/01/06 17:59:30  jonico
    Now all files in the repository have their new header format.


**************************************************************************/

#ifndef BFSERVERBASICSTATE_H
#define BFSERVERBASICSTATE_H

#include "../../strategies/serverstate.h"
#include "brotfruchtnamespace.h"
#include <sstream>

/**
* Namespace Brotfrucht
*/
namespace Brotfrucht {

	using Strategies::ServerState;
	using Exceptions::StrategyException;
	using std::string;
	using std::bad_exception;
	using Parser::object_type;
	using Parser::warning_type;
	using GameControl::ServerCoordinator;
	using std::ostringstream;
	
	/**
	* Class BFServerBasicState
	*
	* This class is the base of all brotfrucht server states. Here, the default actions, that will be done by every state
	* (unless it defines something else) are determined. States, that want to extend and not to entirely replace the action
	* of this base class, should simpkly call the corresponding method in this class before executinh their own stuff
	*/ 
	class BFServerBasicState: public Strategies::ServerState {
	public:
		BFServerBasicState(ServerCoordinator* c,BFInternalServerSpecificRepository* bfssp, BFSharedServerSpecificRepository* bfsharedssp) throw (StrategyException,bad_exception);

		/**
		* Destructor, does nothing
		*/
		virtual ~BFServerBasicState () throw();
		
		/**
		* This method is called from the coordinator's processReceivedRTBMessages method and should send tips to the clients.
		*/
		virtual void  sendServerTips () throw (StrategyException,bad_exception);
	
		/**
		* This method is called by ClientCoordinatorRevokes::resetGame()
		*/
		virtual void reset () throw (bad_exception, StrategyException);

		/**
		* This method is called when the RTB message "Radar [dist] [type] [angle]" occurs. This message occurs each turn and gives information about the closest object in the direction the radar points to.
		* @param dist The distance of the observed object
		* @param type The type of the observed object
		* @param angle The angle the observed object is seen at in rad, relative to the robot front
		*/
		virtual void receiveRTBMessageRadar(double dist,object_type type,double angle) throw (StrategyException, bad_exception);

		/**
		* This method is called when the RTB message "Info [time] [speed] [cannonAngle]" occurs. This message is sent each turn and gives information about the robot itself.
		* @param time The time that has elapsed since the game had started
		* @param speed The velocity of the robot
		* @param cannonAngle The angle the cannon points to
		*/
		virtual void receiveRTBMessageInfo(double time,double speed,double cannonAngle) throw (StrategyException, bad_exception);

		/**
		* This method is called when the RTB message "Coordinates [x] [y] [angle]" occurs. Generally, this message is sent each turn. If the game option SEND_ROBOT_COORDINATES is sent to 2, absolute coordinates are sent. If this value is 1, the coordinates are given relative to the starting point. On the case 0, this message is not sent.
		* @param x And ...
		* @param y ... are the sent coordinates
		* @param angle is the angle the robot front points to
		*/
		virtual void receiveRTBMessageCoordinates(double x,double y,double angle) throw (StrategyException, bad_exception);

		/**
		* This method is called when the RTB message "RobotInfo [energy] [isTeamMate]" occurs. This message is always sent after another robot has been detected by the radar. It gives some information about this might-be foe.
		* @param energy The unexact energy level of the detected robot
		* @param isTeamMate Defines whether the robot is a team mate (team mode is not yet implemented in RTB)
		*/
		virtual void receiveRTBMessageRobotInfo(double energy,bool isTeamMate) throw (StrategyException, bad_exception);

		/**
		* This method is called when the RTB message "RotationReached [what]" occurs. In our case, this message is sent, when a rotation initiated by "RotateTo" or "RotateAmount" has finished or the sweeping has changed direction.
		* param what What this message bears on; 1 = Robot, 2 = Cannon, 4 = Radar
		*/
		virtual void receiveRTBMessageRotationReached(int what) throw (StrategyException, bad_exception);	// >> Maybe we should introduce another
										//    enum for this (TODO)

		/**
		* This method is called when the RTB message "Energy [energy]" occurs. This message is sent at the end of each turn and specifies the health state of the robot.
		* @param energy The unexact energy level
		*/
		virtual void receiveRTBMessageEnergy(double energy) throw (StrategyException, bad_exception);

		/**
		* This method is called when the RTB message "RobotsLeft [num]" occurs. This message is sent at the beginning of the game and after a robot has been killed.
		* @param num The number of remaining robots
		*/
		virtual void receiveRTBMessageRobotsLeft(int num) throw (StrategyException, bad_exception);

		/**
		* This method is called when the RTB message "Collision [type] [angle]" occurs. This message is sent whan the robot has been hit by or hits another object,
		* @param type The type of the hit object
		* @param angle The angle the collision happened at in rad, relative to the robot front
		*/
		virtual void receiveRTBMessageCollision(object_type type,double angle) throw (StrategyException, bad_exception);

		/**
		* This method is called when the RTB message "Warning [warning] [message]" occurs. This message when RTB has notify the robot on a problem
		* @param warning The type of the warning
		* @param message The text of the warning
		*/
		virtual void receiveRTBMessageWarning(warning_type warning,const string& message) throw (StrategyException, bad_exception);

		/**
		* This method is called when the RTB message "Dead" occurs. This message is sent when the robot pegs out.
		*/
		virtual void receiveRTBMessageDead() throw (StrategyException, bad_exception);
		
	protected:
		/**
		 * Obtain the robot name for sub classes
		 */
		 inline string getRobotName() throw () { return _robotName;}
		
	private:
		BFInternalServerSpecificRepository* _bfssp;
		string _robotName;	
		ostringstream _numberconv;
		BFSharedServerSpecificRepository* _bfsharedssp;
		static bool _newGame;
	};
}

#endif
