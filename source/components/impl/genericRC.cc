/*
 * =====================================================================================
 *
 *       Filename:  genericRC.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/19/2010 12:13:39 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Mitchelle Rasquinha (), mitchelle.rasquinha@gatech.edu
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */
#ifndef  _genericaddressdecoder_cc_INC
#define  _genericaddressdecoder_cc_INC

#include	"genericRC.h"

using namespace std;

GenericRC::GenericRC()
{
    name = "Router";
    node_ip = 0;
    address = 0;
}

uint
GenericRC::route_x_y(uint dest)
{
    uint oport = -1;
    uint myx=-1, destx=-1, myy =-1, desty=-1;
    myx = grid_xloc[node_ip];
    myy = grid_yloc[node_ip];
    destx = grid_xloc[ dest ];
    desty = grid_yloc[ dest ];
    if ( myx == destx && myy == desty )
        oport = 0;
    else if ( myx ==  destx )
    {
        if( desty < myy )
            oport = 3;
        else
            oport = 4;
    }
    else
    {
        if( destx < myx )
            oport = 1;
        else
            oport = 2;
    }

#ifdef _DEBUG
    _DBG(" Route HEAD dest: %d oport: %d ", dest, oport);
    _DBG(" addr: %d myid_x: %d myid_y: %d destid_x: %d destid_y: %d", node_ip, myx, myy, destx, desty);
#endif

    return oport;
}

void
GenericRC::push (Flit* f, uint ch )
{
    if(ch > addresses.size())
        cout << "Invalid VC Exception " << endl;


    //Route the head
    if( f->type == HEAD )
    {
        HeadFlit* header = static_cast< HeadFlit* >( f );
        addresses [ch].out_port = route_x_y(header->dst_address);
        addresses [ch].channel = header->vc;
        addresses [ch].route_valid = true;

#ifdef _DEEP_DEBUG
    _DBG(" computed oport %d %d dest: %d",addresses [ch].out_port, addresses [ch].channel, header->dst_address);
#endif

    }
    else if(f->type == TAIL)
    {
        if( !addresses[ch].route_valid)
        {
            _DBG_NOARG("TAIL InvalidAddrException" );
        }
        
        addresses[ch].route_valid = false;
    }
    else if (f->type == BODY)
    {
        if( !addresses[ch].route_valid)
        {
            _DBG_NOARG("BODY InvalidAddrException" );
        }
    }
    else
    {
        _DBG(" InvalidFlitException fty: %d", f->type);
    }

    return ;
}		/* -----  end of method genericRC::push  ----- */


uint
GenericRC::get_output_port ( uint ch)
{
    return addresses[ch].out_port;
}		/* -----  end of method genericRC::get_output_port  ----- */

uint
GenericRC::get_virtual_channel ( uint ch )
{
    return addresses [ch].channel;
}		/* -----  end of method genericRC::get_vc  ----- */

void
GenericRC::resize ( uint ch )
{
    addresses.resize(ch);
    for ( uint i = 0 ; i<ch ; i++ )
    {
        addresses[i].route_valid = false;
    }
    return ;
}		/* -----  end of method genericRC::set_no_channels  ----- */

uint
GenericRC::get_no_channels()
{
    return addresses.size();
}		/* -----  end of method genericRC::get_no_channels  ----- */

bool
GenericRC::is_empty ()
{
    uint channels = addresses.size();
    for ( uint i=0 ; i<channels ; i++ )
        if(addresses[i].route_valid)
            return false;

    return true;
}		/* -----  end of method genericRC::is_empty  ----- */

string
GenericRC::toString () const
{
    stringstream str;
    str << "GenericRC"
        << "\tchannels: " << addresses.size();
    return str.str();
}		/* -----  end of function GenericRC::toString  ----- */
#endif   /* ----- #ifndef _genericaddressdecoder_cc_INC  ----- */

