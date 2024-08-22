#ifndef unpack_HPP
#define unpack_HPP 1 
#include <string>
#include <array>
#include <iostream>

struct unpacker_base{
  std::string m_name;
  virtual bool parse() = 0;
};

template <class _tp>
struct id{ std::string name; _tp range; virtual ~id() noexcept = default; };

template <class _tp, class member_t, class rt_t, class... args>
struct data_region : public id<std::array<uint64_t,2>>, public member_t{
  typedef _tp unit_type;
  typedef id id_t;
  typedef member_t member_type;

public:
  data_region() = default;
  virtual ~data_region() noexcept = default;

  virtual std::ostream& display(std::ostream& os = std::cout) const{
    return os; }

  virtual rt_t fill(args...) = 0;
  virtual bool valid() const = 0;
  virtual uint16_t size() const = 0;
  virtual bool crc32() const = 0;
};
#endif
