#FREESWITCH CMAKE file
#Shane Burrell 2009 (supjigator)

project(freeswitch)

cmake_minimum_required(VERSION 2.6)

INCLUDE_DIRECTORIES(./src/include/
        ./libs/srtp/crypto/include/
        ./libs/srtp/include/
        ./libs/apr/include/
        ./libs/apr-util/include/
        ./libs/libteletone/src/
        /usr/local/include/sofia-sip-1.13
        )

ADD_COMPILE_DEFINITIONS(HAVE_OPENSSL=1)

SET(freeswitch_la_SRCS

        ./src/switch_apr.c
        ./src/switch_apr_queue.c
        ./src/switch_buffer.c
        ./src/switch_core_cert.c
        ./src/switch_caller.c
        ./src/switch_channel.c
        ./src/switch_console.c
        ./src/switch_core_media_bug.c
        ./src/switch_core_timer.c
        ./src/switch_core_asr.c
        ./src/switch_core_event_hook.c
        ./src/switch_core_speech.c
        ./src/switch_core_memory.c
        ./src/switch_core_media.c
        ./src/switch_core_codec.c
        ./src/switch_core_file.c
        ./src/switch_core_hash.c
        ./src/switch_core_sqldb.c
        ./src/switch_core_session.c
        ./src/switch_core_directory.c
        ./src/switch_core_state_machine.c
        ./src/switch_core_io.c
        ./src/switch_core_rwlock.c
        ./src/switch_core_port_allocator.c
        ./src/switch_core.c
        ./src/switch_core_video.c
        ./src/switch_estimators.c
        ./src/switch_hashtable.c
        ./src/switch_ivr_say.c
        ./src/switch_jitterbuffer.c
        ./src/switch_scheduler.c
        ./src/switch_core_db.c
        ./src/switch_dso.c
        ./src/switch_json.c
        ./src/switch_limit.c
        ./src/switch_curl.c
        ./src/switch_loadable_module.c
        ./src/switch_utils.c
        ./src/switch_mprintf.c
        ./src/switch_msrp.c
        ./src/switch_nat.c
        ./src/switch_odbc.c
        ./src/switch_packetizer.c
        ./src/switch_profile.c
        ./src/switch_sdp.c
        ./src/switch_spandsp.c
        ./src/switch_speex.c
        ./src/switch_event.c
        ./src/switch_resample.c
        ./src/switch_regex.c
        ./src/switch_rtp.c
        ./src/switch_ivr_bridge.c
        ./src/switch_ivr_originate.c
        ./src/switch_ivr_async.c
        ./src/switch_ivr_play_say.c
        ./src/switch_ivr_menu.c
        ./src/switch_ivr.c
        ./src/switch_stun.c
        ./src/switch_swig.c
        ./src/switch_utf8.c
        ./src/switch_vad.c
        ./src/switch_version.c
        ./src/switch_vpx.c
        ./src/switch_xml_config.c
        ./src/tone2wav.c
        ./src/switch_log.c
        ./src/switch_xml.c
        ./src/switch_config.c
        ./src/switch_time.c
        ./src/switch_cpp.cpp
        ./src/cJSON.c
        ./src/cJSON_Utils.c
        ./src/fs_encode.c
        ./src/fs_tts.c
        ./src/inet_pton.c
        ./src/g711.c
        ./src/switch_pcm.c
        ./libs/libteletone/src/libteletone_detect.c
        ./libs/libteletone/src/libteletone_generate.c

        ./src/include/switch_am_config.h
        ./src/include/switch.h
        ./src/include/switch_apr.h
        ./src/include/switch_buffer.h
        ./src/include/switch_caller.h
        ./src/include/switch_channel.h
        ./src/include/switch_console.h
        ./src/include/switch_core_event_hook.h
        ./src/include/switch_scheduler.h
        ./src/include/switch_core.h
        ./src/include/switch_core_db.h
        ./src/include/switch_config.h
        ./src/include/switch_event.h
        ./src/include/switch_frame.h
        ./src/include/switch_ivr.h
        ./src/include/switch_dso.h
        ./src/include/switch_loadable_module.h
        ./src/include/switch_module_interfaces.h
        ./src/include/switch_platform.h
        ./src/include/switch_resample.h
        ./src/include/switch_regex.h
        ./src/include/switch_types.h
        ./src/include/switch_utils.h
        ./src/include/switch_rtp.h
        ./src/include/switch_stun.h
        ./src/include/switch_log.h
        ./src/include/switch_xml.h
        ./src/include/switch_cpp.h
        ./src/include/switch_core_video.h
        ./src/include/switch_core_media.h
        ./src/include/switch_curl.h
        ./src/include/switch_estimators.h
        ./src/include/cc.h
        ./src/include/g711.h
        ./src/include/SimpleGlob.h
        ./src/include/switch_bitpack.h
        ./src/include/switch_cJSON.h
        ./src/include/switch_cJSON_Utils.h
        ./src/include/switch_hashtable.h
        ./src/include/switch_image.h
        ./src/include/switch_jitterbuffer.h
        ./src/include/switch_json.h
        ./src/include/switch_limit.h
        ./src/include/switch_mprintf.h
        ./src/include/switch_msrp.h
        ./src/include/switch_nat.h
        ./src/include/switch_packetizer.h
        ./src/include/switch_private.h
        ./src/include/switch_profile.h
        ./src/include/switch_rtcp_frame.h
        ./src/include/switch_spandsp.h
        ./src/include/switch_swigable_cpp.h
        ./src/include/switch_utf8.h
        ./src/include/switch_vad.h
        ./src/include/switch_version.h
        ./src/include/switch_vpx.h
        ./src/include/switch_xml_config.h


        ./src/include/private/switch_apr_pvt.h
        ./src/include/private/switch_core_pvt.h
        ./src/include/private/switch_hashtable_private.h

        ./src/include/test/switch_fct.h
        ./src/include/test/switch_test.h

        ./libs/libteletone/src/libteletone_detect.h
        ./libs/libteletone/src/libteletone_generate.h
        ./libs/libteletone/src/libteletone.h
        ./src/include/switch_odbc.h
        ./src/include/switch_ssl.h

        )


ADD_LIBRARY(freeswitch_la ${freeswitch_la_SRCS})

ADD_EXECUTABLE(freeswitch ./src/switch.c ${freeswitch_la_SRCS} )




