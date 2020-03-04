import subprocess
import sys
import os
import timeit
from okon_benchmark_utils import *


NUMBER_OF_HASHES_TO_BENCHMARK = int(sys.argv[1])
PATH_TO_ORIGINAL_FILE = sys.argv[2]
NUMBER_OF_HASHES_IN_ORIGINAL_FILE = int(sys.argv[3])
BENCHMARK_SEED = int(sys.argv[4]) if len(sys.argv) > 4 else 0

def run_benchmark(hash_to_benchmark):
    os.system('sudo sh -c "sync; echo 3 > /proc/sys/vm/drop_caches"')
    command = ['grep', '-m', '1', '^{}'.format(hash_to_benchmark), PATH_TO_ORIGINAL_FILE]
    start = timeit.default_timer()
    subprocess.run(command, stdout=subprocess.PIPE)
    end = timeit.default_timer()
    return int((end - start) * 1000)


hashes = collect_hashes_to_benchmark(BENCHMARK_SEED, NUMBER_OF_HASHES_TO_BENCHMARK, PATH_TO_ORIGINAL_FILE, NUMBER_OF_HASHES_IN_ORIGINAL_FILE)
results = run_benchmarks(hashes, run_benchmark)

print('Grep benchmark done, result: {}ms'.format(sum(results) / len(results)))
