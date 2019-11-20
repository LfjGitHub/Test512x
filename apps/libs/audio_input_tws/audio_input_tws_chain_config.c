/****************************************************************************
Copyright (c) 2017 Qualcomm Technologies International, Ltd.

FILE NAME
    audio_input_tws_chain_config.c

DESCRIPTION
    Implementation of function to return the appropriate chain configuration data.
*/

#include <operators.h>
#include <panic.h>
#include "audio_input_tws_chain_config.h"
#include <audio_config.h>

static const operator_config_t op_config[] =
{
    MAKE_OPERATOR_CONFIG(capability_id_passthrough, passthrough_role),
    MAKE_OPERATOR_CONFIG(capability_id_none, decoder_role)
};

static const operator_path_node_t encoded_channel[] =
{
    {passthrough_role, 0, 0},
    {decoder_role, 0, 0}
};

static const operator_path_node_t pcm_left_channel[] =
{
    {decoder_role, 0, 0}
};

static const operator_path_node_t pcm_right_channel[] =
{
    {decoder_role, 0, 1}
};

static const operator_path_t paths[] =
{
    {path_encoded, path_with_input, ARRAY_DIM((encoded_channel)), encoded_channel},
    {path_pcm_left, path_with_output, ARRAY_DIM((pcm_left_channel)), pcm_left_channel},
    {path_pcm_right, path_with_output, ARRAY_DIM((pcm_right_channel)), pcm_right_channel},
};

static const operator_path_t paths_mono[] =
{
    {path_encoded, path_with_input, ARRAY_DIM((encoded_channel)), encoded_channel},
    {path_pcm_left, path_with_output, ARRAY_DIM((pcm_left_channel)), pcm_left_channel}
};

/* Mono AAC configuration */
#define OPERATORS_SETUP_SWITCHED_PASSTHROUGH_FORMAT(format) \
{ \
    .key = operators_setup_switched_passthrough_set_format, \
    .value = {.spc_format = (format)} \
}

static const operator_setup_item_t switched_passthrough_pcm_setup[] =
{
    OPERATORS_SETUP_SWITCHED_PASSTHROUGH_FORMAT(spc_op_format_pcm)
};

static const operator_config_t op_config_aac[] =
{
    MAKE_OPERATOR_CONFIG(capability_id_passthrough, passthrough_role),
    MAKE_OPERATOR_CONFIG(capability_id_none, decoder_role),
    MAKE_OPERATOR_CONFIG_WITH_SETUP(capability_id_switched_passthrough_consumer, switched_passthrough_role, switched_passthrough_pcm_setup),
};
static const operator_path_node_t encoded_aac_mono_channel[] =
{
    {passthrough_role, 0, 0},
    {decoder_role, 0, 0}
};

static const operator_path_node_t pcm_aac_mono_left_channel[] =
{
    {decoder_role, 0, 0}
};

static const operator_path_node_t pcm_aac_mono_right_channel[] =
{
    {decoder_role, 0, 1}
};

static const operator_path_node_t pcm_aac_mono_consuming_left_channel[] =
{
    {decoder_role, 0, 0},
    {switched_passthrough_role, 0, 0}
};

static const operator_path_node_t pcm_aac_mono_consuming_right_channel[] =
{
    {decoder_role, 0, 1},
    {switched_passthrough_role, 0, 0}
};

static const operator_path_t paths_aac_mono_left[] =
{
    {path_encoded, path_with_input, ARRAY_DIM((encoded_aac_mono_channel)), encoded_aac_mono_channel},
    {path_pcm_left, path_with_output, ARRAY_DIM((pcm_aac_mono_left_channel)), pcm_aac_mono_left_channel},
    {path_pcm_right, path_with_no_in_or_out, ARRAY_DIM((pcm_aac_mono_consuming_right_channel)), pcm_aac_mono_consuming_right_channel},
};

static const operator_path_t paths_aac_mono_right[] =
{
    {path_encoded, path_with_input, ARRAY_DIM((encoded_aac_mono_channel)), encoded_aac_mono_channel},
    {path_pcm_left, path_with_output, ARRAY_DIM((pcm_aac_mono_right_channel)), pcm_aac_mono_right_channel},
    {path_pcm_right, path_with_no_in_or_out, ARRAY_DIM((pcm_aac_mono_consuming_left_channel)), pcm_aac_mono_consuming_left_channel},
};

static const chain_config_t tws_chain_config =
    MAKE_CHAIN_CONFIG_WITH_PATHS(chain_id_audio_input_tws, audio_ucid_input_a2dp, op_config, paths);

static const chain_config_t tws_chain_config_aptx =
    MAKE_CHAIN_CONFIG_WITH_PATHS(chain_id_audio_input_tws, audio_ucid_input_a2dp_hq_codec, op_config, paths);

static const chain_config_t tws_chain_config_mono =
    MAKE_CHAIN_CONFIG_WITH_PATHS(chain_id_audio_input_tws, audio_ucid_input_a2dp, op_config, paths_mono);

static const chain_config_t tws_chain_config_aptx_mono =
    MAKE_CHAIN_CONFIG_WITH_PATHS(chain_id_audio_input_tws, audio_ucid_input_a2dp_hq_codec, op_config, paths_mono);

static const chain_config_t tws_chain_config_aac_mono_left =
    MAKE_CHAIN_CONFIG_WITH_PATHS(chain_id_audio_input_tws, audio_ucid_input_a2dp, op_config_aac, paths_aac_mono_left);

static const chain_config_t tws_chain_config_aac_mono_right =
    MAKE_CHAIN_CONFIG_WITH_PATHS(chain_id_audio_input_tws, audio_ucid_input_a2dp, op_config_aac, paths_aac_mono_right);

/*
 * return a bool to indicate if the device is configured to play the left channel
 */
static bool isLeftChannel(void)
{
    return (AudioConfigGetTwsChannelModeLocal()== CHANNEL_MODE_LEFT);
}

static const chain_config_t* getChainConfig(audio_input_tws_decoder_t decoder, bool mono_path_enabled)
{
    if(mono_path_enabled)
    {
        if(decoder == audio_input_tws_aptx)
            return &tws_chain_config_aptx_mono;
        else if(decoder == audio_input_tws_aac)
            return isLeftChannel() ? &tws_chain_config_aac_mono_left : &tws_chain_config_aac_mono_right;
        else
            return &tws_chain_config_mono;
    }
    else
    {
        if(decoder == audio_input_tws_aptx)
            return &tws_chain_config_aptx;
        else
            return &tws_chain_config;
    }
}

static capability_id_t getDecoderCapabilityId(audio_input_tws_decoder_t decoder, bool mono_path_enabled)
{
    switch(decoder)
    {
        case audio_input_tws_sbc:
            return capability_id_sbc_decoder;
        case audio_input_tws_aac:
            return capability_id_aac_decoder;
        case audio_input_tws_aptx:
            return (mono_path_enabled)? capability_id_aptx_mono_decoder_no_autosync : capability_id_aptx_decoder;
        default:
            Panic();
            return capability_id_none;
    }
}

static const operator_config_t * getOperatorConfigBasedOnRole(unsigned role)
{
    unsigned i;

    for(i = 0; i < ARRAY_DIM(op_config); ++i)
    {
        if(op_config[i].role == role)
            return &op_config[i];
    }

    Panic();
    return NULL;
}

static operator_config_t getChainConfigFilter(const operator_config_t *base_config, capability_id_t cap_id)
{
    operator_config_t filter;

    /* Copy all fields form base configuration then replace capability ID */
    memcpy(&filter, base_config, sizeof(filter));
    filter.capability_id = cap_id;

    return filter;
}

static operator_config_t getChainConfigFilterForDecoder(audio_input_tws_decoder_t decoder, bool mono_path_enabled)
{
    return getChainConfigFilter(getOperatorConfigBasedOnRole(decoder_role), getDecoderCapabilityId(decoder, mono_path_enabled));
}

kymera_chain_handle_t audioInputTWSCreateChain(audio_input_tws_decoder_t decoder)
{
    bool mono_path_enabled = (AudioConfigGetRenderingMode()==single_channel_rendering);
    operator_config_t filter[] = {getChainConfigFilterForDecoder(decoder, mono_path_enabled)};
    operator_filters_t filters = {.num_operator_filters = ARRAY_DIM(filter), .operator_filters = filter};

    return PanicNull(ChainCreateWithFilter(getChainConfig(decoder, mono_path_enabled), &filters));
}

