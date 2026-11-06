#pragma once

#include <variant>
#include <string>
#include <vector>
#include <map>
#include <concepts>
#include <ostream>

namespace json
{
  struct Data;

  using Null = std::monostate;
  using Boolean = bool;
  using Number = double;
  using String = std::string;
  using Array = std::vector<Data>;
  using Object = std::map<std::string, Data>;

  inline constexpr Null null{};

  struct Data
  {
  private:
    using DataVariant = std::variant<
        Null,
        Boolean,
        Number,
        String,
        Array,
        Object>;

    DataVariant val;

  public:
    // Getter
    DataVariant &variant() & { return val; }
    DataVariant const &variant() const & { return val; }
    DataVariant &&variant() && { return std::move(val); }

    // Type helper
    std::string type() const;

    // Constructors
    Data() : val(null) {}
    Data(Null nullVal) : val(nullVal) {}
    Data(Number numVal) : val(numVal) {}
    Data(String strVal) : val(strVal) {}
    Data(char const *strVal) : val((String)strVal) {}
    Data(Array arrayVal) : val(arrayVal) {}
    Data(Object objVal) : val(objVal) {}

    template <std::same_as<bool> B>
    Data(B boolVal) : val(boolVal) {}

    // Assignment
    Data &operator=(Null const &nullVal);
    Data &operator=(Number const &numVal);
    Data &operator=(String const &strVal);
    Data &operator=(char const *strVal);
    Data &operator=(Array const &arrayVal);
    Data &operator=(Object const &objVal);

    template <std::same_as<bool> B>
    Data &operator=(B const &boolVal)
    {
      val = boolVal;
      return *this;
    }

    // Copy
    Data(Data const &) = default;
    Data &operator=(Data const &) = default;

    // Move
    Data(Data &&) noexcept = default;
    Data &operator=(Data &&) = default;

    // Comparison
    bool operator==(Data const &) const = default;
    std::partial_ordering operator<=>(Data const &) const = default;
  };

  // printing
  std::ostream &operator<<(std::ostream &out, Data const &data);
  std::string format(Data data, int indentSize = 0);

  // variant helpers
  template<typename T>
  bool is(Data data) {
    return std::holds_alternative<T>(data.variant());
  }

  template<typename T>
  T& as(Data& data) {
    return std::get<T>(data.variant());
  }

  template<typename T>
  T const& as(Data const& data) {
    return std::get<T>(data.variant());
  }

  template<typename T>
  T&& as(Data&& data) {
    return std::get<T>(data.variant());
  }
}