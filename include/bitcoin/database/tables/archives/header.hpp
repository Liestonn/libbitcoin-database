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
#ifndef LIBBITCOIN_DATABASE_TABLES_ARCHIVES_HEADER_HPP
#define LIBBITCOIN_DATABASE_TABLES_ARCHIVES_HEADER_HPP

#include <bitcoin/system.hpp>
#include <bitcoin/database/define.hpp>
#include <bitcoin/database/tables/schema.hpp>
#include <bitcoin/database/memory/memory.hpp>

namespace libbitcoin {
namespace database {
namespace header {

BC_PUSH_WARNING(NO_METHOD_HIDING)

struct record
{
    // Sizes.
    static constexpr size_t pk = schema::c::block;
    static constexpr size_t sk = schema::c::hash;
    static constexpr size_t size =
        schema::c::block +
        schema::c::flags +
        sizeof(uint32_t) +
        pk +
        sizeof(uint32_t) +
        sizeof(uint32_t) +
        sizeof(uint32_t) +
        sizeof(uint32_t) +
        schema::c::hash;
    static constexpr size_t total = pk + sk + size;
    static_assert(size == 62u);
    static_assert(total == 97u);
    static constexpr linkage<pk> count() NOEXCEPT { return 1; }

    // Fields.
    uint32_t height;
    uint32_t flags;
    uint32_t mtp;
    uint32_t parent_fk;
    uint32_t version;
    uint32_t time;
    uint32_t bits;
    uint32_t nonce;
    hash_digest root;
    bool valid{ false };

    // Serialializers.

    inline record from_data(reader& source) NOEXCEPT
    {
        height    = source.read_3_bytes_little_endian();
        flags     = source.read_4_bytes_little_endian();
        mtp       = source.read_4_bytes_little_endian();
        parent_fk = source.read_3_bytes_little_endian();
        version   = source.read_4_bytes_little_endian();
        time      = source.read_4_bytes_little_endian();
        bits      = source.read_4_bytes_little_endian();
        nonce     = source.read_4_bytes_little_endian();
        root      = source.read_hash();
        BC_ASSERT(source.get_position() == total);
        valid = source;
        return *this;
    }

    inline bool to_data(finalizer& sink) const NOEXCEPT
    {
        sink.write_3_bytes_little_endian(height);
        sink.write_4_bytes_little_endian(flags);
        sink.write_4_bytes_little_endian(mtp);
        sink.write_3_bytes_little_endian(parent_fk);
        sink.write_4_bytes_little_endian(version);
        sink.write_4_bytes_little_endian(time);
        sink.write_4_bytes_little_endian(bits);
        sink.write_4_bytes_little_endian(nonce);
        sink.write_bytes(root);
        BC_ASSERT(sink.get_position() == total);
        return sink;
    }
};

// Derivations are non-virtual, method-hiding.
// Generally only readers are extended, as there are no write updates.
// Use non-derivation to subset properties (must expose size).
// Use derivation to extend properties.

struct record_height
{
    static constexpr size_t size = record::size;

    inline record_height from_data(reader& source) NOEXCEPT
    {
        height = source.read_3_bytes_little_endian();
        valid = source;
        return *this;
    }

    uint32_t height;
    bool valid{ false };
};

struct record_with_key
  : public record
{
    inline record_with_key from_data(reader& source) NOEXCEPT
    {
        source.rewind_bytes(record::sk);
        key = source.read_hash();
        record::from_data(source);
        return *this;
    }

    hash_digest key;
};

class BCD_API table : public RECORDHASHMAP { public: using RECORDHASHMAP::hashmap; };

BC_POP_WARNING()

} // namespace header
} // namespace database
} // namespace libbitcoin

#endif
