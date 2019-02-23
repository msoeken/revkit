#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <kitty/constructors.hpp>
#include <kitty/print.hpp>
#include <kitty/properties.hpp>

#include "types.hpp"

namespace py = pybind11;

namespace revkit
{

void truth_table( py::module m )
{
  using namespace py::literals;

  py::class_<truth_table_t> _truth_table( m, "truth_table", "Truth table" );

  _truth_table.def( "__str__", &kitty::to_binary<truth_table_t>, "Prints truth table in binary representation" );
  _truth_table.def( "__eq__", &kitty::equal<truth_table_t>, "Returns whether two truth tables are equal" );

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

  _truth_table.def_property_readonly( "num_vars", &truth_table_t::num_vars, "Number of variables" );

  _truth_table.def( "is_horn", &kitty::is_horn<truth_table_t>, R"doc(
    Returns whether truth table is Horn

    .. seealso:: `kitty documentation for is_horn <https://libkitty.readthedocs.io/en/latest/reference.html#_CPPv3N5kitty7is_hornERK2TT>`_
)doc" );

  _truth_table.def( "is_krom", &kitty::is_krom<truth_table_t>, R"doc(
    Returns whether truth table is Krom

    .. seealso:: `kitty documentation for is_krom <https://libkitty.readthedocs.io/en/latest/reference.html#_CPPv3N5kitty7is_kromERK2TT>`_
)doc" );

  _truth_table.def( "is_symmetric_in", []( truth_table_t const& ref, uint32_t var1, uint32_t var2 ) {
    return kitty::is_symmetric_in( ref, var1, var2 );
  }, R"doc(
    Returns whether truth table is symmetric in two variables

    :param int var1: First variable (indexed from 0)
    :param int var2: Second variable (indexed from 0)

    .. seealso:: `kitty documentation for is_symmetric_in <https://libkitty.readthedocs.io/en/latest/reference.html#_CPPv3N5kitty15is_symmetric_inERK2TT7uint8_t7uint8_t>`_
)doc" );

  _truth_table.def( "chow_parameters", &kitty::chow_parameters<truth_table_t>, R"doc(
    Returns the Chow parameters of a function

    .. seealso:: `kitty documentation for chow_parameters <https://libkitty.readthedocs.io/en/latest/reference.html#_CPPv3N5kitty15chow_parametersERK2TT>`_
)doc" );

}

}
