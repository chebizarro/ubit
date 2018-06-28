/*
 * UResource
 */

#include <cstddef>

namespace ubit {

class UResource {
public:
  UResource(const char* start, const size_t len) : resource_data(start), data_len(len) {}

  const char * const &data() const { return resource_data; }
  const size_t &size() const { return data_len; }

  const char *begin() const { return resource_data; }
  const char *end() const { return resource_data + data_len; }

  std::string toString() { return std::string(data(), size()); }

private:
  const char* resource_data;
  const size_t data_len;

};

}
