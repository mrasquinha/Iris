/*
 * =====================================================================================
 *
 *       Filename:  stats.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/13/2010 06:53:38 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  SYED MINHAJ HASSAN
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */

#ifndef  stats_h_INC
#define  stats_h_INC

#include        "../kernel/component.h"
#include        "../kernel/simulator.h"
#include        "../data_types/impl/irisEvent.h"

#include        "../MemCtrl/request.h"

class Statistic
{  	
    ///////////////// Some Counters ///////////////////
//    unsigned long long int Times[NO_OF_THREADS];	// Ugly just to avoid overflow
    	
    public:   
	Component *mc;
	bool *doneOnce[NO_OF_THREADS];
    	double avgLatPerThread[NO_OF_THREADS];	
    	unsigned long long int latPerThread[NO_OF_THREADS];
    	unsigned long long int reqPerThread[NO_OF_THREADS];
    	unsigned long long int hitsPerThread[NO_OF_THREADS];
	unsigned long long int throttlePerThread[NO_OF_THREADS];
	double avgThrottlePerThread[NO_OF_THREADS];
    	double hitRatePerThread[NO_OF_THREADS];
    	float BLPPerThread[NO_OF_THREADS];

    	double avgLatPerBank[NO_OF_BANKS];	
    	unsigned long long int latPerBank[NO_OF_BANKS];
    	unsigned long long int reqPerBank[NO_OF_BANKS];
    	unsigned long long int hitsPerBank[NO_OF_BANKS];
	unsigned long long int throttlePerBank[NO_OF_BANKS];
	double avgThrottlePerBank[NO_OF_BANKS];
    	double hitRatePerBank[NO_OF_BANKS];		

    	double avgLatPerThreadPerBank[NO_OF_THREADS][NO_OF_BANKS];	
    	unsigned long long int latPerThreadPerBank[NO_OF_THREADS][NO_OF_BANKS];	
    	unsigned long long int reqPerThreadPerBank[NO_OF_THREADS][NO_OF_BANKS];
    	unsigned long long int hitsPerThreadPerBank[NO_OF_THREADS][NO_OF_BANKS];
	unsigned long long int throttlePerThreadPerBank[NO_OF_THREADS][NO_OF_BANKS];
	double avgThrottlePerThreadPerBank[NO_OF_THREADS][NO_OF_BANKS];
    	double hitRatePerThreadPerBank[NO_OF_THREADS][NO_OF_BANKS];

	void InitStats();
	void CollectStatsPerRequest(Request *req);
	void CalculateAggregateStats();
	string PrintAggregateStats();
	void CollectStatsPerCycle();
	
	unsigned long long int rbufferOccupancy[NO_OF_CHANNELS][NO_OF_RANKS][NO_OF_BANKS];
	unsigned long long int emptyCycles[NO_OF_CHANNELS][NO_OF_RANKS][NO_OF_BANKS];
	float rbufferOccupancyRatio[NO_OF_CHANNELS][NO_OF_RANKS][NO_OF_BANKS];
//    unsigned long long int NoOccupancyChecks[NO_OF_BANKS];
	
//    unsigned long long int TimesBufferFull[NO_OF_BANKS];
//    unsigned long long int BankReqPerThreadWhenFull[NO_OF_THREADS][NO_OF_BANKS];	

//    unsigned long long int TotalBufferFullTime[NO_OF_BANKS];
//    unsigned long long int CurBufferFullTime[NO_OF_BANKS];

//    unsigned long long int lastBLPCheck;
//    unsigned long long int lastOccupancyCheck;
    ///////////////////////////////////////////////////  	
};/*  -----  end of class stats  ----- */

#endif   /*  ----- #ifndef stats_INC  ----- */
