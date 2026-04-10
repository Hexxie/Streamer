#include <iostream>
#include <string>

#include "AVFoundationInput.hpp"
#include "VideoDecoder.hpp"
#include "FileOutput.hpp"

extern "C"
{
#include <libavdevice/avdevice.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include <libavutil/dict.h>
#include <libavutil/error.h>
#include <libavutil/opt.h>
#include <libswscale/swscale.h>
#include <libavutil/time.h>
}

void print_error(int err_code)
{
  char buf[256];
  av_strerror(err_code, buf, sizeof(buf));
  std::cerr << buf << std::endl;
}

int main()
{
  int ret = 0;

  streamer::AVFoundationInput input;
  AVFormatContext *input_ctx = input.getContext();

  AVStream *in_stream = nullptr;
  if (input.hasVideo()) {
    in_stream = input_ctx->streams[input.getVideoStreamIdx()];
  }

  std::cout << "Input codec_id=" << in_stream->codecpar->codec_id
            << " width=" << in_stream->codecpar->width
            << " height=" << in_stream->codecpar->height
            << " pix_fmt=" << in_stream->codecpar->format
            << "\n";

  // -------------------------
  // 2. Open decoder
  // -------------------------
  streamer::VideoDecoder decoder(input, input.getVideoStreamIdx());
  AVCodecContext *decoder_ctx = decoder.getDecoderContext();

  // -------------------------
  // 3. Create output mp4
  // -------------------------
  streamer::FileOutput output("output.mp4", "mp4");
  AVFormatContext *output_ctx = output.getOutputContext();

  // -------------------------
  // 4. Open encoder
  // -------------------------
  const AVCodec *encoder = avcodec_find_encoder_by_name("h264_videotoolbox");
  if (!encoder)
  {
    std::cerr << "Encoder libx264rgb not found\n";
    avformat_free_context(output_ctx);
    return 1;
  }

  AVStream *out_stream = avformat_new_stream(output_ctx, nullptr);
  if (!out_stream)
  {
    std::cerr << "Failed to create output stream\n";
    avformat_free_context(output_ctx);
    return 1;
  }

  AVCodecContext *encoder_ctx = avcodec_alloc_context3(encoder);
  if (!encoder_ctx)
  {
    std::cerr << "Could not allocate encoder context\n";
    avformat_free_context(output_ctx);
    return 1;
  }

  encoder_ctx->codec_id = encoder->id;
  encoder_ctx->codec_type = AVMEDIA_TYPE_VIDEO;
  encoder_ctx->width = decoder_ctx->width;
  encoder_ctx->height = decoder_ctx->height;
  encoder_ctx->time_base = AVRational{1, 30};
  encoder_ctx->framerate = AVRational{30, 1};

  // Для libx264rgb зазвичай підходить RGB24.
  // Якщо впаде на open2 - значить треба буде подивитися реальний frame->format
  // і/або додати swscale.
  encoder_ctx->pix_fmt = AV_PIX_FMT_NV12;
  encoder_ctx->gop_size = 12;
  encoder_ctx->max_b_frames = 0;
  encoder_ctx->bit_rate = 2'000'000;

  // корисно для mp4
  if (output_ctx->oformat->flags & AVFMT_GLOBALHEADER)
  {
    encoder_ctx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
  }

  ret = avcodec_open2(encoder_ctx, encoder, nullptr);
  if (ret < 0)
  {
    std::cerr << "Failed to open encoder: ";
    print_error(ret);
    avformat_free_context(output_ctx);
    return 1;
  }

  ret = avcodec_parameters_from_context(out_stream->codecpar, encoder_ctx);
  if (ret < 0)
  {
    std::cerr << "Failed to copy encoder params to output stream: ";
    print_error(ret);
    avformat_free_context(output_ctx);
    return 1;
  }

  out_stream->time_base = encoder_ctx->time_base;

  // -------------------------
  // 5. Open output file
  // -------------------------
  if (!(output_ctx->oformat->flags & AVFMT_NOFILE))
  {
    ret = avio_open(&output_ctx->pb, "output.mp4", AVIO_FLAG_WRITE);
    if (ret < 0)
    {
      std::cerr << "Failed to open output file: ";
      print_error(ret);
      avformat_free_context(output_ctx);
      return 1;
    }
  }

  ret = avformat_write_header(output_ctx, nullptr);
  if (ret < 0)
  {
    std::cerr << "Failed to write mp4 header: ";
    print_error(ret);
    avio_closep(&output_ctx->pb);
    avformat_free_context(output_ctx);
    return 1;
  }

  // Create Sws context
  SwsContext *sws_ctx = sws_getContext(
      decoder_ctx->width,
      decoder_ctx->height,
      decoder_ctx->pix_fmt,
      encoder_ctx->width,
      encoder_ctx->height,
      encoder_ctx->pix_fmt,
      SWS_BILINEAR,
      nullptr,
      nullptr,
      nullptr);
  if (!sws_ctx)
  {
    std::cerr << "Failed to create sws context\n";
    av_write_trailer(output_ctx);
    avio_closep(&output_ctx->pb);
    avformat_free_context(output_ctx);
    return 1;
  }

  // -------------------------
  // 6. Read/decode/encode/write loop
  // -------------------------
  AVPacket *pkt = av_packet_alloc();
  AVPacket *out_pkt = av_packet_alloc();
  AVFrame *frame = av_frame_alloc();
  AVFrame *enc_frame = av_frame_alloc();
  enc_frame->format = encoder_ctx->pix_fmt;
  enc_frame->width = encoder_ctx->width;
  enc_frame->height = encoder_ctx->height;

  av_frame_get_buffer(enc_frame, 32);
  ret = av_frame_get_buffer(enc_frame, 32);
  if (ret < 0)
  {
    std::cerr << "av_frame_get_buffer failed: ";
    print_error(ret);
    av_packet_free(&pkt);
    av_packet_free(&out_pkt);
    av_frame_free(&frame);
    av_frame_free(&enc_frame);
    av_write_trailer(output_ctx);
    avio_closep(&output_ctx->pb);
    avformat_free_context(output_ctx);
    return 1;
  }

  if (!pkt || !out_pkt || !frame || !enc_frame)
  {
    std::cerr << "Failed to allocate packet/frame\n";
    av_packet_free(&pkt);
    av_packet_free(&out_pkt);
    av_frame_free(&frame);
    av_frame_free(&enc_frame);
    av_write_trailer(output_ctx);
    avio_closep(&output_ctx->pb);
    avformat_free_context(output_ctx);
    return 1;
  }

  int64_t frame_index = 0;
  int max_frames = 150; // ~5 seconds at 30 fps
  int written_frames = 0;
  bool printed_first_frame = false;

  while (written_frames < max_frames)
  {
    ret = av_read_frame(input_ctx, pkt);
    if (ret == AVERROR(EAGAIN))
    {
      av_usleep(10000); // 10 ms
      continue;
    }
    if (ret < 0)
    {
      std::cerr << "av_read_frame failed: ";
      print_error(ret);
      break;
    }

    //if (pkt->stream_index != video_stream_index)
    //{
   //   av_packet_unref(pkt);
  //    continue;
  //  }

    ret = avcodec_send_packet(decoder_ctx, pkt);
    av_packet_unref(pkt);

    if (ret < 0)
    {
      std::cerr << "avcodec_send_packet failed: ";
      print_error(ret);
      break;
    }

    while (true)
    {
      ret = avcodec_receive_frame(decoder_ctx, frame);

      if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
      {
        break;
      }

      if (ret < 0)
      {
        std::cerr << "avcodec_receive_frame failed: ";
        print_error(ret);
        goto cleanup;
      }

      if (!printed_first_frame)
      {
        std::cout << "First decoded frame: "
                  << "width=" << frame->width
                  << " height=" << frame->height
                  << " format=" << frame->format
                  << "\n";
        printed_first_frame = true;
      }

      av_frame_make_writable(enc_frame);

      sws_scale(
          sws_ctx,
          frame->data,
          frame->linesize,
          0,
          frame->height,
          enc_frame->data,
          enc_frame->linesize);

      enc_frame->pts = frame_index++;
      ret = avcodec_send_frame(encoder_ctx, enc_frame);
      av_frame_unref(frame);

      if (ret < 0)
      {
        std::cerr << "avcodec_send_frame failed: ";
        print_error(ret);
        goto cleanup;
      }

      while (true)
      {
        ret = avcodec_receive_packet(encoder_ctx, out_pkt);

        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
        {
          break;
        }

        if (ret < 0)
        {
          std::cerr << "avcodec_receive_packet failed: ";
          print_error(ret);
          goto cleanup;
        }

        av_packet_rescale_ts(out_pkt, encoder_ctx->time_base, out_stream->time_base);
        out_pkt->stream_index = out_stream->index;

        ret = av_interleaved_write_frame(output_ctx, out_pkt);
        av_packet_unref(out_pkt);

        if (ret < 0)
        {
          std::cerr << "av_interleaved_write_frame failed: ";
          print_error(ret);
          goto cleanup;
        }

        written_frames++;
        if (written_frames >= max_frames)
        {
          break;
        }
      }

      if (written_frames >= max_frames)
      {
        break;
      }
    }
  }

  // flush encoder
  ret = avcodec_send_frame(encoder_ctx, nullptr);
  if (ret >= 0)
  {
    while (true)
    {
      ret = avcodec_receive_packet(encoder_ctx, out_pkt);
      if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
      {
        break;
      }
      if (ret < 0)
      {
        std::cerr << "flush avcodec_receive_packet failed: ";
        print_error(ret);
        break;
      }

      av_packet_rescale_ts(out_pkt, encoder_ctx->time_base, out_stream->time_base);
      out_pkt->stream_index = out_stream->index;
      av_interleaved_write_frame(output_ctx, out_pkt);
      av_packet_unref(out_pkt);
    }
  }

cleanup:
  av_write_trailer(output_ctx);

  av_packet_free(&pkt);
  av_packet_free(&out_pkt);
  av_frame_free(&frame);
  av_frame_free(&enc_frame);

  if (!(output_ctx->oformat->flags & AVFMT_NOFILE))
  {
    avio_closep(&output_ctx->pb);
  }
  if (sws_ctx)
  {
    sws_freeContext(sws_ctx);
  }

  avformat_free_context(output_ctx);

  std::cout << "Done. Wrote output.mp4\n";
  return 0;
}