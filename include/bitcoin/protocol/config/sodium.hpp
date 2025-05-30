/**
 * Copyright (c) 2011-2025 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_PROTOCOL_CONFIG_SODIUM_HPP
#define LIBBITCOIN_PROTOCOL_CONFIG_SODIUM_HPP

#include <bitcoin/system.hpp>
#include <bitcoin/protocol/define.hpp>

namespace libbitcoin {
namespace protocol {

/// Serialization helper for base58 sodium keys.
class BCP_API sodium
{
public:
    DEFAULT_COPY_MOVE_DESTRUCT(sodium);

    /// A list of base85 values.
    /// This must provide operator<< for ostream in order to be used as a
    /// boost::program_options default_value.
    sodium() NOEXCEPT;
    sodium(const std::string& base85) THROWS;
    sodium(const system::hash_digest& value) NOEXCEPT;

    /// True if the key is initialized.
    operator bool() const NOEXCEPT;

    /// Overload cast to internal type.
    operator const system::hash_digest&() const NOEXCEPT;

    /// Get the key as a base85 encoded (z85) string.
    std::string to_string() const NOEXCEPT;

    friend std::istream& operator>>(std::istream& input,
        sodium& argument) THROWS;
    friend std::ostream& operator<<(std::ostream& output,
        const sodium& argument) THROWS;

private:
    system::hash_digest value_;
};

typedef std::vector<sodium> sodiums;

} // namespace protocol
} // namespace libbitcoin

#endif
