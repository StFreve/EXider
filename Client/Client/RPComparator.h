#pragma once
#include "EXider.h"
namespace EXider {
    struct RPComparator {
        bool operator()( const boost::shared_ptr<RemotePC>& rhrp, const boost::shared_ptr<RemotePC>& lhrp ) const {
            return *rhrp < *lhrp;
        }
    };
}