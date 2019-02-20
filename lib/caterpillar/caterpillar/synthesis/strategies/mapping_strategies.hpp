/*------------------------------------------------------------------------------
| This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
| Author(s): Mathias Soeken and Giulia Meuli
*-----------------------------------------------------------------------------*/

#pragma once

#include <cstdint>
#include <unordered_set>

#include "../sat.hpp"

#include <mockturtle/utils/progress_bar.hpp>
#include <mockturtle/views/topo_view.hpp>

namespace caterpillar
{

namespace mt = mockturtle;

struct mapping_strategy_params
{
  /*! \brief Show progress bar. */
  bool progress{false};

  /*! \brief Maximum number of pebbles to use, if supported by mapping strategy (0 means no limit). */
  uint32_t pebble_limit{0u};

  /*! \brief Conflict limit for the SAT solver (0 means no limit). */
  uint32_t conflict_limit{0u};

  /*! \brief Increment pebble numbers, if timeout. */
  bool increment_on_timeout{false};

  /*! \brief Decrement pebble numbers, if satisfiable. */
  bool decrement_on_success{false};
};

template<class LogicNetwork>
class pebbling_mapping_strategy
{

public:
  /* returns the method foreach_step */
  pebbling_mapping_strategy( LogicNetwork const& ntk, mapping_strategy_params const& ps = {} )
      : _ntk( ntk ),
        ps( ps )
  {
    static_assert( mt::is_network_type_v<LogicNetwork>, "LogicNetwork is not a network type" );
    static_assert( mt::has_is_pi_v<LogicNetwork>, "LogicNetwork does not implement the is_pi method" );
    static_assert( mt::has_foreach_fanin_v<LogicNetwork>, "LogicNetwork does not implement the foreach_fanin method" );
    static_assert( mt::has_foreach_gate_v<LogicNetwork>, "LogicNetwork does not implement the foreach_gate method" );
    static_assert( mt::has_num_gates_v<LogicNetwork>, "LogicNetwork does not implement the num_gates method" );
    static_assert( mt::has_foreach_po_v<LogicNetwork>, "LogicNetwork does not implement the foreach_po method" );
    static_assert( mt::has_index_to_node_v<LogicNetwork>, "LogicNetwork does not implement the index_to_node method" );
  }

  template<class Fn>
  inline bool foreach_step( Fn&& fn ) const
  {
    assert( !ps.decrement_on_success || !ps.increment_on_timeout );
    std::vector<std::pair<mockturtle::node<LogicNetwork>, mapping_strategy_action>> store_steps;
    auto limit = ps.pebble_limit;
    int max_steps = 100;
    while ( true )
    {
      pebble_solver<LogicNetwork> solver( _ntk, limit );
      solver.initialize();

      mockturtle::progress_bar bar( 100, "|{0}| current step = {1}", ps.progress );
      percy::synth_result result;

      do
      {
        if ( solver.current_step() >= max_steps )
        {
          result = percy::timeout;
          break;
        }

        bar( std::min<uint32_t>( solver.current_step(), 100 ), solver.current_step() );
        solver.add_step();
        result = solver.solve( ps.conflict_limit );
      } while ( result == percy::failure );

      if ( result == percy::timeout )
      {
        if ( ps.increment_on_timeout )
        {
          limit++;
          continue;
        }
        else if ( !ps.decrement_on_success )
          return false;
      }
      else if ( result == percy::success )
      {
        store_steps = solver.extract_result();
        if ( ps.decrement_on_success )
        {
          limit--;
          continue;
        }
      }

      if ( store_steps.empty() )
        return false;

      for ( auto const& [n, a] : store_steps )
      {
        fn( n, a );
      }

      return true;
    }
  }

private:
  LogicNetwork const& _ntk;
  mapping_strategy_params ps;
};

template<class LogicNetwork>
class bennett_mapping_strategy
{
public:
  bennett_mapping_strategy( LogicNetwork const& ntk, mapping_strategy_params const& ps = {} )
  {
    static_assert( mt::is_network_type_v<LogicNetwork>, "LogicNetwork is not a network type" );
    static_assert( mt::has_foreach_po_v<LogicNetwork>, "LogicNetwork does not implement the foreach_po method" );
    static_assert( mt::has_is_constant_v<LogicNetwork>, "LogicNetwork does not implement the is_constant method" );
    static_assert( mt::has_is_pi_v<LogicNetwork>, "LogicNetwork does not implement the is_pi method" );
    static_assert( mt::has_get_node_v<LogicNetwork>, "LogicNetwork does not implement the get_node method" );

    (void)ps;

    std::unordered_set<mt::node<LogicNetwork>> drivers;
    ntk.foreach_po( [&]( auto const& f ) { drivers.insert( ntk.get_node( f ) ); } );

    auto it = steps.begin();
    mt::topo_view view{ntk};
    view.foreach_node( [&]( auto n ) {
      if ( ntk.is_constant( n ) || ntk.is_pi( n ) )
        return true;

      /* compute step */
      it = steps.insert( it, {n, compute_action{}} );
      ++it;

      if ( !drivers.count( n ) )
        it = steps.insert( it, {n, uncompute_action{}} );

      return true;
    } );
  }

  template<class Fn>
  inline bool foreach_step( Fn&& fn ) const
  {
    for ( auto const& [n, a] : steps )
    {
      fn( n, a );
    }

    return true;
  }

private:
  std::vector<std::pair<mt::node<LogicNetwork>, mapping_strategy_action>> steps;
};

template<class LogicNetwork>
class bennett_inplace_mapping_strategy
{
public:
  bennett_inplace_mapping_strategy( LogicNetwork const& ntk, mapping_strategy_params const& ps = {} )
  {
    static_assert( mt::is_network_type_v<LogicNetwork>, "LogicNetwork is not a network type" );
    static_assert( mt::has_foreach_po_v<LogicNetwork>, "LogicNetwork does not implement the foreach_po method" );
    static_assert( mt::has_is_constant_v<LogicNetwork>, "LogicNetwork does not implement the is_constant method" );
    static_assert( mt::has_is_pi_v<LogicNetwork>, "LogicNetwork does not implement the is_pi method" );
    static_assert( mt::has_get_node_v<LogicNetwork>, "LogicNetwork does not implement the get_node method" );
    static_assert( mt::has_node_to_index_v<LogicNetwork>, "LogicNetwork does not implement the node_to_index method" );
    static_assert( mt::has_clear_values_v<LogicNetwork>, "LogicNetwork does not implement the clear_values method" );
    static_assert( mt::has_set_value_v<LogicNetwork>, "LogicNetwork does not implement the set_value method" );
    static_assert( mt::has_decr_value_v<LogicNetwork>, "LogicNetwork does not implement the decr_value method" );
    static_assert( mt::has_fanout_size_v<LogicNetwork>, "LogicNetwork does not implement the fanout_size method" );
    static_assert( mt::has_foreach_fanin_v<LogicNetwork>, "LogicNetwork does not implement the foreach_fanin method" );

    (void)ps;

    std::unordered_set<mt::node<LogicNetwork>> drivers;
    ntk.foreach_po( [&]( auto const& f ) { drivers.insert( ntk.get_node( f ) ); } );

    ntk.clear_values();
    ntk.foreach_node( [&]( const auto& n ) { ntk.set_value( n, ntk.fanout_size( n ) ); } );

    auto it = steps.begin();
    //mt::topo_view view{ntk};
    ntk.foreach_node( [&]( auto n ) {
      if ( ntk.is_constant( n ) || ntk.is_pi( n ) )
        return true;

      /* decrease reference counts and mark potential target for inplace */
      int target{-1};
      ntk.foreach_fanin( n, [&]( auto f ) {
        if ( ntk.decr_value( ntk.get_node( f ) ) == 0 )
        {
          if ( target == -1 )
          {
            target = ntk.node_to_index( ntk.get_node( f ) );
          }
        }
      } );

      /* check for inplace (only if there is possible target and node is not an output driver) */
      if ( target != -1 && !drivers.count( n ) )
      {
        if constexpr ( mt::has_is_xor_v<LogicNetwork> )
        {
          if ( ntk.is_xor( n ) )
          {
            it = steps.insert( it, {n, compute_inplace_action{
                                           static_cast<uint32_t>(
                                               target )}} );
            ++it;
            it = steps.insert( it, {n, uncompute_inplace_action{
                                           static_cast<uint32_t>(
                                               target )}} );
            return true;
          }
        }
        if constexpr ( mt::has_is_xor3_v<LogicNetwork> )
        {
          if ( ntk.is_xor3( n ) )
          {
            it = steps.insert( it, {n, compute_inplace_action{
                                           static_cast<uint32_t>(
                                               target )}} );
            ++it;
            it = steps.insert( it, {n, uncompute_inplace_action{
                                           static_cast<uint32_t>(
                                               target )}} );
            return true;
          }
        }
      }

      /* compute step */
      it = steps.insert( it, {n, compute_action{}} );
      ++it;

      if ( !drivers.count( n ) )
        it = steps.insert( it, {n, uncompute_action{}} );

      return true;
    } );
  }

  template<class Fn>
  inline bool foreach_step( Fn&& fn ) const
  {
    for ( auto const& [n, a] : steps )
    {
      fn( n, a );
    }

    return true;
  }

private:
  std::vector<std::pair<mt::node<LogicNetwork>, mapping_strategy_action>> steps;
};

} // namespace caterpillar