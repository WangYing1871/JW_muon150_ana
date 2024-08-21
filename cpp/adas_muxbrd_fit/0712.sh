cd /home/wangying/Desktop/150_multiLayerTst/07051513/dec_out/unpack_result/dec_result
rm muon150_0712_link0x5F.root && hadd muon150_0712_link0x5F.root muon-*_dec_mt_hit_data_link0x5F.root
cd -
./adas_muxbrd_sync -f /home/wangying/Desktop/150_multiLayerTst/07051513/dec_out/unpack_result/dec_result/muon150_0712_link0x5F.root -R ./test/detector_hitrange.txt -l 6 -r 1.5 -a ./test/alignment0416v3.txt
