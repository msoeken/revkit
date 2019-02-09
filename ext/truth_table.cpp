#include <pybind11/pybind11.h>

#include <kitty/constructors.hpp>
#include <kitty/print.hpp>

#include "types.hpp"

namespace py = pybind11;

namespace revkit
{

void truth_table( py::module m )
{
  using namespace py::literals;

  py::class_<truth_table_t> _truth_table( m, "truth_table", "Truth table" );

  _truth_table.def( "__str__", &kitty::to_binary<truth_table_t>, "Prints truth table in binary representation" );

  _truth_table.def_static( "from_hex", []( std::string const& hex ) { 
    const auto num_vars = static_cast<uint32_t>( std::log2( hex.size() << 2 ) );
    truth_table_t tt( num_vars );
    kitty::create_from_hex_string( tt, hex );
    return tt;
  }, R"doc(
    Creates truth table from hex string

    :param str hex: Truth table in hexadecimal representation
    :rtype: truth_table
)doc", "hex"_a );

  _truth_table.def_static( "from_binary", []( std::string const& bin ) { 
    const auto num_vars = static_cast<uint32_t>( std::log2( bin.size() ) );
    truth_table_t tt( num_vars );
    kitty::create_from_binary_string( tt, bin );
    return tt;
  }, R"doc(
    Creates truth table from binary string

    :param str bin: Truth table in binary representation
    :rtype: truth_table
)doc", "bin"_a );

  _truth_table.def_static( "from_expression", []( std::string const& expression ) { 
    uint32_t num_vars{0u};
    for ( auto c : expression )
    {
      if ( c < 'a' || c > 'p' ) continue;
      num_vars = std::max<uint32_t>( num_vars, c - 'a' + 1 );
    }
    truth_table_t tt( num_vars );
    kitty::create_from_expression( tt, expression );
    return tt;
  }, R"doc(
    Creates truth table from expression

    :param str bin: Expression
    :rtype: truth_table

    .. seealso:: `kitty documentation for create_from_expression <https://libkitty.readthedocs.io/en/latest/reference.html#_CPPv3N5kitty22create_from_expressionER2TTRKNSt6stringE>`_
)doc", "expression"_a );

}

}
