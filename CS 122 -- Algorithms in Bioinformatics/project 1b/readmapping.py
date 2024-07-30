from enum import Enum
from collections import defaultdict
import argparse

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
            if len(read1) == 50 and len(read2) == 50:
                paired_reads.append((read1, read2))
        return paired_reads

def hamming_distance(read, ref):
    ct = 0
    for i in range(len(read)):
        if read[i] != ref[i]:
            ct = ct + 1
    return ct

def kmer_indexes(reference, kmer_size):
    kmer_index = defaultdict(list)
    for i in range(0, len(reference) - kmer_size - 1):
        kmer = reference[i:i+kmer_size]
        kmer_index[kmer].append(i)
    return kmer_index

def sw_align(read, reference, base):
    #based on smith waterman
    n = len(read)
    m = len(reference)
    dp = [[0] * (m + 1) for _ in range(n + 1)]
    trace = [[(0, 0)] * (m + 1) for _ in range(n + 1)]
    max_score = 0
    max_score_position = (0, 0)

    for i in range(1, n + 1):
        for j in range(1, m + 1):
            match_mismatch_score = dp[i - 1][j - 1] + (0 if read[i - 1] == reference[j - 1] else -1)
            deletion_score = max(dp[i - 1][j] + -2, dp[i - 1][j] + -2)
            insertion_score = max(dp[i][j - 1] + -2, dp[i][j - 1] + -2)
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
                mutations.append(Mutation(1, base + j-1, reference[j-1], read[i-1]))
        elif prev_i == i - 1:
            mutations.append(Mutation(2, base + j-1, '+', read[i-1]))
        else:
            mutations.append(Mutation(3, base + j-1, reference[j-1], '-'))

        i, j = prev_i, prev_j
        #print((i,j))
    mutations.reverse()

    return max_score, mutations


def align(read, reference, kmer_index, kmer_size, base_counts, insertion_map):
    indels = []
    broke = False
    for i in range(0, len(read) - kmer_size + 1, kmer_size):
        kmer = read[i:i+kmer_size]

        if kmer in kmer_index:
            if kmer_index[kmer][0] < i:
                continue
            distance = hamming_distance(reference[kmer_index[kmer][0]: kmer_index[kmer][0]+len(read)-1], read)
            align = kmer_index[kmer][0] - i
            for x in kmer_index[kmer]:
                dist = hamming_distance(reference[x-i:x-i+len(read)], read)

                if (dist < distance):
                    distance = dist 
                    align = x - i
            
            if distance < 3:
                if read == reference[align:align+len(read)]:
                    break #we have a match
                else:
                    for letter in range(len(read)):
                        if read[letter] != reference[align + letter]:
                           base_counts[align + letter][read[letter]] = base_counts[align + letter][read[letter]] + 1
            else:

                indels.append(read)

    for indel in indels:
        for i in range(0, len(indel)-kmer_size, kmer_size):
            kmer = indel[i:i+kmer_size]

            if kmer not in kmer_index:
                continue

            for index in kmer_index[kmer]:
                if (index - i < 0 or index - i + len(indel) >= len(reference)):
                    continue

                alignment = reference[index-i: index-i + len(indel)]
                
                #p = 0
                #insertion_windows = [(129, 135), (749, 755), (15, 25)]
                broke = False
                for x in range(len(indel)):
                    if indel[x] != alignment[x]:
                        printFlag = False
                        if index - i + x + 1 < 0 or index - i + len(indel) + 1 >= len(reference):
                            continue
                        
                        # windowCheck = index - i + x + 1
                        #if (windowCheck > 129 and windowCheck < 135) or (windowCheck > 749 and windowCheck < 756) or (windowCheck > 13 and windowCheck < 21):
                        #    printFlag = True
                        #    print("indel:", indel)
                        #    print("align:", alignment)
                        #    p = p + 1 
                        
                        substr_read = indel[x:]

                        substr_align = reference[index - i + x + 1: index - i + x + 1 + len(indel) - x]
                        
                        '''if (printFlag):
                            print("Ind del", index - i + x + 1)
                            print(substr_read)
                            print(substr_align)'''
                        if substr_read == substr_align:
                            #print("Deletion")
                            base_counts[index - i + x]['-'] += 1
                            broke = True
                            break

                        substr_read = indel[x + 1:]
                        substr_align = reference[index - i + x: index - i + x + len(indel) - x - 1]
                        
                        if (printFlag):
                            print("Ind ", index - i + x + 1)
                            print(substr_read)
                            print(substr_align)

                        if substr_read == substr_align:
                            #print("Pos of insertion: ", index - i + x - 1)
                            base_counts[index - i + x - 1]['+'] += 1
                            insertion_map[index - i + x - 1][indel[x]] += 1
                            broke = True
                            break
                if broke:
                    break        
                '''
                code artifact from attempting to use L/3 matching

                if indel[:16] == alignment[:16]:
                    # first 16 match
                    if indel[:34] == alignment[:34]:
                        #middle 16 match, therefore the issue is in the end
                        rangeStart = 34
                    else:
                        rangeStart = 16
                        # last 16 don't match, issue is somewhere in the middle to end
                else:
                    rangeStart = 0
                    
                for x in range(rangeStart, len(indel)):

                    if indel[x] != alignment[x]:
                        #check for an insertion in the reference genome,
                        #aka a deletion in the read
                        substr_read = indel[x:]
                        substr_align = reference[index - i + x + 1: index - i + x + 1 + len(indel) - x]
                        
                        if (substr_read == substr_align):
                            base_counts[index - i + x - 1]['-'] = base_counts[index - i + x - 1]['-'] + 1
                            broke = True
                            break
                        
                        #check for an insertion in the read (extra base)
                        # aka a deletion in the ref genome 

                        substr_read = indel[x + 1:]
                        substr_align = reference[index - i + x: index - i + x + len(indel) - x - 1]
                        
                        if (substr_read == substr_align):
                            base_counts[index - i + x + 1]['+'] = base_counts[index - i + x + 1]['+'] + 1
                            insertion_map[index - i + x + 1][indel[x]] = insertion_map[index - i + x + 1][indel[x]] + 1
                            broke = True
                            break
                '''
                '''
                attempting to use SW as a second alignment mutation check
                score, muts = sw_align(indel, alignment, index)
                #print("Ind: ", index)
                #print("Indel: ", indel)
                #print("Align: ", alignment)
                #print(muts)

                for mutation in muts:
                    pos = mutation.position
                    base = mutation.donor_gen
                    mutation_type = mutation.type
                    
                    if mutation_type == 2:
                        base_counts[pos]['+'] = base_counts[pos].get('+', 0) + 1
                        insertion_map[pos][base] = insertion_map[pos].get(base, 0) + 1
                    elif mutation_type == 3:
                        base_counts[pos]['-'] = base_counts[pos].get('-', 0) + 1
                '''
                '''
                if indel[:16] == alignment[:16]:
                    if indel[34:] == alignment[34:]:
                        short_indel = indel[16:34]
                        short_alignment = alignment[16:34]
                        mismatchLocation, type = findMismatch(short_indel[:len(short_indel)-1//2], short_alignment[:len(short_indel)-1//2], 16)
                        print(mismatchLocation, type)
                        base_counts[index - i + mismatchLocation - 1][type] = base_counts[index - i + mismatchLocation - 1][type] + 1
                    else:
                        short_indel = indel[34:]
                        short_alignment = alignment[34:]
                        mismatchLocation, type = findMismatch(short_indel[:len(short_indel)-1//2], short_alignment[:len(short_indel)-1//2], 34)
                        print(mismatchLocation, type)
                        print(index - i + mismatchLocation - 1, type)
                        base_counts[index - i + mismatchLocation - 1][type] = base_counts[index - i + mismatchLocation - 1][type] + 1
                else:
                    short_indel = indel[:16]
                    short_alignment = alignment[:16]
                    mismatchLocation, type = findMismatch(short_indel, short_alignment, 0)
                    base_counts[index - i + mismatchLocation - 1][type] = base_counts[index - i + mismatchLocation - 1][type] + 1
                if (type == "+"):
                    insertion_map[index - i + mismatchLocation - 1][indel[mismatchLocation]] = insertion_map[index - i + mismatchLocation - 1][indel[mismatchLocation]] + 1
                '''

# pass in a reference string and an indel substring, find the position of the mismatch
def findMismatch(ref, indel, start):
    for i in range(len(indel)):
        if ref[i] == indel[i]:
            continue
        else:
            if i > 0:
                if ref[i] == indel[i-1]:
                # this is a deletion
                    return (i + start, "-")
                elif ref[i-1] == indel[i]:
                #this is an insertion
                    return (i + start, "+")
    return (start, ' ')

def mutation_finder(reference, paired_reads, mutation_thresh, kmer_size):
    
    mutations = []

    base_counts = [defaultdict(int) for _ in range(len(reference))]
    insertion_map = [defaultdict(int) for _ in range(len(reference))]
    kmer_index = kmer_indexes(reference, kmer_size)
    # Print statements to track which read # we're on
    i = 0
    for read_pair in paired_reads:
        for read in read_pair:
            #print(i)
            #i = i+1

            align(read, reference, kmer_index, kmer_size, base_counts, insertion_map)
            
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
                    pos = pos + 1
                    mutation_type = 1

                if base == '+':
                    #print("Position of insertion: ", pos)
                    #print("Mute type:", mutation_type)
                    max_base = ' '
                    max_insert = 0

                    # choose most likely insert
                    for donor in insertion_map[pos]:
                        val = insertion_map[pos][donor] 
                        if  val > max_insert:
                            max_base = donor
                            max_insert = val                    
                    donor_base = max_base
                else:
                    donor_base = ' ' if base == '-' else base
                new_mutation = Mutation(mutation_type, pos, ref_base, donor_base)
                mutations.append(new_mutation)

    return mutations
    
def main(ref_fasta, ref_paired_reads):
    ref = read_fasta(ref_fasta)
    paired_reads = read_paired_reads(ref_paired_reads)

    mute_thresh = 0.5
    kmer_size = 21

    mutes = mutation_finder(ref, paired_reads, mute_thresh, kmer_size)

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
    # Set up argument parser
    parser = argparse.ArgumentParser(description="Process some files.")
    parser.add_argument('--sample', action='store_true', help='Use sample files')
    parser.add_argument('--grad', action='store_true', help='Use grad files')

    # Parse arguments
    args = parser.parse_args()
    if args.sample:
        ref_fasta = "sample_reference_genome.fasta"
        ref_paired_reads = "sample_no_error_paired_reads.fasta"
    elif args.grad:
        ref_fasta = "project1b-g_reference_genome.fasta"
        ref_paired_reads = "project1b-g_with_error_paired_reads.fasta"
    else:
        ref_fasta = "project1b-u_reference_genome.fasta"
        ref_paired_reads = "project1b-u_with_error_paired_reads.fasta"
    
    main(ref_fasta, ref_paired_reads)