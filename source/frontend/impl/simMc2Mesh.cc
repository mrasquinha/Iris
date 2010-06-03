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

#include	"mesh.h"
#include	"sim_constants.h"
#include	<algorithm>

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
    uint grid_size=0;
    uint max_sim_time = 10000;
    uint phy_link_bits = 128;
    vector<string> traces;
    vector<uint> mc_positions;
    string trace_name, output_path;

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
            if ( word.compare("OUTPUT_PATH") == 0)
                iss >> output_path;
            if ( word.compare("PHY_LINK_WIDTH") == 0)
                iss >> phy_link_bits;
	    if ( word.compare("THREAD_BITS_POSITION") == 0)
                iss >> THREAD_BITS_POSITION;
	    if ( word.compare("MC_ADDR_BITS") == 0)
                 iss >> MC_ADDR_BITS;
	    if ( word.compare("SEND_EARLY_CREDIT") == 0)
                 iss >> send_early_credit;
            if ( word.compare("TRACE") == 0)
            {
                iss >> trace_name;
                traces.push_back(trace_name);
            }
            if ( word.compare("MC_LOC") == 0)
            {
                uint mc_xpos, mc_ypos;
                iss >> mc_xpos;
                iss >> mc_ypos;
                mc_positions.push_back(mc_xpos*grid_size + mc_ypos);
            }
        }
    }

for ( uint i=0; i<argc; i++)
    {
        if( strcmp(argv[i],"--thread_id_bits")==0)
            THREAD_BITS_POSITION = atoi(argv[i+1]);
        if( strcmp(argv[i],"--mc_bits")==0)
            MC_ADDR_BITS = atoi(argv[i+1]);
        if( strcmp(argv[i],"--early_credit")==0)
            send_early_credit = atoi(argv[i+1]);
    }

    /* Number of MC's and the size of the position vector should be the same. */
//    assert(mc_positions.size() == no_mcs);

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
        cerr << " max_phy_link_bits:\t" << max_phy_link_bits << endl;
	cerr << " THREAD_BITS_POSITION:\t" << THREAD_BITS_POSITION<< endl;
        cerr << " MC_ADDR_BITS:\t" << MC_ADDR_BITS<< endl;

    if( traces.size() < (no_nodes - no_mcs) )
    {
        cout << " Not enough trace files for simulation " << endl;
        exit(1);
    }

    Mesh* mesh = new Mesh();
    mesh->init( ports, vcs, credits, buffer_size, no_nodes, grid_size, links);
    mesh->max_sim_time = max_sim_time;

    /* Create the mesh->routers and mesh->interfaces */
    for( uint i=0; i<no_nodes; i++)
    {
        mesh->routers.push_back( new GenericRouterVct());
        mesh->interfaces.push_back ( new GenericInterface());
    }

    /*  Create the TPG and mc modules */
    vector<uint>::iterator itr;
    for( uint i=0; i<no_nodes; i++)
    {
        itr = find(mc_positions.begin(), mc_positions.end(), i);
        if( itr != mc_positions.end())
            mesh->processors.push_back( new GenericFlatMc() );
        else
        {
            mesh->processors.push_back( new GenericTPG() );
            static_cast<GenericTPG*>(mesh->processors[i])->set_trace_filename(traces[i]);
            for ( uint j=0; j<mc_positions.size(); j++)
                static_cast<GenericTPG*>(mesh->processors[i])->mc_node_ip.push_back(mc_positions[j]);;
        }
    }

    /* Create the links */
    for ( uint i=0; i<links; i++)
    { 
        mesh->link_a.push_back(new GenericLink());
        mesh->link_b.push_back(new GenericLink());
    }

    mesh->connect_interface_processor();

    /* Set all the component ids */
    uint comp_id = 0, alink_comp_id = 1000, blink_comp_id = 5000;
    for ( uint i=0 ; i<no_nodes; i++ )
    {
        mesh->processors[i]->setComponentId(comp_id++);
        mesh->interfaces[i]->setComponentId(comp_id++);
        mesh->routers[i]->setComponentId(comp_id++);
    }

    for ( uint i=0 ; i<links; i++ )
    {
        mesh->link_a[i]->setComponentId(alink_comp_id++);
        mesh->link_b[i]->setComponentId(blink_comp_id++);
        mesh->link_a[i]->setup();
        mesh->link_b[i]->setup();
    }

    /*  Set up the node ips for components */
    for ( uint i=0 ; i<no_nodes ; i++ )
    {
        mesh->interfaces[i]->node_ip = i;
        mesh->routers[i]->node_ip = i;
        mesh->processors[i]->node_ip = i;
    }

    /* Set the number of ports, vcs, credits and buffer sizes for the
     * components */
    for ( uint i=0 ; i<no_nodes ; i++ )
    {
        mesh->interfaces[i]->set_no_vcs(vcs);
        mesh->interfaces[i]->set_no_credits(credits);
        mesh->interfaces[i]->set_buffer_size(credits);
//        mesh->processors[i]->set_output_path(output_path);
    }

    mesh->setup();
    for ( uint i=0 ; i<no_nodes ; i++ )
        mesh->processors[i]->set_output_path(output_path);

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
        mesh->routers[i]->set_no_nodes(no_nodes);

    for ( uint i=0 ; i<no_nodes ; i++ )
        for( uint j=0; j < ports ; j++)
            for( uint k=0; k < no_nodes ; k++) // Assuming is a square mesh. 
            {
                static_cast<GenericRouterVct*>(mesh->routers[i])->set_grid_x_location(j,k, grid_x[k]);
                static_cast<GenericRouterVct*>(mesh->routers[i])->set_grid_y_location(j,k, grid_y[k]);
            }

    mesh->connect_interface_routers();
    mesh->connect_routers();

    /*  Printing out all component after setup */
    for ( uint i=0 ; i<no_nodes; i++ )
    {
        cout << "\nTPG: " << i << " " << mesh->processors[i]->toString();
        cout << "\ninterface: " << i << " " << mesh->interfaces[i]->toString();
        cout << "\nrouter: " << i << " " << mesh->routers[i]->toString();
    }

    for ( uint i=0 ; i<links ; i++ )
    {
        cout << "\nlinka_" << i << " " << mesh->link_a[i]->toString();
        cout << "\nlinkb_" << i << " " << mesh->link_b[i]->toString();
    }
    
    Simulator::StopAt(max_sim_time);
    Simulator::Run();

    cerr << mesh->print_stats();

    cerr << "------------ End SimIris ---------------------" << endl;

    delete mesh;

    return 0;
}				/* ----------  end of function main  ---------- */


#endif   /* ----- #ifndef _simmc2mesh_cc_INC  ----- */
