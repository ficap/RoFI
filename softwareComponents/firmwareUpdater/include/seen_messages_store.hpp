#pragma once

#include <algorithm>        // std::max
#include <array>
#include <cassert>          // assert
#include <cstddef>          // std::size_t
#include <limits>           // std::numeric_limits
#include <sys/types.h>
#include <unordered_map>
#include <vector>

namespace rofi::updater {
namespace {
template < typename SEQ_TYPE, std::size_t NUM_MESSAGE_TYPES >
using MessageTypesPage = std::array< SEQ_TYPE, NUM_MESSAGE_TYPES >;

template < typename SEQ_TYPE, std::size_t NUM_MESSAGE_TYPES >
using MessagesPage = std::vector< MessageTypesPage< SEQ_TYPE, NUM_MESSAGE_TYPES > >;

template < typename VERSION, typename SEQ_TYPE, std::size_t NUM_MESSAGE_TYPES >
using VersionPage = std::unordered_map< VERSION, MessagesPage< SEQ_TYPE, NUM_MESSAGE_TYPES > >;
}


template< typename FW_TYPE, typename FW_VERSION, typename SEQ_TYPE, std::size_t NUM_MESSAGE_TYPES >
class Store {
    std::unordered_map< FW_TYPE, VersionPage<FW_VERSION, SEQ_TYPE, NUM_MESSAGE_TYPES > > _store{ 2 };
    std::size_t _numberOfChunksHint;

public:
    explicit Store( std::size_t numberOfChunksHint = 1024 ) : _numberOfChunksHint( numberOfChunksHint ) {};

    void markSeen( std::size_t messageType, FW_TYPE fwType, FW_VERSION version, std::size_t chunkId, SEQ_TYPE seq ) {
        assert( messageType < NUM_MESSAGE_TYPES );

        auto [ typePageIt, _x ] = _store.try_emplace( fwType, 2 );
        auto [ versionPageIt, _y ] = typePageIt->second.try_emplace( version, std::max( chunkId + 1, _numberOfChunksHint ) );
        auto& messagesPage = versionPageIt->second;

        if ( messagesPage.size() <= chunkId ) {
            messagesPage.resize( chunkId + 1 );
        }

        messagesPage[ chunkId ][ messageType ] = seq;
    }

    bool seen( std::size_t messageType, FW_TYPE fwType, FW_VERSION version, std::size_t chunkId, SEQ_TYPE seq ) const {
        assert( messageType < NUM_MESSAGE_TYPES );

        auto typeIter = _store.find( fwType );
        if ( typeIter == _store.end() ) {
            return false;
        }

        const auto& versionPage = typeIter->second;
        auto versionIter = versionPage.find( version );
        if ( versionIter == versionPage.end() ) {
            return false;
        }

        const auto& messagesPage = versionIter->second;
        if ( messagesPage.size() <= chunkId ) {
            return false;
        }

        const auto& stored = messagesPage[ chunkId ][ messageType ];
        const SEQ_TYPE quarter = ( std::numeric_limits< SEQ_TYPE >::max() + 1ULL ) / 4ULL;

        if ( stored <= quarter && seq >= 3 * quarter ) {
            return true;
        }
        if ( stored > 3 * quarter && seq < quarter ) {
            return false;
        }

        return seq <= stored;
    }

    SEQ_TYPE lastSeenSeq( std::size_t messageType, FW_TYPE fwType, FW_VERSION version, std::size_t chunkId ) const {
        assert( messageType < NUM_MESSAGE_TYPES );

        auto typeIter = _store.find( fwType );
        if ( typeIter == _store.end() ) {
            return 0;
        }

        const auto& versionPage = typeIter->second;
        auto versionIter = versionPage.find( version );
        if ( versionIter == versionPage.end() ) {
            return 0;
        }

        const auto& messagesPage = versionIter->second;
        if ( messagesPage.size() <= chunkId ) {
            return 0;
        }

        const auto& stored = messagesPage[ chunkId ][ messageType ];

        return stored;
    }
};

}
