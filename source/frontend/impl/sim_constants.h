/*
 * =====================================================================================
 *
 *       Filename:  sim_constants.cc
 *
 *    Description:  This file contains all global parameters set to the
 *    default values that should be overwritten at the command line or in the
 *    configuration file of the simulator.
 *
 *        Version:  1.0
 *        Created:  05/18/2010 04:09:28 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Mitchelle Rasquinha (), mitchelle.rasquinha@gatech.edu
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */
#include	"../../components/interfaces/genericComponentHeader.h"
#ifndef  _sim_constants_cc_INC
#define  _sim_constants_cc_INC


uint MC_ADDR_BITS = 18;
uint NUM_OF_CONTROLLERS = 8;
uint THREAD_BITS_POSITION = 22;
uint no_nodes ;
uint no_mcs;
uint send_early_credit = 0;

bool USE_MSHR = true;
uint MSHR_SIZE = 8;
uint CACHE_BLOCK_SIZE = 64;
uint NETWORK_ADDRESS_BITS = 32;
uint NETWORK_COMMAND_BITS = 3;
uint NETWORK_THREADID_BITS = 6;

uint NO_OF_CHANNELS = 1;		//  (int)log2() = k bits. 
uint NO_OF_RANKS = 2;       		//  (int)log2() = l bits.
uint NO_OF_BANKS = 8;         		//  (int)log2() = b bits. 
uint NO_OF_ROWS = 8192; 		//4096 //  (int)log2() = r bits. 
uint NO_OF_COLUMNS = 128;         	//  (int)log2() = c bits. 

#endif   /* ----- #ifndef _sim_constants_cc_INC  ----- */
