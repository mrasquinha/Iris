/*
 * =====================================================================================
 *
 *       Filename:  genericPortArbiter.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/14/2010 01:07:06 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Mitchelle Rasquinha (), mitchelle.rasquinha@gatech.edu
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */

#ifndef  _genericportarbiter_h_INC
#define  _genericportarbiter_h_INC

#include	"../interfaces/portArbiter.h"
#include	"../interfaces/genericComponentHeader.h"
/*
 * =====================================================================================
 *        Class:  GenericPortArbiter
 *  Description:  
 * =====================================================================================
 */
class GenericPortArbiter: public PortArbiter
{
    public:
        GenericPortArbiter ();                             /* constructor */
        ~GenericPortArbiter ();
        void destroy();
        bool is_empty( uint ch);
        void resize(uint ports);
        void request(Flit* f, uint port, uint vc);
        void set_req_queue_size(uint ch);
        uint get_no_requests();
        bool is_requested( uint port, uint ch);
        uint pick_winner( uint ch);
        Flit* pull_winner( uint ch);
        void clear( uint ch );
        void clear_winner( uint ch);
        vector<uint> get_requests( uint ch);
        string toString() const;
        unsigned long long int write_time;
        void flush_all_requests();

    protected:

    private:
        vector< vector<bool> > requests;
        vector< vector<Flit*> > flits;
        vector< uint> last_winner;
        vector<bool> locked;
        vector<bool> done;
        uint ports;

}; /* -----  end of class GenericPortArbiter  ----- */


#endif   /* ----- #ifndef _genericportarbiter_h_INC  ----- */
