import json
import os

def parse_file(name):
    data = None
    data_processed = dict()
    with open(name) as f:
        data = json.loads(f.read())
    small_pattern, long_pattern, long_pattern_wildcard = data["benchmarks"]
    data_processed = {
        "name": name,
        "benchmarks": {
            "small_pattern": (small_pattern["real_time"], small_pattern["cpu_time"]),
            "long_pattern": (long_pattern["real_time"], long_pattern["cpu_time"]),
            "long_pattern_wildcard": (long_pattern_wildcard["real_time"], long_pattern_wildcard["cpu_time"]),
        }
    }
    return data_processed

def diff(va, vb, bench, i):
    new = va["benchmarks"][bench][i]
    old = vb["benchmarks"][bench][i]
    return (new - old) / old

def main():
    files = filter(lambda x: x.endswith(".json"), os.listdir("builds"))
    base = parse_file("builds/00_master.json")
    csv = "name,test,real_time,cpu_time\n"
    fmt = "{},{},{},{}\n"

    for f in files:
        if f.startswith("00"):
            continue
        current_data = parse_file(os.path.join("builds", f))
        for test in ["small_pattern", "long_pattern", "long_pattern_wildcard"]:
            csv += fmt.format(
                    current_data["name"],
                    test,
                    diff(current_data, base, test, 0),
                    diff(current_data, base, test, 1))
    with open("output.csv", "w") as f:
        f.write(csv)

if __name__ == "__main__":
    main()
