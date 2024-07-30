from enum import Enum
from collections import defaultdict

class MutationType(Enum):
    Substitution = 1
    Insertion = 2
    Deletion = 3

class Mutation:
    def __init__(self, type, position, ref_gen, donor_gen):
        self.type = type
        self.position = position
        self.ref_gen = ref_gen
        self.donor_gen = donor_gen

    def __str__(self):
        return f"Type: {self.type} Pos: ({self.position}), Ref_Gen: ({self.ref_gen}), Donor_Gen: ({self.donor_gen})\n"
    
    def __repr__(self):
        return f"Mutation(type={self.type}, position={self.position}, ref_gen='{self.ref_gen}', donor_gen='{self.donor_gen}')"

    type: MutationType
    position: int
    ref_gen: str
    donor_gen: str

def read_fasta(filename: str) -> str:
    with open(filename, 'r') as file:
        sequence = ''
        for line in file:
            if line.startswith('>'):
                continue
            sequence += line.strip()
    return sequence

def read_paired_reads(paired):
    with open(paired, 'r') as input_file:
        paired_reads = []
        while True:
            line = input_file.readline()
            if not line:
                break

            if line.strip() == '' or line[0] != '>':
                continue
            read1 = input_file.readline().strip()
            input_file.readline()
            read2 = input_file.readline().strip()
            paired_reads.append((read1, read2))
        return paired_reads

def align(read, reference, match_score, mismatch_penalty, open_penalty, up_score):
    n = len(read)
    m = len(reference)
    dp = [[0] * (m + 1) for _ in range(n + 1)]
    trace = [[(0, 0)] * (m + 1) for _ in range(n + 1)]

    max_score = 0
    max_score_position = (0, 0)

    for i in range(1, n + 1):
        for j in range(1, m + 1):
            match_mismatch_score = dp[i - 1][j - 1] + (match_score if read[i - 1] == reference[j - 1] else mismatch_penalty)
            deletion_score = max(dp[i - 1][j] + open_penalty, dp[i - 1][j] + up_score)
            insertion_score = max(dp[i][j - 1] + open_penalty, dp[i][j - 1] + up_score)
            dp[i][j] = max(0, match_mismatch_score, deletion_score, insertion_score)

            if dp[i][j] == match_mismatch_score:
                trace[i][j] = (i - 1, j - 1)
            elif dp[i][j] == deletion_score:
                trace[i][j] = (i - 1, j)
            else:
                trace[i][j] = (i, j - 1)

            if dp[i][j] > max_score:
                max_score = dp[i][j]
                max_score_position = (i, j)

    mutations = []
    i, j = max_score_position
    while i > 0 and j > 0:
        prev_i, prev_j = trace[i][j]

        if prev_i == i - 1 and prev_j == j - 1:
            if read[i - 1] != reference[j - 1]:
                mutations.append(Mutation(1, j-1, reference[j-1], read[i-1]))
        elif prev_i == i - 1:
            mutations.append(Mutation(2, j-1, '+', read[i-1]))
        else:
            mutations.append(Mutation(3, j-1, reference[j-1], '-'))

        i, j = prev_i, prev_j
        #print((i,j))
    mutations.reverse()

    return max_score, mutations

def mutation_finder(reference, paired_reads, mutation_thresh):
    
    mutations = []

    # Alignment parameters
    match_score = 2
    mismatch_penalty = -1
    open_penalty = -2
    up_score = -2

    base_counts = [{} for _ in range(len(reference))]
    insertion_map = [{} for _ in range(len(reference))]
    # Print statements to track which read # we're on
    i = 0
    for read_pair in paired_reads:
        for read in read_pair:
            print(i)
            i = i+1
            alignment_result = align(read, reference, match_score, mismatch_penalty, open_penalty, up_score)
            read_mutations = alignment_result[1]
            for mutation in read_mutations:
                pos = mutation.position
                base = mutation.donor_gen
                mutation_type = mutation.type
                
                if mutation_type == 2:
                    base_counts[pos]['+'] = base_counts[pos].get('+', 0) + 1
                    insertion_map[pos][base] = insertion_map[pos].get(base, 0) + 1
                elif mutation_type == 3:
                    base_counts[pos]['-'] = base_counts[pos].get('-', 0) + 1
                else:
                    base_counts[pos][base] = base_counts[pos].get(base, 0) + 1

    # Identify mutations
    for pos in range(0, len(reference)):
        total_count = sum(base_counts[pos].values())

        ref_base = reference[pos]
        for base, count in base_counts[pos].items():
            mutation_frequency = count / total_count
            if base != ref_base and (mutation_frequency >= mutation_thresh and count >= 3):
                if base == '+':
                    mutation_type = 2
                elif base == '-':
                    mutation_type = 3
                else:
                    mutation_type = 1

                donor_base = base if base not in '+-' else ' '
                if base == '+':
                    donor_base = next(iter(insertion_map[pos]), ' ')

                new_mutation = Mutation(mutation_type, pos, ref_base, donor_base)
                mutations.append(new_mutation)

    return mutations
    
def main():
    ref_fasta = "project1a_no_repeats_reference_genome.fasta"
    ref_paired_reads = "project1a_no_repeats_with_error_paired_reads.fasta"
    ref = read_fasta(ref_fasta)
    paired_reads = read_paired_reads(ref_paired_reads)

    mute_thresh = 0.5

    mutes = mutation_finder(ref, paired_reads, mute_thresh)

    f = open("predictions.txt", "w")
    for x in range(3):
        for mute in mutes:
            if mute.type == 1 and x == 0:
                f.write('>S' + str(mute.position) + " " + str(mute.ref_gen) + " " + str(mute.donor_gen) + '\n')
            elif mute.type == 2 and x == 1:
                f.write('>I' + str(mute.position) + " "  + str(mute.donor_gen)+ '\n')
            elif x == 2 and mute.type == 3:
                f.write('>D' + str(mute.position) + " " + str(mute.ref_gen)+'\n')
    return 0

if __name__ == '__main__':
    main()