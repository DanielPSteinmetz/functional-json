#include "data.hpp"
#include <sstream>
#include <string>
#include <iomanip>

template<class... Ts> struct overload : Ts... { using Ts::operator()...; };
template<class... Ts> overload(Ts...) -> overload<Ts...>;

namespace json
{
  std::string Data::type() const
  {
    return std::visit(
      overload {
        [](Null) { return "Null"; },
        [](Boolean) { return "Boolean"; },
        [](Number) { return "Number"; },
        [](String) { return "String"; },
        [](Array) { return "Array"; },
        [](Object) { return "Object"; }
      },
      val
    );
  }

  // Assignment
  Data &Data::operator=(Null const &nullVal)
  {
    val = nullVal;
    return *this;
  }
  Data &Data::operator=(Number const &numVal)
  {
    val = numVal;
    return *this;
  }
  Data &Data::operator=(String const &strVal)
  {
    val = strVal;
    return *this;
  }
  Data &Data::operator=(char const *strVal)
  {
    val = (String)strVal;
    return *this;
  }
  Data &Data::operator=(Array const &arrayVal)
  {
    val = arrayVal;
    return *this;
  }
  Data &Data::operator=(Object const &objVal)
  {
    val = objVal;
    return *this;
  }

  // Printing
  class Printer
  {
    std::ostream &out;
    int indentSize = 0;
    int indentLevel = 0;

  public:
    Printer(std::ostream &out, int indentSize = 0)
        : out(out), indentSize(indentSize) {}

    void operator()(Null) { out << "null"; }
    void operator()(Boolean b) { out << (b ? "true" : "false"); }
    void operator()(Number n) { out << n; }

    void operator()(String str)
    {
      out << '"';
      for (char c : str) {
        switch (c) {
          case '"':  out << "\\\""; break;
          case '\\': out << "\\\\"; break;
          case '\n': out << "\\n";  break;
          case '\r': out << "\\r";  break;
          case '\t': out << "\\t";  break;
          // decided not to worry about utf chars
          default: out << c;
        }
      }
      out << '"';
    }

    void operator()(Array arr)
    {
      out << '[';
      indentLevel++;

      bool first = true;
      for (auto const &item : arr)
      {
        // comma
        if (first)
          first = false;
        else
          out << ',';

        // indent
        if (indentSize > 0)
        {
          out << '\n'
              << std::string(indentSize * indentLevel, ' ');
        }

        // item
        std::visit(*this, item.variant());
      }
      indentLevel--;
      if (indentSize > 0)
      {
        out << '\n'
            << std::string(indentSize * indentLevel, ' ');
      }
      out << ']';
    }

    void operator()(Object obj)
    {
      out << '{';
      indentLevel++;

      bool first = true;
      for (auto const &[key, value] : obj)
      {
        // comma
        if (first)
          first = false;
        else
          out << ',';

        // indent
        if (indentSize > 0)
        {
          out << '\n'
              << std::string(indentSize * indentLevel, ' ');
        }

        // key
        out << std::quoted(key) << ':';

        // key/value separation
        if (indentSize > 0)
          out << ' ';

        // value
        std::visit(*this, value.variant());
      }
      indentLevel--;
      if (indentSize > 0)
      {
        out << '\n'
            << std::string(indentSize * indentLevel, ' ');
      }
      out << '}';
    }
  };

  std::ostream &operator<<(std::ostream &out, Data const &data)
  {
    Printer printer(out);
    std::visit(printer, data.variant());
    return out;
  }

  std::string format(Data data, int indentSize)
  {
    std::ostringstream sout;
    Printer printer(sout, indentSize);
    std::visit(printer, data.variant());
    return sout.str();
  }

  std::string type(Data data) {
    return std::visit(
      overload {
        [](Null) { return "Null"; },
        [](Boolean) { return "Boolean"; },
        [](Number) { return "Number"; },
        [](String) { return "String"; },
        [](Array) { return "Array"; },
        [](Object) { return "Object"; }
      },
      data.variant()
    );
  }
}