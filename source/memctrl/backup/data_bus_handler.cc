/*
 * =====================================================================================
 *
 *       Filename:  data_bus_handler.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/23/2010 07:05:47 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  SYED MINHAJ HASSAN
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */

#include <math.h>
#include "data_bus_handler.h"
#include "../MemCtrl/cmd_issuer.h"
#include "../MemCtrl/response_handler.h"

using namespace std;

/*
 *--------------------------------------------------------------------------------------
 *       Class:  DataBusHandler
 *      Method:  DataBusHandler
 * Description:  constructor
 *--------------------------------------------------------------------------------------
 */

DataBusHandler::DataBusHandler()
{
   prevTime = 0; 
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  DataBusHandler
 *      Method:  ~DataBusHandler
 * Description:  destructor
 *--------------------------------------------------------------------------------------
 */
DataBusHandler::~DataBusHandler()
{

}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  DataBusHandler
 *      Method:  process_event
 * Description:  
 *--------------------------------------------------------------------------------------
 */
void DataBusHandler::process_event(IrisEvent* e)
{
    DRAMCmdState *cmd = new DRAMCmdState();
    IrisEvent *event = new IrisEvent();
    *cmd = *((DRAMCmdState*)e->event_data.at(0));	//TODO needs to set this through manifold kernel's links  	
    int burstDelay=0;				    
    
    switch (e->type)
    {	
	case START_READ:
	     burstDelay = ceil(ceil(cmd->req.data.size*1.0/DDR_BUS_WIDTH)*BUS_CYCLE);
	     event->src = (Component*)this;
	     event->dst = (Component*)child2;		// towards response handler
	     event->type = REPLY;	
	     event->event_data.push_back((void*)cmd);	//TODO needs to set this through manifold kernel's links
#ifdef DEEP_DEBUG
	     cout << dec << Simulator::Now() << ": " << hex << cmd->req.address << ": DataBus!! Receiving data from DRAM. Will take cycles = " << dec << burstDelay << endl;
#endif	
	     Simulator::Schedule(Simulator::Now()+burstDelay+1, &ResponseHandler::process_event, (ResponseHandler*)event->dst, event); 
	     break;

	case START_WRITE:		
	     burstDelay = ceil(ceil(cmd->req.data.size*1.0/DDR_BUS_WIDTH)*BUS_CYCLE);  	
	     event->src = (Component*)this;
	     event->dst = (Component*)child1;		// towards DRAM handler
	     event->type = START_WRITE;	
	     event->event_data.push_back((void*)cmd);	//TODO needs to set this through manifold kernel's links
#ifdef DEEP_DEBUG
	     cout << dec << Simulator::Now() << ": " << hex << cmd->req.address << ": DataBus!! Sending data to be written to DRAM. Will take cycles = " << dec << burstDelay << endl;	
#endif
	     Simulator::Schedule(Simulator::Now()+burstDelay+1, &DRAMChannel::process_event, (DRAMChannel*)event->dst, event); 
	     break;
	default:
		delete cmd;
		delete event;
		cout << Simulator::Now() << ": I should not come to default of process event in data bus\n";
    }
   if (prevTime + 19 > Simulator::Now())
   {	cerr << "Error in databus" << endl;
//   	exit(1);
   }
   prevTime = Simulator::Now();

   delete (DRAMCmdState*)e->event_data.at(0);
   delete e;
}

