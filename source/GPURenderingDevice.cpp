// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/GPURenderingDevice.hpp>

namespace GT
{
    namespace GE
    {
        GPURenderingDevice::GPURenderingDevice(const GPURenderingDeviceInfo &info)
            : m_info(info)
        {
        }

        GPURenderingDevice::~GPURenderingDevice()
        {
        }


        void GPURenderingDevice::GetInfo(GPURenderingDeviceInfo &infoOut) const
        {
            infoOut = m_info;
        }
    }
}