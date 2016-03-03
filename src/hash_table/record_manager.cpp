/**
 * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <bitcoin/database/hash_table/record_manager.hpp>

#include <cstddef>
#include <stdexcept>
#include <boost/thread.hpp>
#include <bitcoin/bitcoin.hpp>
#include <bitcoin/database/memory/memory.hpp>
#include <bitcoin/database/memory/memory_map.hpp>

/// -- file --
/// [ header ]
/// [ record_count ]
/// [ payload ]

/// -- header (hash table) --
/// [ bucket ]
/// ...
/// [ bucket ]

/// -- payload (fixed size records) --
/// [ record ]
/// ...
/// [ record ]

namespace libbitcoin {
namespace database {
    
// TODO: guard against overflows.

record_manager::record_manager(memory_map& file, file_offset header_size,
    size_t record_size)
  : file_(file),
    header_size_(header_size),
    record_count_(0),
    record_size_(record_size)
{
}

void record_manager::create()
{
    // Critical Section
    ///////////////////////////////////////////////////////////////////////////
    //boost::shared_lock<boost::shared_mutex> unique_lock(mutex_);

    if (record_count_ != 0)
        throw std::runtime_error("Existing file record count is nonzero.");

    file_.allocate(header_size_ + record_to_position(record_count_));

    write_count();
    ///////////////////////////////////////////////////////////////////////////
}

void record_manager::start()
{
    // Critical Section
    ///////////////////////////////////////////////////////////////////////////
    //boost::shared_lock<boost::shared_mutex> unique_lock(mutex_);

    read_count();
    const auto minimum = header_size_ + record_to_position(record_count_);

    if (minimum > file_.size())
        throw std::runtime_error("Records size exceeds file size.");
    ///////////////////////////////////////////////////////////////////////////
}

void record_manager::sync()
{
    // Critical Section
    ///////////////////////////////////////////////////////////////////////////
    //boost::shared_lock<boost::shared_mutex> unique_lock(mutex_);

    write_count();
    ///////////////////////////////////////////////////////////////////////////
}

array_index record_manager::count() const
{
    // Critical Section
    ///////////////////////////////////////////////////////////////////////////
    //boost::shared_lock<boost::shared_mutex> shared_lock(mutex_);

    return record_count_;
    ///////////////////////////////////////////////////////////////////////////
}

void record_manager::set_count(const array_index value)
{
    // Critical Section
    ///////////////////////////////////////////////////////////////////////////
    //boost::shared_lock<boost::shared_mutex> unique_lock(mutex_);

    BITCOIN_ASSERT(value <= record_count_);

    record_count_ = value;
    ///////////////////////////////////////////////////////////////////////////
}

// Return the next index, regardless of the number created.
// The file is thread safe, the critical section is to protect record_count_.
array_index record_manager::new_records(size_t count)
{
    // Critical Section
    ///////////////////////////////////////////////////////////////////////////
    //boost::shared_lock<boost::shared_mutex> unique_lock(mutex_);

    // Always write after the last index.
    const auto next_record_index = record_count_;

    const size_t position = record_to_position(record_count_ + count);
    const size_t required_size = header_size_ + position;
    file_.allocate(required_size);
    record_count_ += count;

    return next_record_index;
    ///////////////////////////////////////////////////////////////////////////
}

const memory::ptr record_manager::get(array_index record) const
{
    // Ensure requested record is within the file.
    // We avoid a runtime error here to optimize out the count lock.
    BITCOIN_ASSERT_MSG(record < count(), "Read past end of file.");

    const auto memory = file_.access();
    memory->increment(header_size_ + record_to_position(record));
    return memory;
}

// privates

// Read the count value from the first 32 bits of the file after the header.
void record_manager::read_count()
{
    BITCOIN_ASSERT(header_size_ + sizeof(array_index) <= file_.size());

    // The accessor must remain in scope until the end of the block.
    const auto memory = file_.access();
    const auto count_address = memory->buffer() + header_size_;
    record_count_ = from_little_endian_unsafe<array_index>(count_address);
}

// Write the count value to the first 32 bits of the file after the header.
void record_manager::write_count()
{
    BITCOIN_ASSERT(header_size_ + sizeof(array_index) <= file_.size());

    // The accessor must remain in scope until the end of the block.
    auto memory = file_.access();
    auto payload_size_address = memory->buffer() + header_size_;
    auto serial = make_serializer(payload_size_address);
    serial.write_little_endian(record_count_);
}

array_index record_manager::position_to_record(file_offset position) const
{
    return (position - sizeof(array_index)) / record_size_;
}

file_offset record_manager::record_to_position(array_index record) const
{
    return sizeof(array_index) + record * record_size_;
}

} // namespace database
} // namespace libbitcoin
