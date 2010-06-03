/*
 * =====================================================================================
 *
 *       Filename:  stats.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/13/2010 06:53:30 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  SYED MINHAJ HASSAN
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */
#include "MC.h"
#include "../MemCtrl/stats.h"

void Statistic::CollectStatsPerRequest(Request *req)
{
 //   if (!(*doneOnce[req->threadId]))	
    {	
    ///////// calculating runing average ////////////////

    avgLatPerThreadPerBank[req->threadId][req->bankNo] 
	= ( avgLatPerThreadPerBank[req->threadId][req->bankNo] * 1.0 / (reqPerThreadPerBank[req->threadId][req->bankNo] + 1) )
	* ( reqPerThreadPerBank[req->threadId][req->bankNo] * 1.0 )	
	+ ( (req->retireTime - req->arrivalTime) * 1.0 / (reqPerThreadPerBank[req->threadId][req->bankNo] + 1) );
    
    avgThrottlePerThreadPerBank[req->threadId][req->bankNo] 
	= ( avgThrottlePerThreadPerBank[req->threadId][req->bankNo] * 1.0 / (reqPerThreadPerBank[req->threadId][req->bankNo] + 1) )
	* ( reqPerThreadPerBank[req->threadId][req->bankNo] * 1.0 )	
	+ ( req->throttleTime * 1.0 / (reqPerThreadPerBank[req->threadId][req->bankNo] + 1) );

    reqPerThreadPerBank[req->threadId][req->bankNo]++;	
    latPerThreadPerBank[req->threadId][req->bankNo] += (req->retireTime - req->arrivalTime);
    throttlePerThreadPerBank[req->threadId][req->bankNo] += req->throttleTime;	
    if (req->status == OPEN)    
	hitsPerThreadPerBank[req->threadId][req->bankNo]++;
    hitRatePerThreadPerBank[req->threadId][req->bankNo] =  hitsPerThreadPerBank[req->threadId][req->bankNo] * 100.0
							 / reqPerThreadPerBank[req->threadId][req->bankNo];  
    }
//      cout << req->status << endl;
//    cout << "Latency " << latPerThreadPerBank[req->threadId][req->bankNo] << endl;
//    cout << "Average Latency " << avgLatPerThreadPerBank[req->threadId][req->bankNo] << endl;
//    cout << "Requests " << reqPerThreadPerBank[req->threadId][req->bankNo] << endl;
//    cout << "Hits " << hitsPerThreadPerBank[req->threadId][req->bankNo] << endl;
//    cout << "Hit Rate " << hitRatePerThreadPerBank[req->threadId][req->bankNo] << endl;
	
}

void Statistic::CollectStatsPerCycle()
{
    for (unsigned int i=0; i<NO_OF_CHANNELS; i++)
    {
	for (unsigned int j=0; j<NO_OF_RANKS; j++)
    	{
	    for (unsigned int k=0; k<NO_OF_BUFFERS; k++)
    	    {
		if ( ((MC*)mc)->reqH->chan[i].rank[j].rbuffer[k].empty() )
		    emptyCycles[i][j][k]++;
		rbufferOccupancy[i][j][k] += ((MC*)mc)->reqH->chan[i].rank[j].rbuffer[k].size();
	    }
	}
    }		
    
}

void Statistic::InitStats()
{
    for (unsigned int i=0; i<NO_OF_CHANNELS; i++)
    {
	for (unsigned int j=0; j<NO_OF_RANKS; j++)
    	{
	    for (unsigned int k=0; k<NO_OF_BUFFERS; k++)
    	    {
		emptyCycles[i][j][k] = 0;
		rbufferOccupancy[i][j][k] = 0;
	    }	
	}
    }		
/*    for (unsigned int i=0; i<NO_OF_CHANNELS; i++)
    {
	for (unsigned int j=0; j<NO_OF_RANKS; j++)
    	{
	    for (unsigned int k=0; k<NO_OF_BUFFERS; k++)
    	    {
		rbufferOccupancy[i][j][k] += ((MC*)mc)->reqH->chan[i].rank[j].rbuffer[k].size();
	    }
	}
    }		
*/	
    for (unsigned int i=0; i<NO_OF_THREADS; i++)	
    {
	avgLatPerThread[i] = 0.0;
	latPerThread[i] = 0;
	reqPerThread[i] = 0;
    	hitsPerThread[i] = 0;
	hitRatePerThread[i] = 0.0;
    	BLPPerThread[i] = 0.0;
	avgThrottlePerThread[i] = 0.0;
	throttlePerThread[i] = 0;
    }

    for (unsigned int i=0; i<NO_OF_BANKS; i++)
    {
	avgLatPerBank[i] = 0.0;
	latPerBank[i] = 0;
    	reqPerBank[i] = 0;
    	hitsPerBank[i] = 0;
    	hitRatePerBank[i] = 0.0;
	avgThrottlePerBank[i] = 0.0;
	throttlePerBank[i] = 0;
    }	

    for (unsigned int i=0; i<NO_OF_THREADS; i++)	
    {
	for (unsigned int j=0; j<NO_OF_BANKS; j++)
        {	 
	    avgLatPerThreadPerBank[i][j] = 0.0;
	    latPerThreadPerBank[i][j] = 0;   
	    reqPerThreadPerBank[i][j] = 0;
	    hitsPerThreadPerBank[i][j] = 0;
	    hitRatePerThreadPerBank[i][j] = 0.0;
	    avgThrottlePerThreadPerBank[i][j] = 0.0;
	    throttlePerThreadPerBank[i][j] = 0;
	}
    }
}

void Statistic::CalculateAggregateStats()
{
    for (unsigned int i=0; i<NO_OF_CHANNELS; i++)
    {
	for (unsigned int j=0; j<NO_OF_RANKS; j++)
    	{
	    for (unsigned int k=0; k<NO_OF_BUFFERS; k++)
    	    {
		rbufferOccupancyRatio[i][j][k] = rbufferOccupancy[i][j][k] / (Simulator::Now()-emptyCycles[i][j][k]);
	    }
	}
    }		
    for (unsigned int j=0; j<NO_OF_BANKS; j++)
    {    	
	for (unsigned int i=0; i<NO_OF_THREADS; i++)	
    	{
	    
	    latPerBank[j] += latPerThreadPerBank[i][j];
	    throttlePerBank[j] += throttlePerThreadPerBank[i][j];	
	    reqPerBank[j] += reqPerThreadPerBank[i][j];
	    hitsPerBank[j] += hitsPerThreadPerBank[i][j];
	    avgLatPerBank[j] += (avgLatPerThreadPerBank[i][j] * reqPerThreadPerBank[i][j]);
	    avgThrottlePerBank[j] += (avgThrottlePerThreadPerBank[i][j] * reqPerThreadPerBank[i][j]);		
    	}
    	avgLatPerBank[j] = avgLatPerBank[j] / reqPerBank[j];
	avgThrottlePerBank[j] = avgThrottlePerBank[j] / reqPerBank[j];
        hitRatePerBank[j] += hitsPerBank[j] * 100.0 / reqPerBank[j];
    }
    
    for (unsigned int i=0; i<NO_OF_THREADS; i++)	
    {   	
	for (unsigned int j=0; j<NO_OF_BANKS; j++)
    	{ 
	    latPerThread[i] += latPerThreadPerBank[i][j];
	    throttlePerThread[i] += throttlePerThreadPerBank[i][j];
	    reqPerThread[i] += reqPerThreadPerBank[i][j];
	    hitsPerThread[i] += hitsPerThreadPerBank[i][j];
	    avgLatPerThread[i] += (avgLatPerThreadPerBank[i][j] * reqPerThreadPerBank[i][j]);
	    avgThrottlePerThread[i] += (avgThrottlePerThreadPerBank[i][j] * reqPerThreadPerBank[i][j]);		
    	}
	avgLatPerThread[i] = avgLatPerThread[i] / reqPerThread[i];
	avgThrottlePerThread[i] = avgThrottlePerThread[i] / reqPerThread[i];
        hitRatePerThread[i] += hitsPerThread[i] * 100.0 / reqPerThread[i];
    }		
}

string Statistic::PrintAggregateStats()
{
    stringstream str;
    str << dec;	
/*    for (unsigned int i=0; i<NO_OF_THREADS; i++)	
    {   	
	for (unsigned int j=0; j<NO_OF_BANKS; j++)
    	{ 
	    str << "Total Request  for Thread " << i << ", bank " << j << " = " << reqPerThreadPerBank[i][j] << endl;
	}
        str << endl;
    }
    str << endl;			

    for (unsigned int i=0; i<NO_OF_THREADS; i++)	
    {   	
	for (unsigned int j=0; j<NO_OF_BANKS; j++)
    	{ 
		str << "Avg Latency for Thread " << i << ", bank " << j << " = " << avgLatPerThreadPerBank[i][j] << endl;
	}
        str << endl;
    }
    str << endl;

    for (unsigned int i=0; i<NO_OF_THREADS; i++)	
    {   	
	for (unsigned int j=0; j<NO_OF_BANKS; j++)
    	{ 
		cout << "Avg Throttling time for Thread " << i << ", bank " << j << " = " << avgThrottlePerThreadPerBank[i][j] << endl;
	}
        cout << endl;
    }
    cout << endl;		
    
    for (unsigned int i=0; i<NO_OF_THREADS; i++)	
    {   	
	for (unsigned int j=0; j<NO_OF_BANKS; j++)
    	{ 
	    str << "Hits for Thread " << i << ", bank " << j << " = " << hitsPerThreadPerBank[i][j] << endl;
	}
        str << endl;
    }	


    for (unsigned int i=0; i<NO_OF_THREADS; i++)	
    {   	
	for (unsigned int j=0; j<NO_OF_BANKS; j++)
    	{ 
	    str << "Hit Rate for Thread " << i << ", bank " << j << " = " << hitRatePerThreadPerBank[i][j] << endl;
	}
        str << endl;
    }	

*/

    for (unsigned int i=0; i<NO_OF_THREADS; i++)	
    {   	
	str << "Total Request  for Thread " << i << " = " << reqPerThread[i] << endl;
    }
    str << endl;
   
    for (unsigned int i=0; i<NO_OF_THREADS; i++)	
    {   	
	str << "Hits  for Thread " << i << " = " << hitsPerThread[i] << endl;
    }
    str << endl;

    for (unsigned int i=0; i<NO_OF_THREADS; i++)	
    {   	
	str << "Hit Rate  for Thread " << i << " = " << hitRatePerThread[i] << endl;
    }
    str << endl;

    for (unsigned int i=0; i<NO_OF_THREADS; i++)	
    {   	
	str << "Average Latency  for Thread " << i << " = " << avgLatPerThread[i] << endl;
    }
    str << endl;

/*    for (unsigned int i=0; i<NO_OF_THREADS; i++)	
    {   	
	str << "Average Throttling time for Thread " << i << " = " << avgThrottlePerThread[i] << endl;
    }
    str << endl;
*/

 				
    for (unsigned int i=0; i<NO_OF_BANKS; i++)	
    {   	
	str << "Total Request  for Bank " << i << " = " << reqPerBank[i] << endl;
    }
    str << endl;
   
    for (unsigned int i=0; i<NO_OF_BANKS; i++)	
    {   	
	str << "Hits  for Bank " << i << " = " << hitsPerBank[i] << endl;
    }
    str << endl;

    for (unsigned int i=0; i<NO_OF_BANKS; i++)	
    {   	
	str << "Hit Rate  for Bank " << i << " = " << hitRatePerBank[i] << endl;
    }
    str << endl;

    for (unsigned int i=0; i<NO_OF_BANKS; i++)	
    {   	
	str << "Average Latency  for Bank " << i << " = " << avgLatPerBank[i] << endl;
    }
    str << endl;

/*    for (unsigned int i=0; i<NO_OF_BANKS; i++)	
    {   	
	str << "Average Throttling time for Bank " << i << " = " << avgThrottlePerBank[i] << endl;
    }
    str << endl;				
*/
/*     for (unsigned int i=0; i<NO_OF_CHANNELS; i++)
    {
	for (unsigned int j=0; j<NO_OF_RANKS; j++)
    	{
	    for (unsigned int k=0; k<NO_OF_BUFFERS; k++)
    	    {
		str << "Average Occupancy for Channel " << i << ", " << " Rank " << j << ", " << " Bank " << ", " << k << " = " << rbufferOccupancyRatio[i][j][k] << endl;
	    }
	}
  }
    str << endl;	

    for (unsigned int i=0; i<NO_OF_CHANNELS; i++)
    {
	for (unsigned int j=0; j<NO_OF_RANKS; j++)
    	{
	    for (unsigned int k=0; k<NO_OF_BUFFERS; k++)
    	    {
		str << "Total Occupancy Cycles for Channel " << i << ", " << " Rank " << j << ", " << " Bank " << ", " << k << " = " << Simulator::Now() - emptyCycles[i][j][k] << endl;
	    }
	}
    }
    str << endl;			

    for (unsigned int i=0; i<NO_OF_CHANNELS; i++)
    {
	for (unsigned int j=0; j<NO_OF_RANKS; j++)
    	{
	    for (unsigned int k=0; k<NO_OF_BUFFERS; k++)
    	    {
		str << "Empty Cycles for Channel " << i << ", " << " Rank " << j << ", " << " Bank " << ", " << k << " = " << emptyCycles[i][j][k] << endl;
	    }
	}
    }
    str << endl;	
*/
    str << "Total Cycles = " << Simulator::Now() << endl;

    return str.str();
}
