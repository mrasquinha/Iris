/*
 * =====================================================================================
 *
 *       Filename:  MC.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/07/2010 07:15:49 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  SYED MINHAJ HASSAN
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */

#ifndef  MC_h_INC
#define  MC_h_INC

#include        "../kernel/component.h"
#include        "../kernel/simulator.h"
#include        "../data_types/impl/irisEvent.h"
#include        "../MemCtrl/request.h"
#include	"../MemCtrl/request_handler.h" 
#include        "../MemCtrl/bus_handler.h"
#include        "../MemCtrl/bus.h"
#include        "../MemCtrl/dram.h"
#include        "../MemCtrl/NI.h"
#include	"../MemCtrl/refresh_manager.h"
#include	"../MemCtrl/response_handler.h"
#include	"../MemCtrl/stats.h"

class MC
{
    public:
    	NI *ni;
	RequestHandler *reqH;
	RefreshMgr *refMgr;
	Bus *bus;
	DRAM *dram;
	ResponseHandler *responseH;
	Statistic *stats;
	Component *parent;
	bool * doneOnce[NO_OF_THREADS];
	void Init();
	void StartRefresh();
	MC ()
    	{
	    stats = new Statistic();
	//    ni = new NI();	
	    reqH = new RequestHandler();
	    refMgr = new RefreshMgr();
	    bus = new Bus();
	    dram = new DRAM();
	    responseH = new ResponseHandler();		
	};
    	~MC () 
	{
	    delete reqH;
	    delete refMgr;
	    delete bus;
	    delete responseH;
	    delete dram;
	//    delete ni;			
	};
};/*  -----  end of class MC  ----- */

#endif   /*  ----- #ifndef MC_INC  ----- */
