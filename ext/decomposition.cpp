#include <pybind11/pybind11.h>

#include <tweedledum/algorithms/decomposition/dt.hpp>

#include "types.hpp"

namespace py = pybind11;

namespace revkit
{

void decomposition( py::module m )
{
  m.def( "dt_decomposition", &tweedledum::dt_decomposition<netlist_t>, "Direct Toffoli decomposition" );
}

} // namespace revkit
