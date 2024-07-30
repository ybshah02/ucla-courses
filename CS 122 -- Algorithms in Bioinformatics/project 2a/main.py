from collections import defaultdict, deque

def combine_path(path):
    return ''.join([p[0] for p in path]) + path[-1][1:]

def create_cycle_from_path(graph, start_node):
    cycle = []
    edges = deque([start_node])

    while edges:
        node = edges[-1]
        if not graph[node]:
            next_node = edges.pop()
            cycle.append(next_node)
        else:
            next_node = graph[node].pop(0)
            edges.append(next_node)

    cycle.reverse()
    return cycle[:-1]

def identify_start_and_end_nodes(graph):
    end_node = None
    for node in graph:
        for edge in graph[node]:
            if edge not in graph:
                end_node = edge
                break
        if end_node:
            break
    
    start_node = None
    for node in graph:
        outgoing = len(graph[node])
        incoming = 0
        for edges in graph.values():
            incoming += edges.count(node)
        if outgoing > incoming:
            start_node = node
            break
    
    return start_node, end_node

def adjust_graph_for_cycle(graph):
    start_node, end_node = identify_start_and_end_nodes(graph)
    if end_node is not None and start_node is not None:
        graph[end_node] = [start_node]
    return start_node

def create_path(graph):
    start_node = adjust_graph_for_cycle(graph)
    if start_node:
        return create_cycle_from_path(graph, start_node)
    else:
        return []

def generate_de_bruijn_graph(patterns, k):
    graph = defaultdict(set)
    prefix_length = k - 1
    for pattern in patterns:
        prefix = pattern[:prefix_length]
        suffix = pattern[1:]
        graph[prefix].add(suffix)
    return {node: list(edges) for node, edges in graph.items()}

def find_eulerian_path(graph):
    start_node = adjust_graph_for_cycle(graph)
    if start_node:
        cycle = create_cycle_from_path(graph, start_node)
        return cycle
    else:
        return []

def reconstruct_string(segments, k):
    de_bruijn_graph = generate_de_bruijn_graph(segments, k)
    eulerian_path = find_eulerian_path(de_bruijn_graph)
    
    rebuilt_string = ''
    for i in range(len(eulerian_path)):
        if i == 0:
            rebuilt_string += eulerian_path[i]
        else:
            rebuilt_string += eulerian_path[i][-1]
    return rebuilt_string

def find_pattern(pattern_list, kmer):
    try:
        return pattern_list.index(kmer)
    except ValueError:
        return -1

def write_to_file(read_pos_map):
    with open('predictions.txt', 'w') as f:
        for i, read_num in read_pos_map.items():
            f.write(f">read_{read_num}\n")
    
def main():
    #fname = 'project2_sample1_spectrum.fasta'
    fname = 'project2a_spectrum.fasta'

    with open(fname, "r") as f:
        spectrum = [line.strip() for line in f if not line.startswith('>')]

    k = len(spectrum[0])
    reconstructed_genome = reconstruct_string(spectrum, k)

    read_pos_map = {}
    for i in range(len(reconstructed_genome) - k + 1):
        window = reconstructed_genome[i:i+k]
        read_num = find_pattern(spectrum, window)
        if read_num != -1:
            read_pos_map[i] = read_num

    write_to_file(read_pos_map)

if __name__ == "__main__":
    main()