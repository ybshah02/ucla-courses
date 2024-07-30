import random
from sys import stderr 

BOUND_CENETERED_FILE = "boundcentered.fasta"
UNBOUND_CENTERED_FILE = "boundrandomoffset.fasta"

MOTIF_LENGTH = 20
NUM_UPDATES = 60

NUCLEOTIDE_INDEX = {'A': 0, 'C': 1, 'G': 2, 'T': 3}

def process_input(filename):
    sequences = []
    with open(filename, 'r') as file:
        lines = file.readlines()
        sequence = ""
        for i,line in enumerate(lines):
            if line[0] == '>' or i == len(lines)-1:
                if sequence != "":
                    sequences.append(sequence)
                    sequence = ""
            else:
                sequence += line.strip()
    return sequences


def initialize_pwm(sequences, motif_length):
    pwm = [[0, 0, 0, 0] for _ in range(motif_length)]  
    for _ in sequences:
        seq = random.choice(sequences)
        start = random.randint(0, len(seq) - motif_length)
        motif = seq[start:start + motif_length]
        
        for i, nucleotide in enumerate(motif):
            pwm[i][NUCLEOTIDE_INDEX[nucleotide]] += 1

    pwm = [[count / len(sequences) for count in row] for row in pwm]
    return pwm

def score_motif(pwm, motif):
    score = 0
    for i, nucleotide in enumerate(motif):
        score += pwm[i][NUCLEOTIDE_INDEX[nucleotide]]
    return score

def update_pwm(pwm, sequences, motif_length):
    new_pwm = [[0, 0, 0, 0] for _ in range(motif_length)]  

    for seq in sequences:
        best_score = -1
        best_motif = None

        for i in range(len(seq) - motif_length + 1):
            motif = seq[i:i + motif_length]
            current_score = score_motif(pwm, motif)
            if current_score > best_score:
                best_score = current_score
                best_motif = motif

        for i in range(motif_length):
            nucleotide = best_motif[i]
            new_pwm[i][NUCLEOTIDE_INDEX[nucleotide]] += 1

    new_pwm = [[count / len(sequences) for count in row] for row in new_pwm]
    return new_pwm

def predict_peak_summit(sequence, pwm):
    best_score = -1
    best_position = 0 

    for i in range(len(sequence) - MOTIF_LENGTH + 1):
        score = 0 
        for j in range(MOTIF_LENGTH):
            base = sequence[i+j]
            score += pwm[j][NUCLEOTIDE_INDEX[base]]

        if score > best_score:
            best_score = score
            best_position = i + 1 

    return best_position


if __name__ == '__main__':
    print("Processing input...", file=stderr)
    bound_sequences = process_input(BOUND_CENETERED_FILE)
    unbound_sequences = process_input(UNBOUND_CENTERED_FILE)

    print("Generating reverse complements...", file=stderr)
    sequences = bound_sequences 

    print("Initializing PWM...", file=stderr)
    pwm = initialize_pwm(sequences, MOTIF_LENGTH)

    print("Updating PWM...", file=stderr)
    for i in range(NUM_UPDATES):
        pwm = update_pwm(pwm, sequences, MOTIF_LENGTH)
        if i % 10 == 0:
            print(f"Update {i} of {NUM_UPDATES}", file=stderr)

    print("Predicting peak summits...", file=stderr)
    with open('predictions2.txt', 'w') as f:
        for i, sequence in enumerate(unbound_sequences):
            peak = predict_peak_summit(sequence, pwm)
            f.write(f"seq{i+1}\t{peak}\n")
    
    print("Done!", file=stderr)
    
