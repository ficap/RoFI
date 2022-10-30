#pragma once

#include <array>    // array
#include <cstddef>  // std::size_t
#include <cstdint>  // fixed size datatypes
#include <variant>  // variant


namespace rofi::updater::messages {
    using FWType = uint8_t;
    using FWVersion = uint16_t;
    using ChunkId = uint16_t;
    using Version = uint16_t;
    using Seq = uint8_t;


    constexpr std::size_t CHUNK_SIZE = 1024;

    enum MessageType : uint8_t {
        ANNOUNCE = 0,
        REQUEST = 1,
        DATA = 2
    };


    struct AnnounceMessage {
        Seq seq;

        FWType fwType;
        FWVersion fwVersion;
        size_t fwTotalSize;  // in bytes

        uint16_t numOfChunks;
        size_t chunkSize;

        ChunkId chunkId;

        AnnounceMessage() = default;

        AnnounceMessage( Seq seq, FWType fwType, FWVersion fwVersion, size_t fwTotalSize, uint16_t numOfChunks,
                         size_t chunkSize, ChunkId chunkId ) 
            : seq( seq ), fwType( fwType ), fwVersion( fwVersion ), fwTotalSize( fwTotalSize )
            , numOfChunks( numOfChunks ), chunkSize( chunkSize ), chunkId( chunkId ) {}
    };

    struct RequestMessage {
        Seq seq;

        FWType fwType;
        FWVersion fwVersion;
        uint16_t numOfChunks;

        ChunkId chunkId;

        RequestMessage() = default;

        RequestMessage( Seq seq, FWType fwType, FWVersion fwVersion, uint16_t numOfChunks, ChunkId chunkId )
            : seq( seq ), fwType( fwType ), fwVersion( fwVersion ), numOfChunks( numOfChunks ), chunkId( chunkId ) {}
    };

    struct DataMessage {
        Seq seq;

        FWType fwType;
        FWVersion fwVersion;
        uint16_t numOfChunks;

        ChunkId chunkId;
        uint32_t chunkSize;  // in bytes
        std::array < uint8_t, CHUNK_SIZE > data;

        DataMessage() = default;

        DataMessage( Seq seq, FWType fwType, FWVersion fwVersion, uint16_t numOfChunks, ChunkId chunkId,
                     uint32_t chunkSize, const std::array< uint8_t, CHUNK_SIZE >& data = {})
            : seq( seq ), fwType( fwType ), fwVersion( fwVersion ), numOfChunks( numOfChunks ), chunkId( chunkId )
            , chunkSize( chunkSize ), data( data ) {}
    };

    using Message = std::variant< AnnounceMessage, RequestMessage, DataMessage >;
    using NUM_MESSAGE_TYPES = std::variant_size_v< Message >;

    const FWType& _firmwareType( const Message& message );

    const FWVersion& _firmwareVersion( const Message& message );

    const ChunkId& _chunkId( const Message& message );

    const Seq& _seq( const Message& message );
}
