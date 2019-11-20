/****************************************************************************
Copyright (c) 2018 Qualcomm Technologies International, Ltd.

FILE NAME
    audio_input_a2dp_rtp_config.c

DESCRIPTION
    RTP decode operator configuration
*/

#include "audio_input_a2dp_rtp_config.h"
#include "audio_input_a2dp_chain_config.h"

#include <audio_config.h>
#include <print.h>
#include <panic.h>

/* Make sure buffer is large enough to hold number of samples corresponding to the latency */
#define RTP_BUFFER_SIZE (8000)
#define BA_RTP_BUFFER_SIZE (1000)

#define APTX_LL_SAMPLES_PER_CODEWORD        ((unsigned)4)
#define APTX_LL_CHANNELS                    ((unsigned)2)
#define APTX_LL_RTP_BUFFER_RATIO            ((unsigned)2)
#define APTX_LL_MINIMUM_LATENCY_IN_US       (0)
#define APTX_LL_MAXIMUM_LATENCY_DELTA_IN_US (10000)

#define US_PER_SECOND (1000000)
#define CONVERTION_FACTOR_2MS_TO_1MS (2U)

/* MTU? (895) - max tws packet header (5) = 890, and then round down to a multiple of 4 := 888 */
#define RTP_PACKET_LIMITED_LENGTH           (888)
/* MTU? (895) - max share me packet header (13) = 882, and then round down to a multiple of 4 := 880 */
#define RTP_PACKET_LIMITED_LENGTH_SHAREME   (880)

static uint16 aptx_ll_latency_ms = 0;

static uint16 getRtpPacketLength(void)
{
    if(AudioConfigGetPeerMode() == peer_mode_shareme)
        return RTP_PACKET_LIMITED_LENGTH_SHAREME;
    else
        return RTP_PACKET_LIMITED_LENGTH;
}

static rtp_codec_type_t getRtpCodecType(audio_codec_t decoder)
{
    rtp_codec_type_t rtp_codec_type = rtp_codec_type_sbc;

    switch(decoder)
    {
        case audio_codec_sbc:
            rtp_codec_type = rtp_codec_type_sbc;
            break;
        case audio_codec_aptx:
        case audio_codec_aptx_ll:
            rtp_codec_type = rtp_codec_type_aptx;
            break;
        case audio_codec_aptx_hd:
            rtp_codec_type = rtp_codec_type_aptx_hd;
            break;
        case audio_codec_aptx_adaptive:
            rtp_codec_type = rtp_codec_type_aptx_ad;
            break;
        case audio_codec_aac:
            rtp_codec_type = rtp_codec_type_aac;
            break;
        default:
            Panic();
    }

    return rtp_codec_type;
}

static rtp_working_mode_t getRtpWorkingMode(audio_codec_t decoder, bool content_protection)
{
    rtp_working_mode_t rtp_mode;

    if(((decoder == audio_codec_aptx) || (decoder == audio_codec_aptx_ll)) && content_protection == FALSE)
        rtp_mode = rtp_ttp_only;
    else
        rtp_mode = rtp_decode;

    return rtp_mode;
}

static aptx_adaptive_ttp_in_ms_t convertAptxAdaptiveTtpToOperatorsFormat(aptx_adaptive_ttp_latencies_t nq2q_ttp)
{
    aptx_adaptive_ttp_in_ms_t aptx_ad_ttp;

    aptx_ad_ttp.low_latency_0 = nq2q_ttp.low_latency_0_in_ms;
    aptx_ad_ttp.low_latency_1 = nq2q_ttp.low_latency_1_in_ms;
    aptx_ad_ttp.high_quality  = (uint16) (CONVERTION_FACTOR_2MS_TO_1MS * nq2q_ttp.high_quality_in_2ms);
    aptx_ad_ttp.tws_legacy    = (uint16) (CONVERTION_FACTOR_2MS_TO_1MS * nq2q_ttp.tws_legacy_in_2ms);

    return aptx_ad_ttp;
}

static bool hadToAdjustTtpLatency(uint16 *ttp_latency, ttp_latency_t ttp_supported)
{
    uint16 original_ttp_latency = *ttp_latency;

    *ttp_latency = MIN(*ttp_latency, ttp_supported.max_in_ms);
    *ttp_latency = MAX(*ttp_latency, ttp_supported.min_in_ms);

    if (*ttp_latency == original_ttp_latency)
        return FALSE;
    else
        return TRUE;
}

static aptx_adaptive_ttp_in_ms_t getAdjustedAptxAdaptiveTtpLatencies(aptx_adaptive_ttp_in_ms_t aptx_ad_ttp)
{
    ttp_latency_t ttp_supported = AudioConfigGetA2DPTtpLatency();

    if (hadToAdjustTtpLatency(&aptx_ad_ttp.low_latency_0, ttp_supported))
    {
        PRINT(("audio_input_a2dp: aptX Adaptive NQ2Q LL-0 mode TTP adjusted to %u ms\n", aptx_ad_ttp.low_latency_0));
    }

    if (hadToAdjustTtpLatency(&aptx_ad_ttp.low_latency_1, ttp_supported))
    {
        PRINT(("audio_input_a2dp: aptX Adaptive NQ2Q LL-1 mode TTP adjusted to %u ms\n", aptx_ad_ttp.low_latency_1));
    }

    if (hadToAdjustTtpLatency(&aptx_ad_ttp.high_quality, ttp_supported))
    {
        PRINT(("audio_input_a2dp: aptX Adaptive NQ2Q HQ mode TTP adjusted to %u ms\n", aptx_ad_ttp.high_quality));
    }

    ttp_supported = AudioConfigGetTWSTtpLatency();

    if (hadToAdjustTtpLatency(&aptx_ad_ttp.tws_legacy, ttp_supported))
    {
        PRINT(("audio_input_a2dp: aptX Adaptive NQ2Q TWS-Legacy mode TTP adjusted to %u ms\n", aptx_ad_ttp.tws_legacy));
    }

    return aptx_ad_ttp;
}

/****************************************************************************
DESCRIPTION
    Set-up APTX LL additional parameters:
    TTP latency and RTP decoder buffer size.
*/
static void setupRtpAptxLowLatency(Operator rtp_op,  aptx_sprint_params_type aptx_ll_params, uint32 sample_rate)
{
    unsigned ttp_latency_us = (aptx_ll_params.target_codec_level * APTX_LL_SAMPLES_PER_CODEWORD * APTX_LL_CHANNELS * US_PER_SECOND) / sample_rate;
    unsigned buffer_size = aptx_ll_params.initial_codec_level * APTX_LL_RTP_BUFFER_RATIO;

    OperatorsStandardSetBufferSize(rtp_op, buffer_size);
    OperatorsStandardSetTimeToPlayLatency(rtp_op, ttp_latency_us);
    OperatorsStandardSetLatencyLimits(rtp_op, APTX_LL_MINIMUM_LATENCY_IN_US, ttp_latency_us + APTX_LL_MAXIMUM_LATENCY_DELTA_IN_US);

    aptx_ll_latency_ms = (uint16_t)(ttp_latency_us / 1000);
}

/****************************************************************************
DESCRIPTION
    Set-up aptX Adaptive additional parameters:
    Use new message to set target TTP latency for the multiple modes of aptX Adaptive when NQ2Q mode is used
*/
static void setupRtpAptxAdaptiveLatency(Operator rtp_op, aptx_adaptive_ttp_latencies_t nq2q_ttp)
{
    aptx_adaptive_ttp_in_ms_t aptx_ad_ttp = convertAptxAdaptiveTtpToOperatorsFormat(nq2q_ttp);

    aptx_ad_ttp = getAdjustedAptxAdaptiveTtpLatencies(aptx_ad_ttp);

    OperatorsRtpSetAptxAdaptiveTTPLatency(rtp_op, aptx_ad_ttp);
}

/****************************************************************************
DESCRIPTION
    Associate Aac decoder with the rtp capability
*/
static void setupRtpAacCodec(Operator rtp_op, Operator aac_op)
{
    OperatorsRtpSetAacCodec(rtp_op, aac_op);
}

/****************************************************************************
DESCRIPTION
    Limit RTP packet length for aptX
*/
static void setupRtpAptxCodec(Operator rtp_op)
{
    OperatorsRtpSetMaximumPacketLength(rtp_op, getRtpPacketLength());
}

/****************************************************************************
DESCRIPTION
    Set RTP latencies for running in the broadcaster role
*/
static void setupRtpBroadcasterLatency(Operator rtp_op)
{
    OperatorsStandardSetTimeToPlayLatency(rtp_op, TTP_BA_LATENCY_IN_US);
    OperatorsStandardSetLatencyLimits(rtp_op, TTP_BA_MIN_LATENCY_LIMIT_US, TTP_BA_MAX_LATENCY_LIMIT_US);
}

/****************************************************************************
DESCRIPTION
    Configuration messages for RTP that are needed for any decoder
*/
static void configureRtpCommon(Operator rtp_op, audio_codec_t decoder, uint32 sample_rate, A2dpPluginConnectParams *a2dp_connect_params)
{
    bool content_protection = a2dp_connect_params->content_protection;
    rtp_working_mode_t rtp_mode;

    rtp_mode = getRtpWorkingMode(decoder, content_protection);

    OperatorsConfigureRtp(rtp_op, getRtpCodecType(decoder), content_protection, rtp_mode);

    /* Additional time-to-play setup messages */
    OperatorsStandardSetSampleRate(rtp_op, sample_rate);

    /*! @todo: This is a temporary hack. We should perhaps refactor
     *  setting the rtp latency into a separate function. */
    if (a2dp_connect_params->ba_output_plugin)
    {
        setupRtpBroadcasterLatency(rtp_op);
        OperatorsStandardSetBufferSize(rtp_op, BA_RTP_BUFFER_SIZE);
    }
    else
    {
        ttp_latency_t ttp_latency = AudioConfigGetA2DPTtpLatency();
        audioInputConfigureRtpLatency(rtp_op, &ttp_latency);
        OperatorsStandardSetBufferSize(rtp_op, RTP_BUFFER_SIZE);
    }
}

/****************************************************************************
DESCRIPTION
    Configuration of RTP specific to the decoder used.
*/
static void configureRtpSpecificToThisDecoder(Operator rtp_op, kymera_chain_handle_t chain, audio_codec_t decoder,
                                              uint32 sample_rate, A2dpPluginConnectParams *a2dp_connect_params)
{
    switch(decoder)
    {
    case audio_codec_aac:
        setupRtpAacCodec(rtp_op, ChainGetOperatorByRole(chain, decoder_role));
        break;
    case audio_codec_aptx:
        setupRtpAptxCodec(rtp_op);
        break;
    case audio_codec_aptx_ll:
        setupRtpAptxLowLatency(rtp_op, a2dp_connect_params->aptx_sprint_params, sample_rate);
        break;
    case audio_codec_aptx_adaptive:
        setupRtpAptxAdaptiveLatency(rtp_op, a2dp_connect_params->aptx_ad_params.nq2q_ttp);
        break;
    default:
        break;
    }
}

void audioInputA2dpConfigureRtpOperator(Operator rtp_op, kymera_chain_handle_t chain, audio_codec_t decoder,
                                        uint32 sample_rate, A2dpPluginConnectParams *a2dp_connect_params)
{
    configureRtpCommon(rtp_op, decoder, sample_rate, a2dp_connect_params);
    configureRtpSpecificToThisDecoder(rtp_op, chain, decoder, sample_rate, a2dp_connect_params);
}

void audioInputConfigureRtpLatency(Operator rtp_op, ttp_latency_t *ttp_latency)
{
    OperatorsStandardSetTimeToPlayLatency(rtp_op, TTP_LATENCY_IN_US(ttp_latency->target_in_ms));
    OperatorsStandardSetLatencyLimits(rtp_op, TTP_LATENCY_IN_US(ttp_latency->min_in_ms), TTP_LATENCY_IN_US(ttp_latency->max_in_ms));
}

uint16 audioInputA2dpGetLatency(audio_codec_t decoder, audio_input_context_t *ctx, bool *estimated)
{
    *estimated = FALSE;

    if(decoder == audio_codec_aptx_ll)
    {
        if(ctx)
            return aptx_ll_latency_ms;
        else
        {
            *estimated = TRUE;
            return TTP_APTX_LL_ESTIMATED_LATENCY_IN_MS;
        }
    }
    else
    {
        ttp_latency_t ttp_latency = AudioConfigGetA2DPTtpLatency();
        return ttp_latency.target_in_ms;
    }
}
