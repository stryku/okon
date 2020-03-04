option(OKON_WITH_BTREE_BENCHMARK "Include B-tree benchmarking target. \
                                  BEWARE: It needs root privilieges to 'sudo sh -c \"sync; echo 3 > /proc/sys/vm/drop_caches\"'. \
                                  (requires python3)" OFF)
if(OKON_WITH_BTREE_BENCHMARK)
    include(okon_btree_benchmark.cmake)
endif()

option(OKON_WITH_GREP_BENCHMARK "Include grep benchmark. \
                                 BEWARE: It needs root privilieges to 'sudo sh -c \"sync; echo 3 > /proc/sys/vm/drop_caches\"'. \
                                 (requires python3)" OFF)
if(OKON_WITH_GREP_BENCHMARK)
    include(okon_grep_benchmark.cmake)
endif()
