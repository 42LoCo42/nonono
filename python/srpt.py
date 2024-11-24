schedule = [
3,
0,
6,
3,
1,
0,
2,
1,
0,
0,
0,
0,
5,
0,
1,
0,
3,
0,
0,
0,
2,
3,
]

jobs = []
lifetimes = []

def main():
    for t in range(0, 30):
        msg = f"{t}:"

        # add current job
        current = 0 if t >= len(schedule) else schedule[t]
        if current != 0:
            jobs.append(current)
            lifetimes.append(0)

        # increase all lifetimes
        for l in range(0, len(lifetimes)):
            if jobs[l] > 0:
                lifetimes[l] += 1

        if len(jobs) > 0:
            # find shortest job
            shortest = 0
            for j in range(0, len(jobs)):
                if jobs[shortest] < 1:
                    shortest = j
                if jobs[j] > 0 and jobs[j] < jobs[shortest]:
                    shortest = j

            # work
            jobs[shortest] -= 1

            msg += f" work {shortest + 1}"

        msg += f" {jobs}"
        print(msg)
    print(lifetimes)
    print(sum(lifetimes) / len(lifetimes))
main()
