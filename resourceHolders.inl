template <typename Resource, typename ID>
void ResourceHolder<Resource, ID>::load(ID id, const std::string& filename)
{
    std::unique_ptr<Resource> p { new Resource() };
    if ( !p->loadFromFile(resourcePath() + filename) )
        throw std::runtime_error("Couldn't load resource: " + filename);

    insertResource(id, std::move(p));
}


template <typename Resource, typename ID>
const Resource& ResourceHolder<Resource, ID>::get(ID id) const
{
    auto found = mMap.find(id);
    return *found->second;
}

template <typename Resource, typename ID>
Resource& ResourceHolder<Resource, ID>::get(ID id)
{
    auto found = mMap.find(id);
    return *found->second;
}

template <typename Resource, typename ID>
void ResourceHolder<Resource, ID>::insertResource(ID id, std::unique_ptr<Resource> res)
{
    auto inserted = mMap.insert(std::make_pair(id, std::move(res)));
    assert(inserted.second);
}