import revkit

net = revkit.tbs([0, 2, 3, 5, 7, 1, 4, 6])

print("After synthesis")
print(net.to_quil())

print("After decomposition")
net = revkit.dt_decomposition(net)
print(net.to_quil())
