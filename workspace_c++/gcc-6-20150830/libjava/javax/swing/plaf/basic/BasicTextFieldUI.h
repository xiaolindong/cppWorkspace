
// DO NOT EDIT THIS FILE - it is machine generated -*- c++ -*-

#ifndef __javax_swing_plaf_basic_BasicTextFieldUI__
#define __javax_swing_plaf_basic_BasicTextFieldUI__

#pragma interface

#include <javax/swing/plaf/basic/BasicTextUI.h>
extern "Java"
{
  namespace java
  {
    namespace beans
    {
        class PropertyChangeEvent;
    }
  }
  namespace javax
  {
    namespace swing
    {
        class JComponent;
      namespace plaf
      {
          class ComponentUI;
        namespace basic
        {
            class BasicTextFieldUI;
        }
      }
      namespace text
      {
          class Element;
          class View;
      }
    }
  }
}

class javax::swing::plaf::basic::BasicTextFieldUI : public ::javax::swing::plaf::basic::BasicTextUI
{

public:
  BasicTextFieldUI();
  virtual ::javax::swing::text::View * create(::javax::swing::text::Element *);
  static ::javax::swing::plaf::ComponentUI * createUI(::javax::swing::JComponent *);
public: // actually protected
  virtual ::java::lang::String * getPropertyPrefix();
public:
  virtual void installUI(::javax::swing::JComponent *);
public: // actually protected
  virtual void propertyChange(::java::beans::PropertyChangeEvent *);
public:
  static ::java::lang::Class class$;
};

#endif // __javax_swing_plaf_basic_BasicTextFieldUI__
