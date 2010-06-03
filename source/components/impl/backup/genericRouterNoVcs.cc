/*
 * =====================================================================================
 *
 *       Filename:  genericRouterNoVcs.cc
 *
 *    Description: Implementing a generic 4 stage router model
 *    BW->RC->VCA->CC->ST->LT
 *    Buffer write(BW)
 *    Route Computation (RC)
 *    Virtual Channel Allocation (VCA)
 *    Configure Crossbar (CC)
 *    Switch Traversal (ST)
 *    Link Traversal (LT)
 *
 *    Model Description in cycles:
 *    ---------------------------
 *    BW and RC stages happen in the same cycle ( BW
 *    happens into the Address decoders input buffer )
 *    VCA: Req for VC allocation based on credits in the input buffer. Req for
 *    port arbitration of the selected winner out port is also done here.
 *    CC: Pick the winner for port arbitration and configure the crossbar
 *    ST: Move the flits across the crossbar and push it out on the link
 *    LT: is not modelled in this stage and is part of the link component.
 *
 *        Version:  1.0
 *        Created:  03/11/2010 09:20:54 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Mitchelle Rasquinha (), mitchelle.rasquinha@gatech.edu
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */

#ifndef  _genericRouterNoVcs_cc_INC
#define  _genericRouterNoVcs_cc_INC

#include	"genericRouterNoVcs.h"
using namespace std;

MessageState::MessageState()
{
    pipe_stage = INVALID;
}

GenericRouterNoVcs::GenericRouterNoVcs ()
{
    name = "Router" ;
    ticking = false;
}  /* -----  end of method GenericRouterNoVcs::GenericRouterNoVcs  (constructor)  ----- */

GenericRouterNoVcs::~GenericRouterNoVcs()
{
}

void
GenericRouterNoVcs::init (uint p, uint v, uint cr, uint bs)
{
    ports =p;
    vcs =v;
    credits =cr;
    buffer_size = bs;

    address = myId();
    _DBG(" my node ip %d", node_ip);
    /*  set_input_ports(ports); */
    in_buffers.resize(ports);
    decoders.resize(ports);
    mstate.resize(ports*vcs);
    swa.resize(ports,vcs);
    downstream_credits.resize(ports);

    /* set_output_ports(ports); */
    xbar.set_input_ports(ports);
    xbar.set_output_ports(ports);
    xbar.set_no_virtual_channels(vcs);

    /* All decoders and vc arbiters need to know the node_ip for routing */
    for(uint i=0; i<ports; i++)
    {
        decoders[i].node_ip = node_ip;
        decoders[i].address = address;
    }
    /*  set_no_virtual_channels(vcs); */
    for(uint i=0; i<ports; i++)
    {
        downstream_credits[i].resize(vcs);
        in_buffers[i].set_no_vcs( vcs );
        decoders[i].resize( vcs );
    }

    /* Set credits and buffer sizes */
    for(uint i=0; i<ports; i++)
    {
        in_buffers[i].buffer_size = buffer_size;
    }

    for(uint i=0; i<ports; i++)
        for(uint j=0; j<vcs; j++)
            downstream_credits[i][j] = credits;

    /* init the countes */
    packets = 0;
    flits = 0;
    total_packet_latency = 0;

    return ;
}		/* -----  end of function GenericRouterNoVcs::init  ----- */

/* These functions are mainly for DOR routing and are seperated so as to not
 * force DOR modelling in all designs */
void
GenericRouterNoVcs::set_no_nodes( uint nodes )
{
    for ( uint i=0; i<decoders.size(); i++)
    {
        decoders[i].grid_xloc.resize(nodes);
        decoders[i].grid_yloc.resize(nodes);
    }
}

void
GenericRouterNoVcs::set_grid_x_location( uint port, uint x_node, uint value)
{
    decoders[port].grid_xloc[x_node]= value;
}

void
GenericRouterNoVcs::set_grid_y_location( uint port, uint y_node, uint value)
{
    decoders[port].grid_yloc[y_node]= value;
}

/*  End of DOR grid location functions */

void
GenericRouterNoVcs::process_event ( IrisEvent* e )
{
    switch(e->type)
    {
        case LINK_ARRIVAL_EVENT:
            handle_link_arrival_event(e);
            break;
        case TICK_EVENT:
            handle_tick_event(e);
            break;
        default:
            _DBG("GenericRouterNoVcs:: Unk event exception %d", e->type);
            break;
    }
    return ;
}		/* -----  end of function GenericRouterNoVcs::process_event  ----- */

string
GenericRouterNoVcs::print_stats()
{
    stringstream str;
    str << "\n router[" << node_ip << "] packets: " << packets
        << "\n router[" << node_ip << "] flits: " << flits
        << "\n router[" << node_ip << "] packet latency: " << total_packet_latency
        << "\n router[" << node_ip << "] flits/packets: " << (flits+0.0)/(packets)
        << "\n router[" << node_ip << "] average packet latency: " << total_packet_latency/packets
        << "\n router[" << node_ip << "] last_flit_out_cycle: " << last_flit_out_cycle
        << " ";

    return str.str();
}

void
GenericRouterNoVcs::handle_link_arrival_event ( IrisEvent* e )
{
    cout << endl << " handle_link_arrival_event Router";
    LinkArrivalData* data = static_cast<LinkArrivalData*>(e->event_data.at(0));
    if(data->type == FLIT_ID)
    {
        /*  Update stats */
        flits++;
        if( data->ptr->type == TAIL )
        {
            packets++;
        }

//        data->valid = false;

        /*Find the port the flit came in on */
        bool found = false;
        uint port = -1;
        for ( uint i=0 ; i< ports ; i++ )
            if(static_cast<GenericLink*>(input_connections[i])->input_connection) /*  This is here because we may assign null links to corner routers */
            if( e->src_id == static_cast<GenericLink*>(input_connections[i])->input_connection->address)
            {
                found = true;
                port = i;
                break;
            }

        /* Throw and exception if it was not found */
        if( !found )
        {
            _DBG(" Input port not found src_addr: %d", e->src_id);
        }

        /* Push the flit into the buffer */
        in_buffers[port].change_push_channel(data->vc);
        in_buffers[port].push(data->ptr);
        decoders[port].push(data->ptr,data->vc);
        if( data->ptr->type == HEAD )
        {
            HeadFlit* hf = static_cast<HeadFlit*>(data->ptr);
            mstate[port*vcs+data->vc].input_port = port;
            mstate[port*vcs+data->vc].input_channel = data->vc;
            mstate[port*vcs+data->vc].pipe_stage = FULL;
            mstate[port*vcs+data->vc].output_port = decoders[port].get_output_port(data->vc);
            mstate[port*vcs+data->vc].output_channel = data->vc;
            mstate[port*vcs+data->vc].length= hf->length;
            mstate[port*vcs+data->vc].credits_sent= hf->length;
            mstate[port*vcs+data->vc].arrival_time= Simulator::Now();
            mstate[port*vcs+data->vc].clear_message= false;
            mstate[port*vcs+data->vc].flits_in_ib = 0;

        }
        else
        {
#ifdef _DEBUG_ROUTER
            _DBG(" BW inport:%d invc:%d oport:%d ovc:%d ft:%d", port, data->vc,
                 mstate[port*vcs+data->vc].output_port, mstate[port*vcs+data->vc].output_channel, data->ptr->type);
#endif

        }
        
    }
    else if ( data->type == CREDIT_ID)
    {
        /* Find the corresponding output port */
        bool found = false;
        uint port = -1;
        for ( uint i=0 ; ports ; i++ )
            if(static_cast<GenericLink*>(output_connections[i])->output_connection) /* Some links of corner routers may be null */
            if( static_cast<GenericLink*>(output_connections[i])->output_connection->address == e->src_id)
            {
                port = i;
                found = true;
                break;
            }
        if(!found)
        {
            _DBG(" Output port not found src_addr: %d", e->src_id);
        }

        downstream_credits[port][data->vc]++;
#ifdef _DEBUG_ROUTER
        _DBG(" Got a credit port:%d vc:%d in_buffers[port][vc]:%d ", port, data->vc, downstream_credits[port][data->vc]);
#endif

    }
    else
    {
        _DBG( "handle_link_arrival_event Unk data type %d ", data->type);
    }

        /* Tick since you update a credit or flit */
        if(!ticking)
        {
            ticking = true;
            IrisEvent* event = new IrisEvent();
            event->type = TICK_EVENT;
            event->vc = e->vc;
            Simulator::Schedule( floor(Simulator::Now())+1, &NetworkComponent::process_event, this, event);
        }

    return ;
}		/* -----  end of function GenericRouterNoVcs::handle_link_arrival_event  ----- */

void
GenericRouterNoVcs::handle_tick_event ( IrisEvent* e )
{

#ifdef _DEEP_DEBUG
    _DBG_NOARG("Mstate matrix\n");
    for( uint i=0; i<ports*vcs; i++)
        cout <<i<<" "<< mstate[i].toString()<< " buff_occ:" <<in_buffers[mstate[i].
            input_port].get_occupancy(mstate[i].input_channel) << endl;
#endif

    ticking = false;

    /* We need to estimate the actual time at which the input buffer empties
     * and send the credit back. Assuming we have set a message to win
     * arbitration and in ST right now we can check the occupancy and send a
     * credit back 
    for( uint i=0; i<(ports*vcs); i++)
        if( mstate[i].pipe_stage == ST || mstate[i].pipe_stage == SW_ALLOCATED)
        {
            if(in_buffers[mstate[i].input_port].get_occupancy(mstate[i].input_channel) >0 
               && mstate[i].credits_sent>0 )
                send_credit_back(i);
        }
     * */

    /* Switch traversal */
    for( uint i=0; i<ports*vcs; i++)
        if( mstate[i].pipe_stage == ST)
        {
                uint oport = mstate[i].output_port;
                uint och = mstate[i].output_channel;
                uint iport = mstate[i].input_port;
                uint ich = mstate[i].input_channel;
                if( !xbar.is_empty(oport,och) 
                    && mstate[i].flits_in_ib > 0
                    && downstream_credits[oport][och]>0 )
                {
                    in_buffers[iport].change_pull_channel(ich);
                    Flit* f = in_buffers[iport].pull();
                    mstate[i].flits_in_ib--;
                    
                    /*  Update stats */
                    if( f->type == HEAD)
                    {
                        uint lat = Simulator::Now() - mstate[i].arrival_time;
                        total_packet_latency+= lat;
                    }
                    last_flit_out_cycle = Simulator::Now();

                    LinkArrivalData* data = new LinkArrivalData();
                    data->type = FLIT_ID;
                    data->vc = och;
                    data->ptr = f;
#ifdef _DEEP_DEBUG
                    _DBG(" FLIT OUT ftype: %d ", f->type);
                    switch( f->type)
                    {
                        case HEAD:
                            cout << static_cast<HeadFlit*>(f)->toString();
                            break;
                        case BODY:
                            cout << static_cast<BodyFlit*>(f)->toString();
                            break;
                        case TAIL:
                            cout << static_cast<TailFlit*>(f)->toString();
                            break;
                    }
#endif
                    IrisEvent* event = new IrisEvent();
                    event->type = LINK_ARRIVAL_EVENT;
                    event->event_data.push_back(data);
                    event->src_id = address;
                    event->dst_id = output_connections[oport]->address;
                    event->vc = data->vc;
                    Simulator::Schedule( floor(Simulator::Now())+1,
                                     &NetworkComponent::process_event,
                                     output_connections[oport],event);
                    downstream_credits[oport][och]--;
                    if( f->type == TAIL)
                    {
                        mstate[i].clear_message = true;
                        mstate[i].pipe_stage = EMPTY;
                        swa.clear_winner(mstate[i].output_port,mstate[i].output_channel
                             , mstate[i].input_port, mstate[i].input_channel);
                        xbar.pull(mstate[i].output_port,mstate[i].output_channel);
#ifdef _DEBUG_ROUTER
    _DBG(" Tail FO clear pkt for inport %d inch %d oport %d och %d ", iport, ich, oport, och);
#endif
                    }
                    else
                    {
#ifdef _DEBUG_ROUTER
    _DBG(" Flit out for inport %d inch %d oport %d och %d fty: %d ", iport, ich, oport, och, f->type);
#endif
                    }

                    send_credit_back(i);

                }
                else
                {
                    if(in_buffers[iport].get_occupancy(ich)==0)
                    {
                        _DBG_NOARG(" Waiting for flits on inch");
                        ticking = true;
                    }
                    else if (xbar.is_empty(oport,och))
                    {
                        _DBG_NOARG(" switch was allocated but crossbar is empty ");
                    }
                    else if (downstream_credits[oport][och] == 0)
                    {
                        _DBG(" Waiting for downstream credits on oport %d och %d ", oport, och);
                        ticking = true;
                    }
                }

        }


    /* Switch Allocation */
    for( uint i=0; i<ports*vcs; i++)
        if( mstate[i].pipe_stage == SWA_REQUESTED)
        {
            if ( !swa.is_empty())
            {
                uint oport = mstate[i].output_port;
                uint och = mstate[i].output_channel;
                uint iport = mstate[i].input_port;
                uint ich = mstate[i].input_channel;

                VCA_unit vca_winner = swa.pick_winner(oport,och);

                if( vca_winner.inport == iport && vca_winner.inch == och)
                {
                        mstate[i].pipe_stage = ST;
                        xbar.configure_crossbar(iport,oport,och);
                        xbar.push(oport,och);

                _DBG(" SWA won for inport %d inch %d oport %d och %d ", iport, ich, oport, och);
                    /* After allocating the downstream path for this message
                     * send a credit on this inport and this inch for length
                     * number of cycles. Sending them in the ST stage for now */
                }
                else
                {
#ifdef _DEBUG_ROUTER
                    _DBG(" Dint win for inport %d inch %d SWA winner was inport %d inch %d  for oport %d och %d "
                         ,iport, ich, vca_winner.inport, vca_winner.inch, oport, och);
#endif
                }
                ticking = true;
            }
            else
            {
                cout << "ERROR SWA was requested but the switch allocator has no requests" << endl;
            }
        }


    /* Route and Request Switch Allocation */
    for( uint i=0; i<(ports*vcs); i++)
    {
        if( mstate[i].pipe_stage == IB )
        {
            uint iport = mstate[i].input_port;
            uint ich = mstate[i].input_channel;
            uint oport = mstate[i].output_port ;
            uint och = mstate[i].output_channel; 
            //mstate[i].pipe_stage = ROUTED;
            if( !swa.is_requested(oport, och, iport,ich) 
                && downstream_credits[mstate[i].output_port][mstate[i].output_channel] > 0)
            {
                swa.request(oport,och, iport,ich);
                mstate[i].pipe_stage = SWA_REQUESTED;
                ticking = true;
#ifdef _DEBUG_ROUTER
                _DBG(" Req SWA for inport %d inch %d oport %d och %d ", iport, ich,oport,och);
#endif
            }
#ifdef _DEBUG_ROUTER
            else
            {
                if (swa.is_requested(oport, och, iport,ich))
                {
                    _DBG("Cant req SWA. SWA full inport:%d inch:%d oport:%d och:%d",iport,ich,oport,och);
                }
                else if(downstream_credits[mstate[i].output_port][mstate[i].output_channel] == 0)
                {
                    _DBG("Not requesting SWA. No downstream credits for oport:%d och:%d",oport,och);
                }
            }
#endif
            ticking = true;
        }
    }
   
    /*  Input buffering 
     *  */
    for( uint i=0; i<(ports*vcs); i++)
        if( mstate[i].pipe_stage == FULL )
    {
#ifdef _DEBUG_ROUTER
        _DBG(" IB + RC inport:%d invc:%d oport:%d ovc:%d length:%d", mstate[i].input_port, mstate[i].input_channel,
              mstate[i].output_port, mstate[i].output_channel,mstate[i].length);
#endif
            mstate[i].pipe_stage = IB;
            ticking = true;
    }

    for( uint i=0; i<(ports*vcs); i++)
    {
            uint iport = mstate[i].input_port;
            uint ich = mstate[i].input_channel;
            uint oport = mstate[i].output_port ;
            uint och = mstate[i].output_channel; 
            if ((mstate[i].pipe_stage == ST || mstate[i].pipe_stage == IB
			|| mstate[i].pipe_stage == SWA_REQUESTED )
                && (mstate[i].flits_in_ib < in_buffers[iport].get_occupancy(ich)))
                { 
                    mstate[i].flits_in_ib++;
                    ticking = true;
        //_DBG(" Updating flits for IB %d %d flits_in_ib %d occ:%d", mstate[i].input_port, mstate[i].input_channel,mstate[i].flits_in_ib, in_buffers[iport].get_occupancy(ich));
                }
    }

    if(ticking)
    {
        ticking = true;
        IrisEvent* event = new IrisEvent();
        event->type = TICK_EVENT;
        event->vc = e->vc;
        Simulator::Schedule(floor(Simulator::Now())+1, &NetworkComponent::process_event, this, event);
    }

    return;

}		/* -----  end of function GenericRouterNoVcs::handle_input_arbitration_event  ----- */

string
GenericRouterNoVcs::toString() const
{
    stringstream str;
    str << "GenericRouterNoVcs"
        << "\t addr: " << address
        << " node_ip: " << node_ip
        << "\n Input buffers: " << in_buffers.size() << " ";
    if( in_buffers.size() > 0)
        str << in_buffers[0].toString();

    str << "\n SWA: " << swa.toString();

    str << "\n decoders: " << decoders.size() << " ";
    if( decoders.size() > 0)
        str << decoders[0].toString();

    str << "\n Xbar ";
    str << xbar.toString();

    return str.str();
}

string
MessageState::toString() const
{
    stringstream str;
    str << "mstate"
        << " inport: " << input_port
        << " inch: " << input_channel
        << " outport: " << output_port
        << " outch: " << output_channel;
    switch( pipe_stage)
    {
        case INVALID:
            str << " INVALID";
            break;
        case IB:
            str << " IB";
            break;
        case EMPTY:
            str << " EMPTY";
            break;
        case FULL:
            str << " FULL";
            break;
        case ROUTED:
            str << " ROUTED";
            break;
        case SWA_REQUESTED:
            str << " SWA_REQUESTED";
            break;
        case SW_ALLOCATED:
            str << " SW_ALLOCATED";
            break;
        case ST:
            str << " ST";
            break;
        case REQ_OUTVC_ARB:
            str << " REQ_OUTVC_ARB";
            break;
    }

    return str.str();
}

void
GenericRouterNoVcs::send_credit_back(uint i)
{
    mstate[i].credits_sent--;
    LinkArrivalData* data = new LinkArrivalData();
    VirtualChannelDescription* cr = new VirtualChannelDescription();
    cr->port = mstate[i].input_port;
    cr->vc = mstate[i].input_channel;
    data->type = CREDIT_ID;
    data->vc = mstate[i].input_channel;
    IrisEvent* event = new IrisEvent();
    event->type = LINK_ARRIVAL_EVENT;
    event->event_data.push_back(data);
    event->src_id = address;
    event->vc = cr->vc; 
    Simulator::Schedule(Simulator::Now()+1, &NetworkComponent::process_event,input_connections[cr->port], event);
#ifdef _DEBUG_ROUTER
                    _DBG(" Credit back for inport %d inch %d ", cr->port, cr->vc);
#endif
}


#endif   /* ----- #ifndef _genericRouterNoVcs_cc_INC  ----- */

