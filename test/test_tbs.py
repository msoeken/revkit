import revkit
import pytest

def test_tbs_synthesizes_swap():
  net = revkit.tbs([0, 2, 1, 3])
  assert net.num_gates == 3
  assert net.num_qubits == 2

  g = net.gates
  assert [c.index for c in g[0].controls] == [1]
  assert g[0].targets == [0]
  assert g[0].kind == revkit.gate.gate_type.mcx
