/*
 * =====================================================================================
 *
 *       Filename:  addr_map.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/13/2010 02:13:15 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  SYED MINHAJ HASSAN
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */

#ifndef  addr_map_h_INC
#define  addr_map_h_INC

#include        <string>
#include        <sstream>
#include        <iostream>
#include        <cstdlib>
#include        <map>
#include        "../kernel/component.h"
#include        "../kernel/simulator.h"
#include        "../data_types/impl/irisEvent.h"
#include        "../MemCtrl/request.h" 
#include	"../MemCtrl/bank_handler.h"
#include	"../MemCtrl/constants.h"

using namespace std;

/*  
 *  =====================================================================================
 *      Class:  AddrMap
 *      Description:  
 *     
 *     
 *   =====================================================================================
 */

class AddrMap : public Component
{
    public:
        AddrMap ();                             /*  constructor */
        ~ AddrMap ();
	Component* parent;
        void process_event (IrisEvent* e);
	void map_addr(Request *req);
        std::string toString();
                                                                            
    protected:
                                                                            
    private:
                                                                            
}; /*  -----  end of class AddrMap  ----- */

#endif   /*  ----- #ifndef addr_map_INC  ----- */
