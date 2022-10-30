#pragma once

#include "messages.hpp"
#include "rofi_hal.hpp"  // PBuf
#include "span.hpp" // todo: solve

#include <cstddef>  // std::size_t
#include <cstdint>  // UINT16_MAX
#include <stdexcept>  // std::out_of_range; std::runtime_error

namespace rofi::updater::messages {
    using rofi::hal::PBuf;
    using namespace rofi::updater;

    template< typename T >
    void* write( void* buffer, const T& value ) {
        auto* b = static_cast< T* >( buffer );
        *b = value;
        return b + 1;
    }

    template<>
    void* write< std::span< const decltype( DataMessage::data )::value_type > > ( void* buffer, const std::span< const decltype( DataMessage::data )::value_type >& value );

    template< typename T >
    void* read( void* buffer, void* bufferEnd, T& value ) {
        auto* b = static_cast< T* >( buffer );

        if ( static_cast< void* >( b + 1 ) > bufferEnd ) {
            throw std::out_of_range( "Exhausted buffer" );
        }

        value = *b;

        return b + 1;
    }

    // let's assume that the whole packet is in a single PBuf for now
    template<>
    void* read< std::span< decltype( DataMessage::data )::value_type > > ( void* buffer, void* bufferEnd, std::span< decltype( DataMessage::data )::value_type >& value );

    PBuf serialize( const Message& message );

    PBuf serialize( const AnnounceMessage& msg );
    PBuf serialize( const RequestMessage& msg );
    PBuf serialize( const DataMessage& msg );

    Message deserialize( PBuf& buffer );
}
