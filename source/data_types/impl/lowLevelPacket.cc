/*
 * =====================================================================================
 *
 *       Filename:  LowLevelPacket.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/09/2010 08:56:45 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Mitchelle Rasquinha (), mitchelle.rasquinha@gatech.edu
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */

#ifndef  _lowlevelpacket_cc_INC
#define  _lowlevelpacket_cc_INC

#include	"lowLevelPacket.h"

using namespace std;

/* LowLevelPacket Class Impl */
LowLevelPacket::LowLevelPacket()
{
    source = -1;
    destination = -1;
    transaction_id = -1;
    virtual_channel = 0;
    sent_time = 0;
    length = 0;

}

LowLevelPacket::~LowLevelPacket()
{
    flits.clear();
    control_bits.clear();
    payload.clear();
}		/* -----  end of function LowLevelPacket::~LowLevelPacket()  ----- */

void LowLevelPacket::operator=( const LowLevelPacket* p )
{
    source = p->source;
    destination = p->destination;
    transaction_id = p->transaction_id;
    virtual_channel = p->virtual_channel;
    sent_time = p->sent_time;
    msg_class = p->msg_class;
    length = p->length;
    for ( uint i=0; i<control_bits.size(); i++)
        this->control_bits.push_back(p->control_bits[i]); 

    for ( uint i=0; i<payload.size(); i++)
        this->payload.push_back(p->payload[i]); 
}

string
LowLevelPacket::toString () const
{
    stringstream str;
    str << "LowLevelPacket: " << endl;
    str << "\t Source: " << source 
        << "\t Destination: " << destination
        << "\t virtual_channel: " << virtual_channel 
        << "\t sent time: " << sent_time
        << "\t No of flits: " << flits.size()
        << "\t length: " << length
        << "\t msg_class: " << msg_class
        << endl;

    for ( uint i=0 ; i<flits.size() ; i++ )
        if( flits[i]->type == HEAD)
            str << static_cast<HeadFlit*>(flits[i])->toString();
        else if( flits[i]->type == BODY)
            str << static_cast<BodyFlit*>(flits[i])->toString();
        else if( flits[i]->type == TAIL)
            str << static_cast<TailFlit*>(flits[i])->toString();
        else
            str << " Error unk flit type" ;


    return str.str();
}		/* -----  end of function LoeLevelPacket::toString  ----- */

bool
LowLevelPacket::valid_packet ()
{
    if( flits.size())
        return (flits.size() == static_cast<HeadFlit*>(flits[0])->length);
    else
        return false;

}		/* -----  end of function LowLevelPacket::valid_packet  ----- */

void
LowLevelPacket::add ( Flit* ptr )
{
    switch (ptr->type)
    {
        case HEAD:
            {
                HeadFlit* ptr2 = static_cast < HeadFlit*> (ptr);
                this->source = ptr2->src_address;
                this->destination = ptr2->dst_address;
                this->transaction_id = ptr2->transaction_id;
                this->virtual_channel = ptr2->vc;
                this->length = ptr2->length;
                this->msg_class = ptr2->msg_class;
                this->sent_time = ptr2->packet_originated_time;
    for ( uint i=0; i<ptr2->control_bits.size(); i++)
        this->control_bits.push_back(ptr2->control_bits[i]); 

    for ( uint i=0; i<ptr2->payload.size(); i++)
        this->payload.push_back(ptr2->payload[i]); 

                delete ptr2;
                break;
            }
        case BODY:
            {
                BodyFlit* bf = static_cast<BodyFlit*> (ptr);
    for ( uint i=0; i<bf->bf_data.size(); i++)
        this->payload.push_back(bf->bf_data[i]); 
                delete bf;
                break;
            }
        case TAIL:
            {
                TailFlit* tf = static_cast<TailFlit*> (ptr);
                this->sent_time = tf->packet_originated_time;
                delete tf;
                break;
            }
        default:
            fprintf(stdout,"ERROR Unk flit type");
            break;
    }
    return ;
}		/* -----  end of function LowLevelPacket::add  ----- */

Flit* LowLevelPacket::at(unsigned int index)
{
    if(index > flits.size())
        fprintf(stdout," Error Index out of bounds in lowlevelpacket");

    return flits.at(index);
}

Flit* LowLevelPacket::get_next_flit()
{
    Flit *ptr;
    ptr = flits.front();
    flits.pop_front();
    return ptr;
}

void
LowLevelPacket::clear ()
{
    flits.clear();
    return;
}		/* -----  end of function LowLevelPacket::clear  ----- */

uint
LowLevelPacket::size ()
{
    return flits.size();
}		/* -----  end of function LowLevelPacket::size  ----- */

#endif   /* ----- #ifndef _lowlevelpacket_cc_INC  ----- */

