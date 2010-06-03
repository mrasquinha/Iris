/*
 * =====================================================================================
 *
 *       Filename:  mshr.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/12/2010 12:43:25 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  SYED MINHAJ HASSAN
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */

#ifndef  mshr_h_INC
#define  mshr_h_INC

#include        <string>
#include        <sstream>
#include        <iostream>
#include 	<iomanip>
#include        <cstdlib>
#include 	<fstream>
#include        <map>
#include        <math.h>
#include        "../kernel/component.h"
#include        "../kernel/simulator.h"
#include        "../data_types/impl/irisEvent.h"
#include        "../memctrl/request.h"
#include        "../components/impl/genericEvents.h"

using namespace std;

/*  
 *  =====================================================================================
 *      Class:  MSHR_H
 *      Description:  
 *     
 *     
 *   =====================================================================================
 */
extern uint MSHR_SIZE;
class MSHR_H : public Component
{
    public:
        MSHR_H ();                             /*  constructor */
        ~ MSHR_H ();
	ull_int unsink;
	ull_int lastFinishull_int;
	ull_int globalUnSink;
	bool done;
	vector <Request> mshr;
	vector <Request> writeQueue;
	ifstream trace_filename;
	char* filename;
	unsigned int id; 
	unsigned int lastScheduledIndex;
	Component* parent;
	Component* child;
	ull_int GlobalAddrMap(ull_int addr, uint threadId);
        void process_event (IrisEvent* e);
	void DeleteInMSHR(Request* req);	
        void demap_addr(ull_int oldAddress, ull_int newAddress);
	bool waiting;
	Request waitingForMSHR;
	Request nextReq;
	ull_int lastFullull_int;
        std::string toString();
                                                                            
    protected:
                                                                            
    private:
                                                                            
}; /*  -----  end of class MSHR_H  ----- */

#endif   /*  ----- #ifndef mshr_INC  ----- */
