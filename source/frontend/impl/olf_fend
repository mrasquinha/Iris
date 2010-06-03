/*
 * =====================================================================================
 *
 *       Filename:  simMc2Mesh.cc
 *
 *    Description:  This is a 2x2 mesh network with 3 trace packet generators
 *    and a single memory controller
 *
 *        Version:  1.0
 *        Created:  04/20/2010 02:59:08 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Mitchelle Rasquinha (), mitchelle.rasquinha@gatech.edu
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */

#ifndef  _simmc2mesh_cc_INC
#define  _simmc2mesh_cc_INC

#include	<iostream>
#include	<fstream>
#include        "../../components/impl/genericTPG.h"
#include	"../../components/impl/genericRouterNoVcs.h"
#include	"../../components/impl/genericInterface.h"
//#include	"../../components/impl/mcFrontEnd.h"
#include        "../../components/impl/genericRPG.h"
#include        "../../components/impl/genericLink.h"

int
main ( int argc, char *argv[] )
{
    if(argc<2)
    {
        cout << "Error: Requires config file for input parameters\n";
        return 1;
    }

    /* The following parameters must be specified in the config with the
     * correct knobs */
    uint vcs=0, ports=0, buffer_size=0, credits=0;
    uint grid_size=0, no_nodes=0, no_mcs=0;
    uint max_sim_time = 10000;
    vector<string> traces;
    string trace_name;

    ifstream fd(argv[1]);
    string data, word;
    while(!fd.eof())
    {
        getline(fd,data);
        istringstream iss( data, istringstream::in);
        while ( iss >> word )
        {
            if ( word.compare("VCS") == 0)   
                iss >> vcs;
            if ( word.compare("PORTS") == 0) 
                iss >> ports;
            if ( word.compare("BUFFER_SIZE") == 0)
                 iss >> buffer_size;
            if ( word.compare("CREDITS") == 0)
                 iss >> credits;
            if ( word.compare("GRID_SIZE") == 0)
                 iss >> grid_size;
            if ( word.compare("NO_OF_NODES") == 0)
                iss >> no_nodes;
            if ( word.compare("MCS") == 0)
                iss >> no_mcs;
            if ( word.compare("MAX_SIM_TIME") == 0)
                iss >> max_sim_time;
            if ( word.compare("TRACE") == 0)
            {
                iss >> trace_name;
                traces.push_back(trace_name);
            }
        }
    }

    /* Compute additional parameters */
    uint links = (ports + (grid_size -1)*(ports-1)) + ( (ports-1) + (grid_size -1)*(ports-2))*(grid_size-1);
    fd.close();

        /* Creating mesh topology with the following config */
	cerr << "\n-----------------------------------------------------------------------------------\n";
        cerr << "** CAPSTONE - Cycle Accurate Parallel Simulator Technologgy for On-Chip Networks **\n";
        cerr << " This is simIris. A second version of Capstone." << endl;
        cerr << "-- Computer Architecture and Systems Lab                                         --\n"
            << "-- Georgia Institute of Technology                                               --\n"
            << "-----------------------------------------------------------------------------------\n";
        cerr << "Cmd line: ";
        for( int i=0; i<argc; i++)
            cerr << argv[i] << " ";
        cerr << endl;
        cerr << " vcs:\t" << vcs << endl;
        cerr << " ports:\t" << ports << endl;
        cerr << " buffer_size:\t" << buffer_size << endl;
        cerr << " credits:\t" << credits << endl;
        cerr << " no_nodes( spread over a 2D Mesh topology):\t" << no_nodes << endl;
        cerr << " grid size:\t" << grid_size << endl;
        cerr << " links:  \t" << links << endl;
        cerr << " no_of_mcs:\t" << no_mcs << endl;
        cerr << " no_of_traces:\t" << traces.size() << endl;
        cerr << " max_sim_time:\t" << max_sim_time << endl;

    if( traces.size() < (no_nodes - no_mcs) )
    {
        cout << " Not enough trace files for simulation " << endl;
        exit(1);
    }

    vector <Router*> routers;
    vector <Interface*> interfaces;
    vector <Processor*> processors;
    vector <GenericLink*> link_a;
    vector <GenericLink*> link_b;

    /* Create the routers and interfaces */
    for( uint i=0; i<no_nodes; i++)
    {
        routers.push_back( new GenericRouterNoVcs());
        interfaces.push_back ( new GenericInterface());
    }

    /*  Create the TPG and mc modules */
    for( uint i=0; i<(no_nodes-no_mcs); i++)
        processors.push_back( new GenericTPG() );

    for( uint i=0; i<no_mcs; i++)
        processors.push_back( new GenericRPG() );

    for( uint i=0; i<(no_nodes-no_mcs); i++)
        static_cast<GenericTPG*>(processors[i])->set_trace_filename(traces[i]);

    /* Create the links */
    for ( uint i=0; i<links; i++)
    { 
        link_a.push_back(new GenericLink());
        link_b.push_back(new GenericLink());
    }

    /* Connect the interfaces and the processors. And set the links for the
     * interfaces */
    for ( uint i=0 ; i<no_nodes; i++ )
    {
        processors[i]->interface_connections[0] = interfaces[i];
        interfaces[i]->processor_connection = static_cast<NetworkComponent*>(processors[i]);
        interfaces[i]->input_connection = link_b[i];
        interfaces[i]->output_connection = link_a[i];
    }

    /* Set all the component ids */
    uint comp_id = 0, alink_comp_id = 100, blink_comp_id = 200;
    for ( uint i=0 ; i<no_nodes; i++ )
    {
        processors[i]->setComponentId(comp_id++);
        interfaces[i]->setComponentId(comp_id++);
        routers[i]->setComponentId(comp_id++);
    }

    for ( uint i=0 ; i<links; i++ )
    {
        link_a[i]->setComponentId(alink_comp_id++);
        link_b[i]->setComponentId(blink_comp_id++);
        link_a[i]->setup();
        link_b[i]->setup();
    }

    /*  Set up the node ips for components */
    for ( uint i=0 ; i<no_nodes ; i++ )
    {
        interfaces[i]->node_ip = i;
        routers[i]->node_ip = i;
        processors[i]->node_ip = i;
    }

    /* Set the number of ports, vcs, credits and buffer sizes for the
     * components */
    for ( uint i=0 ; i<no_nodes ; i++ )
    {
        interfaces[i]->set_no_vcs(vcs);
        interfaces[i]->set_no_credits(credits);
        interfaces[i]->set_buffer_size(credits);
        processors[i]->set_no_vcs(vcs);
    }

    /* Call setup on all components */
    for ( uint i=0 ; i<no_nodes ; i++ )
    {
        processors[i]->setup();
        interfaces[i]->setup();
        routers[i]->init(ports, vcs, credits, buffer_size);
        /* for now the links dont need a setup. They may later */
        // Give the interface a credit
    }

    /*  Set no of ports and positions for routing */
    vector< uint > grid_x;
    vector< uint > grid_y;
    grid_x.resize(no_nodes);
    grid_y.resize(no_nodes);

    /* Limitation of only modelling squares */
    for ( uint i=0 ; i<grid_size ; i++ )
        for ( uint j=0 ; j<grid_size ; j++ )
        {
            grid_x[(i*grid_size)+j] = j;
            grid_y[(i*grid_size)+j] = i;
        }


    for ( uint i=0 ; i<no_nodes ; i++ )
        routers[i]->set_no_nodes(no_nodes);

    for ( uint i=0 ; i<no_nodes ; i++ )
        for( uint j=0; j < ports ; j++)
            for( uint k=0; k < no_nodes ; k++) // Assuming is a square mesh. 
            {
                static_cast<GenericRouterNoVcs*>(routers[i])->set_grid_x_location(j,k, grid_x[k]);
                static_cast<GenericRouterNoVcs*>(routers[i])->set_grid_y_location(j,k, grid_y[k]);
            }

    // Input and output link connections
    for ( uint i=0 ; i<no_nodes ; i++ )
    {
	link_a[i]->input_connection = interfaces[i];
	link_a[i]->output_connection = routers[i];
	link_b[i]->input_connection = routers[i];
	link_b[i]->output_connection = interfaces[i];
	routers[i]->input_connections.push_back(link_a[i]);
	routers[i]->output_connections.push_back(link_b[i]);
    }

	//  Router input and output connections
    map< uint , uint > east_links;
    map< uint , uint > west_links;
    /* ------------- Configure east links for the router ----------- */
    uint last_link_id = no_nodes;
    for ( uint router_no=0 ; router_no<no_nodes; router_no++ )
    {
        if( (router_no%grid_size) == 0 ) /* Left side end node */
        {
	    link_a[last_link_id]->input_connection = NULL;
	    link_a[last_link_id]->output_connection = routers[router_no];
	    link_b[last_link_id]->input_connection = routers[router_no];
	    link_b[last_link_id]->output_connection = NULL;
            east_links.insert(make_pair(router_no, last_link_id));
            last_link_id++;
        }
        else
        {
	    link_a[last_link_id]->input_connection = routers[router_no-1];
            link_a[last_link_id]->output_connection = routers[router_no];
	    link_b[last_link_id]->input_connection = routers[router_no];
	    link_b[last_link_id]->output_connection = routers[router_no-1];
            east_links.insert(make_pair(router_no, last_link_id));
            west_links.insert(make_pair(router_no-1, last_link_id));
            last_link_id++;
        }
    }

    for ( uint i=0 ; i<no_nodes; i++ )
    {
        map<uint,uint>::iterator link_id = east_links.find(i);
	routers[i]->input_connections.push_back(link_a[link_id->second]);
	routers[i]->output_connections.push_back(link_b[link_id->second]);
    }
    cout << "\n End East links " << last_link_id << endl;
/* ------------ End East links --------------------- */

/* ------------ Begin West links --------------------- */
    for ( uint router_no=0 ; router_no<no_nodes; router_no++ )
    {
        if ( (router_no%grid_size) == (grid_size-1))
        {
	    link_a[last_link_id]->input_connection = routers[router_no];
	    link_a[last_link_id]->output_connection = NULL;
            link_b[last_link_id]->input_connection = NULL;
	    link_b[last_link_id]->output_connection = routers[router_no];
            west_links.insert(make_pair(router_no, last_link_id));
            last_link_id++;
        }
    }

    for ( uint i=0 ; i<no_nodes; i++ )
    {
        map<uint,uint>::iterator link_id = west_links.find(i);
	routers[i]->input_connections.push_back(link_b[link_id->second]);
	routers[i]->output_connections.push_back(link_a[link_id->second]);
    }
    cout << "\n End West links " << last_link_id << endl;
/* ------------ End West links --------------------- */

/* ------------ Begin North links --------------------- */
    map< uint , uint > north_links;
    map< uint , uint > south_links;
    map < uint, uint > col_major_ordering;
    for ( uint i=0; i<grid_size; i++)
        for ( uint j=0; j<grid_size; j++)
            col_major_ordering.insert(make_pair(i*grid_size+j, j*grid_size+i));

    for ( uint router_no=0 ; router_no<no_nodes; router_no++ )
    {
        map<uint,uint>::iterator iter = col_major_ordering.find(router_no);
        uint col_major_router_no = iter->second;
        if( (col_major_router_no%grid_size) == 0 ) /* Top end node */
        {
	    link_a[last_link_id]->input_connection = NULL ;
            link_a[last_link_id]->output_connection = routers[router_no];
	    link_b[last_link_id]->input_connection = routers[router_no];
	    link_b[last_link_id]->output_connection = NULL ;
            north_links.insert(make_pair(router_no, last_link_id));
            last_link_id++;
        }
        else
        {
	    link_a[last_link_id]->input_connection = routers[router_no - grid_size];
	    link_a[last_link_id]->output_connection = routers[router_no];
	    link_b[last_link_id]->input_connection = routers[router_no];
	    link_b[last_link_id]->output_connection = routers[router_no - grid_size];
            north_links.insert(make_pair(router_no, last_link_id));
            south_links.insert(make_pair(router_no - grid_size, last_link_id));
            last_link_id++;
        }
    }

    for ( uint i=0 ; i<no_nodes; i++ )
    {
        map<uint,uint>::iterator link_id = north_links.find(i);
	routers[i]->input_connections.push_back(link_a[link_id->second]);
	routers[i]->output_connections.push_back(link_b[link_id->second]);
    }
    cout << "\n End North links " << last_link_id << endl;
/* ------------ End North links --------------------- */

/* ------------ Begin South links --------------------- */

    for ( uint router_no=0 ; router_no<no_nodes; router_no++ )
    {
        map<uint,uint>::iterator iter = col_major_ordering.find(router_no);
        uint col_major_router_no = iter->second;
        if ( (col_major_router_no%grid_size) == (grid_size-1))
        {
	    link_a[last_link_id]->input_connection = routers[router_no];
            link_a[last_link_id]->output_connection = NULL;
	    link_b[last_link_id]->input_connection = NULL;
	    link_b[last_link_id]->output_connection = routers[router_no];
            south_links.insert(make_pair(router_no, last_link_id));
            last_link_id++;
        }
    }

    for ( uint i=0 ; i<no_nodes; i++ )
    {
        map<uint,uint>::iterator link_id = south_links.find(i);
	routers[i]->input_connections.push_back(link_b[link_id->second]);
	routers[i]->output_connections.push_back(link_a[link_id->second]);
    }
    cout << "\n End North links " << last_link_id << endl;

    if(last_link_id > links )
    {
        cout << "ERROR : incorrect topology last_link_id: " << last_link_id << " links: " << links <<endl;
        exit(1);
    }
    north_links.clear();
    south_links.clear();
/* ------------ End South links --------------------- */

    /*  Printing out all component after setup */
    for ( uint i=0 ; i<no_nodes; i++ )
    {
        cout << "\nTPG: " << i << " " << processors[i]->toString();
        cout << "\ninterface: " << i << " " << interfaces[i]->toString();
        cout << "\nrouter: " << i << " " << routers[i]->toString();
    }

    for ( uint i=0 ; i<links ; i++ )
    {
        cout << "\nlinka_" << i << " " << link_a[i]->toString();
        cout << "\nlinkb_" << i << " " << link_b[i]->toString();
    }
    
    Simulator::StopAt(max_sim_time);
    Simulator::Run();

    for ( uint i=0 ; i<no_nodes ; i++ )
    {
        cerr << routers[i]->print_stats()<< endl;
        cerr << interfaces[i]->print_stats()<< endl;
    }

    for ( uint i=0 ; i<links ; i++ )
    {
        cerr << link_a[i]->print_stats()<< endl;
        cerr << link_b[i]->print_stats()<< endl;
    }

    for ( uint i=0 ; i<no_nodes; i++ )
    {
        delete processors[i];
//        delete interfaces[i];
        delete routers[i];
    }

    for ( uint i=0 ; i<links; i++ )
    {
        delete link_a[i];
        delete link_b[i];
    }

    cerr << "------------ End SimIris ---------------------" << endl;
    return 0;
}				/* ----------  end of function main  ---------- */


#endif   /* ----- #ifndef _simmc2mesh_cc_INC  ----- */
