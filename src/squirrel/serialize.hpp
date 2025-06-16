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

#ifndef HEADER_SUPERTUX_SQUIRREL_SERIALIZE_HPP
#define HEADER_SUPERTUX_SQUIRREL_SERIALIZE_HPP

class ReaderMapping;
class Writer;

namespace ssq {
class Table;
} // namespace ssq

void load_squirrel_table(ssq::Table& table, const ReaderMapping& mapping);
void save_squirrel_table(const ssq::Table& table, Writer& writer);

#endif

/* EOF */
