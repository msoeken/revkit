/*-------------------------------------------------------------------------------------------------
| This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
| Author(s): Giulia Meuli
*------------------------------------------------------------------------------------------------*/

#pragma once

#include "mapping_strategy.hpp"
#include <caterpillar/stg_gate.hpp>
#include <mockturtle/networks/xag.hpp>
#include <mockturtle/views/topo_view.hpp>
#include <tweedledum/networks/netlist.hpp>

namespace caterpillar
{

struct SignalCone
{
  std::vector<uint32_t> leaves;
  mockturtle::node<mockturtle::xag_network> node;
  std::vector<uint32_t> cone_nodes;
};

class xag_mapping_strategy : public mapping_strategy<mockturtle::xag_network>
{

  void accumulate_leaves( mockturtle::topo_view<mockturtle::xag_network> const& xag, SignalCone& cone, mockturtle::signal<mockturtle::xag_network> s )
  {
    auto n = xag.get_node( s );

    if ( xag.is_xor( n ) )
    {
      cone.cone_nodes.push_back( n );

      xag.foreach_fanin( n, [&]( auto si ) {
        accumulate_leaves( xag, cone, si );
      } );
    }
    else
      cone.leaves.push_back( n );
  }

  std::vector<SignalCone> get_cones( mockturtle::topo_view<mockturtle::xag_network> const& xag, mockturtle::node<mockturtle::xag_network> node )
  {
    std::vector<SignalCone> cones;

    xag.foreach_fanin( node, [&]( auto s ) {
      SignalCone cone;
      cone.node = node;

      accumulate_leaves( xag, cone, s );

      std::sort( cone.cone_nodes.begin(), cone.cone_nodes.end() );
      cones.push_back( cone );
    } );

    return cones;
  }

  void order_sets( std::vector<SignalCone>& cones )
  {
    auto leav_1 = cones[0].leaves;
    auto leav_2 = cones[1].leaves;

    std::sort( leav_1.begin(), leav_1.end() );
    std::sort( leav_2.begin(), leav_2.end() );

    leav_1.erase( std::unique( leav_1.begin(), leav_1.end() ), leav_1.end() );
    leav_2.erase( std::unique( leav_2.begin(), leav_2.end() ), leav_2.end() );

    std::vector<uint32_t> diff_0, diff_1;
    std::set_difference( leav_1.begin(), leav_1.end(), leav_1.begin(), leav_1.end(), std::back_inserter( diff_0 ) );
    std::set_difference( leav_1.begin(), leav_1.end(), leav_1.begin(), leav_1.end(), std::back_inserter( diff_1 ) );

    for ( auto elem : leav_1 )
    {
      if ( std::find( diff_0.begin(), diff_0.end(), elem ) != diff_0.end() )
      {
        auto it_move = std::find( leav_1.begin(), leav_1.end(), elem );
        std::move( it_move, it_move + 1, leav_1.begin() );
        break;
      }
    }
    for ( auto elem : leav_1 )
    {
      if ( std::find( diff_1.begin(), diff_1.end(), elem ) != diff_1.end() )
      {
        auto it_move = std::find( leav_1.begin(), leav_1.end(), elem );
        std::move( it_move, it_move + 1, leav_1.begin() );
        break;
      }
    }
  }

  std::vector<std::pair<mockturtle::node<mockturtle::xag_network>, mapping_strategy_action>> compute_cones( mockturtle::topo_view<mockturtle::xag_network> const& xag, std::vector<SignalCone> const& cones, bool uncompute_and )
  {
    std::vector<std::pair<mockturtle::node<mockturtle::xag_network>, mapping_strategy_action>> comp_steps;

    for ( auto cone : cones )
    {
      auto target = xag.node_to_index( cone.leaves[0] );
      for ( auto node : cone.cone_nodes )
      {
        comp_steps.push_back( {node, compute_inplace_action{static_cast<uint32_t>( target )}} );
      }
    }

    if ( uncompute_and )
      comp_steps.push_back( {cones[0].node, uncompute_action{}} );
    else
      comp_steps.push_back( {cones[0].node, compute_action{}} );

    for ( auto cone : cones )
    {
      auto target = xag.node_to_index( cone.leaves[0] );
      for ( auto node : cone.cone_nodes )
      {

        comp_steps.push_back( {node, uncompute_inplace_action{static_cast<uint32_t>( target )}} );
      }
    }
    return comp_steps;
  }

  std::vector<std::pair<mockturtle::node<mockturtle::xag_network>, mapping_strategy_action>> compute_xor( std::vector<SignalCone> const& cones )
  {
    std::vector<std::pair<mockturtle::node<mockturtle::xag_network>, mapping_strategy_action>> comp_steps;
    comp_steps.push_back( {cones[0].node, compute_action{}} );
    for ( auto cone : cones )
    {
      for ( auto node : cone.cone_nodes )
      {
        comp_steps.push_back( {node, compute_action{}} );
      }
    }
    return comp_steps;
  }

public:
  bool compute_steps( mockturtle::xag_network const& ntk ) override
  {
    mockturtle::topo_view xag{ntk};
    std::vector<mockturtle::node<mockturtle::xag_network>> drivers;
    xag.foreach_po( [&]( auto const& f ) { drivers.push_back( xag.get_node( f ) ); } );

    auto it = steps().begin();
    xag.foreach_node( [&]( auto node ) {
      if ( xag.is_and( node ) )
      {
        auto cones = get_cones( xag, node );

        order_sets( cones );

        /* compute step */
        auto cc = compute_cones( xag, cones, false );
        it = steps().insert( it, cc.begin(), cc.end() );
        it = it + cc.size();

        if ( std::find( drivers.begin(), drivers.end(), node ) == drivers.end() )
        {
          auto uc = compute_cones( xag, cones, true );
          it = steps().insert( it, uc.begin(), uc.end() );
        }
      }
      else if ( std::find( drivers.begin(), drivers.end(), node ) != drivers.end() )
      {
        auto cones = get_cones( xag, node );

        auto xc = compute_xor( cones );
        it = steps().insert( it, xc.begin(), xc.end() );
      }
    } );

    return true;
  }
};

} // namespace caterpillar