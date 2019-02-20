/*-------------------------------------------------------------------------------------------------
| This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
| Author(s): Giulia Meuli
*------------------------------------------------------------------------------------------------*/
#pragma once
#include <tweedledum/gates/gate_base.hpp>
#include <tweedledum/networks/netlist.hpp>
#include <tweedledum/networks/qubit.hpp>

#include <kitty/constructors.hpp>
#include <kitty/dynamic_truth_table.hpp>

#include <vector>

namespace caterpillar
{

namespace td = tweedledum;

class stg_gate : public td::gate_base
{

public:
  stg_gate( gate_base const& op, td::qubit_id target )
    : td::gate_base( op )
  {
    (void)target;
  }

  stg_gate( kitty::dynamic_truth_table const& function, std::vector<td::qubit_id> const& controls, td::qubit_id target )
      : gate_base( td::gate_set::num_defined_ops ),
        _function( function ),
        _controls( controls ),
        _target( target )
  {
  }

  stg_gate( std::vector<td::qubit_id> const& controls, td::qubit_id target )
      : gate_base( td::gate_set::mcx ),
        _controls( controls ),
        _target( target )
  {
  }

  stg_gate( gate_base const& op, std::vector<td::qubit_id> const& controls, td::qubit_id target )
      : td::gate_base( op ),
        _controls( controls ),
        _target( target )
  {
  }

  stg_gate( gate_base const& op, std::vector<td::qubit_id> const& controls, std::vector<td::qubit_id> target )
      : td::gate_base( op ),
        _controls( controls ),
        _target( target[0] )
  {
  }

  bool is_unitary_gate() const { return operation() == td::gate_set::num_defined_ops || operation() == td::gate_set::mcx; }

  uint32_t num_controls() const
  {
    return _controls.size();
  }

  template<typename Fn>
  void foreach_control( Fn&& fn ) const
  {
    for ( auto c : _controls )
    {
      fn( c );
    }
  }

  template<typename Fn>
  void foreach_target( Fn&& fn ) const
  {
    fn( _target );
  }

private:
  /*! \brief control function of the stg */
  kitty::dynamic_truth_table _function;

  /*! \brief set qubits in the network are the controls. */
  std::vector<td::qubit_id> _controls;

  /*! \brief target of the gate. */
  td::qubit_id _target;
};

} // namespace caterpillar
