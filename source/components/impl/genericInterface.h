/*
 * =====================================================================================
 *
 *       Filename:  genericInterface.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/24/2010 02:04:22 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Mitchelle Rasquinha (), mitchelle.rasquinha@gatech.edu
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */

#ifndef  _genericinterface_h_INC
#define  _genericinterface_h_INC

#include	"../../data_types/impl/highLevelPacket.h"
#include	"../../data_types/impl/irisEvent.h"
#include	"../interfaces/interface.h"
#include	"../interfaces/networkComponent.h"
#include	"../interfaces/processor.h"
#include	"../interfaces/buffer.h"
#include	"../interfaces/genericComponentHeader.h"
#include	"../impl/genericLink.h"
#include	"genericEvents.h"
#include	"genericBuffer.h"
#include	"genericData.h"
#include	<queue>
#include	<vector>
#include	<math.h>
#define DEFAULT_NO_OF_CREDITS 1
/*
 * =====================================================================================
 *        Class:  GenericInterface
 *  Description:  
 * =====================================================================================
 */
class GenericInterface : public Interface
{
    public:
        GenericInterface ();                             /* constructor */
        ~GenericInterface ();                             /* constructor */

        uint get_no_credits() const;
        void set_no_credits( int credits );
        void set_no_vcs ( uint v );
        void set_buffer_size( uint b );
        void setup( uint v, uint cr);

        string toString() const;
        void process_event( IrisEvent* e);
        string print_stats();

    protected:

    private:
        uint vcs;
        uint buffer_size;
        int credits;
        bool in_packet_cleared;
        bool in_packet_complete;

        bool ticking;
        GenericBuffer out_buffer;
        GenericBuffer in_buffer;
        vector < int > downstream_credits;

        /* The current packet being pushed into the output buffers */
        vector < LowLevelPacket> out_packets;
        vector < uint > out_packet_flit_index;

        /* The current packet being pulled from the input buffers */
        vector < LowLevelPacket> in_packets;
        vector < uint > in_packets_flit_index;
        vector < bool> in_packets_valid;

        vector < bool > in_ready;

        /* event handlers */
        void handle_new_packet_event( IrisEvent* e);
        void handle_ready_event( IrisEvent* e);
        void handle_tick_event( IrisEvent* e);
        void handle_link_arrival( IrisEvent* e);

        /* stats */
        uint flits_in;
        uint packets_in;
        uint flits_out;
        uint packets_out;
        uint total_packets_in_time;

}; /* -----  end of class GenericInterface  ----- */

#endif   /* ----- #ifndef _genericinterface_h_INC  ----- */


