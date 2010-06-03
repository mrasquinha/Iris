/*
 * =====================================================================================
 *
 *       Filename:  NI.h
 *
 *    Description:  This is a test packet to send packets to a interface on
 *    VC0 at some random time and random packet length. 
 *
 *        Version:  1.0
 *        Created:  03/11/2010 05:00:00 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Mitchelle Rasquinha (), mitchelle.rasquinha@gatech.edu
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */

#ifndef  _NI_h_INC
#define  _NI_h_INC

#include	"../tests/MersenneTwister.h"
#include        "../components/interfaces/processor.h"
#include        "../components/impl/genericEvents.h"
#include        "../data_types/impl/highLevelPacket.h"
#include	"../data_types/impl/irisEvent.h"
#include	"../components/impl/genericData.h"
#include	"../MemCtrl/constants.h"
#include	"../MemCtrl/request.h"		
#include	<fstream>

#define DEFAULT_RAN_MAX_TIME 100
//#define PURE_SINK 1

using namespace std;

/*
 * =====================================================================================
 *        Class:  NI
 *  Description:  
 * =====================================================================================
 */
class NI : public Processor
{
    public:
        NI ();                             /* constructor */
        ~NI ();
        Component* mc;
	vector<Request> niQueue;
        void setup(uint no_nodes, uint vcs, uint max_sim_time);
        void finish();
        void process_event(IrisEvent* e);
        string toString() const;
        uint node_ip;
        void set_no_vcs ( uint v );
        void set_output_path( string v );
        bool compare();
	string print_stats() const;
        bool sending;

    protected:

    private:

        uint vcs;
        uint no_nodes;
	unsigned int packets;
        unsigned long long int max_sim_time;
        deque< HighLevelPacket > out_packets;
        deque< HighLevelPacket > sent_packets;
        string out_filename;
        string trace_name;
        ofstream out_file;
        fstream trace_filename;
        vector< bool > ready;
        unsigned int last_vc;
        void handle_new_packet_event( IrisEvent* e);
	void handle_old_packet_event( IrisEvent* e);
        void handle_ready_event(IrisEvent* e);
        void handle_out_pull_event(IrisEvent* e);
	void convertToBitStream(Request* req, HighLevelPacket *hlp);
	void convertFromBitStream(Request* req, HighLevelPacket *hlp);
	bool GetFromNIQueue(Request* req);

}; /* -----  end of class NI  ----- */

#endif   /* ----- #ifndef _NI_h_INC  ----- */
