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

#include <zim/zim.h>
#include <zim/file.h>

#include "tempfile.h"

#include "gtest/gtest.h"

namespace
{

using zim::unittests::TempFile;

using TestContextImpl = std::vector<std::pair<std::string, std::string> >;
struct TestContext : TestContextImpl {
  TestContext(const std::initializer_list<value_type>& il)
    : TestContextImpl(il)
  {}
};

std::ostream& operator<<(std::ostream& out, const TestContext& ctx)
{
  out << "Test context:\n";
  for ( const auto& kv : ctx )
    out << "\t" << kv.first << ": " << kv.second << "\n";
  out << std::endl;
  return out;
}

std::string
emptyZimFileContent()
{
  std::string content;
  content += "ZIM\x04"; // Magic
  content += "\x05" + std::string(3, '\0'); // Version
  content += std::string(16, '\0'); // uuid
  content += std::string(4, '\0'); // article count
  content += std::string(4, '\0'); // cluster count
  content += "\x50" + std::string(7, '\0'); // url ptr pos
  content += "\x50" + std::string(7, '\0'); // title ptr pos
  content += "\x50" + std::string(7, '\0'); // cluster ptr pos
  content += "\x50" + std::string(7, '\0'); // mimelist ptr pos
  content += std::string(4, '\0'); // main page index
  content += std::string(4, '\0'); // layout page index
  content += "\x50" + std::string(7, '\0'); // checksum pos
  content += "\x8a\xbb\xad\x98\x64\xd5\x48\xb2\xb9\x71\xab\x30\xed\x29\xa4\x01"; // md5sum
  return content;
}

class TempZimFile : public TempFile
{
public:
  TempZimFile(const char* name, const std::string& content)
    : TempFile(name)
  {
    write(fd(), &content[0], content.size());
    close();
  }
};


TEST(ZimFile, openingAnInvalidZimFileFails)
{
  const char* const prefixes[] = { "ZIM\x04", "" };
  const unsigned char bytes[] = {0x00, 0x01, 0x11, 0x30, 0xFF};
  for ( const std::string prefix : prefixes ) {
    for ( const unsigned char byte : bytes ) {
      for ( int count = 0; count < 100; count += 10 ) {
        const TestContext ctx{
                {"prefix",  prefix.size() ? "yes" : "no" },
                {"byte", std::to_string(byte) },
                {"count", std::to_string(count) }
        };
        const std::string zimfileContent = prefix + std::string(count, byte);
        const TempZimFile tmpfile("invalid_zim_file", zimfileContent);

        EXPECT_THROW( zim::File(tmpfile.path()), std::runtime_error ) << ctx;
      }
    }
  }
}

TEST(ZimFile, openingAnEmptyZimFileSucceeds)
{
  const TempZimFile tmpfile("empty_zim_file", emptyZimFileContent());

  zim::File zimfile(tmpfile.path());
  ASSERT_TRUE(zimfile.verify());
}

TEST(ZimFile, wrongChecksumInEmptyZimFile)
{
  std::string zimfileContent = emptyZimFileContent();
  zimfileContent[85] = '\xff';
  const TempZimFile tmpfile("wrong_checksum_empty_zim_file", zimfileContent);

  zim::File zimfile(tmpfile.path());
  ASSERT_FALSE(zimfile.verify());
}

} // unnamed namespace
