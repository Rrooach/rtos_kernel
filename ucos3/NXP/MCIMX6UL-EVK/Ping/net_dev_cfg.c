/*
*********************************************************************************************************
*                                              uC/TCP-IP
*                                      The Embedded TCP/IP Suite
*
*                         (c) Copyright 2004-2019; Silicon Laboratories Inc.,
*                                400 W. Cesar Chavez, Austin, TX 78701
*
*                   All rights reserved. Protected by international copyright laws.
*
*                  Your use of this software is subject to your acceptance of the terms
*                  of a Silicon Labs Micrium software license, which can be obtained by
*                  contacting info@micrium.com. If you do not agree to the terms of this
*                  license, you may not use this software.
*
*                  Please help us continue to provide the Embedded community with the finest
*                  software available. Your honesty is greatly appreciated.
*
*                    You can find our product's documentation at: doc.micrium.com
*
*                          For more information visit us at: www.micrium.com
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                  NETWORK DEVICE CONFIGURATION FILE
*
*                                              TEMPLATE
*
* Filename : net_dev_cfg.c
* Version  : V3.05.00
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*********************************************************************************************************
*/

#define    MICRIUM_SOURCE
#define    NET_DEV_CFG_MODULE
#include   <Source/net_cfg_net.h>

#ifdef  NET_IF_ETHER_MODULE_EN
#include   <IF/net_if_ether.h>
#endif

#ifdef  NET_IF_WIFI_MODULE_EN
#include   <IF/net_if_wifi.h>
#endif

#ifdef  NET_IF_LOOPBACK_MODULE_EN
#include   <IF/net_if_loopback.h>
#endif


/*
*********************************************************************************************************
*                           EXAMPLE NETWORK INTERFACE / DEVICE CONFIGURATION
*
* Note(s) : (1) (a) Buffer & memory sizes & alignments configured in number of octets.
*               (b) Data bus size                      configured in number of bits.
*
*           (2) (a) All   network  buffer data area sizes MUST be configured greater than or equal to
*                   NET_BUF_DATA_SIZE_MIN.
*               (b) Large transmit buffer data area sizes MUST be configured greater than or equal to
*                   small transmit buffer data area sizes.
*               (c) Small transmit buffer data area sizes MAY need to be configured greater than or
*                   equal to the specific interface's minimum packet size.
*
*               See also 'net_buf.c       NetBuf_PoolCfgValidate()          Note #3'
*                      & 'net_if_ether.c  NetIF_Ether_BufPoolCfgValidate()  Note #2'.
*
*           (3) (a) MUST configure at least one (1) large receive  buffer.
*               (b) MUST configure at least one (1)       transmit buffer, however, zero (0) large OR
*                       zero (0) small transmit buffers MAY be configured.
*
*               See also 'net_buf.c  NetBuf_PoolCfgValidate()  Note #2'.
*
*           (4) Some processors or devices may be more efficient & may even REQUIRE that buffer data areas
*               align to specific CPU-word/octet address boundaries in order to successfully read/write
*               data from/to devices.  Therefore, it is recommended to align devices' buffer data areas to
*               the processor's or device's data bus width.
*
*               See also 'net_buf.h  NETWORK BUFFER INDEX & SIZE DEFINES  Note #2b'.
*
*           (5) Positive offset from base receive/transmit index, if required by device (or driver) :
*
*               (a) (1) Some device's may receive or buffer additional octets prior to the actual  received
*                       packet.  Thus an offset may be required to ignore these additional octets :
*
*                       (A) If a device does NOT receive or buffer any  additional octets prior to received
*                           packets, then the default offset of '0' SHOULD be configured.
*
*                       (B) However, if a device does receive or buffer additional octets prior to received
*                           packets, then configure the device's receive  offset with the number of
*                           additional octets.
*
*                       See also 'net_buf.h  NETWORK BUFFER INDEX & SIZE DEFINES  Note #2b1A'.
*
*                   (2) Some device's/driver's may require additional octets prior to the actual transmit
*                       packet.  Thus an offset may be required to reserve additional octets :
*
*                       (A) If a device/driver does NOT require any  additional octets prior to  transmit
*                           packets, then the default offset of '0' SHOULD be configured.
*
*                       (B) However, if a device/driver does require additional octets prior to  transmit
*                           packets, then configure the device's transmit offset with the number of
*                           additional octets.
*
*                       See also 'net_buf.h  NETWORK BUFFER INDEX & SIZE DEFINES  Note #2b1B'.
*
*               (b) Since each network buffer data area allocates additional octets for its configured
*                   offset(s) [see 'net_if.c  NetIF_BufPoolInit()  Note #3'], the network buffer data
*                   area size does NOT need to be increased by the number of additional offset octets.
*
*           (6) Flags to configure (optional) device features; bit-field flags logically OR'd :
*
*               (a) NET_DEV_CFG_FLAG_NONE           No device configuration flags selected.
*
*               (b) NET_DEV_CFG_FLAG_SWAP_OCTETS    Swap data octets [i.e. swap data words' high-order
*                                                       octet(s) with data words' low-order octet(s),
*                                                       & vice-versa] if required by device-to-CPU data
*                                                       bus wiring &/or CPU endian word order.
*
*           (7) Network devices with receive descriptors MUST configure the number of receive buffers
*               greater than the number of receive descriptors.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*********************************************************************************************************
*                                    ETHERNET 1 DEVICE CONFIGURATION
*********************************************************************************************************
*********************************************************************************************************
*/

#ifdef  NET_IF_ETHER_MODULE_EN

extern char _enet1_start;
extern char _enet2_start;
                                    /* ----------------- EXAMPLE ETHERNET DEVICE A, #1 CONFIGURATION ------------------ */

const  NET_DEV_CFG_ETHER  NetDev_Cfg_MACNet_ETHER1 = {

    NET_IF_MEM_TYPE_MAIN,           /* Desired receive  buffer memory pool type :                                       */
                                    /*   NET_IF_MEM_TYPE_MAIN        buffers allocated from main memory                 */
                                    /*   NET_IF_MEM_TYPE_DEDICATED   buffers allocated from (device's) dedicated memory */
    1520u,                          /* Desired size      of device's large receive  buffers (in octets) [see Note #2].  */
      30u,                          /* Desired number    of device's large receive  buffers             [see Note #3a]. */
      16u,                          /* Desired alignment of device's       receive  buffers (in octets) [see Note #4].  */
       0u,                          /* Desired offset from base receive  index, if needed   (in octets) [see Note #5a1].*/


    NET_IF_MEM_TYPE_MAIN,           /* Desired transmit buffer memory pool type :                                       */
                                    /*   NET_IF_MEM_TYPE_MAIN        buffers allocated from main memory                 */
                                    /*   NET_IF_MEM_TYPE_DEDICATED   buffers allocated from (device's) dedicated memory */
    1520u,                          /* Desired size      of device's large transmit buffers (in octets) [see Note #2].  */
      15u,                          /* Desired number    of device's large transmit buffers             [see Note #3b]. */
      60u,                          /* Desired size      of device's small transmit buffers (in octets) [see Note #2].  */
      15u,                          /* Desired number    of device's small transmit buffers             [see Note #3b]. */
       4u,                          /* Desired alignment of device's       transmit buffers (in octets) [see Note #4].  */
       0u,                          /* Desired offset from base transmit index, if needed   (in octets) [see Note #5a2].*/


    0x00000000u,                    /* Base address   of dedicated memory, if available.                                */
    0x00000000u,                    /* Size           of dedicated memory, if available (in octets).                    */


    NET_DEV_CFG_FLAG_NONE,          /* Desired option flags, if any (see Note #6).                                      */


            18u,                    /* Desired number of device's receive  descriptors (see Note #7).                   */
            30u,                    /* Desired number of device's transmit descriptors.                                 */


    (CPU_ADDR)(&_enet1_start),      /* Base address   of device's hardware/registers.                                   */

             0u,                    /* Size           of device's data bus (in bits), if available.                     */


   "00:0A:BC:DE:FF:01",             /* Desired device hardware address; may be NULL address or string ...               */
                                    /* ... if  device hardware address configured or set at run-time.                   */
};


const  NET_PHY_CFG_ETHER  NetPhy_Cfg_MACNet_ETHER1 = {
    2u,                             /* Phy bus address. This value is hard-coded.                                       */
    NET_PHY_BUS_MODE_RMII,          /* Phy bus mode. (R)MII is th only mode supported by this PHY.                      */
    NET_PHY_TYPE_EXT,               /* Phy type.                                                                        */
    NET_PHY_SPD_AUTO,               /* Auto-Negotiation determines link speed.                                          */
    NET_PHY_DUPLEX_AUTO,            /* Auto-Negotiation determines link duplex.                                         */
};


#endif                              /* End of Ethernet device configuration.                                            */

/*
*********************************************************************************************************
*********************************************************************************************************
*                                    ETHERNET 1 DEVICE CONFIGURATION
*********************************************************************************************************
*********************************************************************************************************
*/

#ifdef  NET_IF_ETHER_MODULE_EN

const  NET_DEV_CFG_ETHER  NetDev_Cfg_MACNet_ETHER2 = {

    NET_IF_MEM_TYPE_MAIN,           /* Desired receive  buffer memory pool type :                                       */
                                    /*   NET_IF_MEM_TYPE_MAIN        buffers allocated from main memory                 */
                                    /*   NET_IF_MEM_TYPE_DEDICATED   buffers allocated from (device's) dedicated memory */
    1520u,                          /* Desired size      of device's large receive  buffers (in octets) [see Note #2].  */
      10u,                          /* Desired number    of device's large receive  buffers             [see Note #3a]. */
      16u,                          /* Desired alignment of device's       receive  buffers (in octets) [see Note #4].  */
       0u,                          /* Desired offset from base receive  index, if needed   (in octets) [see Note #5a1].*/


    NET_IF_MEM_TYPE_MAIN,           /* Desired transmit buffer memory pool type :                                       */
                                    /*   NET_IF_MEM_TYPE_MAIN        buffers allocated from main memory                 */
                                    /*   NET_IF_MEM_TYPE_DEDICATED   buffers allocated from (device's) dedicated memory */
    1520u,                          /* Desired size      of device's large transmit buffers (in octets) [see Note #2].  */
       5u,                          /* Desired number    of device's large transmit buffers             [see Note #3b]. */
      60u,                          /* Desired size      of device's small transmit buffers (in octets) [see Note #2].  */
       5u,                          /* Desired number    of device's small transmit buffers             [see Note #3b]. */
       4u,                          /* Desired alignment of device's       transmit buffers (in octets) [see Note #4].  */
       0u,                          /* Desired offset from base transmit index, if needed   (in octets) [see Note #5a2].*/

    0x00000000u,                    /* Base address   of dedicated memory, if available.                                */
    0x00000000u,                    /* Size           of dedicated memory, if available (in octets).                    */


    NET_DEV_CFG_FLAG_NONE,          /* Desired option flags, if any (see Note #6).                                      */


              6u,                   /* Desired number of device's receive  descriptors (see Note #7).                   */
             10u,                   /* Desired number of device's transmit descriptors.                                 */


    (CPU_ADDR)(&_enet2_start),      /* Base address   of device's hardware/registers.                                   */

              0u,                   /* Size           of device's data bus (in bits), if available.                     */


   "00:0A:BC:DE:FF:02",             /* Desired device hardware address; may be NULL address or string ...               */
                                    /* ... if  device hardware address configured or set at run-time.                   */
};


const  NET_PHY_CFG_ETHER  NetPhy_Cfg_MACNet_ETHER2 = {
    1u,                             /* Phy bus address. This value is hard-coded.                                       */
    NET_PHY_BUS_MODE_RMII,          /* Phy bus mode. (R)MII is the only mode supported by this PHY.                     */
    NET_PHY_TYPE_EXT,               /* Phy type.                                                                        */
    NET_PHY_SPD_AUTO,               /* Auto-Negotiation determines link speed.                                          */
    NET_PHY_DUPLEX_AUTO,            /* Auto-Negotiation determines link duplex.                                         */
};

#endif


/*
*********************************************************************************************************
*********************************************************************************************************
*                          EXAMPLE NETWORK LOOPBACK INTERFACE CONFIGURATION
*********************************************************************************************************
*********************************************************************************************************
*/

#ifdef NET_IF_LOOPBACK_MODULE_EN

const  NET_IF_CFG_LOOPBACK  NetIF_Cfg_Loopback = {

    NET_IF_MEM_TYPE_MAIN,       /* Desired receive  buffer memory pool type :                                           */
                                /*   NET_IF_MEM_TYPE_MAIN        buffers allocated from main memory                     */
                                /*   NET_IF_MEM_TYPE_DEDICATED   buffers allocated from (loopback's) dedicated memory   */
    1500u,                      /* Desired size      of interface's large receive  buffers (in octets) [see Note #2].   */
      10u,                      /* Desired number    of interface's large receive  buffers             [see Note #3a].  */
       4u,                      /* Desired alignment of interface's       receive  buffers (in octets) [see Note #4].   */
       0u,                      /* Desired offset from base receive  index, if needed      (in octets) [see Note #5a1]. */


    NET_IF_MEM_TYPE_MAIN,       /* Desired transmit buffer memory pool type :                                           */
                                /*   NET_IF_MEM_TYPE_MAIN        buffers allocated from main memory                     */
                                /*   NET_IF_MEM_TYPE_DEDICATED   buffers allocated from (loopback's) dedicated memory   */
    1500u,                      /* Desired size      of interface's large transmit buffers (in octets) [see Note #2].   */
      10u,                      /* Desired number    of interface's large transmit buffers             [see Note #3b].  */
      60u,                      /* Desired size      of interface's small transmit buffers (in octets) [see Note #2].   */
      10u,                      /* Desired number    of interface's small transmit buffers             [see Note #3b].  */
       4u,                      /* Desired alignment of interface's       transmit buffers (in octets) [see Note #4].   */
       0u,                      /* Desired offset from base transmit index, if needed      (in octets) [see Note #5a2]. */


    0x00000000u,                /* Base address   of dedicated memory, if available.                                    */
             0u,                /* Size           of dedicated memory, if available (in octets).                        */


    NET_DEV_CFG_FLAG_NONE,      /* Desired option flags, if any (see Note #6).                                          */
};


#endif                          /* End of network loopback configuration.                                               */


