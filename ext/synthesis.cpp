#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <cstdint>
#include <vector>

#include <tweedledum/algorithms/synthesis/tbs.hpp>

#include "types.hpp"

namespace py = pybind11;

namespace revkit
{

void synthesis( py::module m )
{
  using namespace py::literals;

  m.def("tbs", []( std::vector<uint32_t> const& perm ) { return tweedledum::tbs<netlist_t>( perm ); }, R"doc(
    Transformation based synthesis

    :param List[int] perm: A permutation of the values :math:`\{0, \dots, 2^n - 1\}`.
    :rtype: netlist
)doc", "perm"_a );
}

} // namespace revkit
