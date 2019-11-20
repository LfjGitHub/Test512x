/****************************************************************************
Copyright (c) 2017-2018 Qualcomm Technologies International, Ltd.

FILE NAME
    audio_input_a2dp_chain_config.c

DESCRIPTION
    Implementation of function to return the appropriate chain configuration data.
*/

#include <operators.h>
#include <panic.h>
#include "audio_config.h"
#include "audio_input_a2dp_chain_config.h"

#define BUFFER_SIZE (8000)
static const operator_config_t op_config[] =
{
    MAKE_OPERATOR_CONFIG(capability_id_rtp, rtp_role),
    MAKE_OPERATOR_CONFIG(capability_id_splitter, splitter_role),
    MAKE_OPERATOR_CONFIG(capability_id_sbc_decoder, decoder_role)
};

static const operator_path_node_t encoded_channel[] =
{
    {rtp_role, 0, 0},
    {splitter_role, 0, 0},
    {decoder_role, 0, 0}
};

static const operator_path_node_t forwarding_channel[] =
{
    {splitter_role, 0, 1}
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
    {path_forwarding, path_with_output, ARRAY_DIM((forwarding_channel)), forwarding_channel},
    {path_pcm_left, path_with_output, ARRAY_DIM((pcm_left_channel)), pcm_left_channel},
    {path_pcm_right, path_with_output, ARRAY_DIM((pcm_right_channel)), pcm_right_channel},
};

/* Mono SBC Streams */

#define OPERATORS_SETUP_SWITCHED_PASSTHROUGH_FORMAT(format) \
{ \
    .key = operators_setup_switched_passthrough_set_format, \
    .value = {.spc_format = (format)} \
}

static const operator_setup_item_t switched_passthrough_pcm_setup[] =
{
    OPERATORS_SETUP_SWITCHED_PASSTHROUGH_FORMAT(spc_op_format_pcm)
};

static const operator_setup_item_t passthrough_setup[] =
{
    OPERATORS_SETUP_STANDARD_BUFFER_SIZE(BUFFER_SIZE)
};

static const operator_config_t op_config_mono[] =
{
    MAKE_OPERATOR_CONFIG(capability_id_rtp, rtp_role),
    MAKE_OPERATOR_CONFIG(capability_id_sbc_decoder, decoder_role),
    MAKE_OPERATOR_CONFIG_WITH_SETUP(capability_id_passthrough, passthrough_role, passthrough_setup),
    MAKE_OPERATOR_CONFIG_WITH_SETUP(capability_id_switched_passthrough_consumer, switched_passthrough_role, switched_passthrough_pcm_setup),
};


static const operator_path_node_t encoded_channel_no_splitter[] =
{
    {rtp_role, 0, 0},
    {decoder_role, 0, 0},
};

static const operator_path_node_t pcm_with_passthrough_left_channel[] =
{
    {decoder_role, 0, 0},
    {passthrough_role, 0, 0}
};

static const operator_path_node_t pcm_with_passthrough_right_channel[] =
{
    {decoder_role, 0, 1},
    {passthrough_role, 0, 0}
};

static const operator_path_node_t forwarding_channel_mono_left[] =
{
    {decoder_role, 0, 0},
    {switched_passthrough_role, 0, 0}
};

static const operator_path_node_t forwarding_channel_mono_right[] =
{
    {decoder_role, 0, 1},
    {switched_passthrough_role, 0, 0}
};

static const operator_path_t paths_mono_left[] =
{
    {path_encoded, path_with_input, ARRAY_DIM((encoded_channel_no_splitter)), encoded_channel_no_splitter},
    {path_forwarding, path_with_output, ARRAY_DIM((forwarding_channel_mono_right)), forwarding_channel_mono_right},
    {path_pcm_left, path_with_output, ARRAY_DIM((pcm_with_passthrough_left_channel)), pcm_with_passthrough_left_channel},
};

static const operator_path_t paths_mono_right[] =
{
    {path_encoded, path_with_input, ARRAY_DIM((encoded_channel_no_splitter)), encoded_channel_no_splitter},
    {path_forwarding, path_with_output, ARRAY_DIM((forwarding_channel_mono_left)), forwarding_channel_mono_left},
    {path_pcm_left, path_with_output, ARRAY_DIM((pcm_with_passthrough_right_channel)), pcm_with_passthrough_right_channel},
};

/* Mono AptX (HQ) paths */

static const operator_setup_item_t switched_passthrough_encoded_setup[] =
{
    OPERATORS_SETUP_SWITCHED_PASSTHROUGH_FORMAT(spc_op_format_encoded)
};

static const operator_config_t op_config_hq_mono[] =
{
    MAKE_OPERATOR_CONFIG(capability_id_rtp, rtp_role),
    MAKE_OPERATOR_CONFIG(capability_id_aptx_demux, aptx_demux_role),
    MAKE_OPERATOR_CONFIG(capability_id_none, decoder_role),
    MAKE_OPERATOR_CONFIG_WITH_SETUP(capability_id_switched_passthrough_consumer, switched_passthrough_role, switched_passthrough_encoded_setup),
    MAKE_OPERATOR_CONFIG_WITH_SETUP(capability_id_passthrough, passthrough_role, passthrough_setup),
};

static const operator_path_node_t encoded_hq_channel[] =
{
    {rtp_role, 0, 0},
    {aptx_demux_role, 0, 0},
};

static const operator_path_node_t encoded_hq_left_channel_[] =
{
    {aptx_demux_role, 0, 0},
    {decoder_role, 0, 0},
};

static const operator_path_node_t encoded_hq_right_channel_[] =
{
    {aptx_demux_role, 0, 1},
    {decoder_role, 0, 0},
};

static const operator_path_node_t forwarding_channel_hq_mono_left[] =
{
    {aptx_demux_role, 0, 0},
    {switched_passthrough_role, 0, 0}
};

static const operator_path_node_t forwarding_channel_hq_mono_right[] =
{
    {aptx_demux_role, 0, 1},
    {switched_passthrough_role, 0, 0}
};

static const operator_path_t paths_hq_mono_left[] =
{
    {path_encoded, path_with_input, ARRAY_DIM((encoded_hq_channel)), encoded_hq_channel},
    {path_mono_encoded, path_with_no_in_or_out, ARRAY_DIM((encoded_hq_left_channel_)), encoded_hq_left_channel_},
    {path_forwarding, path_with_output, ARRAY_DIM((forwarding_channel_hq_mono_right)), forwarding_channel_hq_mono_right},
    {path_pcm_left, path_with_output, ARRAY_DIM((pcm_with_passthrough_left_channel)), pcm_with_passthrough_left_channel},
};

static const operator_path_t paths_hq_mono_right[] =
{
    {path_encoded, path_with_input, ARRAY_DIM((encoded_hq_channel)), encoded_hq_channel},
    {path_mono_encoded, path_with_no_in_or_out, ARRAY_DIM((encoded_hq_right_channel_)), encoded_hq_right_channel_},
    {path_forwarding, path_with_output, ARRAY_DIM((forwarding_channel_hq_mono_left)), forwarding_channel_hq_mono_left},
    {path_pcm_left, path_with_output, ARRAY_DIM((pcm_with_passthrough_left_channel)), pcm_with_passthrough_left_channel},
};

/* Mono AAC Paths */

static const operator_config_t op_config_aac_mono[] =
{
    MAKE_OPERATOR_CONFIG(capability_id_rtp, rtp_role),
    MAKE_OPERATOR_CONFIG(capability_id_splitter, splitter_role),
    MAKE_OPERATOR_CONFIG(capability_id_aac_decoder, decoder_role),
    MAKE_OPERATOR_CONFIG_WITH_SETUP(capability_id_switched_passthrough_consumer, switched_passthrough_role, switched_passthrough_pcm_setup),
};

static const operator_path_node_t encoded_aac_channel[] =
{
    {rtp_role, 0, 0},
    {splitter_role, 0, 0},
    {decoder_role, 0, 0}
};

static const operator_path_node_t pcm_aac_left_channel[] =
{
    {decoder_role, 0, 0}
};

static const operator_path_node_t pcm_aac_right_channel[] =
{
    {decoder_role, 0, 1}
};

static const operator_path_node_t pcm_aac_consuming_left_channel[] =
{
    {decoder_role, 0, 0},
    {switched_passthrough_role, 0, 0}
};

static const operator_path_node_t pcm_aac_consuming_right_channel[] =
{
    {decoder_role, 0, 1},
    {switched_passthrough_role, 0, 0}
};

static const operator_path_node_t forwarding_aac_channel[] =
{
    {splitter_role, 0, 1}
};

static const operator_path_t paths_aac_mono_left[] =
{
    {path_encoded, path_with_input, ARRAY_DIM((encoded_aac_channel)), encoded_aac_channel},
    {path_forwarding, path_with_output, ARRAY_DIM((forwarding_aac_channel)), forwarding_aac_channel},
    {path_pcm_left, path_with_output, ARRAY_DIM((pcm_aac_left_channel)), pcm_aac_left_channel},
    {path_pcm_right, path_with_no_in_or_out, ARRAY_DIM((pcm_aac_consuming_right_channel)), pcm_aac_consuming_right_channel},
};

static const operator_path_t paths_aac_mono_right[] =
{
    {path_encoded, path_with_input, ARRAY_DIM((encoded_aac_channel)), encoded_aac_channel},
    {path_forwarding, path_with_output, ARRAY_DIM((forwarding_aac_channel)), forwarding_aac_channel},
    {path_pcm_left, path_with_output, ARRAY_DIM((pcm_aac_right_channel)), pcm_aac_right_channel},
    {path_pcm_right, path_with_no_in_or_out, ARRAY_DIM((pcm_aac_consuming_left_channel)), pcm_aac_consuming_left_channel},
};

/* Create Configs */

static const chain_config_t a2dp_chain_config =
    MAKE_CHAIN_CONFIG_WITH_PATHS(chain_id_audio_input_a2dp, audio_ucid_input_a2dp, op_config, paths);

static const chain_config_t a2dp_hq_chain_config =
    MAKE_CHAIN_CONFIG_WITH_PATHS(chain_id_audio_input_a2dp, audio_ucid_input_a2dp_hq_codec, op_config, paths);

static const chain_config_t a2dp_mono_left_chain_config =
    MAKE_CHAIN_CONFIG_WITH_PATHS(chain_id_audio_input_a2dp, audio_ucid_input_a2dp, op_config_mono, paths_mono_left);

static const chain_config_t a2dp_mono_right_chain_config =
    MAKE_CHAIN_CONFIG_WITH_PATHS(chain_id_audio_input_a2dp, audio_ucid_input_a2dp, op_config_mono, paths_mono_right);

static const chain_config_t a2dp_hq_mono_left_chain_config =
    MAKE_CHAIN_CONFIG_WITH_PATHS(chain_id_audio_input_a2dp, audio_ucid_input_a2dp_hq_codec, op_config_hq_mono, paths_hq_mono_left);

static const chain_config_t a2dp_hq_mono_right_chain_config =
    MAKE_CHAIN_CONFIG_WITH_PATHS(chain_id_audio_input_a2dp, audio_ucid_input_a2dp_hq_codec, op_config_hq_mono, paths_hq_mono_right);

static const chain_config_t a2dp_aac_mono_left_chain_config =
    MAKE_CHAIN_CONFIG_WITH_PATHS(chain_id_audio_input_a2dp, audio_ucid_input_a2dp, op_config_aac_mono, paths_aac_mono_left);

static const chain_config_t a2dp_aac_mono_right_chain_config =
    MAKE_CHAIN_CONFIG_WITH_PATHS(chain_id_audio_input_a2dp, audio_ucid_input_a2dp, op_config_aac_mono, paths_aac_mono_right);

/*
 * return a bool to indicate if the device is configured for mono operation only
 */
static bool isMonoMode(void)
{
    return (AudioConfigGetRenderingMode()==single_channel_rendering);
}

/*
 * return a bool to indicate if the device is configured to play the left channel
 */
static bool isLeftChannel(void)
{
    return (AudioConfigGetTwsChannelModeLocal()== CHANNEL_MODE_LEFT);
}

static capability_id_t getDecoderCapabilityId(audio_codec_t decoder)
{
    switch(decoder)
    {
        case audio_codec_sbc:
            return capability_id_sbc_decoder;
        case audio_codec_aac:
            return capability_id_aac_decoder;
        case audio_codec_aptx:
            return isMonoMode() ? capability_id_aptx_mono_decoder_no_autosync : capability_id_aptx_decoder;
        case audio_codec_aptx_ll:
            return isMonoMode() ? Panic(), capability_id_none : capability_id_aptx_ll_decoder;
        case audio_codec_aptx_hd:
            return isMonoMode() ? capability_id_aptx_hd_mono_decoder : capability_id_aptx_hd_decoder;
        case audio_codec_aptx_adaptive:
            return capability_id_aptx_ad_decoder;
        default:
            Panic();
            return capability_id_none;
    }
}

static operator_config_t getChainConfigFilter(const operator_config_t *base_config, capability_id_t cap_id)
{
    operator_config_t filter;

    /* Copy all fields form base configuration then replace capability ID */
    memcpy(&filter, base_config, sizeof(filter));
    filter.capability_id = cap_id;

    return filter;
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

static const chain_config_t * getChainConfig(audio_codec_t decoder)
{
    if (isMonoMode())
    {
        switch(decoder)
        {
            case audio_codec_aptx:
            case audio_codec_aptx_ll:
            case audio_codec_aptx_hd:
            case audio_codec_aptx_adaptive:
                return isLeftChannel() ? &a2dp_hq_mono_left_chain_config : &a2dp_hq_mono_right_chain_config;
            case audio_codec_aac:
                return isLeftChannel() ? &a2dp_aac_mono_left_chain_config : &a2dp_aac_mono_right_chain_config;
            default:
                return isLeftChannel() ? &a2dp_mono_left_chain_config : &a2dp_mono_right_chain_config;
        }
    }
    else
    {
        switch(decoder)
        {
            case audio_codec_aptx:
            case audio_codec_aptx_ll:
            case audio_codec_aptx_hd:
            case audio_codec_aptx_adaptive:
                return &a2dp_hq_chain_config;
            default:
                return &a2dp_chain_config;
        }
    }
}

kymera_chain_handle_t audioInputA2dpCreateChain(audio_codec_t decoder, chain_variant_mask_t chain_variant)
{
    operator_config_t filter[ARRAY_DIM(op_config)];
    unsigned num_of_filters = 0;

    if (chain_variant & chain_variant_mask_without_splitter)
    {
        filter[num_of_filters] = getChainConfigFilter(getOperatorConfigBasedOnRole(splitter_role), capability_id_none);
        num_of_filters++;
    }

    if (chain_variant & chain_variant_mask_without_rtp)
    {
        filter[num_of_filters] = getChainConfigFilter(getOperatorConfigBasedOnRole(rtp_role), capability_id_none);
        num_of_filters++;
    }

    if (decoder != audio_codec_sbc)
    {
        filter[num_of_filters] = getChainConfigFilter(getOperatorConfigBasedOnRole(decoder_role), getDecoderCapabilityId(decoder));
        num_of_filters++;
    }

    if (num_of_filters)
    {
        operator_filters_t filters = {.operator_filters = filter, .num_operator_filters = num_of_filters};
        return PanicNull(ChainCreateWithFilter(getChainConfig(decoder), &filters));
    }
    else
        return PanicNull(ChainCreate(getChainConfig(decoder)));
}
