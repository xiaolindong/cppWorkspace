
// DO NOT EDIT THIS FILE - it is machine generated -*- c++ -*-

#ifndef __java_io_PipedInputStream__
#define __java_io_PipedInputStream__

#pragma interface

#include <java/io/InputStream.h>
#include <gcj/array.h>


class java::io::PipedInputStream : public ::java::io::InputStream
{

public:
  PipedInputStream();
  PipedInputStream(jint);
  PipedInputStream(::java::io::PipedOutputStream *);
  PipedInputStream(::java::io::PipedOutputStream *, jint);
  virtual void connect(::java::io::PipedOutputStream *);
public: // actually protected
  virtual void receive(jint);
public: // actually package-private
  virtual void receive(JArray< jbyte > *, jint, jint);
public:
  virtual jint read();
  virtual jint read(JArray< jbyte > *, jint, jint);
  virtual jint available();
  virtual void close();
public: // actually package-private
  ::java::io::PipedOutputStream * __attribute__((aligned(__alignof__( ::java::io::InputStream)))) source;
  jboolean closed;
public: // actually protected
  static const jint PIPE_SIZE = 1024;
  JArray< jbyte > * buffer;
  jint in;
  jint out;
private:
  JArray< jbyte > * read_buf;
public:
  static ::java::lang::Class class$;
};

#endif // __java_io_PipedInputStream__