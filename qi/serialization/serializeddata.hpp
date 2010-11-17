/*
** Author(s):
**  - Cedric GESTES <gestes@aldebaran-robotics.com>
**
** Copyright (C) 2010 Aldebaran Robotics
*/

#ifndef     QI_SERIALIZATION_SERIALIZE_HPP_
# define     QI_SERIALIZATION_SERIALIZE_HPP_

#include <iostream>
#include <qi/signature.hpp>

namespace qi {

  namespace serialization {

    class SerializedData {
    public:
      SerializedData() {}
      SerializedData(const std::string &data) : fData(data) {}

      template<typename T>
      void read(T& t)const
      {
        std::string sig = qi::signature<T>::value();

        std::cout << "read(" << fData << "):" << sig << std::endl;
      }

      template<typename T>
      void write(const T& t)
      {
        std::string sig = qi::signature<T>::value();
        std::cout << "write(" << fData << "):" << sig << std::endl;
      }

      std::string str()const {
        return fData;
      }

      void str(const std::string &str) {
        fData = str;
      }

    protected:
      std::string fData;
      //POD Types
      // void serialize(const int &t);
      // void serialize(const float &t);
      // void serialize(const double &t);
      // void serialize(const bool &t);

      // //STLs
      // void serialize(const std::string &t);

      // template <typename U>
      // void serialize(const std::vector<U> &t);

      // template <typename U>
      // void serialize(const std::list<U> &t);
    };

  }
}

//#include <qi/serialization/thrift/serialize.hxx>

#endif      /* !AL_SERIALIZATION_THRIFT_HPP_ */
