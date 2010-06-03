/*
 * =====================================================================================
 *
 *       Filename:  genericPortArbiter.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/14/2010 12:56:37 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Mitchelle Rasquinha (), mitchelle.rasquinha@gatech.edu
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */

#ifndef  _genericportarbiter_cc_INC
#define  _genericportarbiter_cc_INC

#include	"genericPortArbiter.h"

using namespace std;

GenericPortArbiter::GenericPortArbiter()
{
    name = "GenericPortArbiter";
}

GenericPortArbiter::~GenericPortArbiter ()
{
}		/* -----  end of function GenericPortArbiter::~GenericPortArbiter  ----- */

void
GenericPortArbiter::flush_all_requests()
{
    for ( uint i=0 ; i<requests.size() ; i++ )
        for ( uint j=0 ; j<requests[i].size() ; j++ )
            if ( requests[i][j] )
            {
                cout << "\n Flushing pending reqs in port arbiter " << endl;
                delete flits[i][j];
            }
    return;

}

uint
GenericPortArbiter::get_no_requests ()
{
    return requests.size();
}		/* -----  end of function GenericPortArbiter::get_no_channels  ----- */


void
GenericPortArbiter::set_req_queue_size (uint ch )
{

    requests.resize(ch);
    flits.resize(ch);
    last_winner.resize(ch);
    done.resize(ch);
    locked.resize(ch);
    for ( uint i= 0 ; i<ch ; i++ )
    {
        done[i] = false;
        locked[i] = false;
        requests[i].resize(ports);
        flits[i].resize(ports);
    }

    for ( uint i=0; i<ch ; i++ )
    {
        for ( uint j=requests.size() ; j<ch ; j++ )
            requests[i][j] = false;
    }
}

bool
GenericPortArbiter::is_empty ( uint ch )
{
    if(ch >= requests.size())
        cout << "GenericPortArbiter:: Error invalid virtual channel" << endl;

    bool ty =true;
    if(!locked[ch])
        for ( uint i=0; i<requests[ch].size() ; i++ )
            ty = ty & !requests[ch][i];
    else
        ty = ty & !requests[ch][last_winner[ch]];

    return ty;
}		/* -----  end of function GenericPortArbiter::empty  ----- */

void
GenericPortArbiter::request ( Flit* f, uint port, uint vc )
{
    if( vc >= requests.size() || port >= requests[vc].size() )
    {
        cout << "GenericPortArbiter::InvalidVirtualChannelException"
            << "or InvalidPortException" 
            << " or InvalidFlitException" << endl;
        exit(1);
    }

    requests[vc][port] = true;
    flits[vc][port] = f;
    return ;
}		/* -----  end of function GenericPortArbiter::request  ----- */

uint
GenericPortArbiter::pick_winner ( uint ch )
{
    if(ch>=requests.size())
    {
        cout << "GenericPortArbiter::InvalidVCException" << endl;
        exit(1);
    }

    if(!done[ch])
    {
        done[ch] = true;
        if(locked[ch])
            return last_winner[ch];

        for( uint i=last_winner[ch]+1; i<requests[ch].size(); i++)
            if(requests[ch][i])
            {
                last_winner[ch] = i;
                return i;
            }

        for( uint i=0; i<last_winner[ch]+1; i++ )
            if(requests[ch][i])
            {
                last_winner[ch] = i;
                return i;
            }
    }
    else
        return last_winner[ch];

    cout << "GenericPortArbiter:: NoRequestsException" << endl;
    exit(1);
}		/* -----  end of function GenericPortArbiter::pick_winner  ----- */

Flit*
GenericPortArbiter::pull_winner ( uint ch )
{
    if( ch>= requests.size() )
    {
        cout << "GenericPortArbiter:: InvalidVCException" << endl;
        exit(1);
    }

    if(!done[ch])
    {
        for( uint i = last_winner[ch]+1 ; i<requests[ch].size() ; i++ )
            if ( requests[ch][i])
                last_winner[ch] = i;

        for( uint i=0; i<last_winner[ch]+1 ; i++)
            if(requests[ch][i])
                last_winner[ch] = i;
    }
    else
        done[ch] = false;

    if(!requests[ch][last_winner[ch]])
    {
        cout << "GenericPortArbiter::InvalidFlitException" << endl;
        exit(1);
    }

    Flit* f = flits[ch][last_winner[ch]];
    if(f->type == HEAD)
        locked[ch] = true;
    else if ( f->type == TAIL)
        locked[ch] = false;

    requests[ch][last_winner[ch]] = false;
    return f;
}		/* -----  end of function GenericPortArbiter::pull_winner  ----- */

bool
GenericPortArbiter::is_requested ( uint port, uint ch )
{
    if( ch>=requests.size() ||  port>= requests[ch].size() )
    {
        cout << "\nGenericPortArbiter:: InvalidVCException or InvalidPortException" 
            << "\t port: " << port << "\t ch: " << ch << endl
            << "\t req.size: " << requests.size() << "\t req[ch].size: " << requests[ch].size();
        exit(1);
    }

    return requests[ch][port];
}		/* -----  end of function GenericPortArbiter::ready  ----- */

string
GenericPortArbiter::toString () const
{
    stringstream str;
    str << "GenericPortArbiter"
        << "\treqs: " << requests.size();
    return str.str();
}		/* -----  end of function GenericPortArbiter::toString  ----- */


#endif   /* ----- #ifndef _genericportarbiter_cc_INC  ----- */

