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
  }, "Barenco decomposition", "circ"_a, "controls_threshold"_a = 2u );
  m.def( "dt_decomposition", &tweedledum::dt_decomposition<netlist_t>, "Direct Toffoli decomposition", "circ"_a );
}

} // namespace revkit
