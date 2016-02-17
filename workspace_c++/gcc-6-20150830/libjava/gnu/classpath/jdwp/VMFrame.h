
// DO NOT EDIT THIS FILE - it is machine generated -*- c++ -*-

#ifndef __gnu_classpath_jdwp_VMFrame__
#define __gnu_classpath_jdwp_VMFrame__

#pragma interface

#include <java/lang/Object.h>
extern "Java"
{
  namespace gnu
  {
    namespace classpath
    {
      namespace jdwp
      {
          class VMFrame;
        namespace util
        {
            class Location;
        }
        namespace value
        {
            class Value;
        }
      }
    }
  }
}

class gnu::classpath::jdwp::VMFrame : public ::java::lang::Object
{

public:
  VMFrame(::java::lang::Thread *, jlong, ::gnu::classpath::jdwp::util::Location *, ::java::lang::Object *);
  virtual ::gnu::classpath::jdwp::util::Location * getLocation();
  virtual ::gnu::classpath::jdwp::value::Value * getValue(jint, jbyte);
  virtual void setValue(jint, ::gnu::classpath::jdwp::value::Value *);
  virtual ::java::lang::Thread * getThread();
  virtual ::java::lang::Object * getObject();
  virtual jlong getId();
  static const jint SIZE = 8;
private:
  ::java::lang::Thread * __attribute__((aligned(__alignof__( ::java::lang::Object)))) thread;
  ::java::lang::Object * obj;
  ::gnu::classpath::jdwp::util::Location * loc;
  jlong id;
public:
  static ::java::lang::Class class$;
};

#endif // __gnu_classpath_jdwp_VMFrame__
