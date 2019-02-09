#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <sstream>

#include <tweedledum/gates/mcst_gate.hpp>
#include <tweedledum/io/quil.hpp>
#include <tweedledum/io/write_unicode.hpp>
#include <tweedledum/networks/netlist.hpp>

#include "types.hpp"

namespace py = pybind11;

namespace revkit
{

void qubit( py::module m )
{
  py::class_<tweedledum::qubit_id> _qubit( m, "qubit", "Qubit data structure" );
  _qubit.def_property_readonly( "index", &tweedledum::qubit_id::index, "Index of qubit" );
  _qubit.def_property_readonly( "is_complemented", &tweedledum::qubit_id::is_complemented, "True, if qubit is complemented (negative polarity, anti control)" );
  _qubit.def( "__int__", &tweedledum::qubit_id::index, "Cast to qubit's index" );
  _qubit.def( "__bool__", []( tweedledum::qubit_id const& ref ) { return !ref.is_complemented(); }, "Cast to qubit's polarity" );
}

void gate( py::module m )
{
  py::class_<gate_t> _gate( m, "gate", "Gate data structure" );
  _gate.def_property_readonly( "controls", []( gate_t const& ref ) {
    std::vector<tweedledum::qubit_id> controls;
    ref.foreach_control( [&]( auto q ) { controls.push_back( q ); } );
    return controls;
  }, R"doc(
    List of qubits for gate controls

    :rtype: List[qubit]
)doc" );
  _gate.def_property_readonly( "targets", []( gate_t const& ref ) {
    std::vector<uint32_t> targets;
    ref.foreach_target( [&]( auto q ) { targets.push_back( q.index() ); } );
    return targets;
  }, R"doc(
    List of qubit indexes for gate targets

    :rtype: List[int]
)doc" );
  _gate.def_property_readonly( "kind", &gate_t::operation, R"doc(
    Gate kind

    :rtype: gate.gate_type
)doc" );
  _gate.def_property_readonly( "angle", []( gate_t const& ref ) { 
    return ref.rotation_angle().numeric_value();
  }, R"doc(
    Rotation angle, if gate is rotation gate

    :rtype: float
)doc" );

  py::enum_<tweedledum::gate_set>( _gate, "gate_type", "Gate kind enumeration" )
#define GATE(X, Y, Z, W) .value( #X, tweedledum::gate_set:: X )
#include <tweedledum/gates/gate_set.def>
    .export_values();
}

void netlist( py::module m )
{
  py::class_<netlist_t> _netlist( m, "netlist", "Quantum circuit data structure" );
  _netlist.def_property_readonly( "num_gates", &netlist_t::num_gates, "Number of quantum gates in circuit" );
  _netlist.def_property_readonly( "num_qubits", &netlist_t::num_qubits, "Number of qubits in circuit" );
  _netlist.def_property_readonly( "gates", []( netlist_t const& ref ) {
    std::vector<gate_t> gates;
    ref.foreach_cgate( [&]( auto const& n ) { gates.push_back( n.gate ); } );
    return gates;
  }, R"doc(
    List of gates
    
    :rtype: List[gate]
)doc" );
  _netlist.def( "to_quil", []( netlist_t const& ref ) {
    std::ostringstream s;
    tweedledum::write_quil( ref, s );
    return s.str();
  }, "Write circuit to QUIL code" );
  _netlist.def( "to_unicode", []( netlist_t const& ref ) { 
    return tweedledum::to_unicode_str( ref );
  }, "Write circuit to Unicode representation" );
}

} // namespace revkit
