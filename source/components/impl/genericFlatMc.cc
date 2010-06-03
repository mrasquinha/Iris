#ifndef _genericflatmc_cc_INC
#define _genericflatmc_cc_INC

#include "genericFlatMc.h"

using namespace std;

GenericFlatMc::GenericFlatMc ()
{
    name = "GenericFlatMc";
    sending = false;
    interface_connections.resize(1);	
} /* ----- end of function GenericFlatMc::GenericFlatMc ----- */

GenericFlatMc::~GenericFlatMc ()
{
    pending_packets.clear();
    pending_packets_time.clear();
    out_file.close();
    return ;
} /* ----- end of function GenericFlatMc::~GenericFlatMc ----- */

void
GenericFlatMc::setup (uint n, uint v, uint time)
{
    vcs =v;
    no_nodes = n;
    max_sim_time = time;
    address = myId();
    
    packets = 0;
    min_pkt_latency = 999999999;

    ready.resize( vcs );
    ready.insert( ready.begin(), ready.size(), false );
    for(unsigned int i = 0; i < ready.size(); i++)
        ready[i] = false;
    packets_pending = 0;

    /* send ready events for each virtual channel*/
    for( unsigned int i = 0; i < vcs; i++ )
    {
        IrisEvent* event = new IrisEvent();
        event->type = READY_EVENT;
        event->vc = 0;
        Simulator::Schedule(Simulator::Now()+1, &NetworkComponent::process_event,this, event);
    }

    return ;
} /* ----- end of function GenericFlatMc::setup ----- */

void
GenericFlatMc::set_output_path( string name)
{
    // open the output trace file
    stringstream str;
    str << name << "/flat_mc_" << address << "_trace_out.tr";
    out_filename = str.str();
    out_file.open(out_filename.c_str());
    if( !out_file.is_open() )
    {
        stringstream stream;
        stream << "Could not open output trace file " << out_filename << ".\n";
        timed_cout(stream.str());
    }
}

void
GenericFlatMc::finish ()
{
} /* ----- end of function GenericFlatMc::finish ----- */

void
GenericFlatMc::process_event (IrisEvent* e)
{
    switch(e->type)
    {
        case NEW_PACKET_EVENT:
            handle_new_packet_event(e);
            break;
        case OUT_PULL_EVENT:
            handle_out_pull_event(e);
            break;
        case READY_EVENT:
            handle_ready_event(e);
            break;
        default:
            cout << "\nTPG: " << address << "process_event: UNK event type" << endl;
            break;
    }
    return ;
} /* ----- end of function GenericFlatMc::process_event ----- */

void
GenericFlatMc::handle_new_packet_event ( IrisEvent* e)
{
    // get the packet data
    HighLevelPacket* hlp = static_cast< HighLevelPacket* >( e->event_data.at(0));
    double lat = Simulator::Now() - hlp->sent_time;
    if( min_pkt_latency > lat)
        min_pkt_latency = lat;
    _DBG( "-------------- GOT NEW PACKET ---------------\n pkt_latency: %f", lat);
    

    // write out the packet data to the output trace file
    if( !out_file.is_open() )
        out_file.open( out_filename.c_str(), std::ios_base::app );

    if( !out_file.is_open() )
    {
        cout << "Could not open output trace file " << out_filename << ".\n";
    }
        
        out_file << hlp->toString();
        out_file << "\tPkt latency: " << lat << endl;

    // send back a ready event
    IrisEvent* event2 = new IrisEvent();
    event2->type = READY_EVENT;
    event2->vc = 0; //hlp->virtual_channel;
    Simulator::Schedule( Simulator::Now()+1, &NetworkComponent::process_event, interface_connections[0], event2);

    //Increment the response packet counter and update the sending packet time
    packets_pending++;
    pending_packets_time.push_back(Simulator::Now()+200);
    pending_packets.push_back(hlp);


        IrisEvent* event = new IrisEvent();
        event->type = OUT_PULL_EVENT;
        event->vc = 0;
        
        Simulator::Schedule(Simulator::Now()+199, &NetworkComponent::process_event, this, event);
        sending = true;

  //  delete hlp;
    delete e;
    return ;
} /* ----- end of function GenericFlatMc::handle_new_packet_event ----- */

void
GenericFlatMc::handle_out_pull_event ( IrisEvent* e )
{
    MTRand mtrand1;
    if( ready[0] && packets_pending > 0)
    {
        packets++;
        packets_pending--;
        HighLevelPacket* hlp = pending_packets.front();
        //       HighLevelPacket* hlp = static_cast< HighLevelPacket* >( e->event_data.at(0));
   //     HighLevelPacket* hlp = new HighLevelPacket();
        hlp->virtual_channel = 0;

        hlp->transaction_id = mtrand1.randInt(1000);
        hlp->msg_class = RESPONSE_PKT;
        hlp->destination = hlp->source/3;//pending_packets_source.front();
        hlp->source = address;
        hlp->sent_time = pending_packets_time.front();
        pending_packets_time.pop_front();
            pending_packets.pop_front();
            int xx = hlp->data.size();
	for ( uint i=xx ; i < 10*max_phy_link_bits ; i++ )
            hlp->data.push_back(true);
        hlp->data_payload_length = hlp->data.size();
        cout << "\n payload size: " << hlp->data_payload_length << endl;

        hlp->sent_time = Simulator::Now();

        ready[0] = false;
        IrisEvent* event = new IrisEvent();
        event->type = NEW_PACKET_EVENT;
        event->event_data.push_back(hlp);
        Simulator::Schedule( hlp->sent_time, &NetworkComponent::process_event, interface_connections[0], event );
                
        sending = false;
    }

    delete e;
    return ;
} /* ----- end of function GenericFlatMc::handle_out_pull_event ----- */

void
GenericFlatMc::handle_ready_event ( IrisEvent* e)
{

    // send the next packet if it is less than the current time
    ready[0] = true;
#ifdef _DEBUG_TPG
    _DBG_NOARG(" handle_ready_event ");
#endif

    if( !sending && Simulator::Now() < max_sim_time )
    {
        IrisEvent* event = new IrisEvent();
        event->type = OUT_PULL_EVENT;
        event->vc = 0;
        Simulator::Schedule(Simulator::Now()+1, &NetworkComponent::process_event, this, event);
        sending = true;
    }

    delete e;
    return ;
} /* ----- end of function GenericFlatMc::handle_ready_event ----- */

string
GenericFlatMc::toString () const
{
    stringstream str;
    str << "\nGenericFlatMc: "
        << "\t vcs: " << ready.size()
        << "\t address: " <<address
        << "\t node_ip: " << node_ip
        ;
    return str.str();
} /* ----- end of function GenericFlatMc::toString ----- */

string
GenericFlatMc::print_stats() const
{
    stringstream str;
    str << toString()
        << "\n packets:\t " << packets
        << "\n min_pkt_latency:\t" << min_pkt_latency
        ;
    return str.str();
} /* ----- end of function GenericFlatMc::toString ----- */

#endif /* ----- #ifndef _genericflatmc_cc_INC ----- */
