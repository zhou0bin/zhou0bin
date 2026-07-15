#!/usr/bin/env bash

#  ******************************************************************************
#  * Copyright AutoX. 2025 All Rights Reserved.
#  *
#  * Lidar Service launch script.
#  *
#  *
#  *****************************************************************************
# shellcheck disable=SC1090,SC1091,SC2034,SC2119
export SCRIPT_PATH="$0"
DIR=$(realpath "$(dirname "${BASH_SOURCE[0]}")"/../../../../)
cd "$DIR" || exit 1
{
source "${DIR}/onboard/bootstrap/startup/xurban_base.sh"
source "${DIR}/onboard/bootstrap/startup/xcu_base.sh"
}

MODULE="lidar_control_service"
MODULE_PATH="${DIR}/bazel-bin/onboard/drivers/lidar_service/lidar_control_service"

function is_stopped() {
    NUM_PROCESSES="$(pgrep -c -f "${MODULE_PATH}")"
    if [ "${NUM_PROCESSES}" -eq 0 ]; then
        return 1
    else
        return 0
    fi
}

function start() {
    info "lidar control service start......"
    LOG="/dev/null"
    nohup "${MODULE_PATH}" >> /dev/null 2>&1 &
}

function stop() {
    if ! is_stopped; then
        info "${MODULE} process is not running!"
        return
    fi

    if pkill -f "$MODULE_PATH" > /dev/null 2>&1 ; then
        info "Successfully stopped module ${MODULE}."
    fi
}



function usage() {
    info "scripts/lidar_service.sh [start|stop|help]"
    echo ""
    echo "   start - start lidar_service"
    echo "   stop  - stop  lidar_service"
    echo "   help  - show this msg"
    echo ""
}

case $1 in
    start)
        start
        ;;
    stop)
        stop
        ;;
    restart)
        stop
        start
        ;;
    help)
        usage
        ;;
    *)
        start
        ;;
esac
