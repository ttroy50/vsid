#!/bin/bash
#
# Assumed to run in build directory
# 

function reset_logs {
    cwd=$PWD
    cd logs
    find . -type f -name "vsid_pcap*.log" -exec rm -f {} \;
    cd $cwd
}

function move_result_log {
    echo "moving results to $1"
    mv logs/vsid_pcap_classifier_classification.log "$1"
}

function setup_model_and_meters {
    # Copy the model we want to use to the tmp locationa make sure meters are set
    echo "Copying $1 to $2"
    cp $1 $2
    # make sure RTMP regex is off
    ../tools/toggle_attribute_meter.py -f $2 -n all -e
    ../tools/toggle_attribute_meter.py -f $2 -n RtmpRegexMatchMeter
}

function setup_defining_limit {
    ../tools/set_defining_limit.py -f $1 -n $2
}

###############################################################
#
function run_with_limit {
    echo "RUN WITH LIMIT"

    CL=$1
    RUN_MODEL=$2
    THRESHOLD=$3
    # Cutoff Limit 50
    echo "With Cutoff Limit ${CL}"


    RES_DIR="../results/no_port_hints/all_protocols/dt${THRESHOLD}/co${CL}"
    mkdir -p $RES_DIR

    FLOW_RES_FILE="/media/matrim/toshiba/College/project/classification/all_result_flows.yaml"

    BASE_MODEL="../config/protocol_model_db.co${CL}.yaml"

    setup_model_and_meters $BASE_MODEL $RUN_MODEL 

    DEF_LIMITS="2 10 25 50 100 250"

    for DL in $DEF_LIMITS
    do
         if [ $CL -gt $DL ]; then
            echo "    DL = ${DL}"
            reset_logs

            setup_defining_limit $RUN_MODEL $DL

            ../tests/run_all.sh
            RES_FILE="${RES_DIR}/results_dl${DL}.co${CL}.dt${THRESHOLD}.yaml"
            move_result_log $RES_FILE  
            ../tools/analyse_results.py -f $FLOW_RES_FILE -r $RES_FILE > "${RES_FILE}.analysis"

            reset_logs
        fi
    done
}


###############################################################
#
function run_only_video_protocols {
    echo "RUN ONLY VIDEO PROTOCOLS"
    CL=$1
    RUN_MODEL=$2
    THRESHOLD=$3
    # Cutoff Limit 50
    echo "With Cutoff Limit ${CL}"

    RES_DIR="../results/no_port_hints/only_video/dt${THRESHOLD}/co${CL}"
    mkdir -p $RES_DIR

    FLOW_RES_FILE="/media/matrim/toshiba/College/project/classification/results_only_video.yaml"

    BASE_MODEL="../config/protocol_model_db.co${CL}.yaml"

    setup_model_and_meters $BASE_MODEL $RUN_MODEL 

    DEF_LIMITS="2 10 25 50 100 250"

    for DL in $DEF_LIMITS
    do
         if [ $CL -gt $DL ]; then
            echo "    DL = ${DL}"
            reset_logs

            setup_defining_limit $RUN_MODEL $DL

            ../tests/run_only_video.sh
            RES_FILE="${RES_DIR}/results_dl${DL}.co${CL}.dt${THRESHOLD}.yaml"
            move_result_log $RES_FILE  
            ../tools/analyse_results.py -f $FLOW_RES_FILE -r $RES_FILE > "${RES_FILE}.analysis"

            reset_logs
        fi
    done
}


echo $PWD

THRESHOLD=0.075
RUN_MODEL="../config/protocol_model_db.yaml"

CL=50
run_with_limit $CL $RUN_MODEL $THRESHOLD
run_only_video_protocols $CL $RUN_MODEL $THRESHOLD


CL=100
run_with_limit $CL $RUN_MODEL $THRESHOLD
run_only_video_protocols $CL $RUN_MODEL $THRESHOLD


CL=250
run_with_limit $CL $RUN_MODEL $THRESHOLD
run_only_video_protocols $CL $RUN_MODEL $THRESHOLD


CL=500
run_with_limit $CL $RUN_MODEL $THRESHOLD
run_only_video_protocols $CL $RUN_MODEL $THRESHOLD

CL=1000
run_with_limit $CL $RUN_MODEL $THRESHOLD
run_only_video_protocols $CL $RUN_MODEL $THRESHOLD
