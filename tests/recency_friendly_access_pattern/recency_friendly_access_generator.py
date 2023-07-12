import random

size = int(input("Input cache size: "))
k = int(input("Input length of working set(k): "))
N = int(input("Input number of burst repeats(N): "))
list = []

for i in range(N):
    for j in range(1, k + 1):
            list.append(j)

    for j in range (k, 0, -1):
        list.append(j)
        
list_str = [str(i) for i in list]

with open("rfa_test6.in", "w") as f:
    f.write(str(size) + " " + str((2*k*N)) + " ")
    f.write(" ".join(list_str))
