import random

size = int(input("Input cache size: "))
k = int(input("Input length of working set(k, greater than cache size): "))
N = int(input("Input number of burst repeats(N): "))
list = []

for i in range(N):
    for j in range(1, k + 1):
        list.append(j)

list_str = [str(i) for i in list]

with open("ta_test5.in", "w") as f:
    f.write(str(size) + " " + str(k*N) + " ")
    f.write(" ".join(list_str))


