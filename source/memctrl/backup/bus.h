/*
 * =====================================================================================
 *
 *       Filename:  bus.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/05/2010 05:36:22 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  SYED MINHAJ HASSAN
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */

#ifndef  bus_h_INC
#define  bus_h_INC

#include        <string>
#include        <sstream>
#include        <iostream>
#include        <cstdlib>
#include        <map>
#include        "../kernel/component.h"
#include        "../kernel/simulator.h"
#include        "../data_types/impl/irisEvent.h"
#include        "../MemCtrl/request.h"
#include	"../MemCtrl/data_bus_handler.h"
#include	"../MemCtrl/cmd_bus_handler.h"
#include	"../MemCtrl/dram.h" 

using namespace std;

/*  
 *  =====================================================================================
 *      Class:  Bus
 *      Description:  
 *     
 *     
 *   =====================================================================================
 */

class Bus : public Component
{
    public:
        Bus();                             /*  constructor */
        ~ Bus();
	Component* mc;
	Component* parent;
	Component* child1;		// DRAM side link
	Component* child2;		// Response side link
	DataBusHandler dataBus[NO_OF_CHANNELS];
	CmdBusHandler cmdBus[NO_OF_CHANNELS];  
        void process_event (IrisEvent* e);
	void SetLinks();
        std::string toString();
                                                                            
    protected:
                                                                            
    private:
                                                                            
}; /*  -----  end of class Bus  ----- */

#endif   /*  ----- #ifndef bus_INC  ----- */
