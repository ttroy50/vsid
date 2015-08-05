/**
 * @author Thom Troy
 *
 * Copyright (C) 2015 Thom Troy
 */

#ifndef __VSID_PACKET_CALLBACKS_H__
#define __VSID_PACKET_CALLBACKS_H__


namespace VsidCommon
{

/**
 * Called when a flow has finished and is being deleted
 *
 */
class PacketVerdict
{
public:
    /**
     * Set an accept verdict on a packet
     * @param id
     */
    virtual void setAccept(uint32_t id) = 0;

    /**
     * Set a drop verdict on a packet
     * @param id
     */
    virtual void setDrop(uint32_t id) = 0;
};

} // end namespace

#endif // END HEADER GUARD