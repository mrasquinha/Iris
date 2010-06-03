/*
 * =====================================================================================
 *
 *       Filename:  mcFrontEnd.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/13/2010 03:27:02 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Mitchelle Rasquinha (), mitchelle.rasquinha@gatech.edu
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */

#ifndef  _mcfrontend_h_INC
#define  _mcfrontend_h_INC

#include	"../../data_types/impl/highLevelPacket.h"
#include	"../../data_types/impl/irisEvent.h"
#include        "../interfaces/interface.h"
#include        "../interfaces/networkComponent.h"
#include        "../interfaces/processor.h"
#include        "../interfaces/buffer.h"
#include	"../../memctrl/constants.h"
#include	"../../memctrl/request.h"		
#include        "genericEvents.h"
#include        "genericBuffer.h"
#include        "genericArbiter.h"
#include	"genericData.h"
#include	<queue>
#include	<vector>
#include	<math.h>

class MCFrontEnd: public Interface
{
    public:
        MCFrontEnd();
        ~MCFrontEnd();
        void setup();
        uint get_no_credits() const;
        void set_no_credits( uint credits );
        string toString() const;
        string print_stats();
        void process_event( IrisEvent* e);

        uint node_ip;
        GenericOutputBuffer out_buffer;
        GenericArbiter out_arbiter;
        GenericArbiter in_arbiter;
        GenericOutputBuffer in_buffer;

    private:
        unsigned long long int last_out_arbitrate_cycle;
        unsigned long long int last_in_arbitrate_cycle;
        unsigned long long int last_flit_out_cycle;
        /* The current packet being pushed into the output buffers */
        vector < LowLevelPacket> out_packets;
        vector < uint > out_packet_index;

        /* The current packet being pulled from the input buffers */
        vector < LowLevelPacket> in_packets;
        vector < bool > in_ready;

        /* event handlers */
        void handle_new_packet_event( IrisEvent* e);
        void handle_ready_event( IrisEvent* e);
        void handle_tick_event( IrisEvent* e);
        void handle_link_arrival( IrisEvent* e);
        void handle_in_push_event(IrisEvent* e);
        void handle_in_arbitrate_event(IrisEvent* e);
        void handle_out_arbitrate_event(IrisEvent* e);
        void handle_flit_out_event(IrisEvent* e);
        uint check_input_conditions();
        bool ticking;
        uint check_tick();
        void convertFromBitStream(Request* req, HighLevelPacket* hlp);

        /* stats */
        uint flits_in;
        uint packets_in;
        uint flits_out;
        uint packets_out;
        uint total_packets_in_time;
};
#endif   /* ----- #ifndef _mcfrontend_h_INC  ----- */
