/************************************************************************
    $Id: robot_option_type.h,v 1.2 2005/02/24 10:27:55 jonico Exp $
    
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

    $Log: robot_option_type.h,v $
    Revision 1.2  2005/02/24 10:27:55  jonico
    Updated newest version of the framework

    Revision 1.2  2005/01/06 17:59:34  jonico
    Now all files in the repository have their new header format.


**************************************************************************/

#ifndef __ROBOTOPTIONTYPE__
#define __ROBOTOPTIONTYPE__

/**
* Namespace Parser
*/
namespace Parser {
	enum robot_option_type
	{
	  SIGNAL=2,                   // 0 - no signal, > 1 - signal to send (e.g. SIGUSR1 or SIGUSR2) 
	  SEND_SIGNAL=0,              // 0 - false, 1 - true
	  SEND_ROTATION_REACHED=1,    // 0 - no messages
				      // 1 - messages when RotateTo and RotateAmount finished
				      // 2 - messages also when sweep direction is changed

	  USE_NON_BLOCKING=3          // 0 - false, 1 - true 
				      // This option should always be sent as soon as possible

	};
}

#endif 
