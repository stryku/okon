import subprocess
import sys
import os
from okon_benchmark_utils import *


OKON_CALLER = sys.argv[1]
NUMBER_OF_HASHES_TO_BENCHMARK = int(sys.argv[2])
PATH_TO_ORIGINAL_FILE = sys.argv[3]
NUMBER_OF_HASHES_IN_ORIGINAL_FILE = int(sys.argv[4])
BTREE_FILE_TO_BENCHMARK = sys.argv[5]
BENCHMARK_SEED = int(sys.argv[6]) if len(sys.argv) > 6 else 0

def run_benchmark(hash_to_benchmark):
    os.system('sudo sh -c "sync; echo 3 > /proc/sys/vm/drop_caches"')
    command = [OKON_CALLER, hash_to_benchmark, BTREE_FILE_TO_BENCHMARK]
    return int(subprocess.run(command, stdout=subprocess.PIPE).stdout.decode('utf-8'))

hashes = collect_hashes_to_benchmark(BENCHMARK_SEED, NUMBER_OF_HASHES_TO_BENCHMARK, PATH_TO_ORIGINAL_FILE, NUMBER_OF_HASHES_IN_ORIGINAL_FILE)
results = run_benchmarks(hashes, run_benchmark)

print('B-tree benchmark done, result: {}ms'.format(sum(results) / len(results) / 1000.0))
