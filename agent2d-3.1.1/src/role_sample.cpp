// -*-c++-*-

/*
 *Copyright:

 Copyright (C) Hidehisa AKIYAMA

 This code is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 3, or (at your option)
 any later version.

 This code is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this code; see the file COPYING.  If not, write to
 the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.

 *EndCopyright:
 */

/////////////////////////////////////////////////////////////////////

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "role_sample.h"

#include "strategy.h"

#include "bhv_basic_offensive_kick.h"
#include "bhv_basic_move.h"

#include <rcsc/formation/formation.h>

#include <rcsc/player/player_agent.h>
#include <rcsc/player/intercept_table.h>
#include <rcsc/player/debug_client.h>

#include <rcsc/common/logger.h>
#include <rcsc/common/server_param.h>

using namespace rcsc;

const std::string RoleSample::NAME( "Sample" );

/*-------------------------------------------------------------------*/
/*!

 */
namespace {
rcss::RegHolder role = SoccerRole::creators().autoReg( &RoleSample::create,
                                                       RoleSample::NAME );
}

/*-------------------------------------------------------------------*/
/*!

*/

/* 
/    Parameters : 
/       PlayerAgent * agent : Player agent that'll execute this code after kickoff.
/    Description : 
/       This is where the default behavior of standing in place after a kickoff occurs.
*/
bool
RoleSample::execute( PlayerAgent * agent )
{
    // Bool indicating if the ball is kickable.
    bool kickable = agent->world().self().isKickable();

    // Checks if there are other team mates closer to the ball. If there are, 
    // switch kickable to false and let another agent do the kicking.
    if ( agent->world().existKickableTeammate()
         && agent->world().teammatesFromBall().front()->distFromBall()
         < agent->world().ball().distFromSelf() )
    {
        kickable = false;
    }

    // If the agent is the closest to the ball, kick it. 
    // Else, move the agent to it's home position.
    if ( kickable )
    {
        doKick( agent );
    }
    else
    {
        doMove( agent );
    }

    return true;
}

/*-------------------------------------------------------------------*/
/*!

*/
/* 
/    Parameters : 
/       PlayerAgent * agent : Player agent that'll execute this code.
/    Description : 
/       The agent will kick the ball depending on the current strategy. If there isn't one,
/       the agent will run a default BasicOffensiveKick.
/       (See bhv_basic_offensive_kick.cpp)
*/
void
RoleSample::doKick( PlayerAgent * agent )
{
    // This will check in which area the ball is currently in and use the 
    // appropriate strategy, or the default one in case there aren't any
    // others.
    switch ( Strategy::get_ball_area( agent->world().ball().pos() ) ) {
    case Strategy::BA_CrossBlock:
    case Strategy::BA_Stopper:
    case Strategy::BA_Danger:
    case Strategy::BA_DribbleBlock:
    case Strategy::BA_DefMidField:
    case Strategy::BA_DribbleAttack:
    case Strategy::BA_OffMidField:
    case Strategy::BA_Cross:
    case Strategy::BA_ShootChance:
    default:
        Bhv_BasicOffensiveKick().execute( agent );
        break;
    }
}

/*-------------------------------------------------------------------*/
/*!

*/
/* 
/    Parameters : 
/       PlayerAgent * agent : Player agent that'll execute this code.
/    Description : 
/       The agent will move depending on the current strategy. If there isn't one,
/       the agent will run a BasicMove command.
/       BA_(area) Indicates that the ball is currently in (area), where this area is
/       defined by a certain range of positions in the X axis and Y axis. 
/       (See bhv_basic_move.cpp)
/       (See Strategy.cpp Strategy::BallArea)
*/
void
RoleSample::doMove( PlayerAgent * agent )
{
    // This will check in which area the ball is currently in and use the 
    // appropriate strategy, or the default one in case there aren't any
    // others.
    switch ( Strategy::get_ball_area( agent->world() ) ) {
    case Strategy::BA_CrossBlock:
    case Strategy::BA_Stopper:
    case Strategy::BA_Danger:
    case Strategy::BA_DribbleBlock:
    case Strategy::BA_DefMidField:
    case Strategy::BA_DribbleAttack:
    case Strategy::BA_OffMidField:
    case Strategy::BA_Cross:
    case Strategy::BA_ShootChance:
    default:
        Bhv_BasicMove().execute( agent );
        break;
    }
}
