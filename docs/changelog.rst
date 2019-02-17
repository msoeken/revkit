Change Log
==========

Version 3.1 (not yet released)
------------------------------

* Data structures:
    - Quantum circuit (:class:`revkit.netlist`)
    - Gate and qubit (:class:`revkit.gate`, :class:`revkit.qubit`)
    - Truth table (:class:`revkit.truth_table`)

* Decomposition algorithms:
    - Barenco decomposition (:func:`revkit.barenco_decomposition`)
    - Direct Toffoli decomposition (:func:`revkit.dt_decomposition`)

* Synthesis algorithms:
    - Decomposition-based synthesis (:func:`revkit.dbs`)
    - Gray synthesis (:func:`revkit.gray_synth`)
    - Diagonal unitary synthesis (:func:`revkit.diagonal_synth`)
    - Oracle synthesis (:func:`revkit.oracle_synth`)
    - Transformation-based synthesis (:func:`revkit.tbs`)

* Interoperability:
    - Create Qiskit quantum circuit from RevKit quantum circuit (:func:`revkit.netlist.to_qiskit`)
