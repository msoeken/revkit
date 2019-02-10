from _revkit import netlist, gate

from qiskit import QuantumRegister, ClassicalRegister, QuantumCircuit

def _to_qiskit(self, circuit=None, with_classical_register=False):
  """
  Convert RevKit quantum circuit into Qiskit quantum circuit

  :param qiskit.QuantumCircuit circuit: If not `None`, add gates to this circuit
      and also use its quantum registers.  If the circuit does not have enough
      qubit, the method fails. If `None` (default), a new circuit is
      constructed.
  :param bool with_classical_register: Add a classical register, if new circuit
      is constructed (i.e., `circuit` is `None`)
  :rtype: qiskit.QuatumCircuit
  """
  if circuit is None:
    qr = QuantumRegister(self.num_qubits, "qr")

    if with_classical_register:
      cr = ClassicalRegister(self.num_qubits, "cr")
      circuit = QuantumCircuit(qr, cr)
    else:
      circuit = QuantumCircuit(qr)

  # collect all qubits from all quantum registers
  qr = [q for reg in circuit.qregs for q in reg]

  for g in self.gates:
    if g.kind == gate.gate_type.hadamard:
      for t in g.targets:
        circuit.h(qr[t])

    elif g.kind == gate.gate_type.rotation_z:
      for t in g.targets:
        circuit.rz(g.angle, qr[t])

    elif g.kind == gate.gate_type.cx:
      ctrl = g.controls[0]
      for t in g.targets:
        circuit.cx(qr[int(ctrl)], qr[t])
      if not bool(ctrl):
        circuit.x(qr[t])

    elif g.kind == gate.gate_type.mcx:
      ctls = g.controls

      # only at most 2 controls and no negative controls
      if len(ctls) > 2: raise Exception("X gates cannot have more than 2 controls")

      negs = [qr[int(q)] for q in ctls if not bool(q)]
      ctls = [qr[int(q)] for q in ctls]
      tgts = [qr[q] for q in g.targets]

      for t in tgts[1:]:
        circuit.cx(tgts[0], t)
      for n in negs:
        circuit.x(n)
      if len(ctls) == 0:
        circuit.x(tgts[0])
      elif len(ctls) == 1:
        circuit.cx(ctls[0], tgts[0])
      else:
        circuit.ccx(ctls[0], ctls[1], tgts[0])
      for n in negs:
        circuit.x(n)
      for t in tgts[1:]:
        circuit.cx(tgts[0], t)
    else:
      raise Exception(f"Unsupported gate type {g.kind}")

  return circuit

netlist.to_qiskit = _to_qiskit
