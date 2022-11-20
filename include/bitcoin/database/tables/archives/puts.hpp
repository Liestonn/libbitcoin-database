/**
 * Copyright (c) 2011-2022 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef LIBBITCOIN_DATABASE_TABLES_ARCHIVES_PUTS_HPP
#define LIBBITCOIN_DATABASE_TABLES_ARCHIVES_PUTS_HPP

#include <algorithm>
#include <bitcoin/system.hpp>
#include <bitcoin/database/define.hpp>
#include <bitcoin/database/memory/memory.hpp>
#include <bitcoin/database/primitives/primitives.hpp>
#include <bitcoin/database/tables/schema.hpp>

namespace libbitcoin {
namespace database {
namespace table {

/// Puts is an array of input or output fk records.
/// Multiple may be allocated, put_fks.size() (from tx) determines read extent.
class puts
  : public array_map<schema::puts>
{
public:
    using array_map<schema::puts>::arraymap;

    struct record
       : public schema::puts
    {
        linkage<pk> count() const NOEXCEPT
        {
            using namespace system;
            using out = typename linkage<pk>::integer;
            BC_ASSERT(put_fks.size() < power2<uint64_t>(to_bits(schema::put)));
            return possible_narrow_cast<out>(put_fks.size());
        }

        std_vector<uint64_t> put_fks{};

        inline bool from_data(reader& source) NOEXCEPT
        {
            // Clear the single record limit (file limit remains).
            source.set_limit();

            std::for_each(put_fks.begin(), put_fks.end(), [&](auto& fk) NOEXCEPT
            {
                fk = source.read_little_endian<uint64_t, schema::put>();
            });

            BC_ASSERT(source.get_position() == count() * schema::put);
            return source;
        }

        inline bool to_data(writer& sink) const NOEXCEPT
        {
            // Clear the single record limit (file limit remains).
            sink.set_limit();

            std::for_each(put_fks.begin(), put_fks.end(), [&](const auto& fk) NOEXCEPT
            {
                sink.write_little_endian<uint64_t, schema::put>(fk);
            });

            BC_ASSERT(sink.get_position() == count() * schema::put);
            return sink;
        }

        inline bool operator==(const record& other) const NOEXCEPT
        {
            return put_fks == other.put_fks;
        }
    };
};

} // namespace table
} // namespace database
} // namespace libbitcoin

#endif
