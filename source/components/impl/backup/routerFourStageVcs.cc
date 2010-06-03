/*
 * =====================================================================================
 *
 *       Filename:  router4StageVcs.cc
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

#ifndef  _router4stagevcs_cc_INC
#define  _router4stagevcs_cc_INC

#include	"routerFourStageVcs.h"
using namespace std;


/*
 *--------------------------------------------------------------------------------------
 *       Class:  RouterFourStageVcs
 *      Method:  RouterFourStageVcs
 * Description:  constructor
 *--------------------------------------------------------------------------------------
 */
RouterFourStageVcs::RouterFourStageVcs ()
{
    name = "Router" ;
    ticking = false;
}  /* -----  end of method RouterFourStageVcs::RouterFourStageVcs  (constructor)  ----- */

RouterFourStageVcs::~RouterFourStageVcs()
{
}

void
RouterFourStageVcs::set_no_ports( uint p )
{
    ports = p;
}

void
RouterFourStageVcs::set_no_vcs( uint v )
{
    vcs = v;
}

void
RouterFourStageVcs::set_no_credits( int c)
{
    credits = c;
}

void
RouterFourStageVcs::set_buffer_size( uint b)
{
    buffer_size = b;
}
void
RouterFourStageVcs::init ()
{
    address = myId();
    /*  set_input_ports(ports); */
    in_buffers.resize(ports);
    in_arbiters.resize(ports);
    decoders.resize(ports);
    xbar.set_input_ports(ports);
    port_arbiters.resize(ports);

    /* set_output_ports(ports); */
    xbar.set_output_ports(ports);

    /* All decoders and vc arbiters need to know the node_ip for routing */
    for(uint i=0; i<ports; i++)
    {
        decoders[i].node_ip = node_ip;
        decoders[i].address = address;
        in_arbiters[i].node_ip = node_ip;
        in_arbiters[i].address = address;
    }
    /*  set_no_virtual_channels(vcs); */
    for(uint i=0; i<ports; i++)
    {
        in_buffers[i].set_no_vcs( vcs );
        in_arbiters[i].set_req_queue_size( vcs );
        decoders[i].set_no_virtual_channels( vcs );
        port_arbiters[i].set_req_queue_size(ports * vcs);
    }
    xbar.set_no_virtual_channels(vcs);

    /* Set credits and buffer sizes */
    for(uint i=0; i<ports; i++)
    {
        in_buffers[i].buffer_size = buffer_size;
        in_buffers[i].set_no_credits(credits);
    }


    /* init the countes */
    packets = 0;
    flits = 0;
    total_packet_latency = 0;

    return ;
}		/* -----  end of function RouterFourStageVcs::init  ----- */

/* These functions are mainly for DOR routing and are seperated so as to not
 * force DOR modelling in all designs */
void
RouterFourStageVcs::set_no_nodes( uint nodes )
{
    for ( uint i=0; i<decoders.size(); i++)
    {
        decoders[i].grid_xloc.resize(nodes);
        decoders[i].grid_yloc.resize(nodes);
    }
}

void
RouterFourStageVcs::set_grid_x_location( uint port, uint x_node, uint value)
{
    decoders[port].grid_xloc[x_node]= value;
}

void
RouterFourStageVcs::set_grid_y_location( uint port, uint y_node, uint value)
{
    decoders[port].grid_yloc[y_node]= value;
}

/*  End of DOR grid location functions */

void
RouterFourStageVcs::process_event ( IrisEvent* e )
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
            _DBG("RouterFourStageVcs:: Unk event exception %d", e->type);
            break;
    }
    return ;
}		/* -----  end of function RouterFourStageVcs::process_event  ----- */

string
RouterFourStageVcs::print_stats()
{
    stringstream str;
    str << "\n router[" << node_ip << "] packets: " << packets
        << "\n router[" << node_ip << "] flits: " << flits
        << "\n router[" << node_ip << "] packet latency: " << total_packet_latency
        << "\n router[" << node_ip << "] flits/packets: " << (flits+0.0)/(packets)
        << "\n router[" << node_ip << "] average packet latency: " << total_packet_latency/packets
        << " ";

    return str.str();
}

void
RouterFourStageVcs::handle_link_arrival_event ( IrisEvent* e )
{
    LinkArrivalData* data = static_cast<LinkArrivalData*>(e->event_data.at(0));
    if(data->type == FLIT_ID)
    {
        /*  Update stats */
        flits++;
        if( data->ptr->type == TAIL )
        {
            packets++;
            static_cast< TailFlit* >( data->ptr )->scratch_pad_time = Simulator::Now();
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
            exit (1);
        }

        /* Push the flit into the buffer */
#ifdef _DEBUG_ROUTER
        _DBG(" Buffer write port:%d vc:%d ft: %d", port, data->vc,data->ptr->type );
#endif

        in_buffers[port].change_push_channel(data->vc);
        in_buffers[port].push(data->ptr);
        decoders[port].push(data->ptr,data->vc);
#ifdef _DEBUG_ROUTER
        _DBG (" Decoded flit and pushing into in_buffer: %d vc: %d ftype: %d ",port , data->vc, data->ptr->type);
#endif
        

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

        in_buffers[port].got_credit( data->vc);
#ifdef _DEBUG_ROUTER
        _DBG(" Got a credit port:%d vc:%d in_buffers[port][vc]:%d ", port, data->vc, in_buffers[port].get_no_credits(data->vc));
#endif

    }
    else
    {
        _DBG( "handle_link_arrival_event Unk data type %d ", data->type);
        exit(1);
    }

        /* Restart my in arbitration stage */
        if(!ticking)
        {
            IrisEvent* event = new IrisEvent();
            event->type = TICK_EVENT;
            event->vc = e->vc;
            Simulator::Schedule( Simulator::Now()+1, &NetworkComponent::process_event, this, event);
        }
    return ;
}		/* -----  end of function RouterFourStageVcs::handle_link_arrival_event  ----- */

void
RouterFourStageVcs::handle_tick_event ( IrisEvent* e )
{
    ticking = false;

    /* ST and push flit out to link */
    // move flits from the crossbar to the output buffers. Now the 'i' has moved
    // to the output side
    for ( uint i=0 ; i< ports ; i++ )
        for ( uint j=0 ; j< vcs ; j++ )
            if( !xbar.is_empty(i,j) )
            {
                Flit* f = xbar.pull(i,j);
                LinkArrivalData* data = new LinkArrivalData();
                data->type = FLIT_ID;
                data->vc = j;
                data->ptr = f;

                IrisEvent* event = new IrisEvent();
                event->type = LINK_ARRIVAL_EVENT;

                /* Update router stats */
                if(data->ptr->type == TAIL)
                {
                    uint lat = Simulator::Now() - static_cast<TailFlit*>(data->ptr)->scratch_pad_time;
                    total_packet_latency+= lat;
                }

                event->event_data.push_back(data);
                event->src_id = address;
                event->vc = data->vc;
                Simulator::Schedule( Simulator::Now()+1, &NetworkComponent::process_event, output_connections[i], event);

#ifdef _DEBUG_ROUTER
                _DBG("handle_switch_traversal_event & FLIT_OUT port: %d vc: %d ftype: %d ", i, j, f->type);
#endif
                ticking = true;

            }


    /* Pick winner of PA and CC */
    // move flits from the port arbiters to the crossbar and configure the
    // crossbar for the next stage
    for ( uint i=0 ; i<ports ; i++ )
        for ( uint j=0 ; j<vcs ; j++ )
            if( !port_arbiters[i].is_empty(j) && xbar.is_empty(i,j))
            {
                uint winner = port_arbiters[i].pick_winner(j)/vcs;
                xbar.configure_crossbar( winner, i, j);
                Flit* f = port_arbiters[i].pull_winner(j);
#ifdef _DEBUG_ROUTER
                _DBG(" handle_configure_crossbar_event fromport: %d toport: %d vc: %d ftype: %d ", winner, i, j, f->type );
#endif

                xbar.push( winner, j, f);
                ticking = true;

            }


    /*PA: Pick winner of in_arbiter and request for PA */
    vector<bool> ready;
    ready.resize(vcs);
    for ( uint i=0 ; i<ports ; i++ )
        for ( uint j=0 ; j<vcs ; j++ )
            if( !in_arbiters[i].empty() && !port_arbiters[in_arbiters[i].next_port[j]].is_requested(i,j) )
            {

                uint winner = in_arbiters[i].pick_winner();
                Flit *f = in_arbiters[i].pull_winner();
                uint next_port = in_arbiters[i].next_port[j];
#ifdef _DEBUG_ROUTER
            _DBG(" VC_ARBITRATE_EVENT picked winner: %d vc: %d ftype: %d ", i, winner, f->type );
            _DBG(" Req PA for port: %d", next_port );
#endif
                port_arbiters[next_port].request(f, i, j);
                ticking = true;

                    
            }

    // Request for VCA and send credit back
    for ( uint i=0 ; i<ports ; i++ )
        for ( uint j=0 ; j<vcs ; j++ )
            if ( !in_buffers[i].is_empty(j) && !in_arbiters[i].is_requested(j) 
                 && in_buffers[decoders[i].get_output_port(j)].get_no_credits(j) > 0 )
            {
                in_buffers[i].change_pull_channel(j);
                Flit* f = in_buffers[i].pull();
#ifdef _DEBUG_ROUTER
                _DBG (" handle_input_arbitration_event Req VCA for port: %d vc: %d ftype: %d ",i,j, f->type);
#endif
                in_arbiters[i].request( f, j);
                in_arbiters[i].next_port[j] = decoders[i].get_output_port(j);

                /*  send a credit back for the requested vc */
                LinkArrivalData* data = new LinkArrivalData();
                data->type = CREDIT_ID;
                data->vc = j;
                
                IrisEvent* event = new IrisEvent();
                event->type = LINK_ARRIVAL_EVENT;
                event->event_data.push_back(data);
                event->src_id = address;
                event->vc = j;

                Simulator::Schedule(Simulator::Now()+1, &NetworkComponent::process_event, input_connections[i], event);

                /* can tick again since we know in_arbiter was requested */
                ticking = true;
            }
    
    if(ticking)
    {
        IrisEvent* event = new IrisEvent();
        event->type = TICK_EVENT;
        event->vc = e->vc;
        Simulator::Schedule(Simulator::Now()+1, &NetworkComponent::process_event, this, event);
    }

    return ;
}		/* -----  end of function RouterFourStageVcs::handle_input_arbitration_event  ----- */

string
RouterFourStageVcs::toString() const
{
    stringstream str;
    str << "RouterFourStageVcs"
        << "\t addr: " << address
        << " node_ip: " << node_ip
        << "\n Input buffers: " << in_buffers.size() << " ";
    if( in_buffers.size() > 0)
        str << in_buffers[0].toString();

    str << "\n In Arbiters: " << in_arbiters.size() << " ";
    if( in_arbiters.size() > 0)
        str << in_arbiters[0].toString();

    str << "\n decoders: " << decoders.size() << " ";
    if( decoders.size() > 0)
        str << decoders[0].toString();

    str << "\n port_arbiters: " << port_arbiters.size() << " ";
    if( port_arbiters.size() > 0)
        str << port_arbiters[0].toString();

    str << "\n Xbar ";
    str << xbar.toString();

    return str.str();
}


#endif   /* ----- #ifndef _router4stagevcs_cc_INC  ----- */

