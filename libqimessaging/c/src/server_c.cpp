/*
**
** Author(s):
**  - Cedric GESTES <gestes@aldebaran-robotics.com>
**
** Copyright (C) 2010, 2011 Aldebaran Robotics
*/

#include <qimessaging/c/qi_c.h>
#include <qimessaging/functor.hpp>

#include <cstring>
#include <cstdlib>

qi_server_t *qi_server_create(const char *QI_UNUSED(name)) {
//  qi::Server *pserver = new qi::ServerImpl(name);
//  return reinterpret_cast<qi_server_t *>(pserver);
  return NULL;
}

void         qi_server_destroy(qi_server_t *QI_UNUSED(server)) {
//  qi::detail::ServerImpl  *pserver  = reinterpret_cast<qi::detail::ServerImpl *>(server);
//  delete pserver;
}

void         qi_server_connect(qi_server_t *QI_UNUSED(server), const char *QI_UNUSED(address)) {
//  qi::detail::ServerImpl  *pserver  = reinterpret_cast<qi::detail::ServerImpl *>(server);
//  pserver->connect(address);
}


class CFunctor : public qi::Functor {
public:
  CFunctor(const char *complete_sig, BoundMethod func, void *data = 0)
    : _func(func),
      _complete_sig(strdup(complete_sig)),
      _data(data)
  {
    ;
  }

  virtual void call(qi::FunctorParameters &params, qi::FunctorResult& result)const {
    if (_func)
      _func(_complete_sig, reinterpret_cast<qi_message_t *>(&params), reinterpret_cast<qi_message_t *>(&result), _data);
  }

  virtual ~CFunctor() {
    free(_complete_sig);
  }

private:
  BoundMethod   _func;
  char         *_complete_sig;
  void         *_data;

};


void         qi_server_advertise_service(qi_server_t *QI_UNUSED(server), const char *QI_UNUSED(methodSignature), BoundMethod QI_UNUSED(func), void *QI_UNUSED(data)) {
//  qi::detail::ServerImpl  *pserver  = reinterpret_cast<qi::detail::ServerImpl *>(server);
//  CFunctor *fun = new CFunctor(methodSignature, func, data);
//  pserver->advertiseService(methodSignature, fun);
}

void         qi_server_unadvertise_service(qi_server_t *QI_UNUSED(server), const char *QI_UNUSED(methodSignature)) {
//  qi::detail::ServerImpl  *pserver  = reinterpret_cast<qi::detail::ServerImpl *>(server);
//  pserver->unadvertiseService(methodSignature);
}
