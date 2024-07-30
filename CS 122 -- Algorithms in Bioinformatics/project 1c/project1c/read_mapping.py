def parse_fasta(fname):
    with open(fname, 'r') as f:
        seq = ''
        for line in f:
            if line.startswith('>') or line.strip() == '':
                continue
            seq += line.strip()
    return seq

def read_pairs(fname):
    with open(fname, 'r') as f:
        paired_reads = []
        for line in f:
            if line.startswith('>'):
                r1 = next(f).strip()
                next(f)
                r2 = next(f).strip()
                paired_reads.append((r1, r2))
    return paired_reads

def initialize_matrices(sequence_len, genome_len):
    dp_matrix = [[0] * (genome_len + 1) for _ in range(sequence_len + 1)]
    path_matrix = [[(0, 0) for _ in range(genome_len + 1)] for _ in range(sequence_len + 1)]
    return dp_matrix, path_matrix

def calculate_scores(sequence, ref_genome, dp_matrix, path_matrix, match, mismatch, gap_open, gap_extend):
    highest_score = 0
    position_max = (0, 0)
    seq_len = len(sequence)
    genome_len = len(ref_genome)

    for i in range(1, seq_len + 1):
        for j in range(1, genome_len + 1):
            score_deletion = max(dp_matrix[i-1][j] + gap_open, dp_matrix[i-1][j] + gap_extend)
            score_insertion = max(dp_matrix[i][j-1] + gap_open, dp_matrix[i][j-1] + gap_extend)
            score_substitution = dp_matrix[i-1][j-1] + (match if sequence[i-1] == ref_genome[j-1] else mismatch)

            dp_matrix[i][j] = max(0, score_substitution, score_deletion, score_insertion)
            path_matrix[i][j] = (i-1, j-1) if dp_matrix[i][j] == score_substitution else (i-1, j) if dp_matrix[i][j] == score_deletion else (i, j-1)

            if dp_matrix[i][j] > highest_score:
                highest_score = dp_matrix[i][j]
                position_max = (i, j)
    
    return position_max

def trace_mutations(sequence, ref_genome, position_max, path_matrix):
    mut_list = []
    x, y = position_max
    while x > 0 and y > 0:
        prev_x, prev_y = path_matrix[x][y]
        if prev_x == x-1 and prev_y == y-1:
            if sequence[x-1] != ref_genome[y-1]:
                mut_list.append(('S', y-1, ref_genome[y-1], sequence[x-1]))
        elif prev_x == x-1:
            mut_list.append(('I', y-1, '+', sequence[x-1]))
        else:
            mut_list.append(('D', y-1, ref_genome[y-1], '-'))

        x, y = prev_x, prev_y

    return mut_list[::-1]

def update_counts(mut_list, count_matrix, insertions_tracker):
    for mut in mut_list:
        mut_type, idx, _, mut_base = mut
        if mut_type == 'I':
            count_matrix[idx].setdefault('+', 0)
            count_matrix[idx]['+'] += 1
            insertions_tracker[idx].setdefault(mut_base, 0)
            insertions_tracker[idx][mut_base] += 1
        elif mut_type == 'D':
            count_matrix[idx].setdefault('-', 0)
            count_matrix[idx]['-'] += 1
        else:
            count_matrix[idx].setdefault(mut_base, 0)
            count_matrix[idx][mut_base] += 1

def find_mutations(ref_genome, read_pairs, frequency_threshold):
    penalty_gap_extension = -2
    penalty_gap_opening = -2
    penalty_mismatch = -1
    score_match = 2

    count_matrix = [{} for _ in range(len(ref_genome))]
    insertions_tracker = [{} for _ in range(len(ref_genome))]

    for pair in read_pairs:
        for seq in pair:
            dp_matrix, path_matrix = initialize_matrices(len(seq), len(ref_genome))
            position_max = calculate_scores(seq, ref_genome, dp_matrix, path_matrix, score_match, penalty_mismatch, penalty_gap_opening, penalty_gap_extension)
            mut_list = trace_mutations(seq, ref_genome, position_max, path_matrix)
            update_counts(mut_list, count_matrix, insertions_tracker)

    return identify_mutations(ref_genome, count_matrix, insertions_tracker, frequency_threshold)

def identify_mutations(ref_genome, count_matrix, insertions_tracker, frequency_threshold):
    mutations_identified = []
    for pos, base_counts in enumerate(count_matrix):
        base_total = sum(base_counts.values())
        if base_total == 0:
            continue

        for base, count in base_counts.items():
            frequency = count / base_total
            if base != ref_genome[pos] and frequency >= frequency_threshold and count > 3:
                mut_type = 'I' if base == '+' else 'D' if base == '-' else 'S'
                donor_base = next(iter(insertions_tracker[pos])) if mut_type == 'I' else '' if mut_type == 'D' else base
                mutations_identified.append((mut_type, pos, ref_genome[pos], donor_base))

    return mutations_identified

def map_reads_to_genome(reads, genome, genome_id):
    """
    Map each read to the given genome.
    This is a placeholder function and should be replaced with a specialized algorithm.
    """
    read_mappings = {}
    for read_id, read_seq in reads:
        # Placeholder for mapping logic: matching read_seq to genome
        # In practice, use algorithms like BLAST, BWA, or Bowtie
        # For simplicity, let's assume every read maps to the genome
        read_mappings[read_id] = genome_id
    return read_mappings

def deduce_sources(read_mappings):
    """
    Deduce the most probable source genome for each read.
    """
    read_sources = {}
    for read_id, genome_id in read_mappings.items():
        # In a real scenario, this would involve complex logic to handle overlaps and duplicates
        # For simplicity, assume the read maps to the provided genome_id
        read_sources[read_id] = genome_id
    return read_sources

def write_answer_file(read_sources, filename="project1c_answers.txt"):
    """
    Write the answer file with read headers and their deduced source genomes.
    """
    with open(filename, "w") as file:
        for read_id, genome_id in read_sources.items():
            file.write(f"{read_id}\t{genome_id}\n")

if __name__ == "__main__":
    genome_ids = [str(i) for i in range(0, 100)]  # Create a list of genome IDs from 1 to 99
    genome_sequences = {genome_id: parse_fasta(f"project1c_genome_{genome_id}.fasta") for genome_id in genome_ids}
    sample_reads = read_pairs("project1c_reads.fasta")

    read_mappings = {}
    for genome_id, genome_seq in genome_sequences.items():
        mappings = map_reads_to_genome(sample_reads, genome_seq, genome_id)
        read_mappings.update(mappings)

    read_sources = deduce_sources(read_mappings)
    write_answer_file(read_sources)