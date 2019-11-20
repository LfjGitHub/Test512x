/****************************************************************************
Copyright (c) 2017-2018 Qualcomm Technologies International, Ltd.
Part of 6.3.0

FILE NAME
    audio_input_a2dp_forward.c

DESCRIPTION
    Handler for AUDIO_PLUGIN_START_FORWARDING_MSG message for a2dp source.
*/

#include "audio_config.h"
#include "audio_input_a2dp_chain_config.h"
#include "audio_input_a2dp_task.h"
#include "audio_input_a2dp_rtp_config.h"
#include "audio_input_a2dp_forward.h"

#include <audio.h>
#include <audio_config.h>
#include <audio_plugin_forwarding.h>
#include <audio_mixer.h>
#include <operators.h>

static bool codecCanBeForwarded(audio_codec_t codec)
{
    if(codec == audio_codec_sbc || codec == audio_codec_aptx ||
            codec == audio_codec_aac)
        return TRUE;

    return FALSE;
}

static void sendLatencyUpdateToAppTask(audio_input_context_t* ctx, uint16 target_ttp_in_tenths_of_ms)
{
    MAKE_AUDIO_MESSAGE(AUDIO_LATENCY_REPORT, message);
    message->latency = target_ttp_in_tenths_of_ms;
    message->estimated = TRUE;
    message->sink = StreamSinkFromSource(ctx->left_source);
    MessageSend(ctx->app_task, AUDIO_LATENCY_REPORT, message);
}

static bool isMonoMode(void)
{
    return (AudioConfigGetRenderingMode()==single_channel_rendering);
}

static void updateMonoModeForwardingParams(audio_input_context_t* ctx, audio_plugin_forwarding_params_t* params)
{
    if(isMonoMode())
    {
        audio_codec_t transcode_codec;

        params->forwarding.pcm.left_source = ChainGetOutput(ctx->chain, path_forwarding);
        params->forwarding.pcm.right_source = NULL;

        switch(params->source_codec)
        {
            case audio_codec_sbc:
                transcode_codec = audio_codec_sbc;
            break;

            case audio_codec_aptx:
                transcode_codec = audio_codec_none;
            break;

            case audio_codec_aac:
                transcode_codec = audio_codec_none;
            break;

            default:
                transcode_codec = audio_codec_none;
            break;
        }

        params->transcode.codec = transcode_codec;
    }

}
void audioInputA2dpForwardCreate(Task task, const AUDIO_PLUGIN_START_FORWARDING_MSG_T* req, audio_input_context_t* ctx)
{
    audio_plugin_forwarding_params_t params;
    params.source_codec = AudioPluginA2dpTaskGetCodec(task);

    if(codecCanBeForwarded(params.source_codec))
    {
        Operator rtp_op = ChainGetOperatorByRole(ctx->chain, rtp_role);
        ctx->tws.plugin = req->output_plugin;

        params.forwarding.codec_source = ChainGetOutput(ctx->chain, path_forwarding);
        params.forwarding_sink = req->forwarding_sink;
        params.content_protection = req->content_protection;
        params.sample_rate = ctx->sample_rate;
        params.transcode.codec = audio_codec_none;
        params.ttp_latency = AudioConfigGetTWSTtpLatency();

        updateMonoModeForwardingParams(ctx, &params);

        AudioPluginForwardingCreate(task, ctx->tws.plugin, &params);

        audioInputConfigureRtpLatency(rtp_op, &params.ttp_latency);

        SetAudioBusy(task);
    }
}

void audioInputA2dpForwardStart(Task task, audio_input_context_t *ctx, const AUDIO_PLUGIN_FORWARDING_CREATE_CFM_T *cfm)
{
    if(cfm->status == audio_output_success)
    {
        uint16 target_ttp_in_tenths_of_ms = (uint16)(AudioConfigGetTWSTtpLatency().target_in_ms * 10);

        AudioPluginForwardingStart(task, ctx->tws.plugin);
        sendLatencyUpdateToAppTask(ctx, target_ttp_in_tenths_of_ms);
        AudioMixerUpdateChannelMode(AudioConfigGetTwsChannelModeLocal());
        return;
    }

    /* Reset tws state, forwarding failed */
    ctx->tws.plugin = NULL;
    SetAudioBusy(NULL);
}

void AudioPluginA2dpForwardEnableForwardingOutput(Task task, audio_input_context_t* ctx)
{
    audio_codec_t source_codec = AudioPluginA2dpTaskGetCodec(task);

    Operator switched_passthrough = ChainGetOperatorByRole(ctx->chain, switched_passthrough_role);
    if ((switched_passthrough) && (source_codec != audio_codec_aac))
        OperatorsSetSwitchedPassthruMode(switched_passthrough, spc_op_mode_passthrough);

    Operator splitter = ChainGetOperatorByRole(ctx->chain, splitter_role);
    if (splitter)
        OperatorsSplitterEnableSecondOutput(splitter, TRUE);

    SetAudioBusy(NULL);
}

/* Disable forwarded output */
static void AudioPluginA2dpForwardDisableForwardingOutput(Task task, audio_input_context_t* ctx)
{
    audio_codec_t source_codec = AudioPluginA2dpTaskGetCodec(task);

    Operator switched_passthrough = ChainGetOperatorByRole(ctx->chain, switched_passthrough_role);
    if ((switched_passthrough) && (source_codec != audio_codec_aac))
        OperatorsSetSwitchedPassthruMode(switched_passthrough, spc_op_mode_consumer);

    Operator splitter = ChainGetOperatorByRole(ctx->chain, splitter_role);
    if (splitter)
        OperatorsSplitterEnableSecondOutput(splitter, FALSE);
}

void audioInputA2dpForwardDestroy(Task task, audio_input_context_t* ctx)
{
    Operator rtp_op = ChainGetOperatorByRole(ctx->chain, rtp_role);
    ttp_latency_t ttp_latency = AudioConfigGetA2DPTtpLatency();
    uint16 target_ttp_in_tenths_of_ms = (uint16)(ttp_latency.target_in_ms * 10);

    SetAudioBusy(task);
    AudioPluginA2dpForwardDisableForwardingOutput(task, ctx);
    AudioMixerUpdateChannelMode(CHANNEL_MODE_STEREO);
    audioInputConfigureRtpLatency(rtp_op, &ttp_latency);
    sendLatencyUpdateToAppTask(ctx, target_ttp_in_tenths_of_ms);
    AudioPluginForwardingDestroy(task, ctx->tws.plugin);
}

void AudioPluginA2dpForwardHandleDestroyCfm(audio_input_context_t* ctx)
{
    SetAudioBusy(NULL);
    ctx->tws.plugin = NULL;
}
