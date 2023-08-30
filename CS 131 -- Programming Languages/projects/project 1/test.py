from bparser import BParser
from interpreterv1 import Interpreter

def test():
    interpreter = Interpreter()
    with open('test.txt', 'r') as f:
        program = f.readlines()
        interpreter.run(program)

test()