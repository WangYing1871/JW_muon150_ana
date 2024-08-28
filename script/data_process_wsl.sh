#!/bin/bash
current_path=$(pwd)
echo "The current path is: $current_path"

# Split current path with word "source"
first_part=${current_path%%source*}
SW_PATH="${first_part}/c_pp"
helpFunction()
{
    echo ""
    # set color to green
    echo -e '\e[1;32m '
    echo "Usage: $0 "
    echo -e "\t-d First data name [待处理的第一个文件名]"
    echo -e "\t-l Loop number (default 1) [处理的文件个数]"
    echo -e "\t-L Link used in hex value 1 stands for used and 0 for not used\r\n\t\t(default 0xF) \r\n\t [使用的link，16进制，1表示使用，0表示不使用]"
    echo -e "\t-n Calcutate noise [计算噪声]"
    echo -e "\t-p Stop at detector position decoding [探测器位置解码完成后结束]"
    echo -e "\t-T Write txt file while run fec_data_dec for MATLAB plot \r\n\t [运行fec_data_dec时写入txt文件，用于MATLAB画图]"
    echo -e "\t-N Noise file path [噪声文件路径]"
    echo -e "\t-D Skip USB data seperate [跳过USB数据解包]"
    echo -e "\t-F Skip FEC data dec [跳过FEC数据解码]"
    echo -e "\t-P Skip position decoding [跳过位置解码]"
    echo -e "\t-w Data SCA start index [数据SCA起始位置]"
    echo -e "\t-W Data SCA end index [数据SCA结束位置]"
    echo -e "\t-C Decoding position must be continue [解码位置必须连续]"
    echo -e "\t-M Enable HV monitor [启用高压监控]"
    echo -e "\t-V Skip confirm [跳过确认]"
    echo -e "\t-S Set the cpp file path in case when calling from cmd.exe \r\n\t [设置cpp文件路径，用于cmd.exe调用]"
    echo -e "\t-h Help [帮助]"
    echo -e '\t-I initial [初始化]'
    echo -e '\e[1;m '
    exit 1 # Exit script after printing help
}
CALC_NOISE=0
LOOP_NUM=1
LINK_NUM=0xF
STOP_AT_DET_DEC=0
WR_TXT=""
SKIP_USB_DATA_SEPERATE=1
SKIP_FEC_DATA_DEC=1
SKIP_POSI_DEC=1
SKIP_HIT_REC=1
SKIP_TRACKER_SEEK=1
DATA_SCA_START_IDX=200
DATA_SCA_END_IDX=300
HV_MONITOR=0
IS_VERBOSE=0
IS_DECODE_CONTINUE=""
dname=""
while getopts "A:d:l:L:ntN:DFPHTw:W:CMVS:" opt
do
    case "$opt" in
        A ) dname="$OPTARG" ;;
        d ) MUSTC_DATA="$OPTARG" ;;
        l ) LOOP_NUM="$OPTARG" ;;
        L ) LINK_NUM="$OPTARG" ;;
        n ) CALC_NOISE=1 ;;
        t ) WR_TXT="-T" ;;
        N ) NOISE_FILE_PATH="$OPTARG" ;;
        D ) SKIP_USB_DATA_SEPERATE=0 ;;
        F ) SKIP_FEC_DATA_DEC=0 ;;
        P ) SKIP_POSI_DEC=0 ;;
        H ) SKIP_HIT_REC=0 ;;
        T ) SKIP_TRACKER_SEEK=0 ;;
        w ) DATA_SCA_START_IDX="$OPTARG" ;;
        W ) DATA_SCA_END_IDX="$OPTARG" ;;
        C ) IS_DECODE_CONTINUE="-C" ;;
        M ) HV_MONITOR=1 ;;
        V ) IS_VERBOSE=1 ;;
        S ) SW_PATH="$OPTARG" ;;
        I ) IS_INIT=1 ;;
        ? ) helpFunction ;; # Print helpFunction in case parameter is non-existent
    esac
done
# Print helpFunction in case parameters are empty
if [ -z "$MUSTC_DATA" ]
then
    echo "Data path is empty";
    helpFunction
    exit 1
fi
if [ -z "$LOOP_NUM" ]
then
    echo "Loop number is empty";
    helpFunction
    exit 1
fi

if ! [[ "$LOOP_NUM" =~ ^-?[0-9]+$ ]]
then
    echo "Loop number must be an integer"
    helpFunction
    exit 1
fi

if echo "$LINK_NUM" | grep -q "0x"; then
    LINK_NUM="$LINK_NUM"
else
    LINK_NUM="0x$LINK_NUM"
fi


# Begin script in case all parameters are correct
echo "First data path is $MUSTC_DATA"
echo "Loop number is $LOOP_NUM"
if [ "$CALC_NOISE" = 1 ]
then
    echo "Calcaulate noise";
fi

if ! [ -a $MUSTC_DATA ]
then
    echo "File not found";
fi

LINK_CNT=0
for i in $(seq 16)
do
    a=$(($(($((LINK_NUM))>>$((i-1))))&0x1))
    if [ $a -eq 1 ]
    then
        LINK_CNT=$(($LINK_CNT+1))
    fi
done
echo "Link number is $LINK_CNT"

DATA_PREFIX=${MUSTC_DATA%-*}
DATA_SUFFIX=${MUSTC_DATA#*-}
START_LOOP_NUM=${DATA_SUFFIX%.dat}
TOTAL_LOOP_NUM=$(($START_LOOP_NUM+$LOOP_NUM))
echo "++++++"
echo "=== Info ==="
echo "START at loop $START_LOOP_NUM"
echo "Total loop number $TOTAL_LOOP_NUM"
echo "Data prefix is $DATA_PREFIX"
echo "Data suffix is $DATA_SUFFIX"
echo "Data start index is $DATA_SCA_START_IDX"
echo "Data end index is $DATA_SCA_END_IDX"
echo "Set cpp file path to $SW_PATH"
#if [ "$VERBOSE" = 1 ]
#then
#    echo "Verbose mode is on";
#    # Wait the user to press y or enter for continue and n for exit
#    read -p "Continue? [y/n]" -n 1 -r
#    echo    # (optional) move to a new line
#    if [[ ! $REPLY =~ ^[Yy]$ ]]
#    then
#        echo "Exit"
#        exit 1
#    fi
#    # read -p "Press enter to continue"
#fi

echo $START_LOOP_NUM
echo $TOTAL_LOOP_NUM
declare -i start_loop_number=$START_LOOP_NUM
declare -i end_loop_number=$TOTAL_LOOP_NUM

if [ "$NOISE_FILE_PATH" = "" ]; then
  echo "No NOISE_FILE_PATH Geti, exit"
  exit
fi

echo "=+++++++++++>noise++++++++++++++>"
USB_DATA_PROCESS="$SW_PATH/usb_data_separate/build/usb_data_separate"
$USB_DATA_PROCESS -f $NOISE_FILE_PATH/noise-0.dat -r 1>/dev/null
UNPACK="$SW_PATH/adas_data_unpack/build/adas_data_unpack"
NEW_FILE_PATH="${MUSTC_DATA%/*}/dec_out"
for j in $(seq 16)
do
    a=$(($(($((LINK_NUM))>>$((j-1))))&0x1))
    if [ $a -eq 0 ]
    then
        continue
    fi
    $UNPACK -i 6 -L 15 -b 1 -f "$NEW_FILE_PATH/noise-0_mt-$((${j}-1)).dat" 1>/dev/null
done
echo "=+++++++++++<noise++++++++++++++<"

echo "=+++++++++++>signal++++++++++++++>"
if [ "$SKIP_USB_DATA_SEPERATE" = 1 ]
then
    echo "Skip USB data seperate";
else
    echo "USB data seperate";
    USB_DATA_PROCESS="$SW_PATH/usb_data_separate/build/usb_data_separate"
    for ((i=$START_LOOP_NUM; i<$TOTAL_LOOP_NUM; i=i+1))
    do
      $USB_DATA_PROCESS -f "$DATA_PREFIX-${i}.dat" -r 1>/dev/null
    done
    echo -e '\e[1;42m USB data seperate done \e[1;m'
fi
NEW_FILE_PATH="${MUSTC_DATA%/*}/dec_out"
FILE_NAME="${MUSTC_DATA##*/}"
FILE_NAME1=${FILE_NAME%-*}
if [ "$SKIP_FEC_DATA_DEC" = 1 ]
then
    echo "Skip FEC data dec";
else
    UNPACK="$SW_PATH/adas_data_unpack/build/adas_data_unpack"
    for ((i=$START_LOOP_NUM; i<$TOTAL_LOOP_NUM; i=i+1))
    do
        for j in $(seq 16)
        do
            a=$(($(($((LINK_NUM))>>$((j-1))))&0x1))
            if [ $a -eq 0 ]
            then
                continue
            fi
            $UNPACK -i 6 -L 15 -b 0 -f "$NEW_FILE_PATH/$FILE_NAME1-${i}_mt-$((${j}-1)).dat" 1>/dev/null
        done
    done
    echo -e '\e[1;42m unpack data done\e[1;m'
fi

NEW_FILE_PATH="${MUSTC_DATA%/*}/dec_out/unpack_result"
FILE_NAME="${MUSTC_DATA##*/}"
FILE_NAME1=${FILE_NAME%-*}
if [ "$SKIP_FEC_DATA_DEC" = 1 ]
then
    echo "Skip FEC data dec";
else
  DECODE="$SW_PATH"/adas_data_decode/build/adas_data_dec
  for ((i=$START_LOOP_NUM; i<$TOTAL_LOOP_NUM; i=i+1)); do
    for j in $(seq 16); do
      a=$(($(($((LINK_NUM))>>$((j-1))))&0x1))
      if [ $a -eq 0 ]; then
        continue;
      fi
      $DECODE -b noise-0_mt-$((${j}-1))_base.txt -n noise-0_mt-$((${j}-1))_rms.txt -i 6 -e "$SW_PATH"/adas_data_decode/config/muon15.csv -f "$NEW_FILE_PATH/$FILE_NAME1-${i}_mt-$((${j}-1)).root" 1>/dev/null
    done
  done
  echo -e '\e[1;42m decode data done\e[1;m'
fi


NEW_FILE_PATH="${MUSTC_DATA%/*}/dec_out/unpack_result/dec_result"
FILE_NAME="${MUSTC_DATA##*/}"
FILE_NAME1=${FILE_NAME%-*}
if [ "$SKIP_HIT_REC" = 1 ]; then
  echo "Skip hit position reconstruct";
else
  HIT_POSI="$SW_PATH"/hit_position_reconstrcut/build/hit_position_reconstrcut
  for ((i=$START_LOOP_NUM; i<$TOTAL_LOOP_NUM; i=i+1)); do
    for j in $(seq 16); do
      a=$(($(($((LINK_NUM))>>$((j-1))))&0x1))
      if [ $a -eq 0 ]; then
        continue;
      fi
      $HIT_POSI -p "$SW_PATH"/hit_position_reconstrcut/config/link_position_6layer_adas.csv -z "$SW_PATH"/hit_position_reconstrcut/config/z_posiiton_6laeyr_adas_jw.csv -L "$LINK_NUM" -l 6 -V -f "$NEW_FILE_PATH/$FILE_NAME1-${i}_dec_mt-$((${j}-1)).root" 1>/dev/null
    done
  done
  echo -e '\e[1;42m hit_position_reconstrcut done\e[1;m'
fi

NEW_FILE_PATH="${MUSTC_DATA%/*}/dec_out/unpack_result/dec_result"
if [ "$dname" = "" ]; then
  dname="$NEW_FILE_PATH"
fi
FILE_NAME="${MUSTC_DATA##*/}"
FILE_NAME1=${FILE_NAME%-*}
if [ "$SKIP_TRACKER_SEEK" = 1 ]; then
  echo "Skip track seek";
else
  TRACK_SEEK="$SW_PATH"/tracker_seek/build/tracker_seek
  for ((i=$START_LOOP_NUM; i<$TOTAL_LOOP_NUM; i=i+1)); do
    $TRACK_SEEK -A ${dname} -f "$NEW_FILE_PATH/${FILE_NAME1}-${i}_dec_mt_hit_data_link"$LINK_NUM".root" -L 5 -D 6 -l $LINK_NUM -r "$SW_PATH"/tracker_seek/config/offset_initial.txt -j -T 1>/dev/null
  done
  echo -e '\e[1;42m tracker_seek done\e[1;m'
fi
echo "=+++++++++++<signal++++++++++++++<"
