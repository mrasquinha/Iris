#ifndef _generictpg_cc_INC
#define _generictpg_cc_INC

#include        "genericTPG.h"
#define USE_MSHR


GenericTPG::GenericTPG ()
{
    name = "GenericTPG";
    sending = false;
    /* Do this in setup with the address or node_ip appended */
    out_filename = "traceOut.tr";
    last_vc = 0;
    interface_connections.resize(1);	
} /* ----- end of function GenericTPG::GenericTPG ----- */

GenericTPG::~GenericTPG ()
{
    out_file.close();
    mshrHandler->trace_filename.close();
    mshrHandler->mshr.clear();
    delete mshrHandler;

} /* ----- end of function GenericTPG::~GenericTPG ----- */

void
GenericTPG::set_trace_filename( string filename )
{
    trace_name = filename;
    cout<<"TraceName: "<<trace_name<<endl;
}

void
GenericTPG::set_no_vcs ( uint v)
{
    vcs = v;
}

void
GenericTPG::setup (uint n, uint v, uint time)
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


//    InitMSHR();
#ifdef USE_MSHR    
    mshrHandler = new MSHR_H();	
    mshrHandler->id = node_ip;
    mshrHandler->parent = this; 
 //   mc->stats->doneOnce[i] = &mshrHandler[i].done; 
    cout << "mshr trace is: " << trace_name << endl;
    trace_filename = &mshrHandler->trace_filename; 
    mshrHandler->filename = const_cast<char*>(trace_name.c_str());	
#endif    

    trace_filename->open(trace_name.c_str(),ios::in);
    if(!trace_filename->is_open())
    {
        cout << "Err opening trace " << endl;
        exit(1);
    }

#ifdef USE_MSHR
    ull_int addr;
    uint thread_id;
    ull_int time2;
    uint cmd;	
    if (!trace_filename->eof())
    {
	(*trace_filename) >> hex >> addr;
	(*trace_filename) >> thread_id;	
	(*trace_filename) >> dec >> time2;
	(*trace_filename) >> dec >> cmd;
	
	Request *req2 = new Request();
	req2->cmdType = (Command_t)cmd;
	req2->address = addr;
	req2->arrivalull_int = time2;
	req2->threadId = node_ip;
	req2->address = mshrHandler->GlobalAddrMap(req2->address,req2->threadId);

	IrisEvent *event = new IrisEvent();
    	event->src = this;
    	event->dst = (Component*)(mshrHandler);
    	event->event_data.push_back(req2);	
#ifdef DEEP_DEBUG
    	cout << dec << Simulator::Now() << ": " << hex << req2->address << ": First Request of each trace to be send to mshrs" << endl;	
#endif
    	Simulator::Schedule(time2, &MSHR_H::process_event, (MSHR_H*)event->dst, event);   
    }
    /* send ready events for each virtual channel*/
#endif

    for( unsigned int i = 0; i < vcs; i++ )
    {
        IrisEvent* event = new IrisEvent();
        event->type = READY_EVENT;
        event->vc = 0;
        Simulator::Schedule(Simulator::Now()+1, &NetworkComponent::process_event,this, event);
// interface_connections[0], event);
    }


    return ;
} /* ----- end of function GenericTPG::setup ----- */

void
GenericTPG::set_output_path( string name)
{
    // open the output trace file
    stringstream str;
    str << name << "/tpg_" << node_ip << "_trace_out.tr";
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
GenericTPG::finish ()
{
    out_file.close();
    trace_filename->close();
    return ;
} /* ----- end of function GenericTPG::finish ----- */

void
GenericTPG::process_event (IrisEvent* e)
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
} /* ----- end of function GenericTPG::process_event ----- */

void
GenericTPG::handle_new_packet_event ( IrisEvent* e)
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

#ifdef USE_MSHR
    Request * req = new Request();
    convertFromBitStream(req,hlp);		
    IrisEvent *event = new IrisEvent();
    event->src = (Component*)this;
    event->dst = (Component*)mshrHandler;
    event->type = MSHR_DELETE;	
    event->event_data.push_back(req);	
#ifdef _DEEP_DEBUG
    	cout << dec << Simulator::Now() << ": " << hex << req->address << ": Sending to MSHR for Delete" << endl;	
#endif
    Simulator::Schedule(Simulator::Now()+1, &MSHR_H::process_event, (MSHR_H*)event->dst, event);  	
#endif

    delete hlp;

    // send back a ready event
    IrisEvent* event2 = new IrisEvent();
    event2->type = READY_EVENT;
    event2->vc = 0; //hlp->virtual_channel;
    Simulator::Schedule( Simulator::Now()+1, &NetworkComponent::process_event, interface_connections[0], event2);

    delete e;
    return ;
} /* ----- end of function GenericTPG::handle_new_packet_event ----- */

void
GenericTPG::handle_out_pull_event ( IrisEvent* e )
{
#ifdef USE_MSHR
    Request* next_req = GetRequest();
#else
    Request* next_req = GetNextRequest();
#endif
    MTRand mtrand1;
    if( ready[0] && next_req)
    {
        packets++;
        HighLevelPacket* hlp = new HighLevelPacket();
        hlp->virtual_channel = 0;
        hlp->source = address;
        uint rand_destination = mtrand1.randInt(no_mcs-1); //MIN( generator.address(), MAX_ADDSS);
        ull_int tempAddr = next_req->address;
	uint destination=map_addr(&next_req->address);
        mshrHandler->demap_addr(tempAddr,next_req->address);
	hlp->destination = mc_node_ip[rand_destination];

        assert ( hlp->destination < no_nodes);
#ifdef DEEP_DEBUG
        cout << endl << dec << node_ip << " ^^Sending to " << hlp->destination << endl;
#endif
        hlp->transaction_id = mtrand1.randInt(1000);
        hlp->msg_class = ONE_FLIT_REQ;

	convertToBitStream(next_req, hlp);
        if(hlp->sent_time < Simulator::Now())
            hlp->sent_time = Simulator::Now();

#ifdef _DEEP_DEBUG
        _DBG( " Sending pkt: no of packets %d ", packets );
        cout << "HLP: " << hlp->toString() << endl;
#endif
        ready[0] = false;
        IrisEvent* event = new IrisEvent();
        event->type = NEW_PACKET_EVENT;
        event->event_data.push_back(hlp);
        Simulator::Schedule( hlp->sent_time, &NetworkComponent::process_event, interface_connections[0], event );
        sending = true;
        delete next_req;
                
    }
    else if (!ready[0] && next_req)
    {
        IrisEvent* event2 = new IrisEvent();
        event2->type = OUT_PULL_EVENT;
        event2->vc = 0; //e->vc;
        Simulator::Schedule(Simulator::Now()+1, &NetworkComponent::process_event, this, event2);
        sending = true;
        delete next_req;
    }		
    else
        sending = false;
   delete e; 
}

void
GenericTPG::handle_ready_event ( IrisEvent* e)
{

    // send the next packet if it is less than the current time
    ready[0] = true;
    _DBG_NOARG(" handle_ready_event ");
#ifdef _DEBUG_TPG
#endif

    if( Simulator::Now() < max_sim_time )
    {
        IrisEvent* event = new IrisEvent();
        event->type = OUT_PULL_EVENT;
        event->vc = 0; //e->vc;
        Simulator::Schedule(Simulator::Now()+1, &NetworkComponent::process_event, this, event);
        sending = true;
    }

    delete e;
    return ;
} /* ----- end of function GenericTPG::handle_ready_event ----- */
Request*
GenericTPG::GetNextRequest()
{
    ull_int addr =0;
    uint thread_id = 0;
    ull_int time = 0;
    uint cmd;

    if (trace_filename->eof())
       return NULL;

    else
    {
	Request* tempRequest = new Request();
     	(*trace_filename) >> hex >> addr;
     	(*trace_filename) >> thread_id;
     	(*trace_filename) >>dec>> time;
     	(*trace_filename) >>dec>> cmd;

     	tempRequest->cmdType = (Command_t)cmd;
     	tempRequest->address = addr;
     	tempRequest->arrivalull_int = time;	
     	tempRequest->threadId = node_ip;
     	tempRequest->address = mshrHandler->GlobalAddrMap(tempRequest->address,tempRequest->threadId);
#ifdef _DEEP_DEBUG
        cout << "\n Created Req: "
            << "\t cmd: " << cmd
            << "\t thr_id: " << thread_id
            << "\t time: " << time
            << "\t addr: " << addr
            << endl;
#endif

	return tempRequest;
    }
    
}

bool
GenericTPG::GetNewRequest(Request *req)
{
    if (!mshrHandler->writeQueue.empty())
    {
        if (mshrHandler->writeQueue[0].arrivalull_int <= Simulator::Now())
        {
            vector<Request>::iterator bufferIndex = mshrHandler->writeQueue.begin();
            *req = mshrHandler->writeQueue[0];
            mshrHandler->writeQueue.erase(bufferIndex);
            return true;
        }
    }
    else  if (!mshrHandler->mshr.empty() && mshrHandler->lastScheduledIndex < mshrHandler->mshr.size())
    {
        if (mshrHandler->mshr[mshrHandler->lastScheduledIndex].arrivalull_int <= Simulator::Now())
        {
            *req = mshrHandler->mshr[mshrHandler->lastScheduledIndex];
            mshrHandler->lastScheduledIndex++;
            return true;
        }
    }
    return false;
}

Request*
GenericTPG::GetRequest()
{
    Request* req = new Request();
    if (GetNewRequest(req))
    {
        return req;
    }
    delete req;
    return NULL;
}

void
GenericTPG::convertToBitStream(Request* req, HighLevelPacket* hlp)
{
    for ( uint i=0 ; i < NETWORK_ADDRESS_BITS ; i++ )
    {
	bool bit = (bool)((req->address >> i) & 0x1);
        hlp->data.push_back(bit);
    }
    for ( uint i=0 ; i < NETWORK_COMMAND_BITS ; i++ )
    {
	bool bit = (bool)((req->cmdType >> i) & 0x1);
        hlp->data.push_back(bit);
    }
    for ( uint i=0 ; i < NETWORK_THREADID_BITS ; i++ )
    {
	bool bit = (bool)((req->threadId >> i) & 0x1);
        hlp->data.push_back(bit);
    }
    if (req->cmdType == CACHE_WRITEBACK)
	for ( uint i=0 ; i < 8*CACHE_BLOCK_SIZE; i++ )
	{
	    bool bit = true; // sending 1's as data
            hlp->data.push_back(bit);
        }

    hlp->data_payload_length = ceil(hlp->data.size() *1.0 / max_phy_link_bits);	
    hlp->data_payload_length = hlp->data_payload_length * max_phy_link_bits;
    hlp->sent_time = req->arrivalull_int;
    /* 
    for ( uint i=hlp->data.size() ; i < 3*hlp->data_payload_length; i++ )
    {
	bool bit = false;
        hlp->data.push_back(bit);
    }
    hlp->data_payload_length = hlp->data.size();
     * */
}

void 
GenericTPG::convertFromBitStream(Request* req, HighLevelPacket *hlp)
{
    req->address = 0;	
    for (unsigned int i=0; i < NETWORK_ADDRESS_BITS; i++)
    {
	req->address = req->address | (hlp->data[i] << i);
    }
/*    unsigned int temp = 0;
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
*/    req->data.value = 0;
    if (req->cmdType == CACHE_READ || req->cmdType == CACHE_WRITE || req->cmdType == CACHE_PREFETCH)
	for ( uint i=0 ; i < 8*CACHE_BLOCK_SIZE ; i++ )
	{
	    req->data.value = req->data.value | (hlp->data[i+NETWORK_ADDRESS_BITS/*+NETWORK_COMMAND_BITS+NETWORK_THREADID_BITS*/] << i);
	    req->data.size = CACHE_BLOCK_SIZE;
        }
}


short int 
GenericTPG::map_addr(unsigned long long int *addr)
{
    unsigned int temp = MC_ADDR_BITS;   
    unsigned int temp2 = temp-(int)log2(NUM_OF_CONTROLLERS);   
    unsigned int lower_mask = pow(2.0,temp2*1.0)-1;
    unsigned long long int upper_mask = (0xFFFFFFFF)-(pow(2.0,temp*1.0)-1);
    unsigned int lower_addr = (*addr) & lower_mask;
    unsigned long long int upper_addr = ((*addr) & upper_mask) >> (int)log2(NUM_OF_CONTROLLERS);
    short int mc_addr = ((*addr) >> temp2) & (NUM_OF_CONTROLLERS-1);
    //cout << hex << temp2 << " " << (*addr) << " " << upper_addr << " " << lower_addr << " ";
    *addr = upper_addr | lower_addr;
    //cout << (*addr) << " " << mc_addr << endl;     
    return mc_addr;
}




string
GenericTPG::toString () const
{
    stringstream str;
    str << "\nGenericTPG: "
        << "\t trace: " << trace_name
        << "\t vcs: " << ready.size()
        << "\t address: " <<address
        << "\t node_ip: " << node_ip
        ;
    return str.str();
} /* ----- end of function GenericTPG::toString ----- */

string
GenericTPG::print_stats() const
{
    stringstream str;
    str << toString()
        << "\n packets:\t " << packets
        << "\n min_pkt_latency:\t" << min_pkt_latency
        ;
    return str.str();
} /* ----- end of function GenericTPG::toString ----- */

#endif /* ----- #ifndef _generictpg_cc_INC ----- */
