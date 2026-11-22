// student id: 2025200699
// please change the above line to your student id

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>

typedef struct {
    int valid;
    unsigned long long tag;
    unsigned long long lru_counter;
} CacheLine;

typedef struct {
    CacheLine *lines;
} CacheSet;

typedef struct {
    CacheSet *sets;
    int s;
    int E;
    int b;
    unsigned long long lru_clock;
    int hits;
    int misses;
    int evictions;
} Cache;

void printSummary(int hits, int misses, int evictions)
{
  printf("hits:%d misses:%d evictions:%d\n", hits, misses, evictions);
  FILE *output_fp = fopen(".csim_results", "w");
  assert(output_fp);
  fprintf(output_fp, "%d %d %d\n", hits, misses, evictions);
  fclose(output_fp);
}

void printHelp(const char *name)
{
  printf(
      "Usage: %s [-hv] -s <num> -E <num> -b <num> -t <file>\n"
      "Options:\n"
      "  -h         Print this help message.\n"
      "  -v         Optional verbose flag.\n"
      "  -s <num>   Number of set index bits.\n"
      "  -E <num>   Number of lines per set.\n"
      "  -b <num>   Number of block offset bits.\n"
      "  -t <file>  Trace file.\n\n"
      "Examples:\n"
      "  linux>  %s -s 4 -E 1 -b 4 -t traces/yi.trace\n"
      "  linux>  %s -v -s 8 -E 2 -b 4 -t traces/yi.trace\n",
      name, name, name);
}

void accessCache(Cache *cache, unsigned long long addr, int verbose, char op)
{
    unsigned long long set_index = (addr >> cache->b) & ((1ULL << cache->s) - 1);
    unsigned long long tag = addr >> (cache->s + cache->b);
    
    CacheSet *set = &cache->sets[set_index];
    int empty_idx = -1;
    int lru_idx = 0;
    unsigned long long min_lru = -1;

    for (int i = 0; i < cache->E; i++) {
        if (set->lines[i].valid) {
            if (set->lines[i].tag == tag) {
                set->lines[i].lru_counter = ++cache->lru_clock;
                cache->hits++;
                if (verbose) printf("%c %llx, hit\n", op, addr);
                return;
            }
            if (set->lines[i].lru_counter < min_lru) {
                min_lru = set->lines[i].lru_counter;
                lru_idx = i;
            }
        } else {
            if (empty_idx == -1) empty_idx = i;
        }
    }

    cache->misses++;
    if (empty_idx != -1) {
        set->lines[empty_idx].valid = 1;
        set->lines[empty_idx].tag = tag;
        set->lines[empty_idx].lru_counter = ++cache->lru_clock;
        if (verbose) printf("%c %llx, miss\n", op, addr);
    } else {
        cache->evictions++;
        set->lines[lru_idx].tag = tag;
        set->lines[lru_idx].lru_counter = ++cache->lru_clock;
        if (verbose) printf("%c %llx, miss eviction\n", op, addr);
    }
}

int main(int argc, char *argv[])
{
    int s = -1, E = -1, b = -1;
    char *trace_file = NULL;
    int verbose = 0;
    int opt;

    while ((opt = getopt(argc, argv, "hvs:E:b:t:")) != -1) {
        switch (opt) {
            case 's': s = atoi(optarg); break;
            case 'E': E = atoi(optarg); break;
            case 'b': b = atoi(optarg); break;
            case 't': trace_file = optarg; break;
            case 'v': verbose = 1; break;
            case 'h': printHelp(argv[0]); return 0;
            default: printHelp(argv[0]); return 1;
        }
    }

    if (s <= 0 || E <= 0 || b <= 0 || trace_file == NULL) {
        printf("%s: Missing required command line argument\n", argv[0]);
        printHelp(argv[0]);
        return 1;
    }

    Cache cache;
    cache.s = s;
    cache.E = E;
    cache.b = b;
    cache.lru_clock = 0;
    cache.hits = 0;
    cache.misses = 0;
    cache.evictions = 0;

    int num_sets = 1 << s;
    cache.sets = (CacheSet *)malloc(num_sets * sizeof(CacheSet));
    assert(cache.sets != NULL);

    for (int i = 0; i < num_sets; i++) {
        cache.sets[i].lines = (CacheLine *)malloc(E * sizeof(CacheLine));
        assert(cache.sets[i].lines != NULL);
        for (int j = 0; j < E; j++) {
            cache.sets[i].lines[j].valid = 0;
            cache.sets[i].lines[j].tag = 0;
            cache.sets[i].lines[j].lru_counter = 0;
        }
    }

    FILE *fp = fopen(trace_file, "r");
    if (fp == NULL) {
        printf("Error: Could not open trace file %s\n", trace_file);
        return 1;
    }

    char line[256];
    while (fgets(line, sizeof(line), fp) != NULL) {
        char op;
        unsigned long long addr;
        int size;
        
        if (sscanf(line, " %c %llx,%d", &op, &addr, &size) >= 2) {
            if (op == 'I') continue;
            
            if (op == 'L' || op == 'S') {
                accessCache(&cache, addr, verbose, op);
            } else if (op == 'M') {
                accessCache(&cache, addr, verbose, 'L');
                accessCache(&cache, addr, verbose, 'S');
            }
        }
    }
    fclose(fp);

    printSummary(cache.hits, cache.misses, cache.evictions);

    for (int i = 0; i < num_sets; i++) {
        free(cache.sets[i].lines);
    }
    free(cache.sets);

    return 0;
}