//--------------------------------Stamp-------------------------------
//^-^ Author: Zhi Heng            Email: wy187110@mail.ustc.edu.cn     
//^-^ Time: 2024-05-10 17:18:36   Posi: Hefei
//^-^ File: temp.cpp
//--------------------------------------------------------------------
#define info_out(X) std::cout<<"==> "<<__LINE__<<" "<<#X<<" |"<<(X)<<"|\n"
#include <algorithm>

#include <iostream>
#include <cstdlib>
#include <cmath>
#include <string>
#include <vector>
#include <list>
#include <fstream>
#include <bitset>
#include <map>
#include <unordered_map>
#include <thread>
#include <chrono>
#include <array>
#include <sstream>
#include <set>

#include <filesystem>
#include <chrono>
#include <concepts>

#include <boost/timer/timer.hpp>

#include "TFile.h"
#include "TTree.h"
#include "TH1F.h" 
#include "TH1I.h"
#include "THttpServer.h"
#include "TRandom3.h"
#include "TSystem.h"
#include "TClonesArray.h"

#include <boost/type_index.hpp>
#define type_name(x) std::cout<<boost::typeindex::type_id<decltype(x)>().pretty_name()<<" sizeof:"<<sizeof(decltype(x))<<std::endl;
#include "util.hpp"
#include "config.h"
#include "clusters.hpp"
#include "data_strcut_cint.h"
#include "unpack.hpp"

struct event_head{ typedef uint32_t unit_type; unit_type data[2]; };
struct event_tail{ typedef uint32_t unit_type; unit_type data[2]; };
struct event_body{
  constexpr static size_t data_count = 1020;
  typedef uint32_t unit_type;
  unit_type* adcs;
  explicit event_body(){ adcs = new unit_type[data_count]; }
  ~event_body() noexcept {delete[] adcs;} };



struct data_head : public data_region<uint32_t,event_head,size_t,FILE*>{
  typedef data_region base_t;

  typedef typename event_head::unit_type unit_type;

  data_head() = default;
  data_head(FILE* fp){ fill(fp); } 

  uint32_t m_read = 0;


  virtual size_t fill(FILE* fptr) override{
    m_read = fread(this->data,sizeof(unit_type),2,fptr);
    this->data[0] &= 0xFFFFFF00;
    //for(auto&& x : data) util::flip<__CHAR_BIT__>(&x);
    //m_package_ids.emplace_back(pack_id());
    return m_read; }
  virtual bool valid() const override{
    //info_out(std::bitset<32>(data[0]).to_string()); info_out(m_read);
    return this->data[0]==0xFFAB5300 && m_read==2;
  }
  virtual uint16_t size() const override{
    return 2*sizeof(unit_type);
  }
  virtual bool crc32() const override{
    //TODO CRC32 !!!
    return true;
  }


public:
   uint8_t fee_id() const {return uint8_t(data[1] & 0x000000FF);}
   uint16_t pack_id() const {return uint16_t((data[1]&0xFFFF0000)>>16);}

public:
  static std::vector<uint16_t> m_package_ids;

};
//---------------------------------------------------------------------
struct body_sub_head{ typedef uint32_t unit_type; uint32_t data[3]; };
struct body_sub_body{ typedef uint32_t unit_type; std::vector<unit_type> adcs; };
struct body_sub_tail{
  typedef uint32_t unit_type; constexpr static unit_type s_tag = 0x40386666; unit_type data; };

struct data_sub_head : public data_region<uint32_t,body_sub_head,void,uint32_t*>{
  typedef data_region base_t;
  typedef typename body_sub_head::unit_type unit_type;

  uint16_t m_read;
  constexpr static uint32_t s_distance = 3;
public:
  data_sub_head() = default;
  virtual ~data_sub_head() noexcept = default;

public:
  data_sub_head(uint32_t* fp) {fill(fp);}

  virtual void fill(uint32_t* fp) override{
    std::copy(fp,fp+4,std::begin(data));
  }
  virtual bool valid() const override{
    uint16_t const* addr = reinterpret_cast<uint16_t const*>(data);
    return addr[1]==0xAC0F && (addr[0]&0b1100000000000000)==0x4000
      && pack_size0()==(pack_size1()-2)*4;
  }
  virtual uint16_t size() const override{
    //TODO
    return m_read;
  }
  virtual bool crc32() const override{
    //TODO CRC32 !!!
    return true;
  }

  inline uint16_t pack_size0() const{ return (reinterpret_cast<uint16_t const*>(data)[0])&0x3FFF; }
  inline uint32_t pack_size1() const{ return data[2];}
  inline uint32_t trigger_id() const{ return data[1];}
  inline uint8_t basic_id() const {return (uint16_t)(data[3]&0x0000FFFF)>>8;}
  //inline uint8_t basic_th_num() const {return {(uint16_t)(data[3]&0x0000FFFF)&0x00FF};}
};

struct data_sub_body: public data_region<uint32_t,body_sub_body,void,uint32_t*>{
  typedef data_region base_t;
  typedef typename body_sub_body::unit_type unit_type;

  uint32_t m_read;
  uint32_t m_size;
public:
  data_sub_body() = default;
  virtual ~data_sub_body() noexcept = default;

public:
  data_sub_body(uint32_t* fp, uint32_t size): m_size(size) {fill(fp);}

  virtual void fill(uint32_t* fp) override{
    adcs.resize(m_size);
    std::copy(fp,fp+m_size,adcs.data());
  }
  virtual bool valid() const override{
    return true;
  }
  virtual uint16_t size() const override{
    return m_size;
  }
  virtual bool crc32() const override{
    //TODO CRC32 !!!
    return true;
  }

  



  
};

struct data_sub_tail : public data_region<uint32_t,body_sub_tail,void,uint32_t*>{
  typedef data_region base_t;
  typedef typename body_sub_tail::unit_type unit_type;

  uint32_t m_read;
public:
  data_sub_tail() = default;
  virtual ~data_sub_tail() noexcept = default;

public:
  data_sub_tail(uint32_t * fp) {fill(fp);}
  virtual void fill(uint32_t* fp) override{
    data = fp[0];
  }
  virtual bool valid() const override{
    return data==0x40386666;
  }
  virtual uint16_t size() const override{
    return m_read;
  }
  virtual bool crc32() const override{
    //TODO CRC32 !!!
    return true;
  }
};

template <class _tp, class _up , class _fp>
struct unpacker_impl_body : unpacker_base{

  uint32_t* m_range[2];
  uint32_t m_get=0;
  struct package_t{ _tp* m_head; _up* m_body; _fp* m_tail;
    package_t(_tp* a, _up* b, _fp* c):m_head(a),m_body(b),m_tail(c) {} 
    void clear(){
      if (m_head) delete m_head;
      if (m_body) delete m_body;
      if (m_tail) delete m_tail; } };
  std::vector<package_t> m_datas;

  void free(){ for (auto&& x : m_datas) x->clear(); }

  bool parse() override{
    if (std::distance(m_range[0],m_range[1])<=0) return false;
    for (auto iter = m_range[0]; std::distance(iter,m_range[1])>=0;){
      auto* sub_head = new _tp(iter);
      if (sub_head->valid()){
        iter += data_sub_head::s_distance;
        auto* sub_body = new _up(iter,sub_head->pack_size1()-4);
        iter += (sub_head->pack_size1()-4);
        auto* sub_tail = new _fp(iter);
        if ( sub_tail->valid()){
          m_datas.emplace_back(package_t{sub_head,sub_body,sub_tail});


        }else{ delete sub_tail; delete sub_head; iter++; }
      }else{
        delete sub_head; iter++;
      }
    }
    return (m_datas.size()>0);

  }
};




 
//---------------------------------------------------------------------


struct data_body : public data_region<uint32_t,event_body,size_t,FILE*>{
  typedef data_region base_t;
  typedef typename event_body::unit_type unit_type;
  using event_body::data_count;

  uint32_t m_read;

public:
  data_body() = default;
  virtual ~data_body() noexcept = default;

  data_body(FILE* fp) {fill(fp);}

  virtual size_t fill(FILE* fp) override{
    return m_read = fread(adcs,sizeof(unit_type),data_count,fp);
  }
  virtual bool valid() const override{
    return m_read==data_count;
  }
  virtual uint16_t size() const override{
    return 1020;
  }
  virtual bool crc32() const override{
    //TODO CRC32 !!!
    return true;
  }

};


struct data_tail : public data_region<uint32_t,event_tail,size_t,FILE*>{
  typedef data_region base_t;
  typedef typename event_tail::unit_type unit_type;

  uint32_t m_read;

public:
  data_tail() = default;
  virtual ~data_tail() noexcept = default;

  data_tail(FILE* fp){ fill(fp); }

  virtual size_t fill(FILE* fp) override{
    return m_read = fread(this->data,sizeof(unit_type),2,fp); }

  virtual bool valid() const override{
    return m_read==2 && data[0]==0xFFCC0000;
  }

  virtual uint16_t size() const override{
    return m_read;
  }

  virtual bool crc32() const override{
    std::cout<<"TODO!!!"<<std::endl;
    return true;
  }

};


template <class _tp, class _up , class _fp>
struct unpacker_impl : unpacker_base{

  std::string m_fname;

  struct package_t{
    _tp* m_head;
    _up* m_body;
    _fp* m_tail;

    package_t(_tp* a, _up* b, _fp* c):m_head(a),m_body(b),m_tail(c) {}
  };

  std::vector<package_t> m_datas;


  void setfile(std::string const& v) {m_fname = v;}
  std::map<int,FILE*> m_boards;
  std::unordered_map<long,char> m_invalid_data;

  //std::map<int,std::vector<std::array<uint32_t,1020>>> m_demo;

  bool parse() override{
    auto* fp = fopen(m_fname.c_str(),"r");
    if (!fp)
      throw std::invalid_argument("...");
    fseek(fp,0,SEEK_END); fseek(fp,0,SEEK_SET);

    while(!feof(fp)){
      long back = ftell(fp);
      _tp* head = new _tp(fp);
      if (head->valid()){
        
        back = ftell(fp);
        _up* body = new _up(fp);
        if (body->valid()){
          back = ftell(fp);
          _fp* tail = new _fp(fp);
          if (tail->valid()){
            //util::file_append<uint32_t>(m_boards,head->fee_id(),body->adcs,1020);
            m_datas.emplace_back(package_t{head,body,tail});
          }else{
            delete tail; fseek(fp,back,SEEK_SET);
            unsigned char __attribute__((unused)) data; fread(&data,sizeof(unsigned char),1,fp);
          }
        }else{
          delete body; fseek(fp,back,SEEK_SET);
          unsigned char __attribute__((unused)) data; fread(&data,sizeof(unsigned char),1,fp);
        }
      }else{
        delete head; fseek(fp,back,SEEK_SET);
        unsigned char __attribute__((unused)) data; fread(&data,sizeof(unsigned char),1,fp);
      }

    }

    fclose(fp);
    for (auto&& [_,x] : m_boards) fclose(x);
    return (m_datas.size()>0);
  }


  
  
};


int main(int argc, char* argv[]){
//  unpacker_impl<data_head,data_body,data_tail> opt_aa;
//  std::string datname = argc>=2 ? std::string{argv[1]}  : "run-40.dat";
//  opt_aa.setfile(datname);
//  {
//    boost::timer::auto_cpu_timer progress;
//    opt_aa.parse();
//  }
//  info_out(opt_aa.m_datas.size());
//
//  std::vector<uint16_t> pack_ids;
//  std::vector<uint8_t> fee_ids;
//  for(auto&& x : opt_aa.m_datas){
//    pack_ids.emplace_back(x.m_head->pack_id());
//    fee_ids.emplace_back(x.m_head->fee_id());
//  }
//  
//  typedef unpacker_impl_body<data_sub_head,data_sub_body,data_sub_tail> body_parse_t;
//  std::map<int,std::map<uint8_t,std::pair<uint32_t,uint64_t>>> maps;
//  std::vector<uint32_t> data_fec0;
//  for (auto&& x : opt_aa.m_datas){
//    if (x.m_head->fee_id()==std::stoi(argv[2])){
//      std::copy(x.m_body->adcs,x.m_body->adcs+event_body::data_count
//          ,std::back_inserter(data_fec0));
//    }
//
//  }
//
//  body_parse_t body_parse;
//  body_parse.m_range[0] = data_fec0.data();
//  body_parse.m_range[1] = data_fec0.data()+data_fec0.size();
//  {
//    boost::timer::auto_cpu_timer progress;
//  body_parse.parse();
//  }
//  //info_out(body_parse.m_datas.size());
//
//
//
//
//  //to_root
//  auto* rf_out = new TFile("step0.root","recreate");
//  auto* tree = new TTree("data","data");
//  struct channel_data{
//    size_t m_hits;
//    uint32_t m_trigger_id;
//    std::vector<uint16_t> m_global_idxs;
//    std::vector<uint32_t> m_amps;
//
//    void clear(){
//      m_hits = 0; m_trigger_id = 0;
//      m_global_idxs.clear();
//      m_amps.clear(); }
//  };
//  channel_data entry_data;
//  tree->Branch("hits",std::addressof(entry_data.m_hits));
//  tree->Branch("trigger_id",std::addressof(entry_data.m_trigger_id));
//  tree->Branch("global_id",std::addressof(entry_data.m_global_idxs));
//  tree->Branch("amp",std::addressof(entry_data.m_amps));
//
//  config_noise baseline;
//  baseline.m_means.emplace(0,"noise/noise-0_mt-0_base.txt");
//  baseline.m_means.emplace(1,"noise/noise-0_mt-1_base.txt");
//  baseline.m_means.emplace(2,"noise/noise-0_mt-2_base.txt");
//  baseline.m_means.emplace(3,"noise/noise-0_mt-3_base.txt");
//  baseline.m_means.emplace(4,"noise/noise-0_mt-4_base.txt");
//  baseline.m_means.emplace(5,"noise/noise-0_mt-5_base.txt");
//  baseline.m_rms.emplace(0,"noise/noise-0_mt-0_rms.txt");
//  baseline.m_rms.emplace(1,"noise/noise-0_mt-1_rms.txt");
//  baseline.m_rms.emplace(2,"noise/noise-0_mt-2_rms.txt");
//  baseline.m_rms.emplace(3,"noise/noise-0_mt-3_rms.txt");
//  baseline.m_rms.emplace(4,"noise/noise-0_mt-4_rms.txt");
//  baseline.m_rms.emplace(5,"noise/noise-0_mt-5_rms.txt");
//  baseline.config();
//
//
//
//  std::index_sequence<4,8> bit_index00;
//
//  auto const& sub_body_to_entry = [&bit_index00](
//      body_parse_t::package_t const& ref,channel_data& data){
//    data.clear();
//    data.m_trigger_id= ref.m_head->trigger_id();
//    auto adcs = ref.m_body->adcs;
//    for (auto iter = std::begin(adcs); (size_t)std::distance(std::begin(adcs),iter)<adcs.size();){
//      if ((*iter&0xFFFF0000) == 0x66660000){
//        size_t chip_idx = (*iter&0x0000FF00)>>8;
//        size_t chip_adc_size = *iter&0x000000FF;
//        for (auto iter_sub = iter+2; iter_sub != iter+chip_adc_size+1; iter_sub++){
//          uint8_t channel_idx = (*iter_sub&0xFF000000)>>24;
//          if (channel_idx<=128){
//            data.m_hits++;
//            uint32_t amp = *iter_sub&0x00FFFFFF;
//            data.m_amps.emplace_back(amp);
//            uint16_t glb_id{};
//            util::encode(glb_id,bit_index00,chip_idx,channel_idx);
//            data.m_global_idxs.emplace_back(glb_id);
//          }
//        }
//        iter += chip_adc_size+1;
//        continue;
//      }else iter++;
//    }
//
//
//
//    return 0;
//  };
//
//
//  
//  for (/*size_t index=0; */auto&& x : body_parse.m_datas){
//    sub_body_to_entry(x,entry_data);
//    tree->Fill();
//    //if (index++==0) break;
//
//  }
//  tree->Write();
//  rf_out->Write(); rf_out->Close();
//
//
//
//
//  struct clusters_data{
//    size_t m_hits;
//    size_t m_trigger_id;
//    std::vector<uint16_t>* m_det_strips_id;
//    std::vector<uint16_t>* m_ele_channels_id;
//
//    clusters_data(){
//      m_det_strips_id = new std::vector<uint16_t>;
//      m_ele_channels_id = new std::vector<uint16_t>; }
//
//    ~clusters_data(){
//      if (m_det_strips_id) delete m_det_strips_id;
//      if (m_ele_channels_id) delete m_ele_channels_id; }
//
//    void clear(){
//      if (m_det_strips_id) m_det_strips_id->clear();
//      if (m_ele_channels_id) m_ele_channels_id->clear();
//    }
//  };
//  {
//
//    channel_data entry_step0;
//
//
//
//
//    auto* file_unpack = new TFile("step0.root");
//    auto* tree_unpack = static_cast<TTree*>(file_unpack->Get("data"));
//    tree_unpack->SetBranchAddress("hits",std::addressof(entry_step0.m_hits));
//    tree_unpack->SetBranchAddress("trigger_id",std::addressof(entry_step0.m_trigger_id));
//    auto* global_id_vec_ptr = std::addressof(entry_step0.m_global_idxs);
//    tree_unpack->SetBranchAddress("global_id",std::addressof(global_id_vec_ptr));
//    auto* amp_vec_ptr = std::addressof(entry_step0.m_amps);
//    tree_unpack->SetBranchAddress("amp",std::addressof(amp_vec_ptr));
//
//    auto* file_step1 = new TFile("step1.root","recreate");
//    auto* tree_step1 = new TTree("data","data");
//    clusters_data entry_step1;
//    tree_step1->Branch("hits",std::addressof(entry_step1.m_hits));
//    tree_step1->Branch("trigger_id",std::addressof(entry_step1.m_trigger_id));
//    tree_step1->Branch("det_strip",std::addressof(entry_step1.m_det_strips_id));
//
//    auto* cla_clusters_x = new TClonesArray("cluster");
//    auto* cla_clusters_y = new TClonesArray("cluster");
//    tree_step1->Branch("clusters_x",std::addressof(cla_clusters_x),256000,0);
//    tree_step1->Branch("clusters_y",std::addressof(cla_clusters_y),256000,0);
//    cla_clusters_x->BypassStreamer();
//    cla_clusters_y->BypassStreamer();
//
//    config_e2d_table config_e2d;
//    config_e2d.fname("/home/wangying/work/mis/analyse_code/cpp/adas_data_decode/out/muon15.csv");
//    config_e2d.config();
//
//
//    typedef clusters<int> clusters_t;
//
//    clusters_t::s_set_tolerate(1);
//    constexpr static size_t const s_tolerate_hit_max = 40;
//
//
//    {
//      boost::timer::auto_cpu_timer progress;
//    for (decltype(tree_unpack->GetEntries()) i=0, e=tree_unpack->GetEntries()
//        ; i<e; ++i){
//      tree->GetEntry(i);
//
//      int detector_x[384]; std::memset((void*)detector_x,0,384*sizeof(int));
//      int detector_y[384]; std::memset((void*)detector_y,0,384*sizeof(int));
//
//      size_t hit_x=0, hit_y=0;
//      for(size_t index=0; auto&& x : entry_step0.m_global_idxs){
//        auto ids = util::decode(x,bit_index00);
//        if (ids[1]<3){
//          hit_x++;
//          detector_x[config_e2d.m_table.at(ids[1]*128+ids[0]-1)] = 
//            entry_step0.m_amps[index]-std::round(
//                baseline.get_mean(std::stoi(argv[2]),ids[1],ids[0]-1));
//        }else if(ids[1]>=3 && ids[1]<6){
//          hit_y++;
//          detector_y[config_e2d.m_table.at(ids[1]%3*128+ids[0]-1)] =
//            entry_step0.m_amps[index]-std::round(
//                baseline.get_mean(std::stoi(argv[2]),ids[1],ids[0]-1));
//        }
//        index++;
//      }
//      if (hit_x==0 || hit_y==0) continue;
//
//      size_t hits_all_x = 0;
//      size_t hits_all_y = 0;
//      size_t clusters_all_x = 0;
//      size_t clusters_all_y = 0;
//      std::vector<size_t> holes;
//
//      clusters_t clusters_x;
//      /*clusters_x.is_distinguish_sample(true);*/
//      entry_step1.clear();
//      if (hit_x>0 && hit_x<=s_tolerate_hit_max){
//        clusters_x.calc(std::begin(detector_x),std::end(detector_x));
//        clusters_x.hadd();
//        for (auto&& x : clusters_x.m_data){
//          size_t hole=0, strip=0;
//          for(auto&& y : x.m_data) y==0 ? hole++ : strip++;
//          x.m_hole_no = hole; x.m_hits_no = strip;
//          if ((strip-hole)>1){ 
//            hits_all_x+=strip; clusters_all_x++; holes.emplace_back(hole);
//            for (size_t index=0; auto&& y : x.m_data) 
//              if (y>0) entry_step1.m_det_strips_id->emplace_back(x.m_range.first+index++);
//          }
//        }
//      }
//
//      clusters_t clusters_y;
//      /*clusters_y.is_distinguish_sample(true);*/
//      if (hit_y>0 && hit_y<=s_tolerate_hit_max){
//        clusters_y.calc(std::begin(detector_y),std::end(detector_y));
//        clusters_y.hadd();
//        for (auto&& x : clusters_y.m_data){
//          size_t hole=0, strip=0;
//          for(auto&& y : x.m_data) y==0 ? hole++ : strip++;
//          x.m_hole_no = hole; x.m_hits_no = strip;
//          if ((strip-hole)>1){ 
//            hits_all_y+=strip; clusters_all_y++; holes.emplace_back(hole);
//            for (size_t index=0; auto&& y : x.m_data) 
//              if (y>0) entry_step1.m_det_strips_id->emplace_back(x.m_range.first+index++);
//          }
//        }
//        
//      }
//      if (
//          (hits_all_x+hits_all_y)>0
//          && (clusters_all_x+clusters_all_y)>0
//          ){
//        entry_step1.m_hits = hits_all_x+hits_all_y;
//        entry_step1.m_trigger_id = entry_step0.m_trigger_id;
//        cla_clusters_x->Clear();
//        cla_clusters_y->Clear();
//        //info_out(tree_step1->GetEntries());
//        for (std::size_t index=0; auto&& x : clusters_x.m_data) 
//          if (x.m_hits_no-x.m_hole_no>1){
//            new (cla_clusters_x->operator[](index++)) cluster(x); }
//        for (std::size_t index=0; auto&& x : clusters_y.m_data) 
//          if (x.m_hits_no-x.m_hole_no>1){
//            new (cla_clusters_y->operator[](index++)) cluster(x); }
//        tree_step1->Fill();
//
//        //std::cout<<clusters_all_x<<" "<<clusters_all_y<<std::endl;
//        //std::cout<<std::endl;
//
//
//      }
//    }
//    }
//    file_unpack->Close();
//    file_step1->cd();
//    tree_step1->Write();
//    file_step1->Write(); file_step1->Close();
//
//
//  }

  {
    
    boost::timer::auto_cpu_timer progress;
    auto const& erase_min = [](std::multiset<size_t>& d){
      size_t min = *d.begin();
      while(*d.begin()==min) d.erase(d.begin()); };
    struct step1_entry{
      size_t trigger_id;
      TClonesArray* clusters_x;
      TClonesArray* clusters_y;
      
      step1_entry(){
        clusters_x = new TClonesArray("cluster");
        clusters_y = new TClonesArray("cluster");
      }
    };

    TFile* fout = new TFile("step2.root","recreate");
    auto cla_hit_positions = new TClonesArray("hit_position");
    auto* step2_tree = new TTree("data","data");
    step2_tree->Branch("hit_position",std::addressof(cla_hit_positions),256000,0);
    uint32_t trigger_id;
    step2_tree->Branch("trigger_id",std::addressof(trigger_id));
    cla_hit_positions->BypassStreamer();


    std::string dname = argv[1];
    std::unordered_map<std::string,int> in_files;
    in_files.emplace(dname+"/step1_0.root",0);
    in_files.emplace(dname+"/step1_1.root",1);
    in_files.emplace(dname+"/step1_2.root",2);
    in_files.emplace(dname+"/step1_3.root",3);
    in_files.emplace(dname+"/step1_4.root",4);
    in_files.emplace(dname+"/step1_5.root",5);
    std::unordered_map<TTree*,step1_entry> trees;
    std::unordered_map<TTree*,int> layers_id;
    for (auto&& [x,_] : in_files) {
      auto* t = (TTree*)(TFile::Open(x.c_str())->Get("data"));
      trees.emplace(t,step1_entry());
      layers_id.emplace(t,_);
    }

    for (auto&& [x,y] : trees){
      x->SetBranchAddress("trigger_id",std::addressof(y.trigger_id));
      x->SetBranchAddress("clusters_x",std::addressof(y.clusters_x));
      x->SetBranchAddress("clusters_y",std::addressof(y.clusters_y));
    }
    std::unordered_map<TTree*,long long> local_entry;
    std::unordered_map<TTree*,bool> is_update;
    for (auto&& [x,y] : trees) local_entry[x] = 0;
    for (auto&& [x,y] : trees) is_update[x] = false;
    for (auto&& [x,_] : trees) x->GetEntry(0);
    std::multiset<size_t> t_id;
    for(auto&& [_,y] : trees) t_id.insert(y.trigger_id);

    bool status = false;
    while(!t_id.empty() || status){
      for (auto&& [x,_] : trees){
        if(is_update[x]){
          long long eid = local_entry[x]+1;
          if (eid<x->GetEntries()){
            x->GetEntry(++local_entry[x]);
            t_id.insert(trees[x].trigger_id);
          }
        }
      }
      auto id = *t_id.begin();
      cla_hit_positions->Clear();
      for (size_t index=0; auto&& [x,y] : trees){
        if (y.trigger_id==id && local_entry[x] != x->GetEntries()-1){
          is_update[x] = true;
          new (cla_hit_positions->operator[](index++)) 
            hit_position(layers_id[x],y.clusters_x,y.clusters_y);
        }else{
          is_update[x] = false;
        }
      }
      trigger_id = id;
      step2_tree->Fill();

      for (auto&& [_,y] : is_update){
        if (!y){ status = false; break;}
        else status = true;
      }
      //std::cout<<" | ";
      //for (auto&& x : t_id) std::cout<<x<<" ";
      erase_min(t_id);
      //std::cout<<" | ";
      //for (auto&& x : t_id) std::cout<<x<<" ";
      //std::cout<<" | ";
      //for (auto&& x : trees) std::cout<<is_update[x.first]<<" ";
    }

    fout->cd();
    step2_tree->Write();
    fout->Write(); fout->Close();


  }

  {
  }


  /*
  {
  THttpServer* serv = new THttpServer("http:3000");
  TH1F *hpx = new TH1F("hpx","This is the px distribution",1000,-2,8);
  TRandom3 random; float px, py;
  serv->SetDefaultPage("custom.htm");
  gSystem->ProcessEvents();

  auto* fout = new TFile("temp.root","recreate"); fout->cd();
  for (auto&& [x,y] : maps){
    std::stringstream sstr; sstr<<"his_"<<x;
    TH1F* his = new TH1F(sstr.str().c_str(),sstr.str().c_str()
        ,132,0,131);
    for (auto&& [y0,y1] : y){
      if (y0<his->GetBinLowEdge(0) || y0>=his->GetBinLowEdge(his->GetNbinsX())
          || y1.first>20000) continue;
      his->SetBinContent(y0,(float)y1.second);
      random.Rannor(px,py);
      hpx->Fill(px);
      std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
    his->Write();



    //std::cout<<"FEE board: "<<(int)x<<std::endl;
    //for (auto&& [y0,y1] : y){
    //  std::cout<<"  chid: "<<(int)y0<<" i: "<<(double)y1<<"\n";
    //}
  }
  fout->Write(); fout->Close();
  }
  */

  //unpacker_impl_body<data_sub_head,data_sub_body,data_sub_tail> opt_ab;
  //opt_ab.m_range[0] = opt_aa.m_datas[0].m_body->adcs;
  //opt_ab.m_range[1] = opt_aa.m_datas[0].m_body->adcs+event_body::data_count;

  //opt_ab.parse();




//#ifdef demo_others
//  {
//    std::string fname = "run-40.dat";
//    std::ifstream fin(fname.c_str(),std::ifstream::binary);
//    fin.seekg(0,fin.end);
//    long fsize = fin.tellg();
//    fin.seekg(0,fin.beg);
//    char* in_memory = new char[fsize];/
//		fin.read((char*)in_memory,fsize);
//
//    //for (int i=0; i<fsize/4; ++i){
//    //  uint32_t* addr = reinterpret_cast<uint32_t*>(in_memory+i*4);
//    //  util::flip<8>(addr); }
//
//    long index = 0;
//    for (int i=0; i<10000; ++i){
//      //uint32_t* data = reinterpret_cast<uint32_t*>(in_memory);
//      //std::cout<<std::hex<<data[0]<<std::endl;
//      //in_memory += 4;
//      uint32_t hc = *reinterpret_cast<uint32_t*>(in_memory);
//      hc &= 0xFFFFFF00;
//      if (hc==0xFFAB5300){
//        in_memory += 4;
//        uint32_t field = *reinterpret_cast<uint32_t*>(in_memory);
//        //...
//        in_memory += 4;
//        //info_out(std::bitset<32>(field).to_string());
//        //std::cout<<pkg_id<<std::endl;
//        field = *reinterpret_cast<uint32_t*>(in_memory+4080);
//        info_out(std::bitset<32>(field).to_string());
//
//
//        
//      }else{
//        in_memory += 1;
//        continue;
//
//      }
//    }
//  }
//#endif
//  {
//    int array00[384]; std::memset((void*)array00,0,sizeof(int)*384);
//    clusters<int> clusters_a;
//
//    std::vector<size_t> index={252,253,254,255,256,257,258,259,260,261,262,263,264,265,266,267,268};
//    std::vector<int> amp = {-1351,-2265,-3286,-2985,-2361,0,1666,3628,3805,2609,902,0,-1351,-2265,-3286,-2985,-2361};
// 
//    for(std::size_t i=0; auto&&x : index) array00[x] = amp.at(i++);
//    clusters_a.calc(std::begin(array00),std::end(array00));
//    info_out(clusters_a.size());
//    clusters_a.s_set_tolerate(1);
//    clusters_a.is_distinguish_sample(true);
//    clusters_a.hadd();
//    info_out(clusters_a.size());
// }



  return 0;
}
///home/wangying/software/gcc-13.2.0-build/bin/g++ -std=c++23 -pthread -Wall -fPIC -O3 -m64 -rdynamic read.cxx -o read -DTEST_ROOT `root-config --libs --cflags` -lRHTTPSniff -lRHTTP
