from interpreterv3 import Interpreter

with open('test.txt', 'r') as f:
    program = f.readlines()
    interp = Interpreter()
    interp.run(program)
