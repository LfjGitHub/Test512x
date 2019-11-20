/****************************************************************************
Copyright (c) 2017-2018 Qualcomm Technologies International, Ltd.
Part of 6.3.0

FILE NAME
    audio_input_a2dp_chain_config.h

DESCRIPTION
    Types used for chain configuration
*/

#ifndef AUDIO_INPUT_A2DP_CHAIN_CONFIG_H_
#define AUDIO_INPUT_A2DP_CHAIN_CONFIG_H_

#include <chain.h>
#include <audio_data_types.h>

#define MAKE_MASK_FOR_CHAIN_VARIANT_WITHOUT(x) (1 << x)

enum
{
    rtp_role,
    splitter_role,
    decoder_role,
    passthrough_role,
    switched_passthrough_role,
    aptx_demux_role
};

typedef enum
{
    path_encoded,
    path_mono_encoded,
    path_forwarding,
    path_pcm_left,
    path_pcm_right
} path_channel_t;

typedef enum
{
    chain_variant_mask_full_chain = 0,
    chain_variant_mask_without_splitter = MAKE_MASK_FOR_CHAIN_VARIANT_WITHOUT(splitter_role),
    chain_variant_mask_without_rtp      = MAKE_MASK_FOR_CHAIN_VARIANT_WITHOUT(rtp_role)
} chain_variant_mask_t;

/****************************************************************************
DESCRIPTION
    Create the chain with the appropriate configuration filters.
*/
kymera_chain_handle_t audioInputA2dpCreateChain(audio_codec_t decoder, chain_variant_mask_t chain_variant);

#endif /* AUDIO_INPUT_A2DP_CHAIN_CONFIG_H_ */
