#ifndef data_strcut_cint_H
#define data_strcut_cint_H 1 

#include <array>
#include <vector>
#include "TObject.h"
#include "TClonesArray.h"

template <class _tp> struct cluster_;

struct cluster : public TObject{
  std::pair<int,int> range;
  std::vector<int> adc_amp;
  int holes=0;
  int strips=0;

public:
  cluster() = default;
  cluster(cluster_<int> const&);
  ~cluster() noexcept = default;
  cluster& operator=(cluster const&) = default;


  double cog() const;
  void correct(std::vector<cluster>&) const;

public:
  ClassDef(cluster,1)
};

struct hit_position : public TObject{
  int layer_id;
  std::vector<double> position_x;
  std::vector<double> position_y;

  hit_position(int);
  hit_position(int,TClonesArray* clusters_x,TClonesArray* clusters_y);
  ~hit_position();



public:
  ClassDef(hit_position,1)
};








#endif
