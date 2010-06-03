/*
 * =====================================================================================
 *
 *       Filename:  response_handler.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/04/2010 02:06:41 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  SYED MINHAJ HASSAN
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */

#ifndef  response_handler_h_INC
#define  response_handler_h_INC

#include        <string>
#include        <sstream>
#include        <iostream>
#include        <cstdlib>
#include        <map>
#include        "../kernel/component.h"
#include        "../kernel/simulator.h"
#include        "../data_types/impl/irisEvent.h"
#include        "../MemCtrl/request.h"
#include	"../MemCtrl/constants.h" 
#include        "../MemCtrl/NI.h"
#include 	"../MemCtrl/cmd_issuer.h"

using namespace std;

/*  
 *  =====================================================================================
 *      Class:  ResponseHandler
 *      Description:  
 *     
 *     
 *   =====================================================================================
 */

class ResponseHandler : public Component
{
    public:
        ResponseHandler ();                             /*  constructor */
        ~ ResponseHandler ();
	Component* mc;
	Component* parent;
	Component* child;
	vector<Request> responseBuffer;
	bool bufferFull;
        bool serviced[RESPONSE_BUFFER_SIZE];
	bool IsBufferFull();
	Component* reqPtr;
	bool stoppedQueue;
	bool CanStart();
	unsigned int SearchBuffer(DRAMCmdState *cmd);
        unsigned int SendServiced();
        void process_event (IrisEvent* e);
        std::string toString();
                                                                            
    protected:
                                                                            
    private:
                                                                            
}; /*  -----  end of class ResponseHandler  ----- */

#endif   /*  ----- #ifndef response_handler_INC  ----- */
