/*
 * =====================================================================================
 *
 *       Filename:  genericrc.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/19/2010 11:54:57 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Mitchelle Rasquinha (), mitchelle.rasquinha@gatech.edu
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */
#ifndef  _genericrc_h_INC
#define  _genericrc_h_INC

#include	"../interfaces/addressDecoder.h"
#include        "../../data_types/impl/flit.h"
#include	"../../data_types/impl/lowLevelPacket.h"

using namespace std;

/*
 * =====================================================================================
 *        Class:  GenericRC
 *  Description:  
 * =====================================================================================
 */
class GenericRC
{
    public:
        GenericRC ();
        ~GenericRC(){}
        void push( Flit* f, unsigned int vc );
        unsigned int get_output_port ( unsigned int channel);
        unsigned int speculate_port ( Flit* f, unsigned int ch );
        unsigned int speculate_channel ( Flit* f, unsigned int ch );
        unsigned int get_virtual_channel ( unsigned int ch );
        void resize ( unsigned int ch );
        unsigned int get_no_channels ();
        bool is_empty();
        string toString() const;
        uint node_ip;
        uint address;
        vector < uint > grid_xloc;
        vector < uint > grid_yloc;

    protected:

    private:
                string name;
                uint route_x_y( uint addr );

        /*
         * =====================================================================================
         *        Class:  Address
         *  Description:  
         * =====================================================================================
         */
        class Address
        {
            public:
                bool route_valid;
                unsigned int channel;
                unsigned int out_port;

            protected:

            private:

        }; /* -----  end of class Address  ----- */
        vector<Address> addresses;

}; /* -----  end of class GenericRC  ----- */

#endif   /* ----- #ifndef _genericrc_h_INC  ----- */

