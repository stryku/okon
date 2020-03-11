import random

def generate_hashes_indices(seed, number_of_hashes_to_collect, number_of_hashes_in_original_file):
    random.seed(seed)

    indices = []
    for _ in range(number_of_hashes_to_collect):
        indices.append(random.randint(0, number_of_hashes_in_original_file - 1))

    indices.sort()
    return indices


def collect_hashes_to_benchmark(seed, number_of_hashes_to_collect, path_to_original_file, number_of_hashes_in_original_file ):
    indices = generate_hashes_indices(seed, number_of_hashes_to_collect, number_of_hashes_in_original_file)
    hashes = []

    counter = 0
    index_no = 0

    with open(path_to_original_file, 'r') as original_file:
        while index_no < len(indices):
            line = original_file.readline()

            if counter == indices[index_no]:
                print('[{}/{}] Got [{}]: {}'.format(index_no, len(indices), indices[index_no], line[:40]))
                hashes.append(line[:40])
                index_no += 1

            counter += 1

    random.shuffle(hashes)
    return hashes


def run_benchmarks(hashes, benchmark_runner):
    results = []

    counter = 0

    for h in hashes:
        output = benchmark_runner(h)
        results.append(output)
        print('[{}/{}] Benchmarking {}'.format(counter, len(hashes), h))
        counter += 1

    results = results[1:]

    return results
