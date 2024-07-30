import random

def read_fasta(filename):
    sequences = {}
    with open(filename, 'r') as file:
        for line in file:
            line = line.strip()
            if line.startswith('>'):
                current_key = line[1:]
                sequences[current_key] = ''
            else:
                sequences[current_key] += line
    return sequences

def generate_pwm(motifs, k, pseudo_count=1):
    pwm = [[pseudo_count for _ in range(4)] for _ in range(k)]

    for motif in motifs:
        for i, nucleotide in enumerate(motif):
            if nucleotide == 'A':
                pwm[i][0] += 1
            elif nucleotide == 'C':
                pwm[i][1] += 1
            elif nucleotide == 'G':
                pwm[i][2] += 1
            elif nucleotide == 'T':
                pwm[i][3] += 1

    for i in range(k):
        col_sum = sum(pwm[i])
        for j in range(4):
            pwm[i][j] /= col_sum

    return pwm

def calculate_pwm_score(sequence, pwm):
    max_score = float('-inf')
    motif_length = len(pwm)
    for i in range(len(sequence) - motif_length + 1):
        score = sum(pwm[j]['ACGT'.index(sequence[i+j])] for j in range(motif_length))
        max_score = max(max_score, score)
    return max_score

def reverse_complement(seq):
    complement = {'A': 'T', 'T': 'A', 'C': 'G', 'G': 'C'}
    return ''.join(complement[base] for base in reversed(seq))

def greedy_motif_search(sequences, k):
    motifs = [seq[:k] for seq in sequences]
    best_pwm = generate_pwm(motifs, k)
    
    for _ in range(10): 
        for i, seq in enumerate(sequences):
            best_score = float('-inf')
            best_motif = ''
            for j in range(len(seq) - k + 1):
                motif = seq[j:j+k]
                score = calculate_pwm_score(motif, best_pwm)
                if score > best_score:
                    best_score = score
                    best_motif = motif
            motifs[i] = best_motif
        best_pwm = generate_pwm(motifs, k)

    return best_pwm

def predict_sequences(test_sequences, pwm):
    predictions = {}
    for name, seq in test_sequences.items():
        original_score = calculate_pwm_score(seq, pwm)
        rev_comp_score = calculate_pwm_score(reverse_complement(seq), pwm)
        predictions[name] = max(original_score, rev_comp_score)
    return predictions

def write_predictions(predictions, output_file):
    with open(output_file, 'w') as f:
        for seq_name in sorted(predictions, key=predictions.get, reverse=True)[:6000]:
            f.write(seq_name + '\n')

def main():
    bound_sequences = read_fasta("bound.fasta")
    test_sequences = read_fasta("test.fasta")
    
    k = 15
    bound_seqs_list = list(bound_sequences.values())
    pwm = greedy_motif_search(bound_seqs_list, k)
    predictions = predict_sequences(test_sequences, pwm)
    
    write_predictions(predictions, "predictions.txt")

if __name__ == "__main__":
    main()