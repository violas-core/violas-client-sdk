#pragma once

namespace nft
{
    using bytes = std::vector<uint8_t> ;

    class portrait
    {
    private:
        /* data */
        bytes description;
        bytes ipfs_cid;

    public:
        portrait(/* args */);
        ~portrait();
    };
    
    portrait::portrait(/* args */)
    {
    }
    
    portrait::~portrait()
    {
    }
    
}
