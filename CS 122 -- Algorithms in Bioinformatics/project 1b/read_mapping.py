from collections import defaultdict

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
                if len(r1) == 50 and len(r2) == 50:
                    paired_reads.append((r1, r2))
    return paired_reads

def build_kmer_index(ref, size):
    kmer_index = defaultdict(list)
    for i in range(len(ref) - size + 1):
        kmer = ref[i:i+size]
        kmer_index[kmer].append(i)
    return kmer_index

def hamming_distance(seq1, seq2):
    return sum(ch1 != ch2 for ch1, ch2 in zip(seq1, seq2))

def align_sequence(sequence, ref_seq, count_matrix, insert_map, kmer_idx, kmer_length):
    indels_to_process = identify_potential_indels(sequence, kmer_idx, kmer_length, ref_seq, count_matrix)
    process_indel_sequences(indels_to_process, ref_seq, count_matrix, insert_map, kmer_idx, kmer_length)

def identify_potential_indels(sequence, kmer_idx, kmer_length, ref_seq, count_matrix):
    potential_indels = []
    for start in range(0, len(sequence) - kmer_length + 1, kmer_length):
        kmer = sequence[start:start + kmer_length]
        if kmer in kmer_idx:
            alignment_info = find_optimal_alignment(kmer, sequence, ref_seq, kmer_idx)
            update_count_matrix_on_mismatch(alignment_info, sequence, ref_seq, count_matrix)
            if alignment_info['hamming_distance'] >= 3:
                potential_indels.append(sequence)
    return potential_indels

def find_optimal_alignment(kmer, sequence, ref_seq, kmer_idx):
    optimal_alignment = {'base_alignment': None, 'hamming_distance': float('inf')}
    for kmer_pos in kmer_idx[kmer]:
        alignment_start = kmer_pos - sequence.find(kmer)
        hamming_dist = hamming_distance(ref_seq[alignment_start: alignment_start + len(sequence)], sequence)
        if hamming_dist < optimal_alignment['hamming_distance']:
            optimal_alignment.update({'base_alignment': alignment_start, 'hamming_distance': hamming_dist})
    return optimal_alignment

def update_count_matrix_on_mismatch(alignment_info, sequence, ref_seq, count_matrix):
    if alignment_info['hamming_distance'] < 3:
        base_alignment = alignment_info['base_alignment']
        for i in range(len(sequence)):
            if sequence[i] != ref_seq[base_alignment + i]:
                count_matrix[base_alignment + i][sequence[i]] += 1

def process_indel_sequences(indel_sequences, ref_seq, count_matrix, insert_map, kmer_idx, kmer_length):
    for indel_seq in indel_sequences:
        process_single_indel_sequence(indel_seq, ref_seq, count_matrix, insert_map, kmer_idx, kmer_length)

def process_single_indel_sequence(indel_seq, ref_seq, count_matrix, insert_map, kmer_idx, kmer_length):
    for start in range(len(indel_seq) - kmer_length):
        kmer = indel_seq[start:start + kmer_length]
        if kmer in kmer_idx:
            for kmer_pos in kmer_idx[kmer]:
                alignment_start = kmer_pos - start
                if valid_alignment_start(alignment_start, len(ref_seq), len(indel_seq)):
                    check_for_indels(indel_seq, ref_seq, alignment_start, count_matrix, insert_map)

def valid_alignment_start(alignment_start, ref_length, seq_length):
    return 0 <= alignment_start and alignment_start + seq_length < ref_length

def check_for_indels(indel_seq, ref_seq, align_start, count_matrix, insert_map):
    checked = False
    for x in range(len(indel_seq)):
        if indel_seq[x] != ref_seq[align_start + x]:

            if indel_seq[x:] == ref_seq[align_start + x + 1: align_start + x + 1 + len(indel_seq) - x]:
                checked = True
                count_matrix[align_start + x]['-'] += 1
                break

            if indel_seq[x + 1:] == ref_seq[align_start + x: align_start + x + len(indel_seq) - x - 1]:
                checked = True
                count_matrix[align_start + x - 1]['+'] += 1
                insert_map[align_start + x - 1][indel_seq[x]] += 1
                break

        if checked:
            break

def detect_mutations(ref_genome, paired_seq, freq_threshold, kmer_length):
    kmer_idx = build_kmer_index(ref_genome, kmer_length)

    count_matrix = [defaultdict(int) for _ in range(len(ref_genome))]
    insert_map = [defaultdict(int) for _ in range(len(ref_genome))]

    for pair in paired_seq:
        for seq in pair:
            align_sequence(seq, ref_genome, count_matrix, insert_map, kmer_idx, kmer_length)

    mutation_list = []

    for position in range(len(ref_genome)):
        total_counts = sum(count_matrix[position].values())
        ref_nucleotide = ref_genome[position]

        for base, count in count_matrix[position].items():
            if base != ref_nucleotide and ((count / total_counts) >= freq_threshold) and count >= 3:
                mutation_type, donor_base = determine_mutation_type(base, position, insert_map)
                if mutation_type == 'S':
                    mutation_list.append((mutation_type, position + 1, ref_nucleotide, donor_base))
                else:
                    mutation_list.append((mutation_type, position, ref_nucleotide, donor_base))

    return mutation_list

def determine_mutation_type(base, position, insert_map):
    if base == '+':
        max_base, max_insert = max_insertion(insert_map[position])
        return 'I', max_base
    elif base == '-':
        return 'D', ' '
    else:
        return 'S', base if base != '-' else ' '

def max_insertion(insert_map):
    max_base, max_count = max(insert_map.items(), key=lambda x: x[1])
    return max_base, max_count

def write_mutations_to_file(mutations, filename="predictions.txt"):
    with open(filename, "w") as file:
        for mutation in mutations:
            print(mutation)
            if mutation[0] == 'S':
                file.write(f">{mutation[0]}{mutation[1]} {mutation[2]} {mutation[3]}\n")
            elif mutation[0] == 'I':
                file.write(f">{mutation[0]}{mutation[1]} {mutation[3]}\n")
            elif mutation[0] == 'D':
                file.write(f">{mutation[0]}{mutation[1]} {mutation[2]}\n")

def main():
    # ref_fasta = "sample_reference_genome.fasta"
    # ref_paired_reads = "sample_with_error_paired_reads.fasta"
    ref_fasta = "project1b-u_reference_genome.fasta"
    ref_paired_reads = "project1b-u_with_error_paired_reads.fasta"
    
    reference = parse_fasta(ref_fasta)
    # print(reference)

    paired_reads = read_pairs(ref_paired_reads)
    # print(paired_reads)

    mutation_threshold = 0.4
    kmer_size = 20

    mutations = detect_mutations(reference, paired_reads, mutation_threshold, kmer_size)
    write_mutations_to_file(mutations)

if __name__ == "__main__":
    main()