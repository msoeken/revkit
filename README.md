# RevKit 3.1

RevKit 3.1 is a Python quantum compilation library.  RevKit is based on the C++
libraries [tweedledum](https://github.com/boschmitt/tweedledum) and
[caterpillar](https://github.com/gmeuli/caterpillar) (as well as other C++
libraries from the
[EPFL logic synthesis libraries](https://github.com/lsils/lstools-showcase)),
and exposes their functionality by means of simple Python APIs.  It also
contains the module `revkit.export` with several functions for interoperability
with other quantum programming languages and frameworks.

## Example

The following example creates a quantum circuit that computes the oracle
function `(a ∧ b) ⊕ (c ∧ d)` onto an ancilla qubit.

```python
from revkit import netlist, oracle_synth, truth_table
import revkit.export.qiskit

function = truth_table.from_expression("[(ab)(cd)]") # (a ∧ b) ⊕ (c ∧ d)
circuit = oracle_synth(function)

qcircuit = circuit.to_qiskit()
```

## Installation

From PyPi *(not yet available)*

```
pip install revkit
```

From Github with `setuptools`

```
pip install git+https://github.com/msoeken/revkit@develop
```

## Previous versions

RevKit has a 10-year long history. The rapidly changing requirements for
quantum computing also influenced RevKit, such that it was redesigned several
times. Previous versions of RevKit can be found online, but are no longer
maintained. Since version 3.1, RevKit does no longer exist as a standalone
command line application, but solely as a Python library, with the focus of an
easy integration with other Python-based quantum programming frameworks. In
order to access the various functionality in other applications, the libraries
[tweedledum](https://github.com/boschmitt/tweedledum) and
[caterpillar](https://github.com/gmeuli/caterpillar) can be used directly
instead.

## EPFL logic sythesis libraries

RevKit is based on the [EPFL logic synthesis](https://lsi.epfl.ch/page-138455-en.html) libraries.  The other libraries and several examples on how to use and integrate the libraries can be found in the [logic synthesis tool showcase](https://github.com/lsils/lstools-showcase).

