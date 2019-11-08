
/****************************************************************************
Copyright (c) 2019 Qualcomm Technologies International, Ltd.

FILE NAME
    sink_peer_config_def.h

DESCRIPTION 
    This file contains the module specific configuration structure(s) and
    configuration block id(s) to be used with the config_store library.

*/

/*********************************/
/* GENERATED FILE - DO NOT EDIT! */
/*********************************/

#ifndef _SINK_PEER_CONFIG_DEF_H_
#define _SINK_PEER_CONFIG_DEF_H_

#include "config_definition.h"

typedef struct {
    unsigned short word_1;
    unsigned short word_2;
    unsigned short word_3;
    unsigned short word_4;
    unsigned short word_5;
    unsigned short word_6;
    unsigned short word_7;
    unsigned short word_8;
} peer_device_uuid_t;

#define SINK_PEER_WRITEABLE_CONFIG_BLK_ID 1047

typedef struct {
    signed short device_trim_master;
    signed short device_trim_slave;
    signed short device_trim_change;
    signed short device_trim_min;
    signed short device_trim_max;
    unsigned short padding:15;
    unsigned short PeerLinkReserved:1;
} sink_peer_writeable_config_def_t;

#define SINK_PEER_READONLY_CONFIG_BLK_ID 1057

typedef struct {
    unsigned short TwsQualificationSourceStreamDelay;
    unsigned short PeerTwsForceDownmixPio:8;
    unsigned short PeerConnectionPio:8;
    unsigned short ShareMeSource:1;
    unsigned short TwsPairingMode:2;
    unsigned short ShareMePairingMode:2;
    unsigned short PeerPermittedRouting:4;
    unsigned short audioSourcesAvailableToPeer:7;
    unsigned short TwsQualificationEnable:1;
    unsigned short tws_qual_enable_peer_open:1;
    unsigned short UnlockPeerStateMachine:1;
    unsigned short ReconnectAgOnPeerConnection:1;
    unsigned short PeerLinkRecoveryWhileStreaming:1;
    unsigned short ShareMePeerControlsSource:1;
    unsigned short PeerUseQtilUuid:1;
    unsigned short PeerUseDeviceId:1;
    unsigned short PeerUseLiac:1;
    unsigned short TwsSink:1;
    unsigned short TwsSource:1;
    unsigned short ShareMeSink:1;
    unsigned short FixedRoutingMode:2;
    unsigned short TwsSingleDeviceOperation:2;
    peer_device_uuid_t service_record[1];
} sink_peer_readonly_config_def_t;

#endif /* _SINK_PEER_CONFIG_DEF_H_ */
