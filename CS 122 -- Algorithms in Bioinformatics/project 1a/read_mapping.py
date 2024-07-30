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

def write_mutations_to_file(mutations, filename="predictions.txt"):
    with open(filename, "w") as file:
        for mutation in mutations:
            if mutation[0] == 'S':
                file.write(f">{mutation[0]}{mutation[1]} {mutation[2]} {mutation[3]}\n")
            elif mutation[0] == 'I':
                file.write(f">{mutation[0]}{mutation[1]} {mutation[3]}\n")
            elif mutation[1] == 'D':
                file.write(f">{mutation[0]}{mutation[1]} {mutation[2]}\n")

if __name__ == "__main__":
    ref_fasta = "project1a_no_repeats_reference_genome.fasta"
    ref_paired_reads = "project1a_no_repeats_with_error_paired_reads.fasta"
    #ref_fasta = "sample_reference_genome.fasta"
    #ref_paired_reads = "sample_with_error_paired_reads.fasta"
    
    reference = parse_fasta(ref_fasta)
    #print(reference)
    paired_reads = read_pairs(ref_paired_reads)
    #print(paired_reads)

    mutation_threshold = 0.40
    
    mutations = find_mutations(reference, paired_reads, mutation_threshold)
    write_mutations_to_file(mutations)