#ifndef __shmup_project__resourceHolder__
#define __shmup_project__resourceHolder__

#include "ResourcePath.hpp"
#include <map>
#include <iostream>
#include <cassert>
#include <memory>

template <typename Resource, typename ID>
class ResourceHolder {
public:
    void                load(ID, const std::string&);
    const Resource&     get(ID) const;
    Resource&           get(ID);
    void                insertResource(ID, std::unique_ptr<Resource>);

private:
    std::map<ID, std::unique_ptr<Resource>> mMap;
};

#include "resourceHolders.inl"

#endif /* defined(__shmup_project__resourceHolder__) */
