import os
import psutil
import argparse
from copy import deepcopy
from time import process_time

DELTA = 30
ALPHA = {}
ALPHA["A"] = {"A": 0, "C": 110, "G": 48, "T": 94}
ALPHA["C"] = {"A": 110, "C": 0, "G": 118, "T": 48}
ALPHA["G"] = {"A": 48, "C": 118, "G": 0, "T": 110}
ALPHA["T"] = {"A": 94, "C": 48, "G": 110, "T": 0}
dp = []


def read_input_file(filename="input.txt"):
    with open(filename) as f:
        inp = f.read().splitlines()

    split = None
    for i in range(1, len(inp)):
        try:
            int(inp[i])
        except Exception as e:
            split = i
            break

    return (
        inp[0],
        [int(x) for x in inp[1:split]],
        inp[split],
        [int(x) for x in inp[1 + split :]],
    )


def generate_string(base_str, arr):
    if not arr:
        return base_str

    gen_string = base_str
    for i in arr:
        gen_string = gen_string[: i + 1] + gen_string + gen_string[i + 1 :]

    assert len(gen_string) == (2 ** len(arr)) * len(base_str)

    return gen_string


def align_strings(s1, s2):
    dp = [[0 for _ in range(len(s2) + 1)] for _ in range(len(s1) + 1)]

    for i in range(len(s1) + 1):
        dp[i][0] = i * DELTA

    for i in range(len(s2) + 1):
        dp[0][i] = i * DELTA

    for i in range(1, len(s1) + 1):
        for j in range(1, len(s2) + 1):
            dp[i][j] = min(
                dp[i - 1][j - 1] + ALPHA[s1[i - 1]][s2[j - 1]],
                dp[i - 1][j] + DELTA,
                dp[i][j - 1] + DELTA,
            )

    s1_aligned = []
    s2_aligned = []
    l1 = len(s1)
    l2 = len(s2)
    while l1 > 0 and l2 > 0:
        if dp[l1][l2] == dp[l1 - 1][l2 - 1] + ALPHA[s1[l1 - 1]][s2[l2 - 1]]:
            s1_aligned.append(s1[l1 - 1])
            s2_aligned.append(s2[l2 - 1])
            l1 -= 1
            l2 -= 1
        elif dp[l1][l2] == dp[l1 - 1][l2] + DELTA:
            s1_aligned.append(s1[l1 - 1])
            s2_aligned.append("_")
            l1 -= 1
        else:
            s1_aligned.append("_")
            s2_aligned.append(s2[l2 - 1])
            l2 -= 1

    if l1:
        for i in range(l1, 0, -1):
            s1_aligned.append(s1[i - 1])
            s2_aligned.append("_")
    elif l2:
        for i in range(l2, 0, -1):
            s1_aligned.append("_")
            s2_aligned.append(s2[i - 1])

    s1_aligned.reverse()
    s2_aligned.reverse()

    return "".join(s1_aligned), "".join(s2_aligned)


def forward_space_efficient_alignment(s1, s2):

    for i in range(len(s1) + 1):
        dp[i][0] = i * DELTA

    for j in range(1, len(s2) + 1):
        dp[0][1] = j * DELTA
        for i in range(1, len(s1) + 1):
            dp[i][1] = min(
                dp[i - 1][0] + ALPHA[s1[i - 1]][s2[j - 1]],
                dp[i - 1][1] + DELTA,
                dp[i][0] + DELTA,
            )

        for k in range(len(s1) + 1):
            dp[k][0] = dp[k][1]
    return deepcopy(dp[: len(s1) + 1])


def backward_space_efficient_alignment(s1, s2):

    for i in range(len(s1) + 1):
        dp[i][0] = (len(s1) - i) * DELTA

    for j in range(len(s2) - 1, -1, -1):
        dp[len(s1)][1] = (len(s2) - j) * DELTA
        for i in range(len(s1) - 1, -1, -1):
            dp[i][1] = min(
                dp[i + 1][0] + ALPHA[s1[i]][s2[j]],
                dp[i + 1][1] + DELTA,
                dp[i][0] + DELTA,
            )

        for k in range(len(s1) + 1):
            dp[k][0] = dp[k][1]
    return deepcopy(dp[: len(s1) + 1])


L = []


def dc_align_strings(s1, s2):
    if len(s1) <= 2 or len(s2) <= 2:
        L.append(align_strings(s1, s2))
        return

    split = len(s2) // 2

    f = forward_space_efficient_alignment(s1, s2[:split])
    g = backward_space_efficient_alignment(s1, s2[split:])

    minq = f[0][1] + g[0][1]
    q = 0
    for i in range(len(s1) + 1):
        sm = f[i][1] + g[i][1]
        if sm < minq:
            minq = sm
            q = i

    dc_align_strings(s1[:q], s2[:split])
    dc_align_strings(s1[q:], s2[split:])


def write_output_file(
    s1, s2, solution_cost, time_taken, memory_used, filename="output.txt"
):
    with open(filename, "w") as f:
        f.write(f"{s1[:50]} {s1[-50:]}")
        f.write("\n")
        f.write(f"{s2[:50]} {s2[-50:]}")
        f.write("\n")
        f.write(f"{solution_cost:.1f}")
        f.write("\n")
        f.write(f"{time_taken:.4f}")
        f.write("\n")
        f.write(f"{memory_used:.1f}")


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("filename", type=str)
    args = parser.parse_args()

    base_str1, indices1, base_str2, indices2 = read_input_file(args.filename)

    string1 = generate_string(base_str1, indices1)
    string2 = generate_string(base_str2, indices2)

    start = process_time()

    dp = [[0, 0] for _ in range(len(string1) + 1)]
    dc_align_strings(string1, string2)
    output_string1 = "".join([x[0] for x in L])
    output_string2 = "".join([x[1] for x in L])

    solution_cost = 0
    for i in range(len(output_string1)):
        if output_string1[i] != "_" and output_string2[i] != "_":
            solution_cost += ALPHA[output_string1[i]][output_string2[i]]
        else:
            solution_cost += DELTA

    memory_used = psutil.Process(os.getpid()).memory_info().rss // 1024
    time_taken = process_time() - start

    write_output_file(
        output_string1, output_string2, solution_cost, time_taken, memory_used
    )