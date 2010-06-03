/*
 * =====================================================================================
 *
 *       Filename:  mcFrontEnd.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/13/2010 03:26:58 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Mitchelle Rasquinha (), mitchelle.rasquinha@gatech.edu
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */

#ifndef  _mcfrontend_cc_INC
#define  _mcfrontend_cc_INC

#include	"mcFrontEnd.h"
using namespace std;

MCFrontEnd::MCFrontEnd()
{
    name = "MCFrontEnd";
}

MCFrontEnd::~MCFrontEnd ()
{
}		/* -----  end of function MCFrontEnd::~MCFrontEnd  ----- */

void
MCFrontEnd::setup ()
{
    address = myId();

    out_buffer.buffer_size = DEFAULT_OUTPUT_BUFFER_SIZE;
    in_buffer.buffer_size = DEFAULT_INPUT_BUFFER_SIZE;
    in_buffer.set_no_virtual_channels(DEFAULT_VIRTUAL_CHANNELS);
    out_buffer.set_no_virtual_channels(DEFAULT_VIRTUAL_CHANNELS);
    out_packet_index.resize(DEFAULT_VIRTUAL_CHANNELS);
    in_ready.resize(DEFAULT_VIRTUAL_CHANNELS);
    out_arbiter.set_req_queue_size(DEFAULT_VIRTUAL_CHANNELS);
    in_arbiter.set_req_queue_size(DEFAULT_VIRTUAL_CHANNELS);
    in_packets.resize(DEFAULT_VIRTUAL_CHANNELS);
    out_packets.resize(DEFAULT_VIRTUAL_CHANNELS);


    for ( uint i=0; i<DEFAULT_VIRTUAL_CHANNELS ; i++ )
    {
        out_packet_index[i] = 0;
        in_ready[i] = false;
        in_packets[i].destination = address;
        in_packets[i].virtual_channel = i;
    }

    /*  init stats */
    packets_out = 0;
    flits_out = 0;
    packets_in = 0;
    flits_in = 0;
    total_packets_in_time = 0;
    last_out_arbitrate_cycle = 0;
    last_in_arbitrate_cycle = 0;
    /* Init complete */

    /* For now the processor connection of this is a Sink
     * Send a ready event for each vc 
    */
    for ( uint i=0 ; i<out_packets.size() ; i++ )
    {
        IrisEvent* event = new IrisEvent();
        in_packets[i].virtual_channel = i;
        VirtualChannelDescription* vc = new VirtualChannelDescription();
        vc->vc = i;
        event->event_data.push_back(vc);
        event->type = READY_EVENT;
        event->vc = i;
        Simulator::Schedule(Simulator::Now(),&NetworkComponent::process_event, processor_connection, event); 
    }

    return ;
}		/* -----  end of function MCFrontEnd::setup  ----- */

uint
MCFrontEnd::get_no_credits () const
{
    return out_buffer.get_no_credits(0);
}		/* -----  end of function MCFrontEnd::get_no_credits  ----- */

void
MCFrontEnd::set_no_credits ( uint credits )
{
    out_buffer.set_no_credits(credits);
    return ;
}		/* -----  end of function MCFrontEnd::set_no_credits  ----- */

string
MCFrontEnd::toString () const
{
    stringstream str;
    str << "MCFrontEnd: "
        << "\t VC: " << out_packets.size()
        << "\t address: " << address << " node_ip: " << node_ip
        << "\t OutputBuffers: " << out_buffer.toString()
        << "\t InputBuffer: " << in_buffer.toString()
        << "\t out_arbiter: " << out_arbiter.toString()
        << "\t in_arbiter: " << in_arbiter.toString()
        ;
    return str.str();
}		/* -----  end of function MCFrontEnd::toString  ----- */


void
MCFrontEnd::process_event ( IrisEvent* e )
{
    switch(e->type)
    {
        case READY_EVENT:
            handle_ready_event(e);
            break;

        case LINK_ARRIVAL_EVENT:
            handle_link_arrival(e);
            break;

        case IN_ARBITRATE_EVENT:
            handle_in_arbitrate_event(e);
            break;

        case IN_PUSH_EVENT:
            handle_in_push_event(e);
            break;

        case NEW_PACKET_EVENT:
            handle_new_packet_event(e);
            break;

        case OUT_ARBITRATE_EVENT:
            handle_out_arbitrate_event(e);
            break;
            
        case FLIT_OUT_EVENT:
            handle_flit_out_event(e);
            break;

        default:
            cout << "**Unkown event exception! " << e->type << endl;
            break;
    }
    return ;
}		/* -----  end of function MCFrontEnd::process_event  ----- */

void
MCFrontEnd::handle_ready_event ( IrisEvent* e )
{
#ifdef _DEBUG
    stringstream str;
    str << "MCFrontEnd " << address << " " << node_ip << " handle_ready_event ";
    timed_cout(str.str());
#endif

    VirtualChannelDescription* vc = static_cast<VirtualChannelDescription*>(e->event_data.at(0));
    in_ready[vc->vc] = true;

    uint next_tick_type = check_tick();
    if(next_tick_type)
    {
        IrisEvent* event = new IrisEvent();
        event->type = next_tick_type;
        event->vc = e->vc;
        Simulator::Schedule( Simulator::Now()+1, &NetworkComponent::process_event, this, event);
        delete vc;
    }

    delete e;
    return ;
}		/* -----  end of function MCFrontEnd::handle_ready_event  ----- */

void
MCFrontEnd::handle_link_arrival ( IrisEvent* e )
{
    uint tick_time = 1; /*  Waana make sure that the credit recieve event is executed first */
    //Find out if it was a flit or a credit
    LinkArrivalData* uptr = static_cast<LinkArrivalData* >(e->event_data.at(0));

#ifdef _DEBUG
    stringstream str;
    str << "MCFrontEnd " << address << " " << node_ip << " handle_link_arrival "
        << "arrival_type " << uptr->type;
    timed_cout(str.str());
#endif

    if(uptr->type == FLIT_ID)
    {
        flits_in++;
        in_buffer.change_push_channel(uptr->vc);
        in_buffer.push(uptr->ptr);
        if( uptr->ptr->type == TAIL )
            packets_in++;
        uptr->valid = false;

        handle_in_arbitrate_event(e);
    }
    else if ( uptr->type == CREDIT_ID)
    {
        out_buffer.got_credit(uptr->vc);
        tick_time = 1.1;
#ifdef _DEBUG
stringstream str;
str << "MCFrontEnd: got credit for VC " << uptr->vc;
str << "\t no_of_credits: " << out_buffer.get_no_credits(uptr->vc);
timed_cout(str.str());
#endif
        IrisEvent* event = new IrisEvent();
        event->type = OUT_ARBITRATE_EVENT;
        event->vc = e->vc;
        Simulator::Schedule( Simulator::Now()+tick_time, 
                             &MCFrontEnd::process_event, this, event);
    }
    else
    {
        cout << "Exception: Unk link arrival data type! " << endl;
    }


    delete e;
    return ;
}		/* -----  end of function MCFrontEnd::handle_link_arrival  ----- */

void
MCFrontEnd::handle_new_packet_event ( IrisEvent* e )
{
    HighLevelPacket* pkt = static_cast<HighLevelPacket*>(e->event_data.at(0));
    out_packet_index[ pkt->virtual_channel ] = 0;
    out_packets[ pkt->virtual_channel ] = *pkt->to_low_level_packet();

#ifdef _DEBUG
    stringstream str;
    str << "MCFrontEnd " << address << " " << node_ip << " handle_new_packet_event ";
//    str << "MCFrontEnd:: Converted HLP in Interface. LLP is \n" << out_packets[ pkt->virtual_channel].toString();
    timed_cout(str.str());
#endif

    handle_out_arbitrate_event(e);
    return ;
}		/* -----  end of function MCFrontEnd::handle_new_packet_event  ----- */

void
MCFrontEnd::handle_out_arbitrate_event ( IrisEvent* e )
{
    //out packet to out buffer
    for ( uint i=0; i<out_packets.size(); i++ )
        if ( out_packet_index[i] < out_packets[i].size() && !out_buffer.is_channel_full(i))
        {
            out_buffer.change_push_channel(i);
            Flit* ptr = out_packets[i].at(out_packet_index[i]);
            out_buffer.push( ptr);
            out_packet_index[i]++;

#ifdef _DEBUG
stringstream str;
str << "MCFrontEnd " << address << " vc: " << i << " flit=>out_buffer Flit type: " 
    << ptr->type ;
timed_cout(str.str());
#endif

            if(out_packet_index[i] == out_packets[i].size())
            {
                out_packet_index[i] = 0;
                out_packets[i].clear();

                IrisEvent* event = new IrisEvent();
                event->type = READY_EVENT;
                VirtualChannelDescription* vcd = new VirtualChannelDescription();
                vcd->vc = i;
                event->event_data.push_back(vcd);
                event->vc = i;
                Simulator::Schedule(Simulator::Now()+1, &NetworkComponent::process_event, processor_connection, event);

            }
        }

    //Request to arbitrate for flits in out_buffer
    for ( uint i=0; i<out_packets.size(); i++ )
        if( !out_buffer.is_empty(i) 
            && out_arbiter.ready(i) && out_buffer.get_no_credits(i)>0 )
        {
            out_buffer.change_pull_channel(i);
            out_arbiter.request( out_buffer.pull(), i);
#ifdef _DEBUG
stringstream str;
str << "MCFrontEnd " << address << " " << node_ip << " Out arbitrate vc:" << i;
timed_cout(str.str());
#endif
        }

    if(last_flit_out_cycle < Simulator::Now())
    {
        last_flit_out_cycle = Simulator::Now();
        IrisEvent* new_event = new IrisEvent();
        new_event->type = FLIT_OUT_EVENT;
        new_event->vc = e->vc;
        Simulator::Schedule( Simulator::Now()+1, &MCFrontEnd::process_event, this, new_event);
    }
    return;
    return ;
}		/* -----  end of function MCFrontEnd::handle_out_arbitrate_event  ----- */

void
MCFrontEnd::handle_flit_out_event ( IrisEvent* e)
{
    // out out_arbiter to next link
    if(!out_arbiter.empty() && last_out_arbitrate_cycle < Simulator::Now())
    {
        last_out_arbitrate_cycle = Simulator::Now();
       IrisEvent* event = new IrisEvent();
       LinkArrivalData* arrival =  new LinkArrivalData();
       arrival->vc = out_arbiter.pick_winner();
       arrival->ptr = out_arbiter.pull_winner();
       arrival->type = FLIT_ID;
       arrival->valid = true;
       event->event_data.push_back(arrival);
       event->type = LINK_ARRIVAL_EVENT;
       event->src = this;
       event->vc = arrival->vc;

       if( arrival->ptr->type == HEAD )
           packets_out++;
       flits_out++;

       Simulator::Schedule(Simulator::Now()+1, &NetworkComponent::process_event, output_connection, event);
#ifdef _DEBUG
stringstream str;
str << "MCFrontEnd " << address << " " << node_ip << " FLIT_OUT_EVENT Type: "
    << arrival->ptr->type << " vc: " << arrival->vc;
timed_cout(str.str());
#endif
    }

    uint next_event_type = check_tick();
    if(next_event_type)
    {
        IrisEvent* new_event = new IrisEvent();
        new_event->type = next_event_type;
        new_event->vc = e->vc;
        Simulator::Schedule( Simulator::Now()+1, &MCFrontEnd::process_event, this, new_event);
    }
    return ;
}		/* -----  end of function MCFrontEnd::handle_flit_out_event  ----- */

void
MCFrontEnd::convertFromBitStream ( Request* req, HighLevelPacket* hlp )
{
    req->address = 0;
    for (unsigned int i=0; i < NETWORK_ADDRESS_BITS; i++)
    {
	req->address = req->address | (hlp->data[i] << i);
    }
    unsigned int temp = 0;
    for (unsigned int i = 0; i < NETWORK_COMMAND_BITS; i++)
    {
	temp = temp | (hlp->data[i+NETWORK_ADDRESS_BITS] << i);
    }
    req->cmdType = (Command_t)temp;

    req->threadId = 0;
    for (unsigned int i=0; i < NETWORK_THREADID_BITS; i++)
    {
	req->threadId = req->threadId | (hlp->data[i+NETWORK_ADDRESS_BITS+NETWORK_COMMAND_BITS] << i);
    }
    req->data.value = 90;
    if (req->cmdType == CACHE_WRITEBACK)
	for ( uint i=0 ; i < 8*WRITEBACK_SIZE ; i++ )
	{
	    req->data.value = req->data.value | (hlp->data[i+NETWORK_ADDRESS_BITS+NETWORK_COMMAND_BITS+NETWORK_THREADID_BITS] << i);
	    req->data.size = CACHE_WRITEBACK;
        }
    return ;
}		/* -----  end of function MCFrontEnd::convertFromBitStream  ----- */

void
MCFrontEnd::handle_in_push_event ( IrisEvent* e )
{
    for ( uint i=0; i<in_packets.size(); i++ )
        // in packets to processor
        if ( in_ready[i]  && in_packets[i].valid_packet())
        {
            in_ready[i] = false;
            HighLevelPacket* pkt = new HighLevelPacket();
            pkt->from_low_level_packet(&in_packets[i]);
            Request* new_req = new Request();
            convertFromBitStream(new_req, pkt);
            in_packets[i].clear();

stringstream str1;
str1 << "MCFrontEnd " << address << " " << node_ip << " Got new request " << i;
str1 << new_req->toString();
timed_cout(str1.str());
delete new_req;
delete pkt;

            /* 
            IrisEvent* event = new IrisEvent();
            event->type = NEW_PACKET_EVENT;
            event->event_data.push_back(pkt);
            event->vc = i;
            Simulator::Schedule(Simulator::Now()+ 1, 
                                &NetworkComponent::process_event, processor_connection, event);
             * */


#ifdef _DEBUG
stringstream str;
str << "MCFrontEnd " << address << " " << node_ip << " NI->Processor VC: " << i;
timed_cout(str.str());
#endif
             
        }

    uint next_event_type = check_input_conditions();
    if(next_event_type)
    {
        IrisEvent* new_event = new IrisEvent();
        new_event->type = next_event_type;
        new_event->vc = e->vc;
        Simulator::Schedule( Simulator::Now()+1, &MCFrontEnd::process_event, this, new_event);
    }
    return ;
}		/* -----  end of function MCFrontEnd::handle_in_push_event  ----- */

void
MCFrontEnd::handle_in_arbitrate_event ( IrisEvent* e)
{
#ifdef _DEBUG
stringstream str;
str << "MCFrontEnd " << address << " " << node_ip << " handle_in_arbitrate_event";
timed_cout(str.str());
#endif
    // Request arbitration for in buffer
    for ( uint i=0; i<in_packets.size(); i++ )
    {
        if ( in_arbiter.ready(i) && !in_buffer.is_empty(i) )
        {
                    //Send a credit back since you cleared the in_buffer
                    LinkArrivalData* arrival = new LinkArrivalData();
                    arrival->vc = i;
                    arrival->type = CREDIT_ID;
                    arrival->valid = false;
                    IrisEvent* event = new IrisEvent();
                    event->type = LINK_ARRIVAL_EVENT;
                    event->vc = i;
                    event->event_data.push_back(arrival);
                    event->src = this;
                    Simulator::Schedule( Simulator::Now()+1, 
                                 &NetworkComponent::process_event, input_connection, event);
                    in_buffer.change_pull_channel(i);
                    Flit* ptr = in_buffer.pull();
                    in_arbiter.request( ptr, i);
#ifdef _DEBUG
stringstream str;
str << "MCFrontEnd " << address << " " << node_ip 
    << " In_buff-> In_arb vc: " << i << " type: " << ptr->type;
timed_cout(str.str());
#endif
        }
    }
                    
    // arbitrate for the winner and push packets to in_buffer
    for ( uint i=0; i<in_packets.size(); i++ )
        if( !in_arbiter.empty()) // && last_in_arbitrate_cycle < Simulator::Now())
    {
        uint winner = in_arbiter.pick_winner();
            if( winner == i )
            {
                if( in_packets[i].size() == 0 || in_packets[i].size() < in_packets[i].length)
                {
                    last_in_arbitrate_cycle = Simulator::Now();
                    Flit* ptr = in_arbiter.pull_winner();
                    in_packets[i].add(ptr);
                    if( ptr->type == HEAD 
                        && static_cast<HeadFlit*>(ptr)->dst_address != node_ip)
                    {
                        cout << "\ninterface address: " << address << " " << node_ip << " MCFrontEnd::ERROR IncorrectDestinationException" << endl;
                        cout << " in_pkt_dest: " << in_packets[i].destination << "\t node_ip: " << node_ip << endl;
                    }
//                    delete ptr;

#ifdef _DEBUG
stringstream str;
str << "MCFrontEnd " << address << " " << node_ip << " WON_IN_ARB VC: "
    << i << " flit type: " << ptr->type;
timed_cout(str.str());
#endif

                }
            }
    }

    uint next_event_type = check_input_conditions();
    if(next_event_type)
    {
        IrisEvent* new_event = new IrisEvent();
        new_event->type = next_event_type;
        new_event->vc = e->vc;
        Simulator::Schedule( Simulator::Now()+1, &MCFrontEnd::process_event, this, new_event);
    }

    return ;
}		/* -----  end of function MCFrontEnd::handle_in_arbitrate_event  ----- */

uint
MCFrontEnd::check_tick ()
{
    for ( uint i=0 ; i<out_packets.size() ; i++ )
        if(!out_arbiter.empty() || !out_buffer.is_empty(i))
            return OUT_ARBITRATE_EVENT;

    return 0;
}		/* -----  end of function MCFrontEnd::check_tick  ----- */

uint
MCFrontEnd::check_input_conditions ()
{
    for ( uint i=0 ; i<in_packets.size() ; i++ )
        if ( in_ready[i]  && in_packets[i].valid_packet())
            return IN_PUSH_EVENT;

    for ( uint i=0 ; i<in_packets.size() ; i++ )
        if ( (in_arbiter.ready(i) && !in_buffer.is_empty(i)) )
            return IN_ARBITRATE_EVENT;
    
    for ( uint i=0 ; i<in_packets.size() ; i++ )
        if (!in_arbiter.empty() )
            return IN_ARBITRATE_EVENT;
    return 0;
}		/* -----  end of function MCFrontEnd::check_input_conditions  ----- */

string
MCFrontEnd::print_stats ()
{
    stringstream str;
    str << "\n flits_in: " << flits_in
        << "\n packets_in: " << packets_in
        << "\n flits_out: " << flits_out
        << "\n packets_out: " << packets_out
        << "\n total_packets_in_time: " << total_packets_in_time /* need to keep track of the transit time for this */
        ;
    return str.str();
}		/* -----  end of function MCFrontEnd::print_stats  ----- */

#endif   /* ----- #ifndef _mcfrontend_cc_INC  ----- */

