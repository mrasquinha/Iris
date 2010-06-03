/*
 * =====================================================================================
 *
 *       Filename:  constants.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/25/2010 11:18:28 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  SYED MINHAJ HASSAN
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */
#ifndef _CONSTANTS_H
#define _CONSTANTS_H
#include <math.h>

typedef unsigned long long int Time;
typedef unsigned long long int Addr_t;
typedef unsigned int UInt;

#define OPEN_PAGE_POLICY 1
//#define CLOSE_PAGE_POLICY 1

//#define PAR_BS 1
#define FR_FCFS 1
//#define FC_FS 1
//#define NFQ 1

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////      	Following schemes are developed under the following assumptions
//////// 	1) Page Size = Row Size
////////	2) Coulumn size cannot be larger than cache block size
////////	3) Tag bilastFinishTimets t should be less than k+l+r
////////	4) Cache block size * blocks per row = cols per row * column size = row * size
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	 
#define PAGE_INTERLEAVING 1
//#define CACHELINE_INTERLEAVING 1
//#define PERMUTATION 1
//#define SWAPPING 1
//#define NO_SCHEME 1
//#define GENERIC 1
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define NO_OF_THREADS 64
#define NO_OF_CHANNELS 1		//  (int)log2() = k bits. 
#define NO_OF_RANKS 2       		//  (int)log2() = l bits.
#define NO_OF_BANKS 8         		//  (int)log2() = b bits. 
#define NO_OF_BUFFERS NO_OF_BANKS
#define NO_OF_ROWS 8192 		//4096 //  (int)log2() = r bits. 
#define NO_OF_COLUMNS 	128         	//  (int)log2() = c bits. 
#define COLUMN_SIZE 64        		//  (int)log2() = v bits.  Column Size = 2bytes
#define BLOCKS_PER_ROW 128          	//  (int)log2() = n bits.  Cache line Per Row
#define CACHE_BLOCK_SIZE 64         	//  (int)log2() = z bits.  L2 Cache Block Size
#define ROW_SIZE NO_OF_COLUMNS*COLUMN_SIZE //(Also equal to BLOCKS_PER_ROW*CACHE_BLOCK_SIZE)
#define DRAM_SIZE NO_OF_CHANNELS*NO_OF_RANKS*NO_OF_BANKS*NO_OF_ROWS*ROW_SIZE
#define TAG_BITS 8              	// t bits

#define USE_MSHR 1 
#define MSHR_SIZE 8

//#define THREAD_BITS_POSITION 22
//#define CORE_UNCORE_RATIO 1
#define MAX_BUFFER_SIZE 8
#define MAX_CMD_BUFFER_SIZE 3
#define RESPONSE_BUFFER_SIZE 64*8 //(NO_OF_CHANNELS * (NO_OF_BANKS*NO_OF_RANKS*MAX_BUFFER_SIZE + MAX_CMD_BUFFER_SIZE)) / 4  // Size = Upper Limit / 2

#define BATCH_FORM_TIME 2000;
//#define MAX_BATCH_TIME 2000;
#define MAX_BATCH_SIZE 5
#define MAX_READ_OV_WRITE 8

#define NETWORK_ADDRESS_BITS 32
#define NETWORK_THREADID_BITS 6
#define NETWORK_COMMAND_BITS 3 

#define READ_SIZE CACHE_BLOCK_SIZE
#define WRITE_SIZE CACHE_BLOCK_SIZE
#define PREFETCH_SIZE CACHE_BLOCK_SIZE
#define WRITEBACK_SIZE CACHE_BLOCK_SIZE

#define REFRESH_PERIOD CORE_SPEED*64000			// 64ms
#define REFRESH_INC floor(REFRESH_PERIOD/(NO_OF_ROWS*NO_OF_RANKS)) - BUS_CYCLE	// -1 BUS_CYCLE to be on the safe side


//  DDR3 1333 -9-9-9 Gbps memory system.  
//  Composed of 2 Gbit chips.  2 ranks, each rank has 8 2Gbit(x8) chips.
//  This is a 64 bit wide interface.
//  Total is 4096 MB
//  Bus Bandwidth is  10.667 GB/s
//  9-9-9 means 19*2 DDR half cycles for t_CAS and (9 / 667) = 13.5ns for t_RCD, t_RP

#define DDR_BUS_WIDTH 8		// 64 bit = 8 bytes
#define BUS_SPEED 1333		
#define CORE_SPEED 3000		// 3G = 3000 cycles in 1us
#define MEM_SPEED 667
#define MEM_CYCLE (CORE_SPEED*1.0 / MEM_SPEED)	
#define BUS_CYCLE (CORE_SPEED*1.0 / BUS_SPEED)

#define CYCLE_2_NS (CORE_SPEED*1.0 / 1000)

#define t_CMD 	ceil (1.0 * BUS_CYCLE)
#define t_RCD 	ceil (13.5 * CYCLE_2_NS)	// Taken from datasheet
#define t_RRD 	ceil (7.5 * CYCLE_2_NS)		// Taken from datasheet
#define t_RAS 	ceil (36 * CYCLE_2_NS)		// Taken from datasheet
#define t_CAS 	ceil (9.0 * MEM_CYCLE)		// 9-9-9 So 9 Mem Cycles
#define t_RTRS 	ceil (1.0 * MEM_CYCLE)
#define t_OST 	ceil (1.0 * MEM_CYCLE)
#define t_WR 	ceil (15 * CYCLE_2_NS)		// Taken from datasheet
#define t_WTR 	ceil (7.5 * CYCLE_2_NS)		// Taken from datasheet
#define t_RP 	ceil (13.5 * CYCLE_2_NS)	// Taken from datasheet
#define t_CCD 	ceil (DDR_BUS_WIDTH/2 * MEM_CYCLE)// Taken from datasheet
#define t_AL 	0				// No posted CAS will have t_AL = 0
#define t_CWD 	t_CAS-t_CMD
#define t_RC 	ceil (49.5 * CYCLE_2_NS)	// Taken from datasheet
#define t_RTP 	ceil (7.5 * CYCLE_2_NS)		// Taken from datasheet
#define t_RFC 	ceil (160 * CYCLE_2_NS)		// Taken from datasheet

/*
//  DDR3 1333 -6-6-6 Gbps memory system.  
//  Composed of 2 Gbit chips.  2 ranks, each rank has 8 2Gbit(x8) chips.
//  This is a 64 bit wide interface.
//  Total is 4096 MB
//  Bus Bandwidth is  10.667 GB/s
//  6-6-6 means 6*2 DDR half cycles for t_CAS and (6 / 667) = 9ns for t_RCD, t_RP


#define DDR_BUS_WIDTH 8		// 64 bit = 8 bytes
#define BUS_SPEED 1333		
#define CORE_SPEED 3000		// 3G = 3000 cycles in 1us
#define MEM_SPEED 667
#define MEM_CYCLE (CORE_SPEED*1.0 / MEM_SPEED)	
#define BUS_CYCLE (CORE_SPEED*1.0 / BUS_SPEED)

#define CYCLE_2_NS (CORE_SPEED*1.0 / 1000)

#define t_CMD 	ceil (1.0 * BUS_CYCLE)
#define t_RCD 	ceil (9 * CYCLE_2_NS)		// Taken from DRAMSim spd
#define t_RRD 	ceil (6 * CYCLE_2_NS)		// TODO FIXME In most cases doesn't matter
#define t_RAS 	ceil (30 * CYCLE_2_NS)		// Taken from DRAMSim spd
#define t_CAS 	ceil (6.0 * MEM_CYCLE)		// 6-6-6 So 6 Mem Cycles
#define t_RTRS 	ceil (1.0 * MEM_CYCLE)
#define t_OST 	ceil (1.0 * MEM_CYCLE)
#define t_WR 	ceil (10 * CYCLE_2_NS)		// Taken from DRAMSim spd
#define t_WTR 	ceil (t_WR / 2)			// TODO FIXME  t_WTR not defined and I took average of 0 and t_WR
#define t_RP 	ceil (9 * CYCLE_2_NS)		// Taken from DRAMSim spd
#define t_CCD 	ceil (DDR_BUS_WIDTH/2 * MEM_CYCLE)// TODO Bus Cycle = Half Mem Cycle. burst of 8 beats
#define t_AL 	0				// No posted CAS will have t_AL = 0
#define t_CWD 	t_CAS-t_CMD
#define t_RC 	ceil (40.5 * CYCLE_2_NS)	// Taken from DRAMSim spd
#define t_RTP 	ceil (3.0 * MEM_CYCLE)		// TODO don't know yet. default in DRAMSim for DDR2 & DDR3 = 6 ddr half cycles 
#define t_RFC 	ceil (210 * CYCLE_2_NS)		// FIXME Taken from DRAMSim spd
*/

//  DDR2 533 4-4-4 Mbps memory system. 
//  Composed of 72 512 Mbit chips. 2 ranks   
//  Total is 4 GB
//  Bus Bandwidth is 4.3 GB/s
//  4-4-4 means 4*2 DDR half cycles for t_CAS and (4 / 266) = 15ns for t_RCD, t_RP

//  Corresponds to MT36HTJ51272Y-53E 4 GB part from Micron, aka PC2-4200 DRAM
// non-registered mode (doesn't add a cycle)

/*
#define DDR_BUS_WIDTH 8		// 64 bit = 8 bytes
#define BUS_SPEED 533		
#define CORE_SPEED 3000		// 3G = 3000 cycles in 1us
#define MEM_SPEED 266		//  artifact of not knowing correct timing values
#define MEM_CYCLE (CORE_SPEED*1.0 / MEM_SPEED)	// artifact of not knowing correct timing values
#define BUS_CYCLE (CORE_SPEED*1.0 / BUS_SPEED)

#define CYCLE_2_NS (CORE_SPEED*1.0 / 1000)

#define t_CMD 	ceil (1.0 * BUS_CYCLE)
#define t_RCD 	ceil (15 * CYCLE_2_NS)		// Taken from datasheet
#define t_RRD 	ceil (7.5 * CYCLE_2_NS)		// TODO Generic for many DDR2. Taken from Data Sheet of some other part
#define t_RAS 	ceil (40 * CYCLE_2_NS)		// t_RAS = t_RC - t_RCD 
#define t_CAS 	ceil (4.0 * MEM_CYCLE)	// 4-4-4 So 4 Mem Cycles
#define t_RTRS 	ceil (1.0 * MEM_CYCLE)
#define t_OST 	ceil (1.0 * MEM_CYCLE)
#define t_WR 	ceil (15 * CYCLE_2_NS)		// Taken from component data sheet
#define t_WTR 	ceil (10 * CYCLE_2_NS)		// TODO Generic for many DDR2. Taken from Data Sheet of some other part
#define t_RP 	ceil (15 * CYCLE_2_NS)		// Taken from datasheet
#define t_CCD 	ceil (2.0 * MEM_CYCLE)		// TODO Generic for many DDR2. Taken from Data Sheet of some other part
#define t_AL 	0				// No posted CAS will have t_AL = 0
#define t_CWD 	t_CAS-t_CMD
#define t_RC 	ceil (55 * CYCLE_2_NS)		// Taken from datasheet
#define t_RTP 	ceil (7.5 * CYCLE_2_NS)		// TODO Generic for many DDR2. Taken from Data Sheet of some other part 
#define t_RFC 	ceil (105 * CYCLE_2_NS)		// Taken from datasheet
*/

//  DDR2 667 4-4-4 Mbps memory system.  
//  Composed of 1 Gbit chips.  1 rank, each rank has 5 1 Gbit (x8) chips.
//  The 5th chip is for ECC, otherwise this is a 32 bit wide interface.
//  Total is 512 MB
//  Bus Bandwidth is 2.67 GB/s
//  4-4-4 means 4*2 DDR half cycles for t_CAS and (4 / 333M) = 12ns for t_RCD, t_RP

/*
#define DDR_BUS_WIDTH 4		// 32 bit = 4 bytes
#define BUS_SPEED 667		
#define CORE_SPEED 3000		// 3G = 3000 cycles in 1us
#define MEM_SPEED 333		//  artifact of not knowing correct timing values
#define MEM_CYCLE (CORE_SPEED*1.0 / MEM_SPEED)	// artifact of not knowing correct timing values
#define BUS_CYCLE (CORE_SPEED*1.0 / BUS_SPEED)

#define CYCLE_2_NS (CORE_SPEED*1.0 / 1000)

#define t_CMD 	ceil (1.0 * BUS_CYCLE)
#define t_RCD 	ceil (12 * CYCLE_2_NS)		// Taken from DRAMSim spd
#define t_RRD 	ceil (6 * CYCLE_2_NS)		// TODO FIXME In most cases doesn't matter
#define t_RAS 	ceil (45 * CYCLE_2_NS)		// Taken from DRAMSim spd
#define t_CAS 	ceil (4.0 * MEM_CYCLE)	// 4-4-4 So 4 Mem Cycles
#define t_RTRS 	ceil (1.0 * MEM_CYCLE)
#define t_OST 	ceil (1.0 * MEM_CYCLE)
#define t_WR 	ceil (15 * CYCLE_2_NS)		// Taken from DRAMSim spd
#define t_WTR 	ceil (t_WR / 2)			// TODO FIXME  t_WTR not defined and I took average of 0 and t_WR
#define t_RP 	ceil (12 * CYCLE_2_NS)		// Taken from DRAMSim spd
#define t_CCD 	ceil (DDR_BUS_WIDTH/2 * MEM_CYCLE)// TODO Bus Cycle = Half Mem Cycle. burst of 4 beats
#define t_AL 	0				// No posted CAS will have t_AL = 0
#define t_CWD 	t_CAS-t_CMD
#define t_RC 	ceil (57 * CYCLE_2_NS)		// Taken from DRAMSim spd
#define t_RTP 	ceil (3.0 * MEM_CYCLE)		// TODO don't know yet. default in DRAMSim for DDR2 & DDR3 = 6 ddr half cycles 
#define t_RFC 	ceil (127.5 * CYCLE_2_NS)	// FIXME Taken from DRAMSim spd
*/

#endif

