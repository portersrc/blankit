#!/bin/bash

if [ ! -e "data.txt" ]; then
cat > data.txt <<EOF
**************************
*S* *                    *
* * *  *  *************  *
* *   *    ************  *
*    *                   *
************** ***********
*                        *
** ***********************
*      *              G  *
*  *      *********** *  *
*    *        ******* *  *
*       *                *
**************************
EOF
fi


MSG () { echo "$@"; }
DIE () { MSG ERROR "$@"; exit 1; }
x_of () { echo ${1%,*}; }
y_of () { echo ${1#*,}; }

function load_data_file () {
  local mazename="$1"
  local datafile="$2"
  local y=0
  local xmax=0
  [ -r "$datafile" ] || DIE "No data file given"
  while read line
    do
      let y++
      for x in $(seq 0 ${#line})
      do
          local status=${line:$x:1}
          local value
          case "$status" in
              "*"|" ") value="$status";;
              "S") value="S"; eval "${mazename}_start=$((x+1)),$y";;
              "G") value="G"; eval "${mazename}_goal=$((x+1)),$y";;
          esac
          eval "${mazename}_DATA_$((x+1))_${y}='$value'"
          if [ $x -gt $xmax ]; then
              xmax=$x
          fi
      done
  done < "$datafile"
  eval "${mazename}_height=$y"
  eval "${mazename}_width=$xmax"
}

function dump_data () {
  local mazename="$1"
  local width=$(eval echo \$"${mazename}_width")
  local height=$(eval echo \$"${mazename}_height")
  echo "---- $mazename: ${width}x${height} ----"
  echo "   1   5    1   5    1"
  for y in `seq 1 $height`
  do
      printf '%2d ' $y
      for x in `seq 1 $width`
      do
          printf "%c" "`get_value ${mazename}_DATA $x,$y`"
      done
      printf '\n'
  done
}

function get_value() {
  local mazename="$1"
  local xy="$2"
  local value
  eval value=\"\$"${mazename}_$(x_of $xy)_$(y_of $xy)"\"
  echo "$value"
}

function set_value () {
  local mazename="$1"
  local xy="$2"
  local value="$3"
  eval "${mazename}_$(x_of $xy)_$(y_of $xy)='$value'"
}

function get_adj () {
  local xy="$1"
  local dir="$2"
  local x=$(x_of $xy)
  local y=$(y_of $xy)

  case $dir in
      N) echo $x,$((y-1));;
      E) echo $((x+1)),$y;;
      S) echo $x,$((y+1));;
      W) echo $((x-1)),$y;;
  esac
}

function get_from () {
  local mazename="$1"
  local xy="$2"
  local value
  eval value=\"\$"${mazename}_FROM_$(x_of $xy)_$(y_of $xy)"\"
  echo "$value"
}

function is_reachable () {
  local mazename="$1"
  local xy="$2"

  local data=$(get_value ${mazename}_DATA $xy)
  case "$data" in
      '*')
          return 1;;
      S|G|_|@|' ') return 0;;
      *) DIE "BUG: Unknown data in $mazename:$xy='$data'";;
  esac
}

function is_not_yet_reached () {
  local mazename="$1"
  local xy="$2"
  local value=$(get_value $mazename $xy)
  if [ -z "$value" ]
  then
      return 0
  else
      return 1
  fi
}

function search_maze () {
  local mazename="$1"
  local start="$2"
  MSG "$start"
  local start_value=$(get_value $mazename $start)

  set_value ${mazename}_DATA $start "@"

  for dir in N E S W
  do
      local adj=$(get_adj $start $dir)
      local adj_value=$(get_value $mazename $adj)

      if is_reachable $mazename $adj
      then
          if [ -z "$adj_value" ] || [ $adj_value -gt $((start_value+1)) ]
          then
              set_value ${mazename}      $adj $((start_value+1))
              set_value ${mazename}_FROM $adj $start
              search_maze $mazename $adj
          fi
      fi
  done

  set_value ${mazename}_DATA $start "_"
}

function backtrace () {
  local mazename="$1"
  local goal="$2"

  local from=$(get_from $mazename $goal)
  while [ -n "$from" -a "$(get_value ${mazename}_DATA "$from")" != S ]
    do
      set_value ${mazename}_DATA "$from" '%'
      from=$(get_from $mazename $from)
  done
}

trap 'echo; dump_data HOGE; exit' INT

function main () {
  load_data_file HOGE "$1"

  set_value HOGE $HOGE_start 0
  search_maze HOGE $HOGE_start

  set_value HOGE_DATA $HOGE_start 'S'
  set_value HOGE_DATA $HOGE_goal  'G'
  backtrace HOGE $HOGE_goal
  dump_data HOGE
}

if [ -n "$1" ]; then
    main "$1"
else
    main data.txt
fi
