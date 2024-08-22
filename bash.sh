if [ $# -eq 0 ]; then
  echo "Usage: bash bash.sh <absolute-path/to/noise.dat> <absolute-path/to/muons.dat>; with absolute-path as same!"
  echo "Example: bash bash.sh /home/wangying/work/mis/muon150_bk/analyse_code/data/noise-0.dat /home/wangying/work/mis/muon150_bk/analyse_code/data/muon-\*.dat "
  exit -1
fi
cmake=$(which cmake)
root=$(which root)

if [ ! -d result ]; then
  mkdir result
fi

if ! command -v cmake &> /dev/null
then
  echo "cmake no exists, exit"
  exit -1
fi
if ! command -v root &> /dev/null
then
  echo "root no exists, exit"
  exit -1
fi
project_path=$(pwd);
echo "project_path: "$project_path
cd ${project_path}/cpp/usb_data_separate/
if [ ! -d build ]; then
  mkdir build && cd build && cmake ../
else
  cd build && make clean && cmake ../
fi
cd ${project_path}/cpp/adas_data_unpack/
if [ ! -d build ]; then
  mkdir build && cd build && cmake ../
else
  cd build && make clean && cmake ../
fi

echo "================baseline=======================>"
cd ${project_path}/cpp/usb_data_separate/build
./usb_data_separate -f ${1} -r
cd ${project_path}/cpp/adas_data_unpack/build
for i in $(seq 0 5); do
  file_name=$1
  file_path=$(echo "${file_name}" | sed -e 's/[^/]*$//p' -n)
  file_name_str_a=($(echo $file_name | sed 's+/+ +gp' -n))
  file_name_str=${file_name_str_a[$((${#file_name_str_a[@]}-1))]}
  tmp0=$(echo ${file_name_str} | sed 's+\(.*\)\.\(.*\)+\1+p' -n)
  tmp1=$(echo ${file_name_str} | sed 's+\(.*\)\.\(.*\)+\2+p' -n)
  tmp=${tmp0}"_mt-$i".${tmp1}
  ./adas_data_unpack -i 6 -L 15 -b 1 -f ${file_path}/dec_out/$tmp
done
echo "<===============baseline======================="

cd ${project_path}/cpp/adas_data_decode
if [ ! -d build ];then
  mkdir build && cd build && cmake ../
else
  cd build && make clean && cmake ../
fi
cd ${project_path}/cpp/hit_position_reconstrcut
if [ ! -d build ];then
  mkdir build && cd build && cmake ../
else
  cd build && make clean && cmake ../
fi
cd ${project_path}/cpp/adas_muxbrd_fit
if [ ! -d build ];then
  mkdir build && cd build && cmake ../
else
  cd build && make clean && cmake ../
fi

echo "===============signal=========================>"
a="$(ls $2)"
links_root_file=()
names=($(ls $2))
for i in $(seq 0 $((${#names[@]}-1))); do
  file_name=${names[i]}
  file_name_str_a=($(echo $file_name | sed 's+/+ +gp' -n))
  file_name_str=${file_name_str_a[$((${#file_name_str_a[@]}-1))]}
  echo $file_name_str
  file_path=$(echo "${file_name}" | sed -e 's/[^/]*$//p' -n)
  cd ${project_path}/cpp/usb_data_separate/build
  ./usb_data_separate -f ${file_name} -r

  cd ${project_path}/cpp/adas_data_unpack/build
  for j in $(seq 0 5); do
    tmp0=$(echo ${file_name_str} | sed 's+\(.*\)\.\(.*\)+\1+p' -n)
    tmp1=$(echo ${file_name_str} | sed 's+\(.*\)\.\(.*\)+\2+p' -n)
    tmp="${tmp0}"_mt-$j."${tmp1}"
    ./adas_data_unpack -i 6 -L 15 -b 0 -f ${file_path}/dec_out/${tmp}
  done

  cd ${project_path}/cpp/adas_data_decode/build
  for j in $(seq 0 5); do
    tmp0=$(echo ${file_name_str} | sed 's+\(.*\)\.\(.*\)+\1+p' -n)
    tmp="${tmp0}"_mt-$j."root"
    ./adas_data_dec -b noise-0_mt-0_base.txt -n noise-0_mt-0_rms.txt -i 6 -e ../config/muon15.csv -f ${file_path}/dec_out/unpack_result/${tmp}
  done

  cd ${project_path}/cpp/hit_position_reconstrcut/build
  tmp0=$(echo ${file_name_str} | sed 's+\(.*\)\.\(.*\)+\1+p' -n)
  tmp=${tmp0}_dec_mt-0.root
  ./hit_position_reconstrcut -p ../config/link_position_6layer_adas.csv -z ../config/z_posiiton_6laeyr_adas.csv -L 0x3F -l 6 -V -f ${file_path}/dec_out/unpack_result/dec_result/${tmp}

  tmp=${tmp0}_dec_mt_hit_data_link0x3F.root
  file_name_get=${file_path}/dec_out/unpack_result/dec_result/${tmp}
  links_root_file+=(${file_name_get})
done

link_name=""
for i in $(seq 0 $((${#links_root_file[@]}-1)))
do
  link_name="${link_name}"" ${links_root_file[$i]}"
done

echo ${link_name}
hadd -f ${project_path}/result/link0x3F.root ${link_name}

cd ${project_path}/cpp/adas_muxbrd_fit/build
./adas_muxbrd_sync -R ../config/detector_hitrange.txt -a ../config/alignment0416v3.txt -u -v 0 -f ${project_path}/result/link0x3F.root 
./adas_muxbrd_sync -R ../config/detector_hitrange.txt -a ../config/alignment0416v3.txt -u -v 1 -f ${project_path}/result/link0x3F.root 
./adas_muxbrd_sync -R ../config/detector_hitrange.txt -a ../config/alignment0416v3.txt -u -v 2 -f ${project_path}/result/link0x3F.root 
./adas_muxbrd_sync -R ../config/detector_hitrange.txt -a ../config/alignment0416v3.txt -u -v 3 -f ${project_path}/result/link0x3F.root 
./adas_muxbrd_sync -R ../config/detector_hitrange.txt -a ../config/alignment0416v3.txt -u -v 4 -f ${project_path}/result/link0x3F.root 
./adas_muxbrd_sync -R ../config/detector_hitrange.txt -a ../config/alignment0416v3.txt -u -v 5 -f ${project_path}/result/link0x3F.root 





cd ${project_path}




echo "<==============signal=========================="
