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

#include "tempfile.h"

#ifdef _WIN32
#include <codecvt>
#include <windows.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>
#include <fileapi.h>
#undef min
#undef max
#else
#include <unistd.h>
#endif

namespace zim
{

namespace unittests
{

TempFile::TempFile(const char* name)
{
#ifdef _WIN32
  std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> utfConv;
  wchar_t cbase[MAX_PATH];
  wchar_t ctmp[MAX_PATH];
  const std::wstring wname = utfConv.from_bytes(name);
  GetTempPathW(MAX_PATH-(wname.size()+2), cbase);
  // This create a file for us, ensure it is unique.
  // So we need to delete it and create the directory using the same name.
  GetTempFileNameW(cbase, wname.c_str(), 0, ctmp);
  fd_ = _wopen(ctmp, _O_CREAT | _O_TEMPORARY | _O_SHORT_LIVED | _O_RDWR | _O_TRUNC);
  path_ = utfConv.to_bytes(ctmp);
#else
  const char* const TMPDIR = std::getenv("TMPDIR");
  const std::string tmpdir(TMPDIR ? TMPDIR : "/tmp");
  path_ = tmpdir + "/" + name + "_XXXXXX";
  fd_ = mkstemp(&path_[0]);
#endif
}

TempFile::~TempFile()
{
  if (fd_ != -1)
    close();
#ifndef _WIN32
  unlink(path_.c_str());
#endif
}

void
TempFile::close()
{
  assert(fd_ != -1);
  ::close(fd_);
  fd_ = -1;
}

} // namespace unittests

} // namespace zim
