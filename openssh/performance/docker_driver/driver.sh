#!/bin/bash



#
# Run the experiment without blankit
#

echo "Kicking off sshd without blankit"

pushd /root/debloat/pin_probe_debloat_experiments
./drive_sshd.sh nopin &
popd

sleep 5

echo "Beginning 3 consecutive runs of dijkstra over ssh"

{ time ssh localhost 'bash -s' < dijkstra.sh ; } &> nopin.1.out
{ time ssh localhost 'bash -s' < dijkstra.sh ; } &> nopin.2.out
{ time ssh localhost 'bash -s' < dijkstra.sh ; } &> nopin.3.out


echo "Killing sshd"
pkill sshd
sleep 1





#
# Run the experiment with blankit
#

echo "Kicking off sshd without blankit"

pushd /root/debloat/pin_probe_debloat_experiments
./drive_sshd.sh &
popd
sleep 20


echo "Beginning 3 consecutive runs of dijkstra over ssh with blankit"
{ time ssh localhost 'bash -s' < dijkstra.sh ; } &> pin.1.out
{ time ssh localhost 'bash -s' < dijkstra.sh ; } &> pin.2.out
{ time ssh localhost 'bash -s' < dijkstra.sh ; } &> pin.3.out

echo "Killing sshd"
pkill sshd





#
# Calculate the result
#

function get_avg() {
    pnp=$1
    SUM=0
    for i in {1..3}; do
        TIME=$(grep real ${pnp}.${i}.out | awk '{print $2}')                               

        if [ "${TIME:0:2}" != "0m" ]; then                                          
            # sanity check                                                          
            echo "Unexpected time. Should begin with '0m': ${TIME}"                 
            exit 1
        fi                                                                          
        if [ "${TIME: -1}" != "s" ]; then                                           
            # sanity check                                                          
            echo "Unexpected time. Should end with 's': ${TIME}"                    
            exit 1
        fi                                                                          

        TIME=${TIME:2}   # cut 0m                                                   
        TIME=${TIME::-1} # cut s               

        SUM=`echo "$SUM + $TIME" | bc -l`
    done

    AVG=`echo "$SUM / 3" | bc -l`

    echo $AVG
}


NOPIN_AVG=`get_avg "nopin"`
PIN_AVG=`get_avg "pin"`

echo "Avg without blankit: $NOPIN_AVG"
echo "Avg with blankit:    $PIN_AVG"
echo
printf "Slowdown: " && echo "$PIN_AVG / $NOPIN_AVG" | bc -l
echo
