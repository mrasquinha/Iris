/*
 * =====================================================================================
 *
 *       Filename:  myFullyVirtualArbiter.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/27/2010 12:58:11 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Mitchelle Rasquinha (), mitchelle.rasquinha@gatech.edu
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */

#ifndef  _ptopswitcharbiter_cc_INC
#define  _ptopswitcharbiter_cc_INC

#include	"ptop_swa.h"

PToPSwitchArbiter::PToPSwitchArbiter()
{

}

PToPSwitchArbiter::~PToPSwitchArbiter()
{

}

void
PToPSwitchArbiter::resize(uint port)
{
    PORTS = port;
    requested.resize(PORTS);
    locked.resize(PORTS);
    done.resize(PORTS);
    last_port_winner.resize(PORTS);
    requesting_inputs.resize(PORTS);
    last_winner.resize(PORTS);
    port_locked.resize(PORTS);

    for ( uint i=0; i<PORTS; i++)
    {
        requested[i].resize(PORTS);
        requesting_inputs[i].resize(PORTS);
    }

    for ( uint i=0; i<PORTS; i++)
        for ( uint j=0; j<PORTS; j++)
        {
            requested[i][j]=false;
        }

    for ( uint i=0; i<PORTS; i++)
        {
            locked[i] = false;
            done[i] = false;
            last_port_winner[i] = 0;
        }
}

bool
PToPSwitchArbiter::is_requested( uint oport, uint inport )
{
    return requested[oport][inport];
}

void
PToPSwitchArbiter::request(uint oport, uint inport )
{
    requested[oport][inport] = true;
    done[oport] = false;
    requesting_inputs[oport][inport].port = inport;
    return;
}

SA_unit
PToPSwitchArbiter::pick_winner( uint oport)
{
    if(!done[oport]) 
    {
        done[oport] = true;
        if(locked[oport] ) 
            return last_winner[oport];

        else
        {
            locked[oport]= true;

                /* Now look at contesting input ports on this channel and pick
                 * a winner*/
                bool winner_found = false;
                for( uint i=last_port_winner[oport]+1; i<(PORTS); i++)
                    if(requested[oport][i])
                    {
                        last_port_winner[oport] = i;
                        winner_found = true;
                        last_winner[oport].port = requesting_inputs[oport][i].port;
                        return last_winner[oport];
                    }

                if(!winner_found)
                for( uint i=0; i<=last_port_winner[oport]; i++)
                {
                    if(requested[oport][i])
                    {
                        last_port_winner[oport] = i;
                        winner_found = true;
                        last_winner[oport].port = requesting_inputs[oport][i].port;
                        return last_winner[oport];
                    }
                }
                if(!winner_found)
                {
                    printf("ERROR: Cant find port winner" );
                    exit(1);
                }

            }
    }

    return last_winner[oport];
}

void
PToPSwitchArbiter::clear_winner( uint oport, uint inport)
{
    done[oport]= false;
    locked[oport] = false;

    requested[oport][inport] = false;
    return;
}

void
PToPSwitchArbiter::clear_requested( uint oport, uint inport)
{
    requested[oport][inport] = false;
    return;
}

bool
PToPSwitchArbiter::is_empty()
{

    for( uint i=0; i<PORTS; i++)
        for( uint j=0; j<PORTS; j++)
            if(requested[i][j] )
                return false;

    return true;
    
}

string
PToPSwitchArbiter::toString() const
{
    stringstream str;
    str << "PToPSwitchArbiter: matrix size "
        << "\t requested_qu row_size: " << requested.size();
    if( requested.size())
       str << " col_size: " << requested[0].size()
        ;
    return str.str();
}
#endif   /* ----- #ifndef _ptopswitcharbiter_cc_INC  ----- */
