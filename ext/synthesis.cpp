#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <cstdint>
#include <vector>

#include <tweedledum/algorithms/synthesis/gray_synth.hpp>
#include <tweedledum/algorithms/synthesis/tbs.hpp>

#include "types.hpp"

namespace py = pybind11;

namespace revkit
{

void synthesis( py::module m )
{
  using namespace py::literals;

  m.def( "gray_synth", []( py::args parity_terms ) {
    uint32_t num_vars = 0u;

    tweedledum::parity_terms parities;
    for ( auto const& entry : parity_terms ) {
      auto const& tuple = entry.cast<py::tuple>();
      std::string term = tuple[0].cast<py::str>();
      double angle = tuple[1].cast<py::float_>();

      if ( num_vars == 0u ) {
        num_vars = term.size();
      }

      uint32_t iterm{0u};
      for ( auto i = 0u; i < term.size(); ++i ) {
        if ( term[i] == '1' ) {
          iterm |= 1 << i;
        }
      }
      parities.add_term( iterm, tweedledum::angle( angle ) );
    }
    return tweedledum::gray_synth<netlist_t>( num_vars, parities );
  }, R"doc(
    GraySynth synthesis algorithm for parity terms

    :param List[(str,float)] args: A list of tuples of parity terms.
        The first entry of the term is a bitstring where the first bit
        corresponds to the first qubit and is 1 if it is contained in the parity
        term. The second parameter is the angle that should be applied for this
        term.
    :rtype: netlist

    The following example synthesizes a controlled S operation::

        from revkit import gray_synth
        from math import pi

        circ = gray_synth(("01", pi / 4), ("10", pi / 4), ("11", -pi / 4))

    .. seealso:: `tweedledum documentation for gray_synth <https://tweedledum.readthedocs.io/en/latest/algorithms/synthesis/gray_synth.html>`_
)doc" );

  m.def( "tbs", []( std::vector<uint32_t> const& perm ) { return tweedledum::tbs<netlist_t>( perm ); }, R"doc(
    Transformation based synthesis

    :param List[int] perm: A permutation of the values :math:`\{0, \dots, 2^n - 1\}`.
    :rtype: netlist

    .. seealso:: `tweedledum documentation for tbs <https://tweedledum.readthedocs.io/en/latest/algorithms/synthesis/tbs.html>`_
)doc",
         "perm"_a );
}

} // namespace revkit
