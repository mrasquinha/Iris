/*
 * =====================================================================================
 *
 *       Filename:  genericData.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/21/2010 04:59:48 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Mitchelle Rasquinha (), mitchelle.rasquinha@gatech.edu
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */
#ifndef  _genericdata_h_INC
#define  _genericdata_h_INC

#include	"../../data_types/impl/flit.h"

using namespace std;

enum RouterPipeStage { INVALID, EMPTY,IB, FULL, ROUTED, SWA_REQUESTED, SW_ALLOCATED, ST, REQ_OUTVC_ARB };
class InputBufferState
{
    public:
        InputBufferState();
        ~InputBufferState(){}
        uint input_port;
        uint input_channel;
        uint output_port;
        uint output_channel;
        double arrival_time;
        int length;
        int credits_sent;
        vector < uint > possible_ovcs;
        vector < uint > possible_oports;
        RouterPipeStage pipe_stage;
        bool clear_message;
        uint flits_in_ib;
        string toString () const;

};

/*
 * =====================================================================================
 *        Class:  LinkArrivalData
 *  Description:  
 * =====================================================================================
 */
class LinkArrivalData
{
    public:
        LinkArrivalData ();                             /* constructor */
        ~LinkArrivalData ();                             /* constructor */
        uint type;
        uint vc;
        Flit* ptr;
        bool valid;

    protected:

    private:

}; /* -----  end of class LinkArrivalData  ----- */

/*
 * =====================================================================================
 *        Class:  VirtualChannelDescription
 *  Description:  
 * =====================================================================================
 */
class VirtualChannelDescription
{
    public:
        VirtualChannelDescription ();                             /* constructor */
        uint vc;
        uint port;

    protected:

    private:

}; /* -----  end of class VirtualChannelDescription  ----- */

/*
 * =====================================================================================
 *        Class:  RouteEntry
 *  Description:  
 * =====================================================================================
 */
class RouteEntry
{
    public:
        RouteEntry ();                             /* constructor */
        uint destination;
        vector< vector<uint> > ports;
        vector< vector<uint> > channels;

    protected:

    private:

}; /* -----  end of class RouteEntry  ----- */

class SA_unit
{
    public:
        SA_unit(){};
        uint port;
        uint ch;
};

#endif   /* ----- #ifndef _genericdata_h_INC  ----- */

