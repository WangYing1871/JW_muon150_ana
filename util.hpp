//--------------------------------Stamp-------------------------------
//^-^ Author: Zhi Heng            Email: 2835516101@qq.com     
//^-^ Time: 2024-08-12 21:08:29   Posi: Hefei
//^-^ File: util.hpp
//--------------------------------------------------------------------
#ifndef util_HPP
#define util_HPP 1 
#include <concepts>
#include <cmath>
#include <vector>
#include <algorithm>
#include <numeric>
#include <map>
#include <cstdint>
#include <cstdio>
#include <string>
namespace util{
void trim_space(std::string&);


template <class _tp> struct integral_delay{ typedef void type; };
template <>struct integral_delay<uint64_t> {typedef uint32_t type;};
template <>struct integral_delay<uint32_t> {typedef uint16_t type;};
template <>struct integral_delay<uint16_t> {typedef uint8_t type;};

template <class _tp> using integral_delay_t = typename integral_delay<_tp>::type;

template <class _tp>
void swap(_tp* value){
  typedef integral_delay_t<_tp> half_t;
  half_t* addr = reinterpret_cast<half_t*>(value);
  addr[0] ^= addr[1]; addr[1] ^= addr[0]; addr[0] ^= addr[1]; }

void swap(unsigned char* v);
void swap(void*);

template <size_t NN,class _tp>
void flip(_tp* value){
  if constexpr ( std::__is_one_of<_tp,void>::value || sizeof(_tp)*__CHAR_BIT__==NN) return;
  swap(value);
  typedef integral_delay_t<_tp> half_t;

  half_t* addr = reinterpret_cast<half_t*>(value);
  flip<NN,half_t>(addr);
  flip<NN,half_t>(addr+1);
}

template <class _tp>
void file_append(std::map<int,FILE*>& outfiles, int id, _tp const* addr, size_t size){
  if (auto iter=outfiles.find(id); iter == outfiles.end()){
    outfiles.insert(std::make_pair(id,fopen(std::to_string(id).c_str(),"wr")));
    fwrite(addr,sizeof(_tp),size,outfiles[id]);
    return; }
  fwrite(addr,sizeof(_tp),size,outfiles[id]);
}

//some foolish function!!!, I just take them for a task! do use it anyway!
//==>
template <class _iter_t>
bool is_peak(_iter_t first, _iter_t last){
  int distance = std::distance(first,last);
  if (std::abs(distance)<3) return false;
  size_t side_sz = std::floor(distance/3.);
  size_t middle_sz = distance-2*side_sz;
  long long side0 = std::accumulate(first,std::next(first,side_sz),0);
  long long middle = std::accumulate(std::next(first,side_sz),std::prev(last,side_sz),0);
  long long side1 = std::accumulate(std::prev(last,side_sz),last,0);
  return (middle>=side1 && middle>=side0 && side0>=0 && side1>=0)
  || (middle<=side1 && middle<=side0 && side0<=0 && side1<=0);
}

template <class _iter_t>
bool is_same_smaple(_iter_t first, _iter_t second){
  typedef decltype(*std::declval<_iter_t>()) element_t;
  auto iter = std::find_if(first,second,[](element_t t){return t!=0;});
  if (iter == second) return true;
  auto v = *iter;
  for (; iter != second; ++iter)
    if (*iter !=0){
      if (v**iter<0) return false;
      v = *iter; }
  return true; }

//<==


}
namespace util{

template <class _tp, template <class, class...> class _cont_tt=std::vector
  ,template <class...> class _tup_t=std::tuple, class... _args>
_cont_tt<_tp> unpack(_tup_t<_args...> const& _p0){
  constexpr std::size_t NN = sizeof...(_args);
  _cont_tt<_tp> tmp(NN);
  [&]<std::size_t... I>(std::index_sequence<I...>){
    (...,(tmp.at(I) = static_cast<_tp>(std::get<I>(_p0))));
  }(std::make_index_sequence<NN>());
  return tmp; }

template <std::size_t N>
struct size_{
  constexpr std::size_t static value = N; };

template <std::size_t N, std::size_t M, std::size_t value_first, std::size_t... _values>
struct get_helper{
  constexpr std::size_t static value = get_helper<N,M+1,_values...>::value; };
template <std::size_t N, std::size_t value_first, std::size_t... _values>
struct get_helper<N,N,value_first,_values...>{
  constexpr std::size_t static value = value_first; };

template <std::size_t N, class...>
struct get;

template <std::size_t N, std::size_t... _values>
requires (N<=(sizeof...(_values)-1))
struct get<N,std::index_sequence<_values...>>{
  constexpr static std::size_t value = get_helper<N,0,_values...>::value;};

template <std::size_t N, std::size_t... _values>
requires (N<=(sizeof...(_values)-1))
struct get_c{
  constexpr static std::size_t value = get_helper<N,0,_values...>::value;};

template <std::size_t _index_l, std::size_t _index_u, std::size_t... _values>
requires (_index_l>=0 && _index_u<=sizeof...(_values) && _index_l<=_index_u)
struct range_sum_helper{
  static constexpr std::size_t value = get_c<_index_l,_values...>::value
    + range_sum_helper<_index_l+1,_index_u,_values...>::value; };
template <std::size_t _index_l, std::size_t... _values>
struct range_sum_helper<_index_l,_index_l,_values...>{
  static constexpr std::size_t value = 0UL; };

template <std::size_t,std::size_t,class... _args>
struct range_sum;

template <std::size_t _index_l, std::size_t _index_u, std::size_t... _values>
struct range_sum<_index_l,_index_u,std::index_sequence<_values...>>{
  static constexpr std::size_t value =
    range_sum_helper<_index_l,_index_u,_values...>::value; };

template <std::size_t _index_l,std::size_t _index_u,std::size_t... _values>
struct range_sum_c{
  static constexpr std::size_t value =
    range_sum_helper<_index_l,_index_u,_values...>::value; };

template <class _tp, class _up=_tp, class... _args, std::size_t... _values>
requires std::unsigned_integral<_tp> && (sizeof...(_args)>=sizeof...(_values))
void encode(_tp& _dest,std::index_sequence<_values...>
    ,_args&&... _codes){
  _dest = (std::numeric_limits<_tp>::min)();
  constexpr static std::size_t NN = sizeof...(_values);
  auto codes = unpack<_up>(std::make_tuple(std::forward<_args>(_codes)...));
  [&]<std::size_t... I>(std::index_sequence<I...>){
    (...,(_dest |= codes.at(I) <<(range_sum_c<I+1,NN,_values...>::value)));
  }(std::make_index_sequence<NN>{}); }

template <class _tp, std::size_t N>
requires (std::unsigned_integral<_tp> && N<=__CHAR_BIT__*sizeof(_tp))
_tp decode_helper(_tp const& _p0){
  _tp bs = (std::numeric_limits<_tp>::max)();
  bs >>= sizeof(_tp)*__CHAR_BIT__-N;
  return _p0 & bs; }
template<class _tp, class _up=_tp, std::size_t... _values>
std::array<_up,sizeof...(_values)> decode(_tp const& _from, std::index_sequence<_values...>){
  constexpr static std::size_t NN = sizeof...(_values);
  std::array<_up,NN> tmp{};
  [&]<std::size_t... I>(std::index_sequence<I...>){
    (...,(tmp.at(NN-I-1)=decode_helper<_up,get_c<I,_values...>::value>(static_cast<_up>(_from>>range_sum_c<I+1,NN,_values...>::value))));
  }(std::make_index_sequence<NN>{});
  return tmp; }
}
#endif
