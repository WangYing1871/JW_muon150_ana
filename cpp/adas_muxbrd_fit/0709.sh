cd /home/wangying/Desktop/150_multiLayerTst/07091245/dec_out/unpack_result/dec_result
rm muon150_0709_link0x3F.root && hadd muon150_0709_link0x3F.root muon-*_dec_mt_hit_data_link0x3F.root
cd -
./adas_muxbrd_sync -f /home/wangying/Desktop/150_multiLayerTst/07091245/dec_out/unpack_result/dec_result/muon150_0709_link0x3F.root -R ./test/detector_hitrange.txt -l 6 -r 1.5 -a ./test/alignment0416v3.txt
