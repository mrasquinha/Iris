/*
 * =====================================================================================
 *
 *       Filename:  cmd_bus_handler.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/23/2010 07:06:22 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Mitchelle Rasquinha (), mitchelle.rasquinha@gatech.edu
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */

#ifndef  cmd_bus_handler_h_INC
#define  cmd_bus_handler_h_INC

#include        <string>
#include        <sstream>
#include        <iostream>
#include        <cstdlib>
#include        <map>
#include        "../kernel/component.h"
#include        "../kernel/simulator.h"
#include        "../data_types/impl/irisEvent.h"
#include        "../MemCtrl/request.h"
#include	"../MemCtrl/dram.h" 

using namespace std;

/*  
 *  =====================================================================================
 *      Class:  CmdBusHandler
 *      Description:  
 *     
 *     
 *   =====================================================================================
 */

class CmdBusHandler : public Component
{
    public:
        CmdBusHandler ();                             /*  constructor */
        ~ CmdBusHandler ();
	Component* parent;
	Component* child;
        void process_event (IrisEvent* e);
        std::string toString();
                                                                            
    protected:
                                                                            
    private:
                                                                            
}; /*  -----  end of class CmdBusHandler  ----- */

#endif   /*  ----- #ifndef cmd_bus_handler_INC  ----- */
