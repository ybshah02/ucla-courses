from collections import defaultdict

K_SIZE = 10

def load_reads_from_file(fpath):
    with open(fpath, 'r') as f:
        sequence_reads = [line.strip() for line in f if not line.startswith('>')]
    return sequence_reads

def get_kmers_with_min_count(reads, k=K_SIZE):
    kmer_frequency = defaultdict(int)
    
    for read in reads:
        for i in range(len(read) - k + 1):
            kmer = read[i:i+k]
            kmer_frequency[kmer] += 1
    
    frequent_kmers = []
    for kmer, freq in kmer_frequency.items():
        if freq >= 3:
            frequent_kmers.append(kmer)
    
    return frequent_kmers

def build_eulerian_cycle(graph, start):
    path_elements = []
    stack = [start]
    while stack:
        if not (stack[-1] in graph and graph[stack[-1]]):
            path_elements.append(stack.pop())
        else:
            next_node = graph[stack[-1]].pop()
            stack.append(next_node)
    
    return path_elements[::-1][1:]

def find_eulerian_path(graph):
    start_of_path = end_of_path = None
    edge_balance = defaultdict(int)
    
    for origin, destinations in graph.items():
        for destination in destinations:
            edge_balance[origin] += 1
            edge_balance[destination] -= 1
    
    for node, balance in edge_balance.items():
        if balance == 1: 
            start_of_path = node
        elif balance == -1:
            end_of_path = node
    
    if end_of_path and end_of_path not in graph:
        graph[end_of_path] = []
    
    if end_of_path and start_of_path:
        graph[end_of_path].append(start_of_path)
    
    return build_eulerian_cycle(graph, start_of_path)

def assemble_genome_from_kmers(patterns):
    graph_construction = {}
    for sequence in patterns:
        prefix = sequence[:-1]
        suffix = sequence[1:]
        if prefix not in graph_construction:
            graph_construction[prefix] = [suffix]
        else:
            graph_construction[prefix].append(suffix)

    path_graph = {}
    for node, edges in graph_construction.items():
        path_graph[node] = edges.copy()

    path_sequence = find_eulerian_path(path_graph)
    return ''.join([kmer[len(path_sequence[0])-1] for kmer in path_sequence])

def map_reads_to_genome_positions(assembled_genome, reads, k_size):
    kmer_index = defaultdict(list)
    for i in range(len(assembled_genome) - k_size + 1):
        kmer = assembled_genome[i:i+k_size]
        kmer_index[kmer].append(i)

    kmer_positions = defaultdict(list)
    for idx, read in enumerate(reads):
        for i in range(len(read) - k_size + 1):
            kmer = read[i:i+k_size]
            if kmer in kmer_index:
                for position in kmer_index[kmer]:
                    kmer_positions[idx].append(position - i)

    read_to_position = {}
    for read_idx, positions in kmer_positions.items():
        if positions:
            read_to_position[read_idx] = max(set(positions), key=positions.count)
    
    return read_to_position

def save_positions_to_file(position_map):
    with open('predictions.txt', 'w') as output_file:
        for read_num, pos in sorted(position_map.items(), key=lambda x: x[1]):
            output_file.write(f">read_{read_num}\n")

def main():

    #reads = "project2_sample2_reads.fasta"
    reads = "project2b_reads.fasta"

    single_reads = load_reads_from_file(reads) 
    selected_kmers = get_kmers_with_min_count(single_reads)
    genome_assembly = assemble_genome_from_kmers(selected_kmers)

    read_positions = map_reads_to_genome_positions(genome_assembly, single_reads, K_SIZE)
    save_positions_to_file(read_positions)

if __name__ == '__main__':
    main()
