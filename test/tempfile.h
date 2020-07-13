/*
 * Copyright (C) 2020 Veloman Yunkan
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * is provided AS IS, WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, and
 * NON-INFRINGEMENT.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 *
 */

#ifndef ZIM_TEST_TEMPFILE_H
#define ZIM_TEST_TEMPFILE_H

#include <cassert>
#include <string>

namespace zim
{

namespace unittests
{

class TempFile
{
  int fd_ = -1;
  std::string path_;
public:
  explicit TempFile(const char* name);

  TempFile(const TempFile& ) = delete;
  void operator=(const TempFile& ) = delete;

  ~TempFile();

  void close();

  int fd() const { assert(fd_ != -1); return fd_; }
  std::string path() const { return path_; }
};

} // namespace unittests

} // namespace zim

#endif // ZIM_TEST_TEMPFILE_H
