/*
 * =====================================================================================
 *
 *       Filename:  bank_handler.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/13/2010 06:38:44 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  SYED MINHAJ HASSAN
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */

#ifndef  bank_handler_h_INC
#define  bank_handler_h_INC

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
#include	"../MemCtrl/addr_map.h"
#include	"../MemCtrl/bus_handler.h"

using namespace std;

/*  
 *  =====================================================================================
 *      Class:  Bank_Handler
 *      Description:  
 *     
 *     
 *   =====================================================================================
 */

enum CallerType {READS_FIRST, HIT_FIRST, OLDEST_FIRST, UNMARKED_FIRST, HIGHEST_RANKED_FIRST};

class BankHandler : public Component
{
    public:
        BankHandler ();                             /*  constructor */
        ~ BankHandler ();
        UInt bankId;
	UInt bufferId;
        void* myRank;
        Component* parent;
	bool stoppedUpper;
	bool stopSignal;
	bool bufferFull;
        void process_event(IrisEvent* e);
	bool HasWork();
	bool StopUpper();
	bool CanUpperStart();
        bool MainScheduler(Request* req, int* index);
        bool PARBS(Request* req, int* index);
        bool FRFCFS(Request* req, int* index);
        bool FCFS(Request* req, int* index);
	bool OldestFirst(Request* req, int* index);
	bool RowHitFirst(Request* req, int* index);
	bool ReadsFirst(Request* req, int* index);
	bool HighestRankedFirst(Request* req, UInt highest, int* index);
	bool ScheduleUnmarked(Request* req, int* index);
	void FindRank(int priority[]);
	UInt FindHighest();
	bool IsBufferFull();
	void SetReadsOWrite(CallerType caller, Command_t cmdType);
	void SetBypasses(unsigned int index);
	void SetPrevState();
	void RestorePrevState();

	UInt rowBufferIndex;
	UInt prevBufferIndex;
	bool rowOpen;
	unsigned int bypassReq;
        bool prevRowOpen;
    	unsigned int prevBypassReq;
	unsigned int bankTag;	

        std::string toString();

        unsigned long long int bufferOccupancyCounter;
                                                                            
    protected:
                                                                            
    private:
       bool generated;
		uint address;
                                                                            
}; /*  -----  end of class BankHandler  ----- */

#endif   /*  ----- #ifndef bank_handler_INC  ----- */

