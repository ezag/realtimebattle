#! /bin/sh

# JBot, a robot for RealTimeBattle
# written in Java, author:
# (c) 1999 by Ingo Beckmann
# ingoBeckmann@gmx.de

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software Foundation,
# Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
# email: brain@nnTec.de

#GameOptions:
SEND_SIGNAL=0
SEND_ROTATION_REACHED=1
SIGNAL=2
USE_NON_BLOCKING=3

RTB_PATH=@ROBOTDIR@/jBot
RobotName=JBot
SLEEP=1

echo RobotOption $SEND_SIGNAL 0
echo RobotOption $SEND_ROTATION_REACHED 1
echo RobotOption $SIGNAL 0
echo RobotOption $USE_NON_BLOCKING 1
# program options: 1=SLEEP in [ms], 2=Robot Name
exec @JAVA@ -classpath $CLASSPATH:$RTB_PATH JBotMain $SLEEP $RobotName
