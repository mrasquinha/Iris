/*
 * =====================================================================================
 *
 *       Filename:  util.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/25/2010 05:47:17 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Mitchelle Rasquinha (), mitchelle.rasquinha@gatech.edu
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */
#ifndef  _util_cc_INC
#define  _util_cc_INC

#include	"util.h"

void timed_cout ( string str)
{
    cout << "\nTime: " << Simulator::Now() << " " << str;
}
#endif   /* ----- #ifndef _util_cc_INC  ----- */


