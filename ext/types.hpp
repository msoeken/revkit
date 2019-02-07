#pragma once

#include <tweedledum/gates/mcmt_gate.hpp>
#include <tweedledum/networks/netlist.hpp>

namespace revkit
{

using netlist_t = tweedledum::netlist<tweedledum::mcmt_gate>;
using gate_t = netlist_t::gate_type;

}
