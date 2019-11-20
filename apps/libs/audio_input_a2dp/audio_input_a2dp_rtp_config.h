/****************************************************************************
Copyright (c) 2018 Qualcomm Technologies International, Ltd.
Part of 6.3.0

FILE NAME
    audio_input_a2dp_rtp_config.h

DESCRIPTION
    RTP decode operator configuration
*/

#ifndef AUDIO_INPUT_A2DP_RTP_CONFIG_H_
#define AUDIO_INPUT_A2DP_RTP_CONFIG_H_

#include <audio_input_common.h>

/****************************************************************************
DESCRIPTION
    Create the chain with the appropriate configuration filters.
*/
void audioInputA2dpConfigureRtpOperator(Operator rtp_op, kymera_chain_handle_t chain, audio_codec_t decoder,
                                        uint32 sample_rate, A2dpPluginConnectParams *a2dp_connect_params);

/****************************************************************************
DESCRIPTION
    Configures the RTP time to play latency
*/
void audioInputConfigureRtpLatency(Operator rtp_op, ttp_latency_t *ttp_latency);

/****************************************************************************
DESCRIPTION
    Get the TTP latency used for A2DP
*/
uint16 audioInputA2dpGetLatency(audio_codec_t decoder, audio_input_context_t *ctx, bool *estimated);

#endif /* AUDIO_INPUT_A2DP_RTP_CONFIG_H_ */
