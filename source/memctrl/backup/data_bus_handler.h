/*
 * =====================================================================================
 *
 *       Filename:  data_bus_handler.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/23/2010 07:05:38 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  SYED MINHAJ HASSAN
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */

#ifndef  data_bus_handler_h_INC
#define  data_bus_handler_h_INC

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
#include 	"../MemCtrl/constants.h"

using namespace std;

/*  
 *  =====================================================================================
 *      Class:  DataBusHandler
 *      Description:  
 *     
 *     
 *   =====================================================================================
 */

class DataBusHandler : public Component
{
    public:
        DataBusHandler ();                             /*  constructor */
        ~ DataBusHandler ();
	Component* parent;
	Component* child1;		// DRAM's link
	Component* child2; 		// Response Handler's link
        void process_event (IrisEvent* e);
        std::string toString();
	Time prevTime; 
                                                                            
    protected:
                                                                            
    private:
                                                                            
}; /*  -----  end of class DataBusHandler  ----- */

#endif   /*  ----- #ifndef data_bus_handler_INC  ----- */
