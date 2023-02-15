#pragma once

#include <windows_fido_bridge/cbor/detail.hpp>

#include <windows_fido_bridge/binary_io.hpp>
#include <windows_fido_bridge/util.hpp>

#include <array>
#include <iostream>
#include <span>
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <variant>

namespace wfb {

class cbor_value;

namespace detail {

template <typename TChar>
struct basic_cbor_string_type;

template <typename TChar>
inline constexpr uint8_t basic_cbor_string_type_v = basic_cbor_string_type<TChar>::value;

template <> struct basic_cbor_string_type<uint8_t> { static constexpr uint8_t value = CBOR_BYTE_STRING; };
template <> struct basic_cbor_string_type<char> { static constexpr uint8_t value = CBOR_TEXT_STRING; };

}  // namespace detail

template <typename TString>
class basic_cbor_string {
public:
    using string_type = TString;
    using value_type = typename string_type::value_type;
    using view_type = std::basic_string_view<value_type, typename string_type::traits_type>;
    using span_type = std::span<value_type>;
    using const_span_type = std::span<const value_type>;
    using vector_type = std::vector<value_type>;

    explicit basic_cbor_string(binary_reader& reader) {
        auto [type, size] = read_raw_length(reader);
        if (type != detail::basic_cbor_string_type_v<value_type>) {
            throw std::runtime_error(fmt::format("Invalid type value {:02x} for basic_cbor_string", type));
        }

        _str.resize(size);
        reader.read_into(reinterpret_cast<uint8_t*>(_str.data()), _str.size());
    }

    basic_cbor_string(span_type span) : _str(span.data(), span.size()) {}
    basic_cbor_string(const_span_type span) : _str(span.data(), span.size()) {}

    basic_cbor_string(string_type str) : _str(std::move(str)) {}
    basic_cbor_string(view_type str) : _str(str) {}
    basic_cbor_string(const value_type* str) : _str(str) {}

    basic_cbor_string() {}

    void dump_cbor_into(binary_writer& writer) const {
        write_initial_byte_into(writer, detail::basic_cbor_string_type_v<value_type>, _str.size());
        writer.write_string(_str);
    }

    const string_type& string() const { return _str; }
    view_type view() const { return _str; }
    const_span_type span() const { return _str; }
    vector_type vector() const { return vector_type{_str.cbegin(), _str.cend()}; }

    const value_type* c_str() const { return reinterpret_cast<const value_type*>(_str.c_str()); }
    const value_type* data() const { return _str.data(); }
    size_t size() const { return _str.size(); }

    operator string_type() const { return string(); }
    operator view_type() const { return view(); }

    operator vector_type() const { return vector(); }

    std::string dump_debug() const {
        std::stringstream ss;
        dump_debug(ss);
        return ss.str();
    }

    void dump_debug(std::stringstream& ss) const {
        if constexpr (std::is_same_v<value_type, uint8_t>) {
            ss << 'b';
        }

        ss << '"';

        for (auto c : _str) {
            if constexpr (std::is_same_v<value_type, uint8_t>) {
                ss << fmt::format("{:02x}", c);
            } else {
                if (c == '"') {
                    ss << '\\';
                }

                ss << c;
            }
        }

        ss << '"';
    }

private:
    TString _str;
};

template <typename TString>
bool operator==(const basic_cbor_string<TString>& lhs, const basic_cbor_string<TString>& rhs) { return lhs.string() == rhs.string(); }
template <typename TString>
bool operator==(const basic_cbor_string<TString>& lhs, const char* rhs) { return lhs.string() == rhs; }
template <typename TString>
bool operator==(const basic_cbor_string<TString>& lhs, const std::string& rhs) { return lhs.string() == rhs; }
template <typename TString>
bool operator==(const basic_cbor_string<TString>& lhs, std::string_view rhs) { return lhs.view() == rhs; }

template <typename TString>
bool operator<(const basic_cbor_string<TString>& lhs, const basic_cbor_string<TString>& rhs) { return lhs.string() < rhs.string(); }

using cbor_byte_string = basic_cbor_string<std::basic_string<uint8_t>>;
using cbor_text_string = basic_cbor_string<std::basic_string<char>>;

}  // namespace wfb
