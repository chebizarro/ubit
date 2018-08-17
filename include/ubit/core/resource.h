/*
 * Resource.h: Embedded resources
 * 
 * (C) Copyright 2018 Chris Daley <chebizarro@gmail.com>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 */

#include <cstddef>

#ifndef UBIT_CORE_RESOURCE_H_
#define	UBIT_CORE_RESOURCE_H_

namespace ubit {

class Resource {
public:
  Resource(const char* start, const size_t len) : resource_data(start), data_len(len) {}

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

#endif // UBIT_CORE_RESOURCE_H_
