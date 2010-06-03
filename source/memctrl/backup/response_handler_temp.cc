/*
 * =====================================================================================
 *
 *       Filename:  response_handler.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/04/2010 02:06:48 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Mitchelle Rasquinha (), mitchelle.rasquinha@gatech.edu
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */

#include <math.h>
#include "response_handler.h"
#include "request_handler.h"
//#include "cmd_issuer.h"
using namespace std;

/*
 *--------------------------------------------------------------------------------------
 *       Class:  ResponseHandler
 *      Method:  ResponseHandler
 * Description:  constructor
 *--------------------------------------------------------------------------------------
 */

ResponseHandler::ResponseHandler()
{
    bufferFull = false;
    stoppedQueue = false;	
    responseBuffer.clear();	
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  ResponseHandler
 *      Method:  ~ResponseHandler
 * Description:  destructor
 *--------------------------------------------------------------------------------------
 */
ResponseHandler::~ResponseHandler()
{

}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  ResponseHandler
 *      Method:  process_event
 * Description:  bool ResponseHandler::IsBufferFull()
 *--------------------------------------------------------------------------------------
 */
void ResponseHandler::process_event(IrisEvent* e)
{
    Request *req = new Request();	
    IrisEvent *event = new IrisEvent();	
    DRAMCmdState cmd;		
    switch (e->type)
    {	
	case PUSH_BUFFER:
	     *req = *((Request*)e->event_data.at(0));
	     delete (Request*)e->event_data.at(0);
	     responseBuffer.push_back(*req);
	//     cout << responseBuffer.size() << endl;
#ifdef DEEP_DEBUG
	    cout << dec << Simulator::Now() << ": " << hex << req->address << ": I am in ResponseHandler PUSH BUFFER with " << dec << req->cmdType << " and tag " << req->tag << endl;
#endif

	    if (IsBufferFull())		// check and set buffer full signal
	    {
		IrisEvent *event2 = new IrisEvent();	
		event2->src = (Component*)this;
	     	event2->dst = (Component*)reqPtr;	// stop cmd queue from receiving any more 	
		event2->type = STOP_CMD_QUEUE;
		stoppedQueue = true;	
	        Simulator::Schedule(Simulator::Now()+1, &RequestHandler::process_event, (RequestHandler*)event2->dst, event2);
#ifdef DEEP_DEBUG
		cout << dec << Simulator::Now() << ": " << hex << req->address << ": Response handler have sent a stop cmd queue signal " << dec << req->cmdType << " and tag " << req->tag << endl;
#endif
	    }
//	    delete (Request*)e->event_data.at(0);	
	    delete req;		
	    delete event;		
	break;
	case REPLY:	
	     cmd = *((DRAMCmdState*)e->event_data.at(0));

	     vector<Request>::iterator bufferIndex = responseBuffer.begin();
	     unsigned int index = SearchBuffer(&cmd);		// Search the req, returns the index
	     if (!(NI*)child->niQueue.empty())
	     {
#ifdef DEEP_DEBUG
	      cout << dec << Simulator::Now() << ": " << hex << cmd.req.address << ": I am in Response Handler REPLY with tag " << dec << cmd.req.tag << endl;
#endif	
		*req = responseBuffer[index];
		req->retireTime = Simulator::Now()+1;
		(NI*)child->niQueue[0] = req;				// put the request into NI queue if its free
		responseBuffer.erase(index+bufferIndex);		// delete the req ferom response buffer	
		
		if (stoppedQueue && CanStart())
	        {
		    IrisEvent *event2 = new IrisEvent();	
		    event2->src = (Component*)this;
		    event2->dst = (Component*)reqPtr;	// stop cmd queue from receiving any more 	
		    event2->type = START_CMD_QUEUE;
		    stoppedQueue = false;	
		    Simulator::Schedule(Simulator::Now()+1, &RequestHandler::process_event, (RequestHandler*)event2->dst, event2);
#ifdef DEEP_DEBUG
	cout << dec << Simulator::Now() << ": " << hex << req->address << ": Response handler have sent a start cmd queue signal " << dec << req->cmdType << " and tag " << req->tag << endl;
#endif
		}			

#ifdef DEBUG	    
	cout << dec << Simulator::Now()+1 << ": " << hex << cmd.req.address << ": Total Time taken by request " << dec << req->retireTime - req->arrivalTime << endl;
#endif	 

 	     }
	     else
	     {			 	
	     	event->src = (Component*)this;
	     	event->dst = (Component*)this;		
	     	event->type = REPLY;	
	     	event->event_data.push_back((void*)cmd);
	     	Simulator::Schedule(Simulator::Now()+1, &ResponseHandler::process_event, (ResponseHandler*)event->dst, event); 
	     } 	

	    delete (DRAMCmdState*)e->event_data.at(0);	
	    break;	
	default:
		cout << dec << Simulator::Now() << ": I should not come to default of process event in response handler\n";
    }		
    delete e;
}

///////// Search the response buffer. Find the request, erase it from the buffer and return it ////////

unsigned int ResponseHandler::SearchBuffer(DRAMCmdState *cmd)
{		
    for (unsigned int i=0; i<responseBuffer.size(); i++)
    {
	if (responseBuffer[i].tag == cmd->req.tag)
	{	
	    return i;
	}
    }	    
    cout << "Error: scheduled cmd could not be found in Response Buffer\n";
    return -1;	
}

bool ResponseHandler::IsBufferFull()
{
    if ( responseBuffer.size() >= RESPONSE_BUFFER_SIZE-1 )
    {
	bufferFull = true;	
        return true;
    }
    else
    { 
	bufferFull = false;
        return false;
    }
}

bool ResponseHandler::CanStart()
{
    if ( responseBuffer.size() <= 0.75*(RESPONSE_BUFFER_SIZE) )
        return true;
    else 
        return false;
}

std::string ResponseHandler::toString()
{
//	cout << bufferId <<endl;
	return "ResponseHandler";
}

