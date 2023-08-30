from intbase import InterpreterBase, ErrorType
from bparser import BParser
from classes import ClassDefinition
import re

class Interpreter(InterpreterBase):
    def __init__(self, console_output=True, inp=None, trace_output=False):
        super().__init__(console_output, inp)
        self.program = []
        self.classes = {}
        self.local_variables = {}
        self.curr_class = None

    def run(self, program):
        global currClass
        super().reset()
        valid, parsed_program = BParser.parse(program)
        #if not valid:
        #    super().error(ErrorType.SYNTAX_ERROR, f"Program has syntax errors present.")
        #    return False
        
        self.program = parsed_program.copy()

        if not self.find_main_and_classes(parsed_program):
            self.output_name_error()
        
        self.initialize_classes(parsed_program)
            
        for c in self.classes.copy().values():
            print(f"Class {c.get_name()}")
            print(f"Fields: {c.get_all_fields()}")
            print("Methods: ")
            for m in c.methods.copy().values():
                print(f" name: {m.get_name()} ")
                print(f" params: {m.get_params()} ")
                print(f" body: {m.get_body()}")
                print()

            print()
        
        print('Starting program execution')
        self.curr_class = self.classes[InterpreterBase.MAIN_CLASS_DEF]
        self.execute_method(InterpreterBase.MAIN_CLASS_DEF, InterpreterBase.MAIN_FUNC_DEF)

    def find_main_and_classes(self, program):
        has_main = False
        for item in program:
            if item[0] == InterpreterBase.CLASS_DEF:
                # verify that classes are named correctly
                if not re.match(r"^[a-zA-Z_][a-zA-Z0-9_]*$", item[1]):
                    self.output_name_error()
                    return False

                # check for main
                if item[1] == InterpreterBase.MAIN_CLASS_DEF:
                    has_main = True

                # add class name to list of classes
                if item[1] in self.classes.keys():
                    self.output_name_error()
                else:
                    self.classes[item[1]] = ClassDefinition(item[1])
        
        class_names = self.classes.copy().keys()
        
        return has_main
    
    def initialize_classes(self, program, curr_class=None):
        # iterate through each element in the program array

        for item in program:
            for elem in item:
                # if the element is not a list, it is either a class/field declaration and initialization, or method
                if type(elem) is not list:
                    # handle class declaration
                    if item[0] == InterpreterBase.CLASS_DEF:
                        class_names = self.classes.copy().keys()
                        if item[1] in class_names:
                            curr_class = self.classes[item[1]]
                else:
                    # handle method declaration
                    if elem[0] == InterpreterBase.METHOD_DEF:
                        if elem[1] in curr_class.get_method_names():
                            self.output_name_error()
                        curr_class.add_method(elem[1], elem[2], elem[3])

                    # handle field declaration
                    elif elem[0] == InterpreterBase.FIELD_DEF:
                        if elem[1] in curr_class.get_field_names():
                            self.output_name_error()
                        curr_class.add_field(elem[1], elem[2])
        

        

    def interpret_statement(self, statement, local_variables):
        
        if type(statement) is not list:
            statement_def = statement
        else:
            statement_def = statement[0]

        if statement_def == InterpreterBase.PRINT_DEF:
            return self.execute_print(statement, local_variables)
        elif statement_def == InterpreterBase.CALL_DEF:
            return self.execute_call(statement, local_variables)
        elif statement_def == InterpreterBase.IF_DEF:
            return self.execute_if(statement, local_variables)
        elif statement_def == InterpreterBase.WHILE_DEF:
            return self.execute_while(statement, local_variables)
        elif statement_def == InterpreterBase.SET_DEF:
            return self.execute_set(statement, local_variables)
        elif statement_def == InterpreterBase.INPUT_INT_DEF:
            var_name = statement[1]
            user_inp = super().get_input()
            local_variables[var_name] = int(user_inp)
            return True
        elif statement_def == InterpreterBase.INPUT_STRING_DEF:
            var_name = statement[1]
            user_inp = super().get_input()
            local_variables[var_name] = str(user_inp)
            return True
        elif statement_def == InterpreterBase.BEGIN_DEF:
            for line in statement[1:]:
                self.interpret_statement(line, local_variables)
            return True
        elif statement_def == InterpreterBase.RETURN_DEF:
            print(statement)
            if len(statement) > 1:
                return self.evaluate_expression(statement[1], local_variables)
    
    def execute_print(self, statement, local_variables):
        output_str = ''
        for arg in statement[1:]:
            if type(arg) is list:
                output_str += str(self.evaluate_expression(arg, local_variables))
            else:
                if arg.startswith('"') and arg.endswith('"'):
                    output_str += arg[1:-1]
                elif self.is_constant(arg)[0] or self.is_variable(arg)[0]:
                    output_str += str(self.get_constant_or_variable(arg, local_variables))
        
        super().output(output_str)
        return True

    def execute_call(self, statement, local_variables):
        obj = statement[1]
        method_name = statement[2]
        processed_params = []
        if len(statement) > 2:   
            params = statement[3:]
            for param in params:
                if type(param) is list:
                    processed_params.append(self.evaluate_expression(param, local_variables))
                else:
                    processed_params.append(param)
        
        return self.execute_method(obj, method_name, processed_params)
    
    def execute_method(self, class_name, method_name, params=[]):
        if class_name == InterpreterBase.ME_DEF:
            class_obj = self.curr_class
            method_obj = self.curr_class.get_method(method_name)
        else:
            if class_name in self.classes:
                class_obj = self.classes[class_name]
                method_obj = class_obj.get_method(method_name)
            elif class_name in self.local_variables:
                print(class_name)
                class_obj = self.local_variables[class_name]
                method_obj = class_obj.get_method(method_name)

        if not method_obj:
            self.output_name_error()
        
        if len(params) != len(method_obj.get_params().keys()):
            self.output_type_error()
        
        method_obj.update_params(params)

        body = method_obj.get_body()
        if body[0] == InterpreterBase.BEGIN_DEF:
            for line in body:
                print('LISP line: ', line)
                self.interpret_statement(line, self.local_variables)
        else:
            print('LISP line: ', body)
            self.interpret_statement(body, self.local_variables)
            
        return True
    
    def execute_set(self, statement, local_variables):
        var_name = statement[1]
        if type(statement[2]) is list:
            var_value = self.evaluate_expression(statement[2], local_variables)
        else:
            var_value = self.get_constant_or_variable(statement[2], local_variables)
            if not self.is_constant(var_value):
                self.output_name_error()


        self.local_variables[var_name] = var_value
        #print("var name: ", var_name, " value: ", self.local_variables[var_name])

        return True
    
    def execute_if(self, statement, local_variables):
        condition = statement[1]
        if condition == 'true':
            condition_eval = True
        elif condition == 'false':
            condition_eval = False
        else:
            condition_eval = self.evaluate_expression(condition, local_variables)
        if type(condition_eval) is not bool:
            self.output_type_error()
        
        condition_true_body = statement[2]
        if len(statement) > 3:
            condition_false_body = statement[3]
        else:
            condition_false_body = None

        if condition_eval:
            return self.interpret_statement(condition_true_body, local_variables)
        elif condition_false_body:
            return self.interpret_statement(condition_false_body, local_variables)

    def execute_while(self, statement, local_variables):
        condition = statement[1]
        print(type(self.evaluate_expression(condition, local_variables)))
        if not isinstance(self.evaluate_expression(condition, local_variables), bool):
            self.output_type_error()
        
        body = statement[2]
        while self.evaluate_expression(condition, local_variables):
            for line in body:
                self.interpret_statement(line, local_variables)
        
        return True

    def evaluate_expression(self, statement, local_variables):

        op = statement[0]

        if op in ['+', '-', '*', '/', '>', '<', '==', '!=', '>=', '<=', '%', '&', '|']:

            left = statement[1]
            left_val = None
            right_val = None

            left_val = self.get_constant_or_variable(left, local_variables)
            if len(statement) > 2:
                right = statement[2]
                if isinstance(left_val, list):
                    right_val = self.evaluate_expression(right, local_variables)
                else:
                    right_val = self.get_constant_or_variable(right, local_variables)
            
            # handle arithmetic expressions
            if isinstance(left_val, int) and isinstance(right_val, int) and op in ['+', '-', '*', '/', '%']:
                    return self.get_arithmetic(op, left_val, right_val)
            else:
                self.output_type_error()
            
            # handle comparisons with integers and strings
            if isinstance(left_val, int) and isinstance(right_val, int) or isinstance(right_val, str) and isinstance(left_val, str) and op in ['>', '>=', '<', '<=', '==', '!=']:
                return self.get_int_or_string_comparison(op, left_val, right_val)
            
            # handle concatentation of two strings
            elif isinstance(left_val, str) and isinstance(right_val, str) and op == '+':
                return left_val + right_val
            
            # handle boolean comparisons
            elif isinstance(left_val, bool) and isinstance(right_val, bool) and op in ['!=', '==', '&', '|']:
                return self.get_bool_comparison(op, left_val, right_val)
            
            # handle null comparison
            elif isinstance(left_val, object) and op in ['==', '!=']:
                return self.get_null_comparison(op, left_val)
            
            # handle unary not
            elif op == '!' and len(statement) == 2:
                return not left_val
        
        if op == InterpreterBase.NEW_DEF:
            class_name = statement[1]
            if class_name in self.classes:
                c = self.classes[class_name]
                new_obj = ClassDefinition(class_name)
                new_obj.fields = c.fields.copy()
                new_obj.methods = c.methods.copy()
                return new_obj
            else:
                self.output_type_error()


    def format_values(self, value):
        # value is a Bool
        if value in ['true', 'false']:
            if value == 'true':
                return (True, True)
            else:
                return (True, False)
            
        # value is a String
        elif value.startswith('"') and value.endswith('"'):
            return (True, value.strip('"'))
        
        # value is null
        elif value == '"null"':
            return (True, None)
        
        # value is an integer
        elif (value.startswith('-') and value[1:].isdigit()) or value.isdigit():
            return (True, int(value))
        else:
            return (False, None)

    def is_constant(self, value):
        if isinstance(value, str):
            return self.format_values(value)
        else:
            return (False, None)
    
    def is_variable(self, var):
        var_found = False
        val = None
    
        for c in self.classes.values():
            fields = c.get_all_fields()
            methods = c.get_all_methods()
            if var in fields.keys():
                var_found = True
                val = fields[var]
            
            for m in methods.values():
                if var in m.get_params():
                    var_found = True
                    val = m.get_param_value(var)

        return (var_found, self.format_values(val)[1])
    
    def get_constant_or_variable(self, statement, local_variables):

        is_constant, c_val = self.is_constant(statement)
        if is_constant:
            return c_val
        elif statement in local_variables:
            return local_variables[statement]
        else:
            is_variable, v_val = self.is_variable(statement)
            if is_variable:
                return v_val
        
        if not is_constant and not is_variable:
            self.output_name_error()
        
        return None
    
    def get_arithmetic(self, op, left, right):
        if op == "+":
            return left + right
        elif op == "-":
            return left - right
        elif op == "*":
            return left * right
        elif op == "/":
            return left // right
        elif op == "%":
            return left % right
        
    def get_int_or_string_comparison(self, op, left, right):
        if op == "<":
            return left < right
        elif op == ">":
            return left > right
        elif op == "<=":
            return left <= right
        elif op == ">=":
            return left >= right
        elif op == "!=":
            return left != right
        elif op == "==":
            return left == right
    
    def concat_strings(self, left, right):
        return left + right
    
    def get_bool_comparison(self, op, left, right):
        if op == "!=":
            return left != right
        elif op == "==":
            return left == right
        elif op == "&":
            return left and right
        elif op == "|":
            return left or right
    
    def get_null_comparison(self, op, left):
        if op == "==":
            return left is None
        elif op == "!=":
            return left is not None

    def output_name_error(self, err_txt=None, line_num=None):
        super().error(ErrorType.NAME_ERROR, err_txt, line_num)
    
    def output_syntax_error(self, err_txt=None, line_num=None):
        super().error(ErrorType.SYNTAX_ERROR, err_txt, line_num)
    
    def output_type_error(self, err_txt=None, line_num=None):
        super().error(ErrorType.TYPE_ERROR, err_txt, line_num)
