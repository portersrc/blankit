#!/bin/bash
set -e


file_in=$1


# Average thread latency, in ms
t_lat=$(cat $file_in | grep "Latency" | awk '{print $2}')
t_lat=${t_lat::-2} # trim "ms"
echo $t_lat


# Thread latency stdev, in ms
t_stdev=$(cat $file_in | grep "Latency" | awk '{print $3}')
t_stdev=${t_stdev::-2} # trim "ms"
echo $t_stdev


# Average thread req/sec
#t_req_p_sec=$(cat $file_in | grep "Req/Sec" | awk '{print $2}')
#echo $t_req_p_sec


# Thread req/sec stdev
#t_req_p_sec_stdev=$(cat $file_in | grep "Req/Sec" | awk '{print $3}')
#echo $t_req_p_sec


# Socket errors - connect
err_connect=$(cat $file_in | grep "Socket errors" | awk '{print $4}')
if [ "$err_connect" != "" ]; then
    err_connect=${err_connect::-1} # trim ","
    echo $err_connect


    # Socket errors - read
    err_read=$(cat $file_in | grep "Socket errors" | awk '{print $6}')
    err_read=${err_read::-1} # trim ","
    echo $err_read


    # Socket errors - write
    err_write=$(cat $file_in | grep "Socket errors" | awk '{print $8}')
    err_write=${err_write::-1} # trim ","
    echo $err_write


    # Socket errors - timeout
    err_timeout=$(cat $file_in | grep "Socket errors" | awk '{print $10}')
    echo $err_timeout
else
    echo "NO SOCKET ERRORS"
fi


# Requests/sec
req_p_sec=$(cat $file_in | grep "Requests/sec"  | awk '{print $2}')
echo $req_p_sec


# Transfer/sec
transfer_p_sec=$(cat $file_in | grep "Transfer/sec"  | awk '{print $2}')
transfer_p_sec=${transfer_p_sec::-2} # trim "GB"
echo $transfer_p_sec

