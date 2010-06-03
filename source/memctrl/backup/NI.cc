/*
 * =====================================================================================
 *
 *       Filename:  packetSource.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/11/2010 05:12:58 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Mitchelle Rasquinha (), mitchelle.rasquinha@gatech.edu
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */
#ifndef  _packetsource_cc_INC
#define  _packetsource_cc_INC

#include	"NI.h"
#include	"MC.h"
#include        "response_handler.h"  
//#define SEND_ONE_PACKET 1
using namespace std;

NI::NI()
{
    name = "NI";
    interface_connections.resize(1);
    mc = (Component*)(new MC());
    ((MC*)mc)->parent = this;
    ((MC*)mc)->ni = this; 
    ((MC*)mc)->Init();	
}

NI::~NI()
{
}

void
NI::set_no_vcs ( uint v)
{
    vcs = v;
}

void
NI::setup(uint n, uint v, uint time)
{

    vcs =v;
    max_sim_time = time;
    no_nodes = n;
    packets = 0;
    address = myId();
    
    ready.resize( vcs );
    ready.insert( ready.begin(), ready.size(), false );
    for(unsigned int i = 0; i < ready.size(); i++)
        ready[i] = false;


   //  send ready events for each virtual channel
    for( unsigned int i = 0; i < vcs; i++ )
    {
        VirtualChannelDescription* vc = new VirtualChannelDescription();
        vc->vc = i;

        IrisEvent* event = new IrisEvent();
        event->type = READY_EVENT;
        event->vc = i;
        event->event_data.push_back(vc);
        Simulator::Schedule(Simulator::Now()+1, &NetworkComponent::process_event,this, event);
// interface_connections[0], event);
    }

    return ;
} /* ----- end of function GenericTPG::setup ----- */

void
NI::set_output_path( string name)
{
    // open the output trace file
    stringstream str;
    str << name << "/ni_" << address << "_trace_out.tr";
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
NI::finish()
{
    out_file.close();
}


void 
NI::process_event(IrisEvent* e)
{
    switch(e->type)
    {
        case NEW_PACKET_EVENT:
            handle_new_packet_event(e);
            break;

	case OLD_PACKET_EVENT:
            handle_old_packet_event(e);
            break;

        case OUT_PULL_EVENT:
            handle_out_pull_event(e);
            break;

        case READY_EVENT:
            handle_ready_event(e);
            break;

        default:
            cout << "NI:: Unk event exception" << endl;
            break;
    }
    return ;
} /* ----- end of function GenericTPG::process_event ----- */

void 
NI::handle_new_packet_event(IrisEvent* e)
{
#ifdef _DEBUG
    stringstream str;
    str << "NI " << address << " handle_new_packet_event " << e->vc ;
    timed_cout( str.str());
#endif
    if (!((MC*)mc)->reqH->oneBufferFull)
    {
	HighLevelPacket* hlp = static_cast<HighLevelPacket*>(e->event_data.at(0));

    	hlp->recv_time = Simulator::Now();	
    	_DBG_NOARG(" NI*********** GOT NEW PACKET *******************");
    	cout << hlp->toString() << " Recv time: " << Simulator::Now() 
             << " pkt_latency: " << Simulator::Now() - hlp->sent_time << endl;
    	out_file << hlp->toString() << " Recv time: " << Simulator::Now() 
        << " pkt_latency: " << Simulator::Now() - hlp->sent_time << endl;

	Request* req = new Request();
	convertFromBitStream(req, hlp);
    	req->arrivalTime = hlp->recv_time;	

	cout << hex << req->address << ", cmdType " << req->cmdType << ", ThreadId " << req->threadId << endl; 

	IrisEvent *e2 = new IrisEvent();
	e2->src = this;
	e2->dst = ((MC*)mc)->reqH;
    	e2->event_data.push_back((void*)req);
    	e2->type = START;	
    	Simulator::Schedule(Simulator::Now()+1, &RequestHandler::process_event, (RequestHandler*)e2->dst, e2);	

	 // send back a ready event
    	IrisEvent* event = new IrisEvent();
    	event->type = READY_EVENT;
    	VirtualChannelDescription* vc = new VirtualChannelDescription();
    	event->vc = 0;	
    	event->event_data.push_back(vc);
    	Simulator::Schedule( Simulator::Now()+1, &NetworkComponent::process_event, interface_connections[0], event);	
	delete hlp;
    }
    else
    {
	IrisEvent* event = new IrisEvent();
        event->type = OLD_PACKET_EVENT;
	static_cast<HighLevelPacket*>(e->event_data.at(0))->recv_time = Simulator::Now();
	event->event_data.push_back(e->event_data.at(0));
        Simulator::Schedule(Simulator::Now()+1, &NetworkComponent::process_event, this, event);
    }

    delete e;
    return ;
}
void 
NI::handle_out_pull_event(IrisEvent* e)
{
#ifdef _DEBUG
    stringstream str;
    str << "NI " << address << " handle_out_pull_event " << e->vc ;
    timed_cout( str.str());
#endif
    
    MTRand mtrand1;
	Request* req = new Request();
    if (ready[0] && GetFromNIQueue(req))
    {
    	    //  Send a high level packet now 
	    vector<Request>::iterator queueIndex = niQueue.begin();	
            IrisEvent* event = new IrisEvent();
            HighLevelPacket* hlp = new HighLevelPacket();
            event->type = NEW_PACKET_EVENT;
            hlp->virtual_channel = 0;
            hlp->source = address;

            // Random packet generator should change this to match the
            // distribution 
      //      if( node_ip == 1 ) 
        //    	hlp->destination = 0;
       	  //  else
            //	hlp->destination = 1;
		hlp->destination = req->threadId;
            hlp->transaction_id = mtrand1.randInt(1000);
		hlp->msg_class = ONE_FLIT_REQ; //RESPONSE_PKT;
	
	    convertToBitStream(req, hlp);      
            hlp->sent_time = req->retireTime; //Simulator::Now();

            event->event_data.push_back(hlp);
		 cout << "I also came here " << hlp->toString() << endl;	
            Simulator::Schedule(Simulator::Now()+1, &NetworkComponent::process_event,interface_connections[0], event);

            ready[0] = false;
	    niQueue.erase(queueIndex);
            
/*
        IrisEvent* event3 = new IrisEvent();
        event3->type = SEND_TO_NI; //ask response handler for more
        event3->dst = ((MC*)mc)->responseH; //e->vc;
        Simulator::Schedule(Simulator::Now()+1, &ResponseHandler::process_event, (ResponseHandler*)event3->dst, event3);
*/

    }
/*    else if (!ready[0] && GetFromNIQueue(req))
    {
        IrisEvent* event2 = new IrisEvent();
        event2->type = OUT_PULL_EVENT;
        event2->vc = 0; //e->vc;
        Simulator::Schedule(Simulator::Now()+1, &NetworkComponent::process_event, this, event2);
        sending = true;
    }		
*/    else
	sending = false;

    cout << ready[0] << " mini " << GetFromNIQueue(req); 
    delete req;	
    delete e;
}

void 
NI::handle_ready_event(IrisEvent* e)
{
#ifdef _DEBUG
    stringstream str;
    str << "NI " << address << " handle_ready_event " << e->vc ;
    timed_cout( str.str());
#endif

    ready[0] = true;

    if( Simulator::Now() < max_sim_time )
    {
        IrisEvent* event = new IrisEvent();
        event->type = OUT_PULL_EVENT;
        event->vc = 0; //e->vc;
        Simulator::Schedule(Simulator::Now()+1, &NetworkComponent::process_event, this, event);
        sending = true;
    }
    delete e;
}


void 
NI::handle_old_packet_event(IrisEvent* e)
{
#ifdef _DEBUG
    stringstream str;
    str << "NI " << address << " handle_old_packet_event " << e->vc ;
    timed_cout( str.str());
#endif
    
    if (!((MC*)mc)->reqH->oneBufferFull)
    {
	HighLevelPacket* hlp = static_cast<HighLevelPacket*>(e->event_data.at(0));
	Request* req = new Request();
	convertFromBitStream(req, hlp);
    	req->arrivalTime = hlp->recv_time; 
	IrisEvent *e2 = new IrisEvent();
	e2->src = this;
	e2->dst = (Component*)((MC*)mc)->reqH;
    	e2->event_data.push_back((void*)req);
    	e2->type = START;	
    	Simulator::Schedule(Simulator::Now()+1, &RequestHandler::process_event, (RequestHandler*)e2->dst, e2);

    IrisEvent* event = new IrisEvent();
    event->type = READY_EVENT;
    VirtualChannelDescription* vc = new VirtualChannelDescription();
    event->vc = 0;
    event->event_data.push_back(vc);
    Simulator::Schedule(Simulator::Now()+1, &NetworkComponent::process_event, interface_connections[0], event);
	    delete hlp;
    }
    else
    {
	IrisEvent* event2 = new IrisEvent();
        event2->type = OLD_PACKET_EVENT;
	event2->event_data.push_back(e->event_data.at(0));
        Simulator::Schedule(Simulator::Now()+1, &NetworkComponent::process_event, this, event2);
    }
	
    // Send ready event back 
    delete e;
}

string 
NI::toString() const
{
    stringstream str;
    str << "NI"
        << "\t addr: " << address
        << "\tOutput File= " << out_filename;
    return str.str();
}

bool
NI::GetFromNIQueue(Request* req)
{
//    vector<Request>::iterator queueIndex = niQueue.begin();
    if (!niQueue.empty())
    {	
	*req = niQueue[0];
//        niQueue.erase(queueIndex);
	return true;
    }
    else
	return false;
}

void 
NI::convertToBitStream(Request* req, HighLevelPacket *hlp)
{
    for ( uint i=0 ; i < NETWORK_ADDRESS_BITS ; i++ )
    {
	bool bit = (bool)((req->address >> i) & 0x1); 
        hlp->data.push_back(bit);
    }
/*    for ( uint i=0 ; i < NETWORK_COMMAND_BITS ; i++ )
    {
	bool bit = (bool)((req->cmdType >> i) & 0x1); 
        hlp->data.push_back(bit);
    }
    for ( uint i=0 ; i < NETWORK_THREADID_BITS ; i++ )
    {
	bool bit = (bool)((req->threadId >> i) & 0x1); 
        hlp->data.push_back(bit);
    }
*/    if (req->cmdType == CACHE_READ || req->cmdType == CACHE_WRITE || req->cmdType == CACHE_PREFETCH)
	for ( uint i=0 ; i < 8*CACHE_BLOCK_SIZE ; i++ )
	{
	    bool bit = false;	// sending 0's as data 
            hlp->data.push_back(bit);
        }
	        
    hlp->data_payload_length = ceil(hlp->data.size() *1.0 / max_phy_link_bits);
    hlp->data_payload_length = hlp->data_payload_length * max_phy_link_bits;
	
    for ( uint i=hlp->data.size(); i < hlp->data_payload_length; i++ )
    {
	bool bit = false; 
        hlp->data.push_back(bit);
    } 
}

void 
NI::convertFromBitStream(Request* req, HighLevelPacket *hlp)
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
    req->data.value = 0;
    if (req->cmdType == CACHE_WRITEBACK)
	for ( uint i=0 ; i < 8*WRITEBACK_SIZE ; i++ )
	{
	    req->data.value = req->data.value | (hlp->data[i+NETWORK_ADDRESS_BITS+NETWORK_COMMAND_BITS+NETWORK_THREADID_BITS] << i);
	    req->data.size = WRITEBACK_SIZE;
        }
}

string
NI::print_stats() const
{
    stringstream str;
    ((MC*)mc)->stats->CalculateAggregateStats();
    str << ((MC*)mc)->stats->PrintAggregateStats();
    return str.str();
}

#endif   /* ----- #ifndef _NI_cc_INC  ----- */
