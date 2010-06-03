/*
 * =====================================================================================
 *
 *       Filename:  rank_handler.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/13/2010 05:30:27 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  SYED MINHAJ HASSAN
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */

#ifndef  rank_handler_h_INC
#define  rank_handler_h_INC

#include        <string>
#include        <sstream>
#include        <iostream>
#include        <cstdlib>
#include        <map>
#include        "../kernel/component.h"
#include        "../kernel/simulator.h"
#include        "../MemCtrl/request.h"
#include	"../MemCtrl/bank_handler.h"
#include	"../MemCtrl/constants.h"

using namespace std;

/*  
 *  =====================================================================================
 *      Class:  RankHandler
 *      Description:  
 *     
 *     
 *   =====================================================================================
 */

typedef vector<Request> ReqBuffer;

class RankHandler
{
    public:
        RankHandler ();                             /*  constructor */
        ~ RankHandler ();
        short rankId;
	ReqBuffer rbuffer[NO_OF_BUFFERS];
	BankHandler bank[NO_OF_BANKS];
	unsigned int readsOWrite;
	unsigned int prevReadsOWrite;                                             
    protected:
                                                                            
    private:
                                                                            
}; /*  -----  end of class RankHandler  ----- */

#endif   /*  ----- #ifndef rank_handler_INC  ----- */
