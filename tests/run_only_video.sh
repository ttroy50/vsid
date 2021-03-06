 #!/bin/bash

# Setup the database
../tools/toggle_protocol_model.py -f ../config/protocol_model_db.yaml -n all -e
../tools/toggle_protocol_model.py -f ../config/protocol_model_db.yaml -n DNS
../tools/toggle_protocol_model.py -f ../config/protocol_model_db.yaml -n HTTPS
../tools/toggle_protocol_model.py -f ../config/protocol_model_db.yaml -n HTTP

# run the tests
src/pcap_classifier/src/vsid_pcap_classifier -c ../config/config_pcap_classifier.yaml -p /media/matrim/toshiba/College/project/classification/rtsp_rtp//rtsp_fullmetal_14.pcap
src/pcap_classifier/src/vsid_pcap_classifier -c ../config/config_pcap_classifier.yaml -p /media/matrim/toshiba/College/project/classification/rtsp_rtp//rtsp_fullmetal_21.pcap
src/pcap_classifier/src/vsid_pcap_classifier -c ../config/config_pcap_classifier.yaml -p /media/matrim/toshiba/College/project/classification/rtsp_rtp//rtsp_fullmetal_22.pcap
src/pcap_classifier/src/vsid_pcap_classifier -c ../config/config_pcap_classifier.yaml -p /media/matrim/toshiba/College/project/classification/rtsp_rtp//rtsp_fullmetal_23.pcap
src/pcap_classifier/src/vsid_pcap_classifier -c ../config/config_pcap_classifier.yaml -p /media/matrim/toshiba/College/project/classification/rtsp_rtp//rtsp_fullmetal_24.pcap
src/pcap_classifier/src/vsid_pcap_classifier -c ../config/config_pcap_classifier.yaml -p /media/matrim/toshiba/College/project/classification/rtsp_rtp//rtsp_fullmetal_25.pcap
src/pcap_classifier/src/vsid_pcap_classifier -c ../config/config_pcap_classifier.yaml -p /media/matrim/toshiba/College/project/classification/rtsp_rtp//rtsp_fullmetal_26.pcap
src/pcap_classifier/src/vsid_pcap_classifier -c ../config/config_pcap_classifier.yaml -p /media/matrim/toshiba/College/project/classification/rtsp_rtp//rtsp_fullmetal_27.pcap
src/pcap_classifier/src/vsid_pcap_classifier -c ../config/config_pcap_classifier.yaml -p /media/matrim/toshiba/College/project/classification/rtsp_rtp//rtsp_fullmetal_29.pcap
src/pcap_classifier/src/vsid_pcap_classifier -c ../config/config_pcap_classifier.yaml -p /media/matrim/toshiba/College/project/classification/rtsp_rtp//rtsp_fullmetal_30.pcap
src/pcap_classifier/src/vsid_pcap_classifier -c ../config/config_pcap_classifier.yaml -p /media/matrim/toshiba/College/project/classification/rtsp_rtp//rtsp_fullmetal_31.pcap
src/pcap_classifier/src/vsid_pcap_classifier -c ../config/config_pcap_classifier.yaml -p /media/matrim/toshiba/College/project/classification/rtsp_rtp//rtsp_FullMetal_i_32.pcap
src/pcap_classifier/src/vsid_pcap_classifier -c ../config/config_pcap_classifier.yaml -p /media/matrim/toshiba/College/project/classification/rtsp_rtp//rtsp_FullMetal_i_33.pcap
src/pcap_classifier/src/vsid_pcap_classifier -c ../config/config_pcap_classifier.yaml -p /media/matrim/toshiba/College/project/classification/rtsp_rtp//rtsp_FullMetal_i_34.pcap
src/pcap_classifier/src/vsid_pcap_classifier -c ../config/config_pcap_classifier.yaml -p /media/matrim/toshiba/College/project/classification/rtsp_rtp//rtsp_FullMetal_i_35.pcap
src/pcap_classifier/src/vsid_pcap_classifier -c ../config/config_pcap_classifier.yaml -p /media/matrim/toshiba/College/project/classification/rtsp_rtp//rtsp_FullMetal_ii_36.pcap
src/pcap_classifier/src/vsid_pcap_classifier -c ../config/config_pcap_classifier.yaml -p /media/matrim/toshiba/College/project/classification/rtsp_rtp//rtsp_FullMetal_i_37.pcap
src/pcap_classifier/src/vsid_pcap_classifier -c ../config/config_pcap_classifier.yaml -p /media/matrim/toshiba/College/project/classification/rtsp_rtp//rtsp_FullMetal_i_38.pcap
src/pcap_classifier/src/vsid_pcap_classifier -c ../config/config_pcap_classifier.yaml -p /media/matrim/toshiba/College/project/classification/rtsp_rtp//rtsp_FullMetal_i_39.pcap
src/pcap_classifier/src/vsid_pcap_classifier -c ../config/config_pcap_classifier.yaml -p /media/matrim/toshiba/College/project/classification/rtsp_rtp//rtsp_FullMetal_i_40.pcap
src/pcap_classifier/src/vsid_pcap_classifier -c ../config/config_pcap_classifier.yaml -p /media/matrim/toshiba/College/project/classification/rtsp_rtp//rtsp_FullMetal_i_41.pcap
src/pcap_classifier/src/vsid_pcap_classifier -c ../config/config_pcap_classifier.yaml -p /media/matrim/toshiba/College/project/classification/rtsp_rtp//rtsp_i_fullmetal_10.pcap
src/pcap_classifier/src/vsid_pcap_classifier -c ../config/config_pcap_classifier.yaml -p /media/matrim/toshiba/College/project/classification/rtsp_rtp//rtsp_i_fullmetal_11.pcap
src/pcap_classifier/src/vsid_pcap_classifier -c ../config/config_pcap_classifier.yaml -p /media/matrim/toshiba/College/project/classification/rtsp_rtp//rtsp_i_fullmetal_9.pcap

src/pcap_classifier/src/vsid_pcap_classifier -c ../config/config_pcap_classifier.yaml -p /media/matrim/toshiba/College/project/classification/Youtube//youtube_html5_chrome.pcap

src/pcap_classifier/src/vsid_pcap_classifier -c ../config/config_pcap_classifier.yaml -p /media/matrim/toshiba/College/project/classification/http_live_streaming//ss_hls_cs_filtered.pcap
src/pcap_classifier/src/vsid_pcap_classifier -c ../config/config_pcap_classifier.yaml -p /media/matrim/toshiba/College/project/classification/http_live_streaming//ss_hls_freyr2_filtered.pcap
src/pcap_classifier/src/vsid_pcap_classifier -c ../config/config_pcap_classifier.yaml -p /media/matrim/toshiba/College/project/classification/http_live_streaming//ss_hls_work1_filtered.pcap
src/pcap_classifier/src/vsid_pcap_classifier -c ../config/config_pcap_classifier.yaml -p /media/matrim/toshiba/College/project/classification/http_live_streaming//ss_hls_work2_filtered.pcap
src/pcap_classifier/src/vsid_pcap_classifier -c ../config/config_pcap_classifier.yaml -p /media/matrim/toshiba/College/project/classification/http_live_streaming//ss_hls_work3_filtered.pcap
src/pcap_classifier/src/vsid_pcap_classifier -c ../config/config_pcap_classifier.yaml -p /media/matrim/toshiba/College/project/classification/http_live_streaming//ss_hls_work4_filtered.pcap
src/pcap_classifier/src/vsid_pcap_classifier -c ../config/config_pcap_classifier.yaml -p /media/matrim/toshiba/College/project/classification/http_progressive//digitalocean_nginx_http_80_wifi_home_firefox1.pcap
src/pcap_classifier/src/vsid_pcap_classifier -c ../config/config_pcap_classifier.yaml -p /media/matrim/toshiba/College/project/classification/http_progressive//digitalocean_nginx_http_80_wifi_home_firefox.pcap

src/pcap_classifier/src/vsid_pcap_classifier -c ../config/config_pcap_classifier.yaml -p /media/matrim/toshiba/College/project/classification/http_progressive//digitalocean_vlc_http_8080__vlc_freyr_wlan_home_mp4_tbbt_s8e11.pcap
src/pcap_classifier/src/vsid_pcap_classifier -c ../config/config_pcap_classifier.yaml -p /media/matrim/toshiba/College/project/classification/http_progressive//digitalocean_vlc_http_8080__vlc_freyr_wlan_home_mp4_tbbt_s8e12.pcap
src/pcap_classifier/src/vsid_pcap_classifier -c ../config/config_pcap_classifier.yaml -p /media/matrim/toshiba/College/project/classification/http_progressive//digitalocean_vlc_http_8080__vlc_freyr_wlan_offaly_mp4_tbbt_s8e15.pcap
src/pcap_classifier/src/vsid_pcap_classifier -c ../config/config_pcap_classifier.yaml -p /media/matrim/toshiba/College/project/classification/http_progressive//digitalocean_vlc_http_8080__vlc_freyr_wlan_offaly_mp4_tbbt_s8e16.pcap
src/pcap_classifier/src/vsid_pcap_classifier -c ../config/config_pcap_classifier.yaml -p /media/matrim/toshiba/College/project/classification/http_progressive//digitalocean_vlc_http_8080__vlc_freyr_wlan_offaly_mp4_tbbt_s8e17.pcap

src/pcap_classifier/src/vsid_pcap_classifier -c ../config/config_pcap_classifier.yaml -p /media/matrim/toshiba/College/project/classification/http_progressive//digitalocean_vlc_nginx_http80_wifi_home_vlc_3.pcap
src/pcap_classifier/src/vsid_pcap_classifier -c ../config/config_pcap_classifier.yaml -p /media/matrim/toshiba/College/project/classification/http_progressive//digitalocean_vlc_nginx_http80_wifi_home_vlc.pcap
src/pcap_classifier/src/vsid_pcap_classifier -c ../config/config_pcap_classifier.yaml -p /media/matrim/toshiba/College/project/classification/http_progressive//digiweb_http80_wifi_home_firefox_got_1.pcap
src/pcap_classifier/src/vsid_pcap_classifier -c ../config/config_pcap_classifier.yaml -p /media/matrim/toshiba/College/project/classification/http_progressive//http_FullMetal_47.pcap
src/pcap_classifier/src/vsid_pcap_classifier -c ../config/config_pcap_classifier.yaml -p /media/matrim/toshiba/College/project/classification/http_progressive//http_FullMetal_48.pcap
src/pcap_classifier/src/vsid_pcap_classifier -c ../config/config_pcap_classifier.yaml -p /media/matrim/toshiba/College/project/classification/http_progressive//http_FullMetal_49.pcap
src/pcap_classifier/src/vsid_pcap_classifier -c ../config/config_pcap_classifier.yaml -p /media/matrim/toshiba/College/project/classification/http_progressive//http_FullMetal_50.pcap
src/pcap_classifier/src/vsid_pcap_classifier -c ../config/config_pcap_classifier.yaml -p /media/matrim/toshiba/College/project/classification/http_progressive//http_FullMetal_51.pcap

src/pcap_classifier/src/vsid_pcap_classifier -c ../config/config_pcap_classifier.yaml -p /media/matrim/toshiba/College/project/classification/rtmp//rtmp_nbc_sport_vlc_starstream_ch24q1_freyr_home_wifi1.pcap
src/pcap_classifier/src/vsid_pcap_classifier -c ../config/config_pcap_classifier.yaml -p /media/matrim/toshiba/College/project/classification/rtmp//rtmp_rugby_first_half.pcap
src/pcap_classifier/src/vsid_pcap_classifier -c ../config/config_pcap_classifier.yaml -p /media/matrim/toshiba/College/project/classification/rtmp//rtmp_rugby_second_half.pcap
src/pcap_classifier/src/vsid_pcap_classifier -c ../config/config_pcap_classifier.yaml -p /media/matrim/toshiba/College/project/classification/rtmp//rtmp_tsn_vlc_starstream_ch23q1_freyr_home_wifi1.pcap
src/pcap_classifier/src/vsid_pcap_classifier -c ../config/config_pcap_classifier.yaml -p /media/matrim/toshiba/College/project/classification/rtmp//rtmp_tsn_vlc_starstream_ch23q1_freyr_home_wifi2.pcap
src/pcap_classifier/src/vsid_pcap_classifier -c ../config/config_pcap_classifier.yaml -p /media/matrim/toshiba/College/project/classification/rtmp//rtmp_tsn_vlc_starstream_ch23q1_freyr_home_wifi.pcap
src/pcap_classifier/src/vsid_pcap_classifier -c ../config/config_pcap_classifier.yaml -p /media/matrim/toshiba/College/project/classification/rtmp//rtpm_starstream.pcap
src/pcap_classifier/src/vsid_pcap_classifier -c ../config/config_pcap_classifier.yaml -p /media/matrim/toshiba/College/project/classification/rtmp//ss_ch3_cn_rtmp1.pcap
src/pcap_classifier/src/vsid_pcap_classifier -c ../config/config_pcap_classifier.yaml -p /media/matrim/toshiba/College/project/classification/rtmp//ss_charity_shield.pcap
src/pcap_classifier/src/vsid_pcap_classifier -c ../config/config_pcap_classifier.yaml -p /media/matrim/toshiba/College/project/classification/rtmp//ss_rtmp_work2.pcap
src/pcap_classifier/src/vsid_pcap_classifier -c ../config/config_pcap_classifier.yaml -p /media/matrim/toshiba/College/project/classification/rtmp//ss_rtmp_work4pcap.pcap
src/pcap_classifier/src/vsid_pcap_classifier -c ../config/config_pcap_classifier.yaml -p /media/matrim/toshiba/College/project/classification/rtmp//starstream_ch14q1_rom_udi_wifi_dell_home-1l.pcap

src/pcap_classifier/src/vsid_pcap_classifier -c ../config/config_pcap_classifier.yaml -p /media/matrim/toshiba/College/project/classification/http_progressive/http_ff_nginx1.pcap
src/pcap_classifier/src/vsid_pcap_classifier -c ../config/config_pcap_classifier.yaml -p /media/matrim/toshiba/College/project/classification/http_progressive/http_ff_nginx3.pcap
src/pcap_classifier/src/vsid_pcap_classifier -c ../config/config_pcap_classifier.yaml -p /media/matrim/toshiba/College/project/classification/http_progressive/http_ff_nginx.pcap
