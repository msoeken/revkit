from revkit import netlist, tbs
import pytest

def test_netlist():
  circ = tbs([0, 2, 1, 3])
  assert 2 == circ.num_qubits
  assert 3 == circ.num_gates
  assert circ.to_quil() == "CNOT 1 0\nCNOT 0 1\nCNOT 1 0\n"
  assert circ.to_qasm() == 'OPENQASM 2.0;\ninclude "qelib1.inc";\nqreg q[2];\ncreg c[2];\ncx q[1],q[0];\ncx q[0],q[1];\ncx q[1],q[0];\n'
