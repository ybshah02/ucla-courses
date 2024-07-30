import random

NUCLEOTIDES = {'A': 0, 'C': 1, 'G': 2, 'T': 3}

def parse_fasta(fname):
    sequences = []
    sequence = ""

    with open(fname, 'r') as file:
        for line in file:
            line = line.strip()
            if line.startswith('>'):
                if sequence:
                    sequences.append(sequence)
                    sequence = ""
            else:
                sequence += line
        if sequence:
            sequences.append(sequence)

    return sequences

def init(seqs, motif_length):
    counts = {i: {'A': 0, 'C': 0, 'G': 0, 'T': 0} for i in range(motif_length)}
    
    for _ in seqs:
        seq = random.choice(seqs)
        start = random.randint(0, len(seq) - motif_length)
        motif = seq[start:start + motif_length]
        
        for i, nucleotide in enumerate(motif):
            counts[i][nucleotide] += 1
    
    pwm = [[counts[i][nuc] / len(seqs) for nuc in 'ACGT'] for i in range(motif_length)]
    
    return pwm

def get_score(pwm, motif):
    return sum(pwm[i][NUCLEOTIDES[nuc]] for i, nuc in enumerate(motif))

def find_best_motif(seq, pwm, motif_length):
    best_motif, best_score = '', float('-inf')
    for i in range(len(seq) - motif_length + 1):
        motif = seq[i:i+motif_length]
        score = get_score(pwm, motif)
        if score > best_score:
            best_score, best_motif = score, motif
    return best_motif


def update(pwm, sequences, motif_length):
    updated_pwm = [[0] * 4 for _ in range(motif_length)]
    sequence_count = len(sequences)

    for seq in sequences:
        best_motif = find_best_motif(seq, pwm, motif_length)
        if best_motif:
            for i, nuc in enumerate(best_motif):
                updated_pwm[i][NUCLEOTIDES[nuc]] += 1

    for i in range(motif_length):
        for j in range(4):
            updated_pwm[i][j] /= sequence_count

    return updated_pwm

def find_peak(seq, pwm):
    scores = [(i+1, sum(pwm[j][NUCLEOTIDES[seq[i+j]]] for j in range(20))) for i in range(len(seq) - 19)]
    best_position, _ = max(scores, key=lambda x: x[1])
    return best_position

def main():
    sequences = parse_fasta('boundcentered.fasta') 
    pwm = init(sequences, 20)

    for i in range(60):
        pwm = update(pwm, sequences, 20)

    with open('predictions.txt', 'w') as f:
        for i, sequence in enumerate(parse_fasta('boundrandomoffset.fasta')):
            peak = find_peak(sequence, pwm)
            f.write(f"seq{i+1}\t{peak}\n")

if __name__ == '__main__':
    main()
    