/*
 * =====================================================================================
 *
 *       Filename:  HighLevelPacket.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/09/2010 08:23:37 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Mitchelle Rasquinha (), mitchelle.rasquinha@gatech.edu
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */

#ifndef  _highlevelpacket_h_INC
#define  _highlevelpacket_h_INC
#include	"../../components/interfaces/genericComponentHeader.h"
#include	"lowLevelPacket.h"
#include	"../../kernel/simulator.h"
#include	<sstream>
#include	<string>
#include	<math.h>

#define CONTROL_VECTOR_SIZE 16
using namespace std;

enum virtual_network { VN0, VN1, VN2};
//enum message_class { HOM /* Carries requests from a caching agent to the home agent, mainly used for  coherence. Mentioned her for completeness */,
//    SNP /* Snoop */,
//    NDR /* Non data response. This class is used to send short reponse messages */,
//    DRS /* Data response */,
//    NCB /* Non coherent bypass */,
//    NCS /* Non coherent standard */
//};
//


/*
 * =====================================================================================
 *        Class:  HighLevelPacket
 *  Description:  
 *
 * =====================================================================================
 */
class HighLevelPacket
{
    public:
        /* ====================  LIFECYCLE     ======================================= */
        HighLevelPacket ();                             /* constructor */
        ~HighLevelPacket();

        /* ====================  STATISTICS ======================================= */
        /* Keep track of the packet transit time */
        simTime get_transit_time();

        /* ====================  OPERATIONS      ======================================= */
        void to_low_level_packet(LowLevelPacket* llp) ;
        void from_low_level_packet(LowLevelPacket* llp);

        /* ====================  OPERATORS     ======================================= */
        string toString() const;
        /* Assuming that a minimal check on the transaction id and source should suffice. In accordance with the QPI spec all packets have the transaction Id which can be mapped back to the caching agent that generated the request. Once the transaction is completed and the response arrives at the caching agent, a tear down is initiated and the transaction ids may be reused at the source.*/
        bool operator==( const HighLevelPacket* p );

        uint source;
        uint destination;
        virtual_network vn;
        message_class msg_class;
        uint virtual_channel;
        uint transaction_id;
        simTime sent_time;
        simTime recv_time;
        unsigned int data_payload_length;       /* length of the data payload in bytes */
        vector<bool> data;

    protected:

    private:

}; /* -----  end of class HighLevelPacket  ----- */

#endif   /* ----- #ifndef _highlevelpacket_h_INC  ----- */

