/*
** Author(s):
**  - Cedric GESTES <gestes@aldebaran-robotics.com>
**
** Copyright (C) 2012 Aldebaran Robotics
*/

#pragma once
#ifndef _QIMESSAGING_SERVICE_INFO_HPP_
#define _QIMESSAGING_SERVICE_INFO_HPP_

#include <qimessaging/api.hpp>
#include <string>
#include <qimessaging/datastream.hpp>

namespace qi
{

  class QIMESSAGING_API ServiceInfo {
  public:
    ServiceInfo();

    inline void setName(const std::string &name)                { _name = name; }
    inline void setServiceId(unsigned int serviceId)            { _serviceId = serviceId; }
    inline void setMachineId(const std::string &machineId)      { _machineId = machineId; }
    inline void setProcessId(unsigned int processId)            { _processId = processId; }
    inline void setEndpoints(const std::vector<std::string> &v) { _endpoints = v; }
    inline void addEndpoint(const std::string &ep)              { _endpoints.push_back(ep); }

    inline const std::string              &name() const        { return _name; }
    inline unsigned int                    serviceId() const   { return _serviceId; }
    inline const std::string              &machineId() const   { return _machineId; }
    inline unsigned int                    processId() const   { return _processId; }
    inline const std::vector<std::string> &endpoints() const   { return _endpoints; }

    friend QIMESSAGING_API qi::IDataStream &operator>>(qi::IDataStream &stream, ServiceInfo &sinfo);

  protected:
    std::string               _name;
    unsigned int              _serviceId;
    std::string               _machineId;
    unsigned int              _processId;
    std::vector<std::string>  _endpoints;
    void                     *_reserved;
  };

  QIMESSAGING_API qi::ODataStream &operator<<(qi::ODataStream &stream, const ServiceInfo &sinfo);
  QIMESSAGING_API qi::IDataStream &operator>>(qi::IDataStream &stream, ServiceInfo &sinfo);

}; // !qi

#endif  // _QIMESSAGING_SERVICE_INFO_HPP_
