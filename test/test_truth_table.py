from revkit import truth_table
import pytest

def test_read_truth_table():
  assert truth_table.from_hex("e8") == truth_table.from_binary("11101000")
  assert truth_table.from_expression("<abc>") == truth_table.from_hex("e8")

def test_truth_table_properties():
  tt = truth_table.from_expression("(a[bc])")
  assert 3 == tt.num_vars
  assert not tt.is_symmetric_in(0, 1)
  assert not tt.is_symmetric_in(0, 2)
  assert tt.is_symmetric_in(1, 2)
  assert not tt.is_horn()
  assert tt.is_krom()
  assert (2, [2, 1, 1]) == tt.chow_parameters()

