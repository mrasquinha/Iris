/*
 * =====================================================================================
 *
 *       Filename:  dram.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/23/2010 07:25:51 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  SYED MINHAJ HASSAN
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */

#ifndef  dram_h_INC
#define  dram_h_INC

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

using namespace std;

class DRAMChannel : public Component
{
    public:
        DRAMChannel ();                             /*  constructor */
        ~ DRAMChannel ();
	Component* mc;
	Component* parent;
	Component* child;
        void process_event (IrisEvent* e);
        std::string toString();
                                                                            
    protected:
                                                                            
    private:
                                                                            
}; /*  -----  end of class DRAMChannel  ----- */


/*  
 *  =====================================================================================
 *      Class:  DRAM
 *      Description:  
 *     
 *     
 *   =====================================================================================
 */

class DRAM : public Component
{
    public:
        DRAM ();                             /*  constructor */
        ~ DRAM ();
	Component* mc;
	Component* parent;
	DRAMChannel dc[NO_OF_CHANNELS]; 
        void process_event (IrisEvent* e);
	void SetLinks();
        std::string toString();
                                                                            
    protected:
                                                                            
    private:
                                                                            
}; /*  -----  end of class DRAM  ----- */

#endif   /*  ----- #ifndef dram_INC  ----- */
