import os
import shutil
import subprocess
import sys

extension = ".exe" if sys.platform in ["win32", "cygwin"] else ""
print(f"Using `{extension}` extension")

COMMITS = """
7123bfa9081c4e4396964076044ada76086233b3 07_sse4_plus_generate_bitmask
6ef9d0ee8eb49a98c1d39680a2234541bd802c21 06_sse4.1
61679702894050dfbbe5931208a0710ce50193ce 05_generate_bitmask
0e998dbb0f1338b775d2e9b3ed7d2595e50f8609 04_merge_bytes_2
699ace6826180dc27e252b5534f18f1e3a55b4c0 03_merge_bytes
28ce6435b79c25bf1b88d4114cb0a5fe2114eccd 02_specific_size
5dcb251686c61bfd4e4a04d847047a45d6d05e67 01_recursive_mode
3793eda08a9080bdef04db392137b193e3ab4327 00_master
"""

def parse_commits():
    c = COMMITS.strip()
    data = []
    for l in c.split("\n"):
        commithash, name = l.split(" ")
        data.append((commithash, name))
    return data

def run(commit, name):
    current_path = os.getcwd()
    build_path = os.path.join(current_path, "builds")
    if not os.path.exists(build_path):
        os.mkdir(build_path)
    version_path = os.path.join(build_path, f"build_{name}")
    subprocess.run(f"git checkout {commit}".split(" "), check=True)
    shutil.rmtree(version_path, ignore_errors=True)
    os.mkdir(version_path)
    subprocess.run(f"cmake ../.. -DCMAKE_BUILD_TYPE=Release -DSAPEAOB_BUILD_BENCHMARK=ON -GNinja".split(" "), check=True, cwd=version_path)
    subprocess.run(f"ninja -j12".split(" "), check=True, cwd=version_path)
    exec_name = os.path.join(".", "benchmark", f"bench-sapeaob{extension}")
    print(f"Running {exec_name}")
    subprocess.run(f"{exec_name} --benchmark_out=../{name}.json".split(" "), check=True, cwd=version_path, shell=True)

def main():
    data = parse_commits()
    for (c, n) in data:
        print("Running", n)
        run(c, n)

if __name__ == "__main__":
    main()
