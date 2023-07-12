import random

size = int(input("Input cache size: "))
k = int(input("Input length of working set(k, less than cache size): "))
N = int(input("Input number of burst repeats(N): "))
A = int(input("Input number of working set repeats(A): "))
m = int(input("Input length of random set(m, greater than cash size): "))
list = []

for i in range(N):
    for counter in range(A):
        for j in range(1, k + 1):
            list.append(j)

    for j in range(m):
        list.append(random.randint(k+1, m + k))

list_str = [str(i) for i in list]

with open("ma2_test2.in", "w") as f:
    f.write(str(size) + " " + str((k*A + m)*N) + " ")
    f.write(" ".join(list_str))
