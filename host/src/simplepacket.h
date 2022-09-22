#ifndef SIMPLEPACKET_H
#define SIMPLEPACKET_H

#include <tuple>
#include "packet.h"
#include "field.h"

namespace c2 {
    template<class...> class Parser;
    template<class T, class... Args> class Parser<T, Args...> {
    public:
        static std::tuple<T, Args...> parse_tuple(
            std::vector<char>::const_iterator data,
            const std::vector<char>::const_iterator& end
        ) {
            return std::tuple_cat(std::make_tuple<T>(Field<T>::parse_field(data, end)), Parser<Args...>::parse_tuple(data, end));
        }
    };
    template<> class Parser<> {
    public:
        static std::tuple<> parse_tuple(
            std::vector<char>::const_iterator data,
            const std::vector<char>::const_iterator& end
        ) {
            return std::make_tuple<>();
        }
    };

    template<size_t I, class...> class Serializer;
    template<size_t I, class T, class... Args> class Serializer<I, T, Args...> {
    public:
        template<class... TupArgs>
        static void serialize_tuple(const std::tuple<TupArgs...>& vals, std::vector<char>& buf) {
            Field<T>::serialize_field(std::get<I>(vals), buf);
            Serializer<I + 1, Args...>::serialize_tuple(vals, buf);
        }
    };
    template<size_t I> class Serializer<I> {
    public:
        template<class... TupArgs>
        static void serialize_tuple(const std::tuple<TupArgs...>& val, std::vector<char>& buf) {}
    };
}

template<class... Args> class SimplePacket : public Packet {
public:
    SimplePacket(unsigned char type, Args&... args) : Packet(type), fields(args...) {}
    SimplePacket(unsigned char type, std::tuple<Args...> fields) : Packet(type), fields(fields) {}
    static void register_type(unsigned char type) {
        Packet::register_type(type, [type](const std::vector<char>& buf) {
            return std::make_unique<SimplePacket>(type, c2::Parser<Args...>::parse_tuple(buf.cbegin(), buf.cend()));
            });
    }
    SerializedPacket serialize() const {
        std::unique_ptr<std::vector<char>> buf = std::make_unique<std::vector<char>>();
        c2::Serializer<0, Args...>::serialize_tuple(fields, *buf.get());
        SerializedPacket spkt(_type, std::move(buf));
        return spkt;
    }
private:
    std::tuple<Args...> fields;
};

#endif