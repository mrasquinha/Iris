/*
 * =====================================================================================
 *
 *       Filename:  channel_handler.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/13/2010 05:30:27 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Mitchelle Rasquinha (), mitchelle.rasquinha@gatech.edu
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */

#ifndef  channel_handler_h_INC
#define  channel_handler_h_INC

#include        <string>
#include        <sstream>
#include        <iostream>
#include        <cstdlib>
#include        <map>
#include        "../kernel/component.h"
#include        "../kernel/simulator.h"
#include        "../MemCtrl/request.h"
#include	"../MemCtrl/rank_handler.h" 
#include 	"../MemCtrl/constants.h"

using namespace std;

/*  
 *  =====================================================================================
 *      Class:  ChannelHandler
 *      Description:  
 *     
 *     
 *   =====================================================================================
 */

class ChannelHandler
{
    public:
        ChannelHandler ();                             /*  constructor */
        ~ ChannelHandler ();
	short chanId;
	RankHandler rank[NO_OF_RANKS];                                             
    protected:
                                                                            
    private:
                                                                            
}; /*  -----  end of class ChannelHandler  ----- */

#endif   /*  ----- #ifndef channel_handler_INC  ----- */
