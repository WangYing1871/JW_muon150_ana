#ifndef clusters_HPP
#define clusters_HPP 1 
#include <algorithm>
#include <numeric>
#include <vector>
#include <iostream>
#include <string>
#include <limits>
#include <list>
#include "util.hpp"

template <class _tp>
struct cluster_{
  typedef cluster_ self_t;
  std::pair<size_t,size_t> m_range;
  std::vector<_tp> m_data;
  size_t m_hole_no = 0;
  size_t m_hits_no = 0;

  cluster_() = default;
  ~cluster_() noexcept = default;
  self_t& operator=(self_t const&) = default;

  _tp sum() const {return std::accumulate(std::begin(m_data),std::end(m_data),0);}

  template <class _iter_t>
  cluster_(_iter_t begin, _iter_t first, _iter_t last):
    m_range(std::distance(begin,first),std::distance(begin,last)){
    m_data.resize(size());
    std::copy(first,last,m_data.begin()); }

  std::ostream& display(std::ostream& os = std::cout){
    os<<"["<<m_range.first<<","<<m_range.second<<") "; 
    for (auto&& x : m_data) os<<x<<" ";
    return os<<"\n";
  }

  size_t size() const {return m_range.second-m_range.first;}
  int distance(self_t const& rhs) const {
    if (rhs.m_range.second<=m_range.first) return rhs.m_range.second-m_range.first;
    if (rhs.m_range.first>=m_range.second) return rhs.m_range.first-m_range.second;
    return (std::numeric_limits<int>::min)();
  }


  self_t operator+(self_t const& rhs){
    self_t rt;
    int dis = distance(rhs);
    if (dis==std::numeric_limits<int>::min()) return *this; //TODO
    if (dis>=0){
      rt.m_range.first = m_range.first;
      rt.m_range.second = rhs.m_range.second;
      rt.m_data.resize(size()+dis+rhs.size());
      std::copy(m_data.begin(),m_data.end(),rt.m_data.begin());
      std::copy(rhs.m_data.rbegin(),rhs.m_data.rend(),rt.m_data.rbegin());
    }else if (dis<0){
      //TODO
    }
    return rt;
  }

  void operator+=(self_t const& rhs){
    int dis = distance(rhs);
    if (dis==std::numeric_limits<int>::min()) return;
    if (dis>=0){
      m_data.resize(size()+dis+rhs.size());
      m_range.second = rhs.m_range.second;
      std::copy(rhs.m_data.rbegin(),rhs.m_data.rend(),m_data.rbegin());
    }else if(dis<0){
      //TODO
    }
  }

public:
  void update(){
    m_hole_no=0;
    m_hits_no=0;
    auto iend = std::end(m_data);
    for (auto ibegin = std::begin(m_data);ibegin != iend;){
      if(*ibegin>0) { m_hits_no++; ibegin++;}
      else if(*ibegin==0){
        if (auto iter = std::find_if(ibegin,iend,[](_tp v){return v>0;})
            ;iter != iend){
          if (ibegin!=std::begin(m_data)) m_hole_no++;
          ibegin = iter;
        }else break;
      }
    }
  }


};
template <class _tp>
struct clusters{
  clusters() = default;
  ~clusters() noexcept = default;
  //size_t m_t;
  typedef cluster_<_tp> cluster_t;
  std::list<cluster_t> m_data;
  bool _is_distinguish_sample=false;

  inline std::size_t size() const {return m_data.size();}
  void is_distinguish_sample(bool v) {_is_distinguish_sample = v;}


  template <class _iter_t>
  int calc(_iter_t begin, _iter_t end){
    m_data.clear();
    for (auto iter = begin; iter != end;){
      if (*iter!=0){
        if (iter==std::prev(end,1)){
          m_data.emplace_back(cluster_t(begin,iter,iter+1));
          iter++;
          continue;
        }
        for (auto iter_sub = iter+1; iter_sub !=end; ++iter_sub){
          if (*iter_sub==0){ 
            m_data.emplace_back(cluster_t(begin,iter,iter_sub)); iter = iter_sub; break;}
          if (std::distance(iter_sub,end)==1){
            m_data.emplace_back(cluster_t(begin,iter,iter_sub+1)); iter = end; break;
          }
        }
      }else iter++;
    }
    return 0;
  }

  void hadd(){
    if (m_data.size()<=1) return;
    for(auto iter = std::begin(m_data); iter != std::prev(std::end(m_data),1);){
      while((size_t)(iter->distance(*std::next(iter,1)))<=s_hole_tolerate){
        if (_is_distinguish_sample){
          if (iter==std::prev(std::end(m_data),1)) return;
          _tp sum_this = iter->sum();
          _tp sum_next = std::next(iter,1)->sum();
          if ((sum_this^sum_next)<0 && 
              util::is_peak(std::begin(iter->m_data),std::end(iter->m_data))){
            iter++;
            if (iter==std::prev(std::end(m_data),1)) return;
            continue;
          }
        }
        iter->operator+=(*std::next(iter,1));
        m_data.erase(std::next(iter,1));
        if (iter==std::prev(std::end(m_data),1)) return;
      }
      iter++;
    }
  }

private:
  static size_t s_hole_tolerate;


public:
  static void s_set_tolerate(size_t a) {s_hole_tolerate = a;}
  static size_t s_get_tolerate() {return s_hole_tolerate;}
};
template <class _tp>
size_t clusters<_tp>::s_hole_tolerate = 0;

#endif
