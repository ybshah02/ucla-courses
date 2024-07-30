def read_and_score_intervals_with_sliding_window(input_file_path, window_size=10):
    raw_scores = []
    with open(input_file_path, 'r') as file:
        next(file)
        for line in file:
            char = line.strip()
            if char == 'z':
                score = 3
            elif char == 'y':
                score = 2
            elif char == 'x':
                score = 1
            else: 
                score = 0
            raw_scores.append(score)

    adjusted_scores = []
    half_window = window_size // 2
    for i in range(len(raw_scores)):
        window_start = max(0, i - half_window)
        window_end = min(len(raw_scores), i + half_window + 1)
        window_score = sum(raw_scores[window_start:window_end])
        
        normalized_score = window_score / (window_end - window_start)
        adjusted_scores.append((i, normalized_score))

    return adjusted_scores

def select_top_intervals(scores):
    sorted_scores = sorted(scores, key=lambda x: (-x[1], x[0]))
    top_intervals = [line_number + 1 for line_number, score in sorted_scores[:50000]]
    return top_intervals

def write_output(output_file_path, top_intervals):
    with open(output_file_path, 'w') as file:
        for interval in top_intervals:
            file.write(f"{interval}\n")

def main():
    input_file_path = 'input.fasta'
    output_file_path = 'predictions.txt'

    scores = read_and_score_intervals_with_sliding_window(input_file_path, window_size=5)
    top_intervals = select_top_intervals(scores)
    write_output(output_file_path, top_intervals)

if __name__ == "__main__":
    main()
