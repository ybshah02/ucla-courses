from functools import reduce

# Problem 1

def convert_to_decimal(bits):
    exponents = range(len(bits)-1, -1, -1)
    nums = [num[0] * 2**num[1] for num in zip(bits,exponents)]
    return reduce(lambda acc, num: acc+num, nums)

#print(convert_to_decimal([1, 0, 1, 1, 0]))
#print(convert_to_decimal([1, 0, 1,]))

# Problem 2

# a

def parse_csv(lines):
    return [(lambda item : (item.split(',')[0], int(item.split(',')[1])))(item) for item in lines]

#print(parse_csv(lines=["apple,8", "pear,24", "gooseberry,-2"]))

# b

def unique_characters(sentence):
    return set(sentence)

#print(unique_characters('happy'))

# c

def squares_dict(lower_bound, upper_bound):
    return {x: x**2 for x in range(lower_bound, upper_bound+1)}

#print(squares_dict(1, 5))

# Problem 3

def strip_characters(sentence, chars_to_remove):
    return ''.join([char for char in sentence if char not in chars_to_remove])

#print(strip_characters("Hello, world!", {"o", "h", "l"}))

# Problem 7

def largest_sum(nums, k):
    if k < 0 or k > len(nums):
        raise ValueError
    elif k == 0:
        return 0
    
    max_sum = None
    for i in range(len(nums)-k+1):
        sum = 0
        for num in nums[i:i+k]:
            sum += num
        if max_sum is None or sum > max_sum:
            max_sum = sum
    return max_sum

#print(largest_sum([3,5,6,2,3,4,5], 3))
#print(largest_sum([10,-8,2,6,-1,2], 4))

# Problem 8

# a

class Event:
    def __init__(self, start_time, end_time):
        self.start_time = start_time
        self.end_time = end_time

        if start_time > end_time:
            raise ValueError


# event = Event(10, 20)
# print(f"Start: {event.start_time}, End: {event.end_time}")
# try:
#     invalid_event = Event(20, 10)
#     print("Success")
# except ValueError:
#     print("Created an invalid event")

# b

class Calendar:
    def __init__(self):
        self.events = list()
    
    def get_events(self):
        return self.events
    
    def add_event(self, event):
        if type(event) == Event:
            self.events.append(event)
        else:
            raise TypeError

# calendar = Calendar()
# print(calendar.get_events())
# calendar.add_event(Event(10, 20))
# print(calendar.get_events()[0].start_time)
# try:
#     calendar.add_event("not an event")
# except TypeError:
#     print("Invalid event")

# Problem 9

def outf(x):
    def inf(y):
        return x - y
    return inf