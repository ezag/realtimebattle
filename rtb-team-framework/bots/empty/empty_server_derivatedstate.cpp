/************************************************************************
    $Id: empty_server_derivatedstate.cpp,v 1.2 2005/02/24 10:27:53 jonico Exp $
    
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

    $Log: empty_server_derivatedstate.cpp,v $
    Revision 1.2  2005/02/24 10:27:53  jonico
    Updated newest version of the framework

    Revision 1.2  2005/01/06 17:59:31  jonico
    Now all files in the repository have their new header format.


**************************************************************************/

#include <common.h>

#include "empty_server_derivatedstate.h"
#include "empty_sharedserverspecificrepository.h"

namespace empty {	
	empty_ServerDerivatedState::empty_ServerDerivatedState(ServerCoordinator* i , empty_InternalServerSpecificRepository* empty_issp, empty_SharedServerSpecificRepository* empty_sharedssp) throw (StrategyException,bad_exception): empty_ServerBasicState(i, empty_issp, empty_sharedssp) {}
		
	/**
	* Implement here the overrided methods
	*/
}
