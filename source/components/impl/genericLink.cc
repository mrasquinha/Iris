/*
 * =====================================================================================
 *
 *       Filename:  genericLink.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/27/2010 11:48:46 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Mitchelle Rasquinha (), mitchelle.rasquinha@gatech.edu
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */

#ifndef  _genericlink_cc_INC
#define  _genericlink_cc_INC

#include	"genericLink.h"

void
GenericLink::setup()
{
    name = "link";
    address = myId();
    node_ip = 0;
    flits_passed= 0;
    credits_passed=0;
}

void
GenericLink::process_event ( IrisEvent* e )
{
    switch(e->type)
    {
        case LINK_ARRIVAL_EVENT:
            handle_link_arrival_event(e);
            break;
        default:
            cout << " Unk event type at link " << endl;
            break;
    }

    return ;
}		/* -----  end of function GenericLink::process_event  ----- */

void
GenericLink::handle_link_arrival_event( IrisEvent* e)
{
    LinkArrivalData* data = static_cast<LinkArrivalData*>(e->event_data.at(0));

#ifdef _DEBUG
    _DBG("handle_link_arrival_event vc: %d src_id: %d dest: %d, data type: %d", data->vc, e->src_id, e->dst_id, data->type);
#endif

    if(e->src_id == input_connection->address)
    {
        /* Update stats */
        flits_passed++;

        EventId uid = Simulator::Schedule( Simulator::Now()+ 0.75, &NetworkComponent::process_event, output_connection, e);
//        cout << " inseid link: " << uid.time << " " << uid.uid << " " << uid.nextUID;
    }
    else
    {
        /* Update stats */
        credits_passed++;
        Simulator::Schedule( Simulator::Now()+ 0.75, &NetworkComponent::process_event, input_connection, e);
    }
}

string
GenericLink::toString () const
{
    stringstream str;
    str << "GenericLink: "
        << "\taddress: " << address;
    if(input_connection)
        str << "\tinput_connection: " << static_cast<NetworkComponent*>(input_connection)->address;
    if(output_connection)
        str << "\toutput_connection: " << static_cast<NetworkComponent*>(output_connection)->address;
    
    return str.str();
}		/* -----  end of function GenericLink::toString  ----- */

string
GenericLink::print_stats() const
{
    stringstream str;
    str << endl << toString()
        << "\n link[" << address << "] flits_passed: " << flits_passed 
        << "\n link[" << address << "] credits_passed: " << credits_passed
        << ". " << endl;

    return str.str();
}


#endif   /* ----- #ifndef _genericlink_cc_INC  ----- */
