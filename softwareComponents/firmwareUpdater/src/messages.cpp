#include "messages.hpp"

namespace rofi::updater::messages {
const FWType& _firmwareType( const Message& message ) {
    return *std::visit(
            []( const auto& m ) { return &m.fwType; },
            message
    );

}

const FWVersion& _firmwareVersion( const Message& message ) {
    return *std::visit(
            []( const auto& m ) { return &m.fwVersion; },
            message
    );
}

const ChunkId& _chunkId( const Message& message ) {
    return *std::visit(
            []( const auto& m ) { return &m.chunkId; },
            message
    );
}

const Seq& _seq( const Message& message ) {
    return *std::visit(
            []( const auto& m ) { return &m.seq; },
            message
    );
}

}
