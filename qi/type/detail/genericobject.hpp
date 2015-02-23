#pragma once
/*
**  Copyright (C) 2013 Aldebaran Robotics
**  See COPYING for the license
*/

#ifndef _QI_TYPE_DETAIL_GENERIC_OBJECT_HPP_
#define _QI_TYPE_DETAIL_GENERIC_OBJECT_HPP_

#include <map>
#include <string>

#include <boost/smart_ptr/enable_shared_from_this.hpp>

#include <qi/api.hpp>
#include <qi/type/detail/manageable.hpp>
#include <qi/future.hpp>
#include <qi/signal.hpp>
#include <qi/type/typeobject.hpp>

#ifdef _MSC_VER
#  pragma warning( push )
#  pragma warning( disable: 4251 )
#endif

namespace qi
{

/* ObjectValue
 *  static version wrapping class C: Type<C>
 *  dynamic version: Type<DynamicObject>
 *
 * All the methods are convenience wrappers that bounce to the ObjectTypeInterface,
 * except Event Loop management
 * This class has pointer semantic. Do not use directly, use AnyObject,
 * obtained through Session, DynamicObjectBuilder or ObjectTypeBuilder.
 */
class QI_API GenericObject
  : public Manageable
  , public boost::enable_shared_from_this<GenericObject>
{
public:
  GenericObject(ObjectTypeInterface *type, void *value);
  ~GenericObject();
  const MetaObject &metaObject();

#ifdef DOXYGEN
  // Help doxygen and the header reader a bit.
  template<typename R>
  R call(
         const std::string& eventName,
         qi::AutoAnyReference p1 = qi::AutoAnyReference(),
         qi::AutoAnyReference p2 = qi::AutoAnyReference(),
         qi::AutoAnyReference p3 = qi::AutoAnyReference(),
         qi::AutoAnyReference p4 = qi::AutoAnyReference(),
         qi::AutoAnyReference p5 = qi::AutoAnyReference(),
         qi::AutoAnyReference p6 = qi::AutoAnyReference(),
         qi::AutoAnyReference p7 = qi::AutoAnyReference(),
         qi::AutoAnyReference p8 = qi::AutoAnyReference());

  template<typename R>
  qi::Future<R> async(
                      const std::string& eventName,
                      qi::AutoAnyReference p1 = qi::AutoAnyReference(),
                      qi::AutoAnyReference p2 = qi::AutoAnyReference(),
                      qi::AutoAnyReference p3 = qi::AutoAnyReference(),
                      qi::AutoAnyReference p4 = qi::AutoAnyReference(),
                      qi::AutoAnyReference p5 = qi::AutoAnyReference(),
                      qi::AutoAnyReference p6 = qi::AutoAnyReference(),
                      qi::AutoAnyReference p7 = qi::AutoAnyReference(),
                      qi::AutoAnyReference p8 = qi::AutoAnyReference());
#else
  // Declare genCall, using overloads for all argument count instead of default values.
  #define genCall(n, ATYPEDECL, ATYPES, ADECL, AUSE, comma) \
  template<typename R> R call(                              \
    const std::string& methodName comma                     \
    QI_GEN_ARGSDECLSAMETYPE(n, qi::AutoAnyReference));
  QI_GEN(genCall)
  #undef genCall

  #define genCall(n, ATYPEDECL, ATYPES, ADECL, AUSE, comma) \
  template<typename R> qi::Future<R> async(                 \
    const std::string& methodName comma                     \
    QI_GEN_ARGSDECLSAMETYPE(n, qi::AutoAnyReference));
  QI_GEN(genCall)
  #undef genCall
#endif // DOXYGEN

  qi::Future<AnyReference> metaCall(unsigned int method, const GenericFunctionParameters& params, MetaCallType callType = MetaCallType_Auto, Signature returnSignature = Signature());

  /** Find method named name callable with arguments parameters
   */
  int findMethod(const std::string& name, const GenericFunctionParameters& parameters);

  /** Resolve the method Id and bounces to metaCall
   * @param nameWithOptionalSignature method name or method signature
   * 'name::(args)' if signature is given, an exact match is required
   * @param params arguments to the call
   * @param callType type of the call
   * @param returnSignature force the method to return a type
   */
  qi::Future<AnyReference> metaCall(const std::string &nameWithOptionalSignature, const GenericFunctionParameters& params, MetaCallType callType = MetaCallType_Auto, Signature returnSignature = Signature());

  void post(const std::string& eventName,
            qi::AutoAnyReference p1 = qi::AutoAnyReference(),
            qi::AutoAnyReference p2 = qi::AutoAnyReference(),
            qi::AutoAnyReference p3 = qi::AutoAnyReference(),
            qi::AutoAnyReference p4 = qi::AutoAnyReference(),
            qi::AutoAnyReference p5 = qi::AutoAnyReference(),
            qi::AutoAnyReference p6 = qi::AutoAnyReference(),
            qi::AutoAnyReference p7 = qi::AutoAnyReference(),
            qi::AutoAnyReference p8 = qi::AutoAnyReference());

  void metaPost(unsigned int event, const GenericFunctionParameters& params);
  void metaPost(const std::string &nameWithOptionalSignature, const GenericFunctionParameters &in);

  /** Connect an event to an arbitrary callback.
   *
   * If you are within a service, it is recommended that you connect the
   * event to one of your Slots instead of using this method.
   */
  template <typename FUNCTOR_TYPE>
  qi::FutureSync<SignalLink> connect(const std::string& eventName, FUNCTOR_TYPE callback,
                       MetaCallType threadingModel = MetaCallType_Direct);


  qi::FutureSync<SignalLink> connect(const std::string &name, const SignalSubscriber& functor);

  /// Calls given functor when event is fired. Takes ownership of functor.
  qi::FutureSync<SignalLink> connect(unsigned int signal, const SignalSubscriber& subscriber);

  /** Connect an event to a method.
   * Recommended use is when target is not a proxy.
   * If target is a proxy and this is server-side, the event will be
   *    registered localy and the call will be forwarded.
   * If target and this are proxies, the message will be routed through
   * the current process.
   */
  qi::FutureSync<SignalLink> connect(unsigned int signal, AnyObject target, unsigned int slot);

  /// Disconnect an event link. Returns if disconnection was successful.
  qi::FutureSync<void> disconnect(SignalLink linkId);

  template<typename T>
  qi::FutureSync<T> property(const std::string& name);

  template<typename T>
  qi::FutureSync<void> setProperty(const std::string& name, const T& val);

  //Low Level Properties
  qi::FutureSync<AnyValue> property(unsigned int id);
  qi::FutureSync<void> setProperty(unsigned int id, const AnyValue &val);


  bool isValid() { return type && value;}
  ObjectTypeInterface*  type;
  void*        value;
};

// C4251
template <typename FUNCTION_TYPE>
qi::FutureSync<SignalLink> GenericObject::connect(const std::string& eventName,
                                                    FUNCTION_TYPE callback,
                                                    MetaCallType model)
{
  return connect(eventName,
    SignalSubscriber(AnyFunction::from(callback), model));
}

namespace detail
{
  template<typename T> void hold(T data) {}

  template<typename T> void setPromise(qi::Promise<T>& promise, AnyValue& v)
  {
    try
    {
      qiLogDebugC("qi.adapter") << "converting value";
      T val = v.to<T>();
      qiLogDebugC("qi.adapter") << "setting promise";
      promise.setValue(val);
      qiLogDebugC("qi.adapter") << "done";
    }
    catch(const std::exception& e)
    {
      qiLogErrorC("qi.adapter") << e.what();
      promise.setError(e.what());
    }
  }

  template<> inline void setPromise(qi::Promise<void>& promise, AnyValue&)
  {
    promise.setValue(0);
  }

  template <typename T>
  void futureAdapterGeneric(AnyReference val, qi::Promise<T> promise)
  {
    qiLogDebugC("qi.adapter") << "futureAdapter trigger";
    TemplateTypeInterface* ft1 = QI_TEMPLATE_TYPE_GET(val.type(), Future);
    TemplateTypeInterface* ft2 = QI_TEMPLATE_TYPE_GET(val.type(), FutureSync);
    TemplateTypeInterface* futureType = ft1 ? ft1 : ft2;
    ObjectTypeInterface* onext = dynamic_cast<ObjectTypeInterface*>(futureType->next());
    GenericObject gfut(onext, val.rawValue());
    // Need a live shared_ptr for shared_from_this() to work.
    boost::shared_ptr<GenericObject> ao(&gfut, hold<GenericObject*>);
    if (gfut.call<bool>("hasError", 0))
    {
      qiLogDebugC("qi.adapter") << "futureAdapter: future in error";
      std::string s = gfut.call<std::string>("error", 0);
      qiLogDebugC("qi.adapter") << "futureAdapter: got error: " << s;
      promise.setError(s);
      return;
    }
    qiLogDebugC("qi.adapter") << "futureAdapter: future has value";
    AnyValue v = gfut.call<AnyValue>("value", 0);
    // For a Future<void>, value() gave us a void*
    if (futureType->templateArgument()->kind() == TypeKind_Void)
      v = AnyValue(qi::typeOf<void>());
    qiLogDebugC("qi.adapter") << v.type()->infoString();
    setPromise(promise, v);
    qiLogDebugC("qi.adapter") << "Promise set";
    val.destroy();
  }

  // futureAdapter helper that detects and handles value of kind future
  // return true if value was a future and was handled
  template <typename T>
  inline bool handleFuture(AnyReference val, Promise<T> promise)
  {
    TemplateTypeInterface* ft1 = QI_TEMPLATE_TYPE_GET(val.type(), Future);
    TemplateTypeInterface* ft2 = QI_TEMPLATE_TYPE_GET(val.type(), FutureSync);
    TemplateTypeInterface* futureType = ft1 ? ft1 : ft2;
    qiLogDebugC("qi.object") << "isFuture " << val.type()->infoString() << ' ' << !!ft1 << ' ' << !!ft2;
    if (!futureType)
      return false;

    TypeInterface* next = futureType->next();
    ObjectTypeInterface* onext = dynamic_cast<ObjectTypeInterface*>(next);
    GenericObject gfut(onext, val.rawValue());
    // Need a live shared_ptr for shared_from_this() to work.
    boost::shared_ptr<GenericObject> ao(&gfut, &hold<GenericObject*>);
    boost::function<void()> cb = boost::bind(futureAdapterGeneric<T>, val, promise);
    // Careful, gfut will die at the end of this block, but it is
    // stored in call data. So call must finish before we exit this block,
    // and thus must be synchronous.
    try
    {
      gfut.call<void>("_connect", cb);
    }
    catch (std::exception& e)
    {
      qiLogWarningC("qi.object") << "future connect error " << e.what();
    }
    return true;
  }

  struct AutoRefDestroy
  {
    AnyReference val;
    AutoRefDestroy(AnyReference ref) : val(ref) {}
    ~AutoRefDestroy()
    {
      val.destroy();
    }
  };

  template <typename T>
  inline T extractFuture(qi::Future<qi::AnyReference> metaFut)
  {
    AnyReference val =  metaFut.value();
    AutoRefDestroy destroy(val);

    static TypeInterface* targetType;
    QI_ONCE(targetType = typeOf<T>());
    try
    {
      std::pair<AnyReference, bool> conv = val.convert(targetType);
      if (!conv.first.type())
        throw std::runtime_error(std::string("Unable to convert call result to target type: from ")
          + val.signature(true).toPrettySignature() + " to " + targetType->signature().toPrettySignature());
      else
      {
        if (conv.second)
        {
          AutoRefDestroy destroy(conv.first);
          return *conv.first.ptr<T>(false);
        }
        else
          return *conv.first.ptr<T>(false);
      }
    }
    catch(const std::exception& e)
    {
      throw std::runtime_error(std::string("Return argument conversion error: ") + e.what());
    }
  }

  template <>
  inline void extractFuture<void>(qi::Future<qi::AnyReference> metaFut)
  {
    AnyReference val = metaFut.value();
    val.destroy();
  }

  template <typename T>
  inline void futureAdapter(qi::Future<qi::AnyReference> metaFut, qi::Promise<T> promise)
  {
    qiLogDebugC("qi.object") << "futureAdapter " << qi::typeOf<T>()->infoString()<< ' ' << metaFut.hasError();
    //error handling
    if (metaFut.hasError()) {
      promise.setError(metaFut.error());
      return;
    }

    AnyReference val =  metaFut.value();
    if (handleFuture(val, promise))
      return;

    static TypeInterface* targetType;
    QI_ONCE(targetType = typeOf<T>());
    try
    {
      std::pair<AnyReference, bool> conv = val.convert(targetType);
      if (!conv.first.type())
        promise.setError(std::string("Unable to convert call result to target type: from ")
          + val.signature(true).toPrettySignature() + " to " + targetType->signature().toPrettySignature() );
      else
      {
        promise.setValue(*conv.first.ptr<T>(false));
      }
      if (conv.second)
        conv.first.destroy();
    }
    catch(const std::exception& e)
    {
      promise.setError(std::string("Return argument conversion error: ") + e.what());
    }
    val.destroy();
  }

  template <>
  inline void futureAdapter<void>(qi::Future<qi::AnyReference> metaFut, qi::Promise<void> promise)
  {
    //error handling
    if (metaFut.hasError()) {
      promise.setError(metaFut.error());
      return;
    }
    AnyReference val =  metaFut.value();
    if (handleFuture(val, promise))
      return;

    promise.setValue(0);
    val.destroy();
  }

  template <typename T>
  inline void futureAdapterVal(qi::Future<qi::AnyValue> metaFut, qi::Promise<T> promise)
  {
    //error handling
    if (metaFut.hasError()) {
      promise.setError(metaFut.error());
      return;
    }
    const AnyValue& val =  metaFut.value();
    try
    {
      promise.setValue(val.to<T>());
    }
    catch (const std::exception& e)
    {
      promise.setError(std::string("Return argument conversion error: ") + e.what());
    }
  }

  template<>
  inline void futureAdapterVal(qi::Future<qi::AnyValue> metaFut, qi::Promise<AnyValue> promise)
  {
    if (metaFut.hasError())
      promise.setError(metaFut.error());
    else
      promise.setValue(metaFut.value());
  }
}


/* Generate R GenericObject::call(methodname, args...)
 * for all argument counts
 * The function packs arguments in a vector<AnyReference>, computes the
 * signature and bounce those to metaCall.
 */

#define pushi(z, n,_) params.push_back(p ## n);

#define genCall(n, ATYPEDECL, ATYPES, ADECL, AUSE, comma)         \
  template<typename R> R GenericObject::call(                     \
      const std::string& methodName       comma                   \
      QI_GEN_ARGSDECLSAMETYPE(n, qi::AutoAnyReference))           \
  {                                                               \
    if (!value || !type) {                                        \
      throw std::runtime_error("Invalid GenericObject");          \
    }                                                             \
    std::vector<qi::AnyReference> params;                         \
    params.reserve(n);                                            \
    BOOST_PP_REPEAT(n, pushi, _)                                  \
    qi::Future<AnyReference> fmeta = metaCall(methodName, params, \
        MetaCallType_Direct, typeOf<R>()->signature());           \
    return detail::extractFuture<R>(fmeta);                       \
  }
QI_GEN(genCall)
#undef genCall

#define genCall(n, ATYPEDECL, ATYPES, ADECL, AUSE, comma)                \
  template<typename R> qi::Future<R> GenericObject::async(               \
      const std::string& methodName       comma                          \
      QI_GEN_ARGSDECLSAMETYPE(n, qi::AutoAnyReference))                  \
  {                                                                      \
    if (!value || !type) {                                               \
      return makeFutureError<R>("Invalid GenericObject");                \
    }                                                                    \
    std::vector<qi::AnyReference> params;                                \
    params.reserve(n);                                                   \
    BOOST_PP_REPEAT(n, pushi, _)                                         \
    qi::Promise<R> res;                                                  \
    qi::Future<AnyReference> fmeta = metaCall(methodName, params,        \
        MetaCallType_Queued, typeOf<R>()->signature());                  \
    fmeta.connect(boost::bind<void>(&detail::futureAdapter<R>, _1, res), \
        FutureCallbackType_Sync);                                        \
    return res.future();                                                 \
  }
QI_GEN(genCall)
#undef genCall

#undef pushi

template<typename T>
qi::FutureSync<T> GenericObject::property(const std::string& name)
{
  int pid = metaObject().propertyId(name);
  if (pid < 0)
    return makeFutureError<T>("Property not found");
  qi::Future<AnyValue> f = type->property(value, pid);
  qi::Promise<T> p;
  f.connect(boost::bind(&detail::futureAdapterVal<T>,_1, p),
      FutureCallbackType_Sync);
  return p.future();
}

template<typename T>
qi::FutureSync<void> GenericObject::setProperty(const std::string& name, const T& val)
{
  int pid = metaObject().propertyId(name);
  if (pid < 0)
    return makeFutureError<void>("Property not found");
  return type->setProperty(value, pid, AnyValue::from(val));
}

/* An AnyObject is actually of a Dynamic type: The underlying TypeInterface*
 * is not allways the same.
 * Override backend shared_ptr<GenericObject>
*/
template<>
class QI_API TypeImpl<boost::shared_ptr<GenericObject> > :
  public DynamicTypeInterface
{
public:
  virtual AnyReference get(void* storage)
  {
    detail::ManagedObjectPtr* val = (detail::ManagedObjectPtr*)ptrFromStorage(&storage);
    AnyReference result;
    if (!*val)
    {
      return AnyReference();
    }
    return AnyReference((*val)->type, (*val)->value);
  }

  virtual void set(void** storage, AnyReference source)
  {
    qiLogCategory("qitype.object");
    detail::ManagedObjectPtr* val = (detail::ManagedObjectPtr*)ptrFromStorage(storage);
    TemplateTypeInterface* templ = dynamic_cast<TemplateTypeInterface*>(source.type());
    if (templ)
      source = AnyReference(templ->next(), source.rawValue());
    if (source.type()->info() == info())
    { // source is objectptr
      detail::ManagedObjectPtr* src = source.ptr<detail::ManagedObjectPtr>(false);
      if (!*src)
        qiLogWarning() << "NULL Object";
      *val = *src;
    }
    else if (source.kind() == TypeKind_Dynamic)
    { // try to dereference dynamic type in case it contains an object
      set(storage, source.content());
    }
    else if (source.kind() == TypeKind_Object)
    { // wrap object in objectptr: we do not keep it alive,
      // but source type offers no tracking capability
      detail::ManagedObjectPtr op(new GenericObject(static_cast<ObjectTypeInterface*>(source.type()), source.rawValue()));
      *val = op;
    }
    else if (source.kind() == TypeKind_Pointer)
    {
      PointerTypeInterface* ptype = static_cast<PointerTypeInterface*>(source.type());
      // FIXME: find a way!
      if (ptype->pointerKind() == PointerTypeInterface::Shared)
        qiLogInfo() << "Object will *not* track original shared pointer";
      set(storage, *source);
    }
    else
      throw std::runtime_error((std::string)"Cannot assign non-object " + source.type()->infoString() + " to Object");
  }

  typedef DefaultTypeImplMethods<detail::ManagedObjectPtr, TypeByPointerPOD<detail::ManagedObjectPtr> > Methods;
  _QI_BOUNCE_TYPE_METHODS(Methods);
};

}

#ifdef _MSC_VER
#  pragma warning( pop )
#endif

#endif
