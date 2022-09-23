#ifndef SIMPLEPACKET_H
#define SIMPLEPACKET_H

#include <tuple>
#include <functional>
#include "packet.h"
#include "field.h"

namespace c2 {
    template<class...> struct Parser;
    template<class T, class... Args> struct Parser<T, Args...> {
        static std::tuple<T, Args...> parse_tuple(
            std::vector<char>::const_iterator data,
            const std::vector<char>::const_iterator& end
        ) {
            return std::tuple_cat(std::make_tuple<T>(Field<T>::parse_field(data, end)), Parser<Args...>::parse_tuple(data, end));
        }
    };
    template<> struct Parser<> {
        static std::tuple<> parse_tuple(
            std::vector<char>::const_iterator data,
            const std::vector<char>::const_iterator& end
        ) {
            return std::make_tuple<>();
        }
    };

    template<size_t I, class...> struct Serializer;
    template<size_t I, class T, class... Args> struct Serializer<I, T, Args...> {
        template<class... TupArgs>
        static void serialize_tuple(const std::tuple<TupArgs...>& vals, std::vector<char>& buf) {
            Field<T>::serialize_field(std::get<I>(vals), buf);
            Serializer<I + 1, Args...>::serialize_tuple(vals, buf);
        }
    };
    template<size_t I> struct Serializer<I> {
        template<class... TupArgs>
        static void serialize_tuple(const std::tuple<TupArgs...>& val, std::vector<char>& buf) {}
    };
}


template<class... Args> class SimplePacket : public Packet {

public:
    SimplePacket(packettype_t type, Args&... args) : Packet(type), fields(args...) {}
    SimplePacket(packettype_t type, std::tuple<Args...> fields) : Packet(type), fields(fields) {}

    static
    std::function<std::unique_ptr<SimplePacket>(const std::vector<uint8_t>&)>
    build(packettype_t type) {
        return [type](const std::vector<uint8_t>& buf) {
            return std::make_unique<SimplePacket>(type, c2::Parser<Args...>::parse_tuple(buf.cbegin(), buf.cend()));
        };
    }

    SerializedPacket serialize() const {
        std::vector<uint8_t> buf;
        c2::Serializer<0, Args...>::serialize_tuple(fields, buf);
        SerializedPacket spkt(_type, std::move(buf));
        return spkt;
    }

private:
    std::tuple<Args...> fields;
    
};

#endif