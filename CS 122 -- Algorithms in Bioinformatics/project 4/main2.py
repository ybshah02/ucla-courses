def load_observations(input_file_path):  
    observations = []
    with open(input_file_path, 'r') as file:
        next(file)  # Skip the header line
        for line in file:
            char = line.strip()
            observations.append(char)
    return observations

def write_predictions(output_file_path, top_intervals):
    """
    Write the top interval predictions to a file.

    Parameters:
    - output_file_path: Path to the file where predictions will be written.
    - top_intervals: A list of interval line numbers, sorted by their probability of overlapping gene bodies.
    """
    with open(output_file_path, 'w') as file:
        for interval in top_intervals:
            file.write(f"{interval}\n")

def forward(observations, states, start_prob, trans_prob, emit_prob):
    fwd = [{}]
    scales = []  # To store scaling factors

    # Initial probabilities
    scale_factor = sum(start_prob[state] * emit_prob[state][observations[0]] for state in states)
    scales.append(scale_factor)
    for state in states:
        fwd[0][state] = start_prob[state] * emit_prob[state][observations[0]] / scale_factor
    
    # Forward pass
    for t in range(1, len(observations)):
        fwd.append({})
        scale_factor = 0  # Reset scale_factor for the current timestep
        for state in states:
            fwd[t][state] = sum((fwd[t-1][prev_state] * trans_prob[prev_state][state] * emit_prob[state][observations[t]]) for prev_state in states)
            scale_factor += fwd[t][state]
        scales.append(scale_factor)
        for state in states:
            fwd[t][state] /= scale_factor  # Normalize

    # Calculate scaled probability of the sequence
    prob = sum(fwd[-1][state] for state in states)
    return fwd, prob

def backward(observations, states, trans_prob, emit_prob):
    bkw = [{} for _ in range(len(observations))]
    T = len(observations)
    # Initialize base cases (t == T)
    for state in states:
        bkw[T-1][state] = 1
    
    # Run Backward algorithm
    for t in range(T-2, -1, -1):
        for state in states:
            bkw[t][state] = sum((trans_prob[state][next_state] * emit_prob[next_state][observations[t+1]] * bkw[t+1][next_state]) for next_state in states)
    
    prob = sum(bkw[0][state] for state in states)
    return bkw, prob

def forward_backward(observations, states, start_prob, trans_prob, emit_prob):
    fwd, prob_forward = forward(observations, states, start_prob, trans_prob, emit_prob)
    bkw, prob_backward = backward(observations, states, trans_prob, emit_prob)
    
    # Calculate posterior probabilities for states given observations
    posteriors = []
    for t in range(len(observations)):
        posterior = {state: (fwd[t][state] * bkw[t][state]) / prob_forward for state in states}
        posteriors.append(posterior)
    
    return posteriors

def read_and_score_intervals_with_hmm(input_file_path):
    top_n = 50000
    # Placeholder: Load your observations ('n', 'x', 'y', 'z') from the file
    observations = load_observations(input_file_path)

    # Here you would define your HMM parameters based on your understanding of the data
    states = ['G', 'O']  # Gene body, Outside gene body
    start_probability = {'G': 0.5, 'O': 0.5}
    transition_probability = {
       'G': {'G': 0.7, 'O': 0.3},
       'O': {'G': 0.4, 'O': 0.6},
    }
    emission_probability = {
       'G': {'n': 0.0, 'x': 0.4, 'y': 0.4, 'z': 0.2},
       'O': {'n': 0.5, 'x': 0.2, 'y': 0.2, 'z': 0.1},
    }
    
    # Run the forward-backward algorithm to get posterior probabilities for each interval
    posteriors = forward_backward(observations, states, start_probability, transition_probability, emission_probability)
    
    g_probabilities = [(i+1, p['G']) for i, p in enumerate(posteriors)]
    top_intervals = sorted(g_probabilities, key=lambda x: x[1], reverse=True)[:top_n]
    top_intervals = [interval[0] for interval in top_intervals]  # Extract just the interval numbers

    # Write the top interval predictions to a file
    write_predictions('predictions.txt', top_intervals)

read_and_score_intervals_with_hmm('input.fasta')