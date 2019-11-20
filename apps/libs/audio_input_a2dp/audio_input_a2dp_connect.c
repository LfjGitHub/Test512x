/****************************************************************************
Copyright (c) 2016-2018 Qualcomm Technologies International, Ltd.
Part of 6.3.0

FILE NAME
    audio_input_a2dp_connect.c

DESCRIPTION
    Implementation of AUDIO_PLUGIN_CONNECT_MSG message for a2dp source.
    Remaining message handlers are implemented in audio_input_common library.
*/

#include "audio_input_a2dp_connect.h"
#include "audio_input_a2dp_chain_config.h"
#include "audio_input_a2dp_rtp_config.h"
#include "audio_input_a2dp_task.h"
#include "audio_input_a2dp_broadcast.h"

#include <operators.h>
#include <audio_mixer.h>
#include <audio.h>
#include <panic.h>

/* Make sure buffer is large enough to hold number of samples corresponding to the latency */
#define SPLITTER_BUFFER_SIZE (2048)


static unsigned getKickPeriodFromTask(Task task)
{
    if(AudioPluginA2dpTaskIsBroadcaster(task))
        return BA_KICK_PERIOD;

    return AudioInputCommonGetKickPeriodFromCodec(AudioPluginA2dpTaskGetCodec(task));
}

static bool isUsingAptxAdaptiveInQ2qMode(audio_codec_t codec, aptx_adaptive_params_t aptx_ad_params)
{
    if ((codec == audio_codec_aptx_adaptive) && (aptx_ad_params.q2q_enabled))
        return TRUE;
    else
        return FALSE;
}

static void configureAndConnectChain(kymera_chain_handle_t chain, audio_codec_t decoder,
                                     uint32 sample_rate, A2dpPluginConnectParams *a2dp_connect_params)
{
    Operator operator;

    operator = ChainGetOperatorByRole(chain, rtp_role);
    if(operator != INVALID_OPERATOR)
        audioInputA2dpConfigureRtpOperator(operator, chain, decoder, sample_rate, a2dp_connect_params);

    operator = ChainGetOperatorByRole(chain, splitter_role);
    if(operator != INVALID_OPERATOR)
        OperatorsConfigureSplitter(operator, SPLITTER_BUFFER_SIZE, FALSE, operator_data_format_encoded);

    operator = ChainGetOperatorByRole(chain, aptx_demux_role);
    if(operator != INVALID_OPERATOR)
        OperatorsStandardSetSampleRate(operator, sample_rate);

    ChainConnect(chain);
}

void AudioInputA2dpConnectHandler(audio_input_context_t *ctx, Task task,
        const AUDIO_PLUGIN_CONNECT_MSG_T *msg)
{
    A2dpPluginConnectParams* a2dpParams = (A2dpPluginConnectParams *)msg->params;
    audio_codec_t codec = AudioPluginA2dpTaskGetCodec(task);
    chain_variant_mask_t chain_variant = chain_variant_mask_full_chain;

    if (isUsingAptxAdaptiveInQ2qMode(codec, a2dpParams->aptx_ad_params))
        chain_variant |= chain_variant_mask_without_rtp;

    AudioInputCommonDspPowerOn();
    OperatorsFrameworkSetKickPeriod(getKickPeriodFromTask(task));

    ctx->left_source = StreamSourceFromSink(msg->audio_sink);
    AudioInputCommonSetMusicProcessingContext(ctx, a2dpParams);

    ctx->tws.mute_until_start_forwarding = a2dpParams->peer_is_available;
    ctx->sample_rate = msg->rate;
    ctx->app_task = msg->app_task;

    if(!AudioPluginA2dpTaskIsBroadcaster(task))
    {
        audio_mixer_connect_t connect_data;

        ctx->chain = audioInputA2dpCreateChain(codec, chain_variant);
        configureAndConnectChain(ctx->chain, codec, ctx->sample_rate, a2dpParams);

        connect_data.left_src = ChainGetOutput(ctx->chain, path_pcm_left);
        connect_data.right_src = ChainGetOutput(ctx->chain, path_pcm_right);
        connect_data.connection_type = CONNECTION_TYPE_MUSIC_A2DP;
        connect_data.sample_rate = ctx->sample_rate;
        connect_data.channel_mode = CHANNEL_MODE_STEREO;
        connect_data.variable_rate = FALSE;

        ctx->mixer_input = AudioMixerConnect(&connect_data);

        PanicFalse(ctx->mixer_input != audio_mixer_input_error_none);

        /* Disconnect A2DP source as close to start as possible */
        StreamDisconnect(ctx->left_source, 0);
        StreamConnect(ctx->left_source, ChainGetInput(ctx->chain, path_encoded));
        ChainStart(ctx->chain);

        AudioInputCommonConnect(ctx, task);
    }
    else
    {
        chain_variant |= chain_variant_mask_without_splitter;
        ctx->chain = audioInputA2dpCreateChain(codec, chain_variant);
        configureAndConnectChain(ctx->chain, codec, ctx->sample_rate, a2dpParams);

        ctx->ba.plugin = a2dpParams->ba_output_plugin;
        audioInputA2dpBroadcastCreate(task, ctx->ba.plugin, ctx);

        SetAudioBusy(task);
    }
}

#ifdef HOSTED_TEST_ENVIRONMENT
void AudioInputA2dpTestReset(void)
{
    AudioMixerTestReset();
}
#endif
