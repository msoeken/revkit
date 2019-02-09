#include <pybind11/pybind11.h>

#include <tweedledum/algorithms/decomposition/barenco.hpp>
#include <tweedledum/algorithms/decomposition/dt.hpp>

#include "types.hpp"

namespace py = pybind11;

namespace revkit
{

void decomposition( py::module m )
{
  using namespace py::literals;

  m.def( "barenco_decomposition", []( netlist_t const& circ, unsigned controls_threshold ) {
    tweedledum::barenco_params params;
    params.controls_threshold = controls_threshold;
    return tweedledum::barenco_decomposition<netlist_t>( circ, params );
  }, R"doc(
    Barenco decomposition

    Decomposes all Multiple-controlled Toffoli gates with more than `controls_threshold` controls into Toffoli gates with at most `controls_threshold` controls.

    :param netlist circ: Input circuit
    :param int controls_threshold: Maximum number of controls of gates in resulting circuit
    :rtype: netlist

    .. seealso:: `tweedledum documentation for barenco_decomposition <https://tweedledum.readthedocs.io/en/latest/algorithms/decomposition/barenco.html>`_
)doc", "circ"_a, "controls_threshold"_a = 2u );

  m.def( "dt_decomposition", &tweedledum::dt_decomposition<netlist_t>, R"doc(
    Direct Toffoli decomposition

    Decomposes all Multiple-controlled Toffoli gates with 2, 3 or 4 controls into Clifford+T.

    :param netlist circ: Input circuit
    :rtype: netlist

    .. seealso:: `tweedledum documentation for dt_decomposition <https://tweedledum.readthedocs.io/en/latest/algorithms/decomposition/dt.html>`_
)doc", "circ"_a );
}

} // namespace revkit
