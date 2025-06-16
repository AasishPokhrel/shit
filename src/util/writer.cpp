//  SuperTux
//  Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "util/writer.hpp"

#include <sexp/value.hpp>
#include <sexp/io.hpp>

#include "physfs/ofile_stream.hpp"
#include "util/log.hpp"

Writer::Writer(const std::string& filename) :
  m_filename(filename),
  out(new OFileStream(filename)),
  out_owned(true),
  indent_depth(0),
  lists()
{
  out->precision(7);
}

Writer::Writer(std::ostream& newout) :
  m_filename("<stream>"),
  out(&newout),
  out_owned(false),
  indent_depth(0),
  lists()
{
  out->precision(7);
}

Writer::~Writer()
{
  if (lists.size() > 0) {
    log_warning << m_filename << ": Not all sections closed in Writer" << std::endl;
  }
  if (out_owned)
    delete out;
}

void
Writer::write_comment(const std::string& comment)
{
  *out << "; " << comment << "\n";
}

void
Writer::start_list(const std::string& listname, bool string)
{
  indent();
  *out << '(';
  if (string)
    write_escaped_string(listname);
  else
    *out << listname;
  *out << '\n';
  indent_depth += 2;

  lists.push_back(listname);
}

void
Writer::end_list(const std::string& listname)
{
  if (lists.size() == 0) {
    log_warning << m_filename << ": Trying to close list '" << listname << "', which is not open" << std::endl;
    return;
  }
  if (lists.back() != listname) {
    log_warning << m_filename << ": trying to close list '" << listname << "' while list '" << lists.back() << "' is open" << std::endl;
    return;
  }
  lists.pop_back();

  indent_depth -= 2;
  indent();
  *out << ")\n";
}

void
Writer::write(const std::string& name, int value)
{
  indent();
  *out << '(' << name << ' ' << value << ")\n";
}

void
Writer::write(const std::string& name, float value)
{
  indent();
  *out << '(' << name << ' ' << value << ")\n";
}

void
Writer::write(const std::string& name, const UID& uid)
{
  indent();
  *out << '(' << name << ' ' << uid << ")\n";
}

/** This function is needed to properly resolve the overloaded write()
    function, without it the call write("foo", "bar") would call
    write(name, bool), not write(name, string, bool) */
void
Writer::write(const std::string& name, const char* value)
{
  write(name, value, false);
}

void
Writer::write(const std::string& name, const std::string& value,
              bool translatable)
{
  indent();
  *out << '(' << name;
  if (translatable) {
    *out << " (_ ";
    write_escaped_string(value);
    *out << "))\n";
  } else {
    *out << " ";
    write_escaped_string(value);
    *out << ")\n";
  }
}

void
Writer::write(const std::string& name, bool value)
{
  indent();
  *out << '(' << name << ' ' << (value ? "#t" : "#f") << ")\n";
}

void
Writer::write(const std::string& name,
              const std::vector<int>& value)
{
  indent();
  *out << '(' << name;
  for (const auto& i : value)
    *out << " " << i;
  *out << ")\n";
}

void
Writer::write(const std::string& name,
              const std::vector<unsigned int>& value,
              int width)
{
  indent();
  *out << '(' << name;
  if (!width)
  {
    for (const auto& i : value)
      *out << " " << i;
  }
  else
  {
    *out << "\n";
    indent();
    int count = 0;
    for (const auto& i : value) {
      *out << i;
      count += 1;
      if (count >= width) {
        *out << "\n";
        indent();
        count = 0;
      } else {
        *out << " ";
      }
    }
  }
  *out << ")\n";
}

void
Writer::write(const std::string& name,
              const std::vector<float>& value)
{
  indent();
  *out << '(' << name;
  for (const auto& i : value)
    *out << " " << i;
  *out << ")\n";
}

void
Writer::write(const std::string& name,
              const std::vector<std::string>& value)
{
  indent();
  *out << '(' << name;
  for (const auto& i : value) {
    *out << " ";
    write_escaped_string(i);
  }
  *out << ")\n";
}

void
Writer::write_sexp(const sexp::Value& value, bool fudge)
{
  if (value.is_array()) {
    if (fudge) {
      indent_depth -= 1;
      indent();
      indent_depth += 1;
    } else {
      indent();
    }
    *out << "(";
    auto& arr = value.as_array();
    for(size_t i = 0; i < arr.size(); ++i) {
      write_sexp(arr[i], false);
      if (i != arr.size() - 1) {
        *out << " ";
      }
    }
    *out << ")\n";
  } else {
    *out << value;
  }
}

void
Writer::write(const std::string& name, const sexp::Value& value)
{
  indent();
  *out << '(' << name << "\n";
  indent_depth += 4;
  write_sexp(value, true);
  indent_depth -= 4;
  indent();
  *out << ")\n";
}

void
Writer::write_compressed(const std::string& name, const std::vector<unsigned int>& value)
{
  indent();
  if (value.empty())
  {
    *out << '(' << name << ")\n";
    return;
  }
  *out << '(' << name << ' ';

  int repeater = 0;
  unsigned int repeated_value = 0;
  for (const auto& i : value)
  {
    if (repeater && i == repeated_value)
    {
      ++repeater;
    }
    else
    {
      if (repeater > 1)
        *out << -repeater << ' ' << repeated_value << ' ';
      else if (repeater == 1)
        *out << repeated_value << ' ';

      repeater = 1;
      repeated_value = i;
    }
  }
  if (repeater > 1)
    *out << -repeater << ' ' << repeated_value;
  else
    *out << repeated_value;

  *out << ")\n";
}

void
Writer::write_escaped_string(const std::string& str)
{
  *out << '"';
  for (const char* c = str.c_str(); *c != 0; ++c) {
    if (*c == '\"')
      *out << "\\\"";
    else if (*c == '\\')
      *out << "\\\\";
    else
      *out << *c;
  }
  *out << '"';
}

void
Writer::indent()
{
  for (int i = 0; i<indent_depth; ++i)
    *out << ' ';
}

/* EOF */
