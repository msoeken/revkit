from _revkit import netlist, gate

from qiskit import QuantumRegister, ClassicalRegister, QuantumCircuit

def _to_qiskit(self):
  qr = QuantumRegister(self.num_qubits, "qr")
  cr = QuantumRegister(self.num_qubits, "cr")
  circuit = QuantumCircuit(qr, cr)

  for g in self.gates:
    if g.kind == gate.gate_type.rotation_z:
      for t in g.targets:
        circuit.rz(g.angle, qr[t])

    elif g.kind == gate.gate_type.cx:
      ctrl = g.controls[0]
      if not bool(ctrl): assert False
      for t in g.targets:
        circuit.cx(qr[int(ctrl)], qr[t])

    elif g.kind == gate.gate_type.mcx:
      ctls = g.controls

      # only at most 2 controls and no negative controls
      if len(ctls) > 2: assert False
      if len([q for q in ctls if not bool(q)]) > 0: assert False

      ctls = [qr[int(q)] for q in ctls]
      tgts = [qr[q] for q in g.targets]

      for t in tgts[1:]:
        circuit.cx(tgts[0], t)
      if len(ctls) == 0:
        circuit.x(tgts[0])
      elif len(ctls) == 1:
        circuit.cx(ctls[0], tgts[0])
      else:
        circuit.ccx(ctls[0], ctls[1], tgts[0])
      for t in tgts[1:]:
        circuit.cx(tgts[0], t)
    else:
      raise BaseException(f"Unsupported gate type {g.kind}")

  return circuit

netlist.to_qiskit = _to_qiskit
