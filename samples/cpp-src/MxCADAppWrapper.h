#pragma once

#include <memory>

class MxCADAppWrapper
{
public:
    MxCADAppWrapper();
    ~MxCADAppWrapper();

    bool Free();

private:
    struct Private;
    std::unique_ptr<Private> m_p;
};

