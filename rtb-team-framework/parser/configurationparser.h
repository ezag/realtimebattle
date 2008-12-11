/************************************************************************
    $Id: configurationparser.h,v 1.3 2008/12/11 11:52:44 fakko Exp $
    
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

    $Log: configurationparser.h,v $
    Revision 1.3  2008/12/11 11:52:44  fakko
    - Applied two patches from the Debian package of RealTimeBattle
    - Several fixes of includes to make the code compile with latest gcc versions

    Revision 1.2  2005/02/24 10:27:55  jonico
    Updated newest version of the framework

    Revision 1.2  2005/01/06 17:59:33  jonico
    Now all files in the repository have their new header format.


**************************************************************************/

#ifndef CPARSER_H
#define CPARSER_H

#include <string>
#include <map>
#include "../exceptions/configurationparserexception.h"


/**
* Namespace Parser
*/
namespace Parser {
	using Exceptions::ParserException;
	using std::bad_exception;
	using Exceptions::ConfigurationParserException;
	using std::string;
	using std::map;
	
	/**
	* class ConfigurationParser
	*/
	class ConfigurationParser {
	/*
	* Public stuff
	*/
	public:
		ConfigurationParser() throw (bad_exception);
		void addFile(const string& filename) throw(ParserException, bad_exception);
		const map<string, map<string, string> >& getConfigurationData() const throw();
	/*
	* Private stuff
	*/
	private:
		string erase_wspace(const string& s) const throw (bad_exception);
	
		/**
		* true, if the given character is allowed inside a section, a key  or a value.
		*/
		bool iscchar(int c) const throw ();

		map<string, map<string, string> > _configurationData;
	};
}
#endif // CPARSER_H
