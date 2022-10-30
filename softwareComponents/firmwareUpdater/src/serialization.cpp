#include "serialization.hpp"

namespace rofi::updater::messages {

template<>
void* write< std::span< const decltype( DataMessage::data )::value_type > > ( void* buffer, const std::span< const decltype( DataMessage::data )::value_type >& value ) {
    auto* b = static_cast< decltype( DataMessage::data )::pointer >( buffer );
    for ( auto& i : value ) {  // todo: maybe use memcpy or similar
        *b = i;
        b += 1;
    }

    return b;
}

template<>
void* read< std::span< decltype( DataMessage::data )::value_type > > ( void* buffer, void* bufferEnd, std::span< decltype( DataMessage::data )::value_type >& value ) {
    auto* b = static_cast< decltype( DataMessage::data )::pointer >( buffer );
    for ( auto& i : value ) {  // todo: maybe use memcpy or similar
        i = *b;
        b += 1;
    }

    return b;
}

PBuf serialize( const Message& message ) {
    PBuf b = PBuf::empty();
    std::visit(
            [ & ]( const auto& message ) { PBuf a = serialize( message ); b = a; },
            message
    );

    return b;
}

PBuf serialize( const AnnounceMessage& msg ) {
    constexpr std::size_t payloadSize = sizeof( MessageType ) + sizeof( msg );
    static_assert( payloadSize < UINT16_MAX, "Max PBuf size exceeded" );

    PBuf buffer = PBuf::allocate( payloadSize );
    assert( buffer.simple() && "Expecting simple PBuf" );

    void* b = buffer.payload();

    b = write( b, MessageType::ANNOUNCE );
    b = write( b, msg.fwType );
    b = write( b, msg.fwVersion );
    b = write( b, msg.chunkId );
    b = write( b, msg.seq );
    b = write( b, msg.chunkSize );
    b = write( b, msg.numOfChunks );
    b = write( b, msg.fwTotalSize );
    b = b;

    return buffer;
}

PBuf serialize( const RequestMessage& msg ) {
    constexpr std::size_t payloadSize = sizeof( MessageType ) + sizeof( msg );
    static_assert( payloadSize < UINT16_MAX, "Max PBuf size exceeded" );

    PBuf buffer = PBuf::allocate( payloadSize );
    assert( buffer.simple() && "Expecting simple PBuf" );

    void* b = buffer.payload();

    b = write( b, MessageType::REQUEST );
    b = write( b, msg.fwType );
    b = write( b, msg.fwVersion );
    b = write( b, msg.chunkId );
    b = write( b, msg.seq );
//        b = write( b, msg.chunkSize );  // todo: should we send also these??
    b = write( b, msg.numOfChunks );
//        b = write( b, msg.fwTotalSize );
    b = b;

    return buffer;
}

PBuf serialize( const DataMessage& msg ) {
    const std::size_t payloadSize = sizeof( MessageType ) + sizeof( msg ) - sizeof( msg.data ) + msg.chunkSize;
    assert( payloadSize < UINT16_MAX );

    PBuf buffer = PBuf::allocate( static_cast< int >( payloadSize ) );
    assert( buffer.simple() && "Expecting simple PBuf" );

    void* b = buffer.payload();

    b = write( b, MessageType::DATA );
    b = write( b, msg.fwType );
    b = write( b, msg.fwVersion );
    b = write( b, msg.chunkId );
    b = write( b, msg.seq );
    b = write( b, msg.numOfChunks );
    b = write( b, msg.chunkSize );

    std::span< const decltype( DataMessage::data )::value_type > x{ msg.data.data(), msg.chunkSize };

    b = write( b, x );
//        b = write(b, msg.fwTotalSize);  // todo: should we send also this??
    b = b;

    return buffer;
}

Message deserialize( PBuf& buffer ) {  // todo: we can't use const reference because chunksBegin is not const available
    auto it = buffer.chunksBegin();
    void* ptr = it->mem();
    void* end = static_cast< uint8_t* >( ptr ) + it->size();

    MessageType messageType;
    Seq seq;
    FWType fwType;
    FWVersion fwVersion;

    ChunkId chunkId;

    ptr = read( ptr, end, messageType );
    ptr = read( ptr, end, fwType );
    ptr = read( ptr, end, fwVersion );
    ptr = read( ptr, end, chunkId );
    ptr = read( ptr, end, seq );

    Message m;

    switch ( messageType ) {
        case MessageType::ANNOUNCE: {
            m.emplace< 0 >( seq, fwType, fwVersion, 0, 0, 0, chunkId );

            auto& announce = std::get< AnnounceMessage >( m );

            ptr = read( ptr, end, announce.chunkSize );
            ptr = read( ptr, end, announce.numOfChunks );
            ptr = read( ptr, end, announce.fwTotalSize );

            break;
        }
        case MessageType::REQUEST: {
            m.emplace< 1 >( seq, fwType, fwVersion, 0, chunkId );

            auto& request = std::get< RequestMessage >( m );

            ptr = read( ptr, end, request.numOfChunks );
            break;
        }
        case MessageType::DATA: {
            m.emplace< 2 >( seq, fwType, fwVersion, 0, chunkId, 0 );

            auto& data = std::get< DataMessage >( m );

            ptr = read( ptr, end, data.numOfChunks );
            ptr = read( ptr, end, data.chunkSize );

            std::span a( data.data );
            auto x = a.subspan( 0, data.chunkSize );

            ptr = read( ptr, end, x );

            break;
        }
        default:
            throw std::runtime_error( "Unknown message messageType" );
    }

    return m;
}
}
