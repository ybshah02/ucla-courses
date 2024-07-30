from collections import defaultdict

READS_FILE = "project2b_reads.fasta"
PAIRED_READS_FILE = "project2b_paired_reads.fasta"

K = 10

def generate_spectrum_from_reads(patterns, k=K):
    kmer_count_map = defaultdict(int)
    
    for pattern in patterns:
        for i in range(len(pattern) - k + 1):
            kmer_count_map[pattern[i:i+k]] += 1
    return [kmer for kmer, count in kmer_count_map.items() if count >= 3]

def create_cycle_from_path(input_graph, start_node):
    cycle_items = []
    remaining_edges = [start_node]
    while remaining_edges:
        current_node = remaining_edges[-1]
        if current_node in input_graph and input_graph[current_node]:
            edge = input_graph[current_node][-1]
            input_graph[current_node] = input_graph[current_node][:-1]
            remaining_edges.append(edge)
        else:
            cycle_items.append(remaining_edges[-1])
            remaining_edges = remaining_edges[:-1]
    cycle_items = cycle_items[::-1][:-1]
    return cycle_items

def create_path(input_graph):
    end_node = ""
    for node, edges in input_graph.items():
        for edge in edges:
            if edge not in input_graph:
                end_node = edge

    start_node = ""
    for node, edges in input_graph.items():
        count_elements = sum(edge == node for edges in input_graph.values() for edge in edges)
        if len(edges) > count_elements:
            start_node = node

    input_graph.setdefault(end_node, []).append(start_node)
    return create_cycle_from_path(input_graph, start_node)

def combine_path(items):
    return ''.join([item[len(items[0])-1] for item in items])

def reconstruct_string(patterns):
    de_bruijn_graph = {}
    for pattern in patterns:
        de_bruijn_graph.setdefault(pattern[:-1], list()).append(pattern[1:])

    euler_graph = {}
    for key, values in de_bruijn_graph.items():
        euler_graph[key] = list(values)

    return combine_path(create_path(euler_graph))

def process_read_file(filename):
    reads = []
    with open(filename, 'r') as file:
        lines = file.readlines()
        for line in lines:
            if not line.startswith('>'):
                reads.append(line.strip())
    return reads

if __name__ == '__main__':
    reads = process_read_file(READS_FILE) 
    spectrum_items = generate_spectrum_from_reads(reads)
    reconstructed_genome = reconstruct_string(spectrum_items)

    kmer_index = defaultdict(list)
    for index in range(len(reconstructed_genome) - K + 1):
        kmer_index[reconstructed_genome[index:index+K]].append(index)
    

    read_positions_map = defaultdict(list)
    for read_number, read in enumerate(reads):
        for index in range(len(read) - K + 1):
            kmer = read[index:index+K]
            if kmer in kmer_index:
                for position in kmer_index[kmer]:
                    read_positions_map[read_number].append(position - index)

    final_positions = {}
    for read_number, positions in read_positions_map.items():
        final_positions[read_number] = max(set(positions), key=positions.count)

    for read_number, _ in sorted(final_positions.items(), key=lambda x: x[1]):
        print(f">read_{read_number}")