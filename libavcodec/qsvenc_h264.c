/*
 * Intel MediaSDK QSV based H.264 enccoder
 *
 * copyright (c) 2013 Yukinori Yamazoe
 *
 * This file is part of Libav.
 *
 * Libav is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * Libav is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Libav; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */


#include <stdint.h>
#include <sys/types.h>
#include <mfx/mfxvideo.h>

#include "avcodec.h"
#include "internal.h"
#include "h264.h"
#include "qsv.h"
#include "qsvenc.h"

typedef struct QSVH264EncContext {
    AVClass *class;
    QSVEncContext qsv;
} QSVH264EncContext;

static av_cold int qsv_enc_init(AVCodecContext *avctx)
{
    QSVH264EncContext *q = avctx->priv_data;

    return ff_qsv_enc_init(avctx, &q->qsv);
}

static int qsv_enc_frame(AVCodecContext *avctx, AVPacket *pkt,
                         const AVFrame *frame, int *got_packet)
{
    QSVH264EncContext *q = avctx->priv_data;

    return ff_qsv_enc_frame(avctx, &q->qsv, pkt, frame, got_packet);
}

static av_cold int qsv_enc_close(AVCodecContext *avctx)
{
    QSVH264EncContext *q = avctx->priv_data;

    return ff_qsv_enc_close(avctx, &q->qsv);
}

#define OFFSET(x) offsetof(QSVH264EncContext, x)
#define VE AV_OPT_FLAG_VIDEO_PARAM | AV_OPT_FLAG_ENCODING_PARAM
static const AVOption options[] = {
    { "async_depth", "Number which limits internal frame buffering", OFFSET(qsv.async_depth), AV_OPT_TYPE_INT, { .i64 = ASYNC_DEPTH_DEFAULT }, 0, INT_MAX, VE },
    { "timeout", "Maximum timeout in milliseconds when the device has been busy", OFFSET(qsv.timeout), AV_OPT_TYPE_INT, { .i64 = TIMEOUT_DEFAULT }, 0, INT_MAX, VE },
    { "profile", NULL, OFFSET(qsv.profile), AV_OPT_TYPE_INT, { .i64 = MFX_PROFILE_UNKNOWN }, 0, INT_MAX, VE, "profile" },
    { "unknown" , NULL, 0, AV_OPT_TYPE_CONST, { .i64 = MFX_PROFILE_UNKNOWN      }, INT_MIN, INT_MAX, VE, "profile" },
    { "baseline", NULL, 0, AV_OPT_TYPE_CONST, { .i64 = MFX_PROFILE_AVC_BASELINE }, INT_MIN, INT_MAX, VE, "profile" },
    { "main"    , NULL, 0, AV_OPT_TYPE_CONST, { .i64 = MFX_PROFILE_AVC_MAIN     }, INT_MIN, INT_MAX, VE, "profile" },
    { "high"    , NULL, 0, AV_OPT_TYPE_CONST, { .i64 = MFX_PROFILE_AVC_HIGH     }, INT_MIN, INT_MAX, VE, "profile" },
    { "level", NULL, OFFSET(qsv.level), AV_OPT_TYPE_INT, { .i64 = MFX_LEVEL_UNKNOWN }, 0, INT_MAX, VE, "level" },
    { "unknown", NULL, 0, AV_OPT_TYPE_CONST, { .i64 = MFX_LEVEL_UNKNOWN }, INT_MIN, INT_MAX, VE, "level" },
    { "1"      , NULL, 0, AV_OPT_TYPE_CONST, { .i64 = MFX_LEVEL_AVC_1   }, INT_MIN, INT_MAX, VE, "level" },
    { "1b"     , NULL, 0, AV_OPT_TYPE_CONST, { .i64 = MFX_LEVEL_AVC_1b  }, INT_MIN, INT_MAX, VE, "level" },
    { "11"     , NULL, 0, AV_OPT_TYPE_CONST, { .i64 = MFX_LEVEL_AVC_11  }, INT_MIN, INT_MAX, VE, "level" },
    { "12"     , NULL, 0, AV_OPT_TYPE_CONST, { .i64 = MFX_LEVEL_AVC_12  }, INT_MIN, INT_MAX, VE, "level" },
    { "13"     , NULL, 0, AV_OPT_TYPE_CONST, { .i64 = MFX_LEVEL_AVC_13  }, INT_MIN, INT_MAX, VE, "level" },
    { "2"      , NULL, 0, AV_OPT_TYPE_CONST, { .i64 = MFX_LEVEL_AVC_2   }, INT_MIN, INT_MAX, VE, "level" },
    { "21"     , NULL, 0, AV_OPT_TYPE_CONST, { .i64 = MFX_LEVEL_AVC_21  }, INT_MIN, INT_MAX, VE, "level" },
    { "22"     , NULL, 0, AV_OPT_TYPE_CONST, { .i64 = MFX_LEVEL_AVC_22  }, INT_MIN, INT_MAX, VE, "level" },
    { "3"      , NULL, 0, AV_OPT_TYPE_CONST, { .i64 = MFX_LEVEL_AVC_3   }, INT_MIN, INT_MAX, VE, "level" },
    { "31"     , NULL, 0, AV_OPT_TYPE_CONST, { .i64 = MFX_LEVEL_AVC_31  }, INT_MIN, INT_MAX, VE, "level" },
    { "32"     , NULL, 0, AV_OPT_TYPE_CONST, { .i64 = MFX_LEVEL_AVC_32  }, INT_MIN, INT_MAX, VE, "level" },
    { "4"      , NULL, 0, AV_OPT_TYPE_CONST, { .i64 = MFX_LEVEL_AVC_4   }, INT_MIN, INT_MAX, VE, "level" },
    { "41"     , NULL, 0, AV_OPT_TYPE_CONST, { .i64 = MFX_LEVEL_AVC_41  }, INT_MIN, INT_MAX, VE, "level" },
    { "42"     , NULL, 0, AV_OPT_TYPE_CONST, { .i64 = MFX_LEVEL_AVC_42  }, INT_MIN, INT_MAX, VE, "level" },
    { "5"      , NULL, 0, AV_OPT_TYPE_CONST, { .i64 = MFX_LEVEL_AVC_5   }, INT_MIN, INT_MAX, VE, "level" },
    { "51"     , NULL, 0, AV_OPT_TYPE_CONST, { .i64 = MFX_LEVEL_AVC_51  }, INT_MIN, INT_MAX, VE, "level" },
    { "52"     , NULL, 0, AV_OPT_TYPE_CONST, { .i64 = MFX_LEVEL_AVC_52  }, INT_MIN, INT_MAX, VE, "level" },
    { NULL },
};

static const AVClass class = {
    .class_name = "h264_qsv",
    .item_name  = av_default_item_name,
    .option     = options,
    .version    = LIBAVUTIL_VERSION_INT,
};

static const AVCodecDefault qsv_enc_defaults[] = {
    { "i_qfactor", "-0.96" },
    { "i_qoffset", "-1.0" },
    { "b_qfactor", "1.04" },
    { "b_qoffset", "1.0" },
    { "coder",     "-1" },
    { "g",         "-1" },
    { "bf",        "-1" },
    { "refs",      "-1" },
    { NULL },
};

AVCodec ff_h264_qsv_encoder = {
    .name           = "h264_qsv",
    .long_name      = NULL_IF_CONFIG_SMALL("H.264 / AVC / MPEG-4 AVC / MPEG-4 part 10 (Intel Quick Sync Video acceleration)"),
    .priv_data_size = sizeof(QSVH264EncContext),
    .type           = AVMEDIA_TYPE_VIDEO,
    .id             = AV_CODEC_ID_H264,
    .init           = qsv_enc_init,
    .encode2        = qsv_enc_frame,
    .close          = qsv_enc_close,
    .capabilities   = CODEC_CAP_DELAY,
    .pix_fmts       = (const enum AVPixelFormat[]){ AV_PIX_FMT_NV12, AV_PIX_FMT_NONE },
    .priv_class     = &class,
    .defaults       = qsv_enc_defaults,
};