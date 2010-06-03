/*
 * =====================================================================================
 *
 *       Filename:  request.h
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

#ifndef  request_h_INC
#define  request_h_INC

#include        <string>
#include        <sstream>
#include        <iostream>
#include        <cstdlib>
#include        <map>
#include        "../kernel/component.h"
#include        "../kernel/simulator.h"
#include        "../components/interfaces/genericComponentHeader.h"

using namespace std;

/*  
 *  =====================================================================================
 *      Class:  Request
 *      Description:  
 *     
 *     
 *   =====================================================================================
 */

enum CStatus {OPEN, CLOSED, CONFLICT, IDLE};
enum Command_t {CACHE_NOP, CACHE_READ, CACHE_WRITE, CACHE_WRITEBACK, CACHE_PREFETCH, REFRESH};
extern uint NO_OF_CHANNELS;
extern uint NO_OF_RANKS;
extern uint NO_OF_BANKS;
extern uint NO_OF_COLUMNS;
extern uint NO_OF_ROWS;

struct Data
{
    unsigned long long int value;
    short size;	
};
typedef Data Data;

class Request
{
    public:
        Request();
        ~ Request();
        uint channelNo;
        uint dimmNo;
        uint rankNo;
        uint bankNo;
        uint columnNo;
        uint rowNo;
        Command_t cmdType;
        uint threadId; 
        ull_int address;
        ull_int arrivalull_int;
        ull_int retireull_int;
	ull_int throttleull_int;
	Data data;
        bool mark;
        bool local;
//	int indexWhenScheduled; 
	bool scheduledInMSHR;
	CStatus status;
	int tag;
	
                                                                            
    protected:
                                                                            
    private:

}; /* ----- end of class Request ------ */

#endif   /*  ----- #ifndef request_INC  ----- */
