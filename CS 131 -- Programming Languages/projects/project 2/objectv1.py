"""
Module handling the operations of an object. This contains the meat
of the code to execute various instructions.
"""

from env_v1 import EnvironmentManager
from intbase import InterpreterBase, ErrorType
from type_valuev1 import create_value
from type_valuev1 import Type, Value


class ObjectDef:
    STATUS_PROCEED = 0
    STATUS_RETURN = 1
    STATUS_NAME_ERROR = 2
    STATUS_TYPE_ERROR = 3

    def __init__(self, interpreter, class_def, trace_output):
        self.interpreter = interpreter  # objref to interpreter object. used to report errors, get input, produce output
        self.class_def = class_def  # take class body from 3rd+ list elements, e.g., ["class",classname", [classbody]]
        self.trace_output = trace_output
        self.parent = self.__map_parent_to_object()
        self.__map_fields_to_values()
        self.__map_method_names_to_method_definitions()
        self.__create_map_of_operations_to_lambdas()  # sets up maps to facilitate binary and unary operations, e.g., (+ 5 6)

    def get_class_def(self):
        return self.class_def
    
    def get_method_info(self, method_name, num_params):
        method_info = self.methods.get(method_name)

        if method_info and len(method_info.formal_params) == num_params:
            return method_info
        else:
            if self.parent:
                method_info = self.parent.get_method_info(method_name, num_params)

        return method_info

    def call_method(self, method_name, actual_params, line_num_of_caller):
        """
        actual_params is a list of Value objects (all parameters are passed by value).

        The caller passes in the line number so we can properly generate an error message.
        The error is then generated at the source (i.e., where the call is initiated).
        """
        if method_name not in self.methods:
            self.interpreter.error(
                ErrorType.NAME_ERROR,
                "unknown method " + method_name,
                line_num_of_caller,
            )

        method_info = self.get_method_info(method_name, len(actual_params))

        if len(actual_params) != len(method_info.formal_params):
            self.interpreter.error(
                ErrorType.NAME_ERROR,
                "unkown method " + method_name,
                line_num_of_caller,
            )
        env = (
            EnvironmentManager()
        )  # maintains lexical environment for function; just params for now

        valid_types = {'int': Type.INT, 'bool': Type.BOOL, 'string': Type.STRING}

        for formal, actual in zip(method_info.formal_params, actual_params):
            if formal[0] in valid_types:
                if valid_types[formal[0]] != actual.type() and actual.type() != Type.CLASS:
                    self.interpreter.error(
                        ErrorType.NAME_ERROR, 
                        f"Invalid paramater input {actual.value()} to {formal[1]}",
                        line_num_of_caller
                    )
            env.set(formal[1], actual)

        # since each method has a single top-level statement, execute it.
        status, return_value = self.__execute_statement(env, method_info.code)
        # if the method explicitly used the (return expression) statement to return a value, then return that
        # value back to the caller
        method_return_type = method_info.get_return_type()
        if status == ObjectDef.STATUS_RETURN:
            if isinstance(return_value, ObjectDef):
                if return_value.class_def.get_name() == method_return_type:
                    return return_value
                else:
                    self.interpreter.error(
                        ErrorType.TYPE_ERROR,
                        f"type mismatch {method_return_type} and {return_value.class_def.get_name()}",
                        line_num_of_caller
                    )

            return_val = return_value.value()
            return_val_type = return_value.type()

            if self.check_return_type(method_return_type, return_val, return_val_type):
                if return_val == None:
                    return self.get_default_return_val(method_return_type)
                else:
                    return return_value
            else:
                self.interpreter.error(
                    ErrorType.TYPE_ERROR,
                    "Return value is not of the return type",
                    line_num_of_caller
                )
            return return_value
        else:
            return self.get_default_return_val(method_return_type)
    
    def check_return_type(self, return_type, return_val, return_val_type):
        if return_val_type in [Type.INT, Type.STRING, Type.BOOL, Type.CLASS, Type.NOTHING] and return_val is None:
            return True
        
        if return_type == 'int' and not return_val_type == Type.INT:
            return False
        elif return_type == 'string' and not return_val_type == Type.STRING:
            return False
        elif return_type == 'bool' and not return_val_type == Type.BOOL:
            return False
        elif return_val_type == Type.CLASS:
            return_val = return_val.get_class_def()
            temp = return_val
            parents = []
            while temp.get_parent() != None:
                temp = temp.get_parent()
                parents.append(return_val.get_parent().get_name())
            if return_type != return_val.get_name() or return_type not in parents or return_val != 'null':
                return False
        
        return True
    
    def get_default_return_val(self, return_type):
        if return_type == 'int':
            return Value(Type.INT, 0)
        elif return_type == 'string':
            return Value(Type.STRING, '')
        elif return_type == 'bool':
            return Value(Type.BOOL, False)
        elif return_type != 'void':
            return Value(Type.CLASS, None)

    def __execute_statement(self, env, code):
        """
        returns (status_code, return_value) where:
        - status_code indicates if the next statement includes a return
            - if so, the current method should terminate
            - otherwise, the next statement in the method should run normally
        - return_value is a Value containing the returned value from the function
        """
        if self.trace_output:
            print(f"{code[0].line_num}: {code}")
        tok = code[0]
        if tok == InterpreterBase.BEGIN_DEF:
            return self.__execute_begin(env, code)
        if tok == InterpreterBase.SET_DEF:
            return self.__execute_set(env, code)
        if tok == InterpreterBase.IF_DEF:
            return self.__execute_if(env, code)
        if tok == InterpreterBase.CALL_DEF:
            return self.__execute_call(env, code)
        if tok == InterpreterBase.WHILE_DEF:
            return self.__execute_while(env, code)
        if tok == InterpreterBase.RETURN_DEF:
            return self.__execute_return(env, code)
        if tok == InterpreterBase.INPUT_STRING_DEF:
            return self.__execute_input(env, code, True)
        if tok == InterpreterBase.INPUT_INT_DEF:
            return self.__execute_input(env, code, False)
        if tok == InterpreterBase.PRINT_DEF:
            return self.__execute_print(env, code)
        if tok == InterpreterBase.LET_DEF:
            return self.__execute_let(env, code)

        self.interpreter.error(
            ErrorType.SYNTAX_ERROR, "unknown statement " + tok, tok.line_num
        )

    # (begin (statement1) (statement2) ... (statementn))
    def __execute_begin(self, env, code):
        for statement in code[1:]:
            status, return_value = self.__execute_statement(env, statement)
            if status == ObjectDef.STATUS_RETURN:
                return (
                    status,
                    return_value,
                )  # could be a valid return of a value or an error
        # if we run thru the entire block without a return, then just return proceed
        # we don't want the calling block to exit with a return
        return ObjectDef.STATUS_PROCEED, None
    
    def __execute_let(self, env, code):
        old_env = env.copy()
        let_vars = []
        for temp_var in code[1]:
            if not self.class_def.check_field_type(temp_var[0], temp_var[2]):
                self.interpreter.error(
                    ErrorType.TYPE_ERROR,
                    "Invalid type for let variable",
                    code[0].line_num
                )

            val = self.__evaluate_expression(env, temp_var[2], code[0].line_num)
            if temp_var[1] in let_vars:
                self.interpreter.error(
                    ErrorType.NAME_ERROR,
                    f"{temp_var} is already defined as a let variable",
                    code[0].line_num
                )
            else:
                env.set(temp_var[1], create_value(temp_var[2]))
                let_vars.append(temp_var[1])
        self.__execute_begin(env, code[1:])
        env.environment.clear()
        env.environment.update(old_env.environment)

        return ObjectDef.STATUS_PROCEED, None

    # (call object_ref/me methodname param1 param2 param3)
    # where params are expressions, and expresion could be a value, or a (+ ...)
    # statement version of a method call; there's also an expression version of a method call below
    def __execute_call(self, env, code):
        return ObjectDef.STATUS_PROCEED, self.__execute_call_aux(
            env, code, code[0].line_num
        )

    # (set varname expression), where expresion could be a value, or a (+ ...)
    def __execute_set(self, env, code):
        val = self.__evaluate_expression(env, code[2], code[0].line_num)
        self.__set_variable_aux(env, code[1], val, code[0].line_num)
        return ObjectDef.STATUS_PROCEED, None

    # (return expression) where expresion could be a value, or a (+ ...)
    def __execute_return(self, env, code):
        if len(code) == 1:
            # [return] with no return expression
            return ObjectDef.STATUS_RETURN, create_value(InterpreterBase.NOTHING_DEF)
        return ObjectDef.STATUS_RETURN, self.__evaluate_expression(
            env, code[1], code[0].line_num
        )

    # (print expression1 expression2 ...) where expresion could be a variable, value, or a (+ ...)
    def __execute_print(self, env, code):
        output = ""
        for expr in code[1:]:
            # TESTING NOTE: Will not test printing of object references
            term = self.__evaluate_expression(env, expr, code[0].line_num)
            if isinstance(term, ObjectDef):
                val = str(term)
                val = val.replace("objectv1", "interpreters.s23.objectv2")
            else:
                val = term.value()
                typ = term.type()
                if typ == Type.BOOL:
                    val = "true" if val else "false"
            # document - will never print out an object ref
            output += str(val)
        self.interpreter.output(output)
        return ObjectDef.STATUS_PROCEED, None

    # (inputs target_variable) or (inputi target_variable) sets target_variable to input string/int
    def __execute_input(self, env, code, get_string):
        inp = self.interpreter.get_input()
        if get_string:
            val = Value(Type.STRING, inp)
        else:
            val = Value(Type.INT, int(inp))

        self.__set_variable_aux(env, code[1], val, code[0].line_num)
        return ObjectDef.STATUS_PROCEED, None

    # helper method used to set either parameter variables or member fields; parameters currently shadow
    # member fields
    def __set_variable_aux(self, env, var_name, value, line_num):

        # parameter shadows fields
        if not isinstance(value, ObjectDef) and value.type() == Type.NOTHING:
            self.interpreter.error(
                ErrorType.TYPE_ERROR, "can't assign to nothing " + var_name, line_num
            )
        param_val = env.get(var_name)
        if param_val is not None:
            if param_val.type() != value.type():
                self.interpreter.error(
                    ErrorType.TYPE_ERROR,
                    f'type mismatch: {var_name} is not of type {value.type()}',
                    line_num
                )
            env.set(var_name, value)
            return

        if var_name in self.fields or env.get(var_name) != None:
            if isinstance(value, ObjectDef):
                print(self.fields[var_name].value())

            elif self.fields[var_name].type() == value.type():
                self.fields[var_name] = value
            else:
                self.interpreter.error(
                    ErrorType.TYPE_ERROR
                )
        elif self.parent:
            if var_name in self.parent.fields:
                if self.parent.fields[var_name].type() == value.type():
                    self.parent.fields[var_name] = value
                else:
                    self.interpreter.error(
                        ErrorType.TYPE_ERROR
                    )
        else:
            self.interpreter.error(
                ErrorType.NAME_ERROR, "unknown variable " + var_name, line_num
            )


    # (if expression (statement) (statement) ) where expresion could be a boolean constant (e.g., true), member
    # variable without ()s, or a boolean expression in parens, like (> 5 a)
    def __execute_if(self, env, code):
        condition = self.__evaluate_expression(env, code[1], code[0].line_num)
        if condition.type() != Type.BOOL:
            self.interpreter.error(
                ErrorType.TYPE_ERROR,
                "non-boolean if condition " + ' '.join(x for x in code[1]),
                code[0].line_num,
            )
        if condition.value():
            status, return_value = self.__execute_statement(
                env, code[2]
            )  # if condition was true
            return status, return_value
        if len(code) == 4:
            status, return_value = self.__execute_statement(
                env, code[3]
            )  # if condition was false, do else
            return status, return_value
        return ObjectDef.STATUS_PROCEED, None

    # (while expression (statement) ) where expresion could be a boolean value, boolean member variable,
    # or a boolean expression in parens, like (> 5 a)
    def __execute_while(self, env, code):
        while True:
            condition = self.__evaluate_expression(env, code[1], code[0].line_num)
            if condition.type() != Type.BOOL:
                self.interpreter.error(
                    ErrorType.TYPE_ERROR,
                    "non-boolean while condition " + ' '.join(x for x in code[1]),
                    code[0].line_num,
                )
            if not condition.value():  # condition is false, exit loop immediately
                return ObjectDef.STATUS_PROCEED, None
            # condition is true, run body of while loop
            status, return_value = self.__execute_statement(env, code[2])
            if status == ObjectDef.STATUS_RETURN:
                return (
                    status,
                    return_value,
                )  # could be a valid return of a value or an error

    # given an expression, return a Value object with the expression's evaluated result
    # expressions could be: constants (true, 5, "blah"), variables (e.g., x), arithmetic/string/logical expressions
    # like (+ 5 6), (+ "abc" "def"), (> a 5), method calls (e.g., (call me foo)), or instantiations (e.g., new dog_class)
    def __evaluate_expression(self, env, expr, line_num_of_statement):
        if not isinstance(expr, list):
            # locals shadow member variables
            val = env.get(expr)
            if expr == InterpreterBase.ME_DEF:
                return self
            if val is not None:
                return val
            if expr in self.fields:
                return self.fields[expr]
            # need to check for variable name and get its value too
            value = create_value(expr)
            if value is not None:
                return value
            self.interpreter.error(
                ErrorType.NAME_ERROR,
                "invalid field or parameter " + expr,
                line_num_of_statement,
            )

        operator = expr[0]
        if operator in self.binary_op_list:
            operand1 = self.__evaluate_expression(env, expr[1], line_num_of_statement)
            operand2 = self.__evaluate_expression(env, expr[2], line_num_of_statement)
            if operand1.type() == operand2.type() and operand1.type() == Type.INT:
                if operator not in self.binary_ops[Type.INT]:
                    self.interpreter.error(
                        ErrorType.TYPE_ERROR,
                        "invalid operator applied to ints",
                        line_num_of_statement,
                    )
                return self.binary_ops[Type.INT][operator](operand1, operand2)
            if operand1.type() == operand2.type() and operand1.type() == Type.STRING:
                if operator not in self.binary_ops[Type.STRING]:
                    self.interpreter.error(
                        ErrorType.TYPE_ERROR,
                        "invalid operator applied to strings",
                        line_num_of_statement,
                    )
                return self.binary_ops[Type.STRING][operator](operand1, operand2)
            if operand1.type() == operand2.type() and operand1.type() == Type.BOOL:
                if operator not in self.binary_ops[Type.BOOL]:
                    self.interpreter.error(
                        ErrorType.TYPE_ERROR,
                        "invalid operator applied to bool",
                        line_num_of_statement,
                    )
                return self.binary_ops[Type.BOOL][operator](operand1, operand2)
            if operand1.type() == operand2.type() and operand1.type() == Type.CLASS:
                
                if operator not in self.binary_ops[Type.CLASS]:
                    self.interpreter.error(
                        ErrorType.TYPE_ERROR,
                        "invalid operator applied to class",
                        line_num_of_statement,
                    )

                return self.binary_ops[Type.CLASS][operator](operand1, operand2)
            # error what about an obj reference and null
            self.interpreter.error(
                ErrorType.TYPE_ERROR,
                f"operator {operator} applied to two incompatible types",
                line_num_of_statement,
            )
        if operator in self.unary_op_list:
            operand = self.__evaluate_expression(env, expr[1], line_num_of_statement)
            if operand.type() == Type.BOOL:
                if operator not in self.unary_ops[Type.BOOL]:
                    self.interpreter.error(
                        ErrorType.TYPE_ERROR,
                        "invalid unary operator applied to bool",
                        line_num_of_statement,
                    )
                return self.unary_ops[Type.BOOL][operator](operand)

        # handle call expression: (call objref methodname p1 p2 p3)
        if operator == InterpreterBase.CALL_DEF:
            return self.__execute_call_aux(env, expr, line_num_of_statement)
        # handle new expression: (new classname)
        if operator == InterpreterBase.NEW_DEF:
            return self.__execute_new_aux(env, expr, line_num_of_statement)

    # (new classname)
    def __execute_new_aux(self, _, code, line_num_of_statement):
        obj = self.interpreter.instantiate(code[1], line_num_of_statement)
        return Value(Type.CLASS, obj)

    # this method is a helper used by call statements and call expressions
    # (call object_ref/me methodname p1 p2 p3)
    def __execute_call_aux(self, env, code, line_num_of_statement):
        # determine which object we want to call the method on
        obj_name = code[1]
        if obj_name == InterpreterBase.ME_DEF:
            obj = self
        elif obj_name == InterpreterBase.SUPER_DEF:
            obj = self.parent
        else:
            obj = self.__evaluate_expression(
                env, obj_name, line_num_of_statement
            )
            if not isinstance(obj, ObjectDef):
                obj = obj.value()
        # prepare the actual arguments for passing
        if obj is None:
            self.interpreter.error(
                ErrorType.FAULT_ERROR, "null dereference", line_num_of_statement
            )
        actual_args = []
        for expr in code[3:]:
            actual_args.append(
                self.__evaluate_expression(env, expr, line_num_of_statement)
            )

        return self.parse_inheritance(code[2], obj, actual_args, line_num_of_statement)
    
    def parse_inheritance(self, name, obj, actual_args, line_num_of_statement):
        print([m.get_method_name() for m in obj.class_def.get_methods()])
        if name in [m.get_method_name() for m in obj.class_def.get_methods()]:
            return obj.call_method(name, actual_args, line_num_of_statement)
        elif obj.parent and name in [m.get_method_name() for m in obj.parent.class_def.get_methods()]:
            return self.parse_inheritance(name, obj.parent, actual_args, line_num_of_statement)
        else:
            self.interpreter.error(
                ErrorType.NAME_ERROR,
                "unknown method " + name,
                line_num_of_statement,
            )

    def __map_method_names_to_method_definitions(self):
        self.methods = {}
        for method in self.class_def.get_methods():
            self.methods[method.method_name] = method

    def __map_fields_to_values(self):
        self.fields = {}
        for field in self.class_def.get_fields():
            self.fields[field.field_name] = create_value(field.default_field_value)
    
    def __map_parent_to_object(self):
        if self.class_def != 'main' and self.class_def.get_parent() is not None:
            return ObjectDef(self.interpreter, self.class_def.get_parent(), self.trace_output)
        return None

    def __create_map_of_operations_to_lambdas(self):
        self.binary_op_list = [
            "+",
            "-",
            "*",
            "/",
            "%",
            "==",
            "!=",
            "<",
            "<=",
            ">",
            ">=",
            "&",
            "|",
        ]
        self.unary_op_list = ["!"]
        self.binary_ops = {}
        self.binary_ops[Type.INT] = {
            "+": lambda a, b: Value(Type.INT, a.value() + b.value()),
            "-": lambda a, b: Value(Type.INT, a.value() - b.value()),
            "*": lambda a, b: Value(Type.INT, a.value() * b.value()),
            "/": lambda a, b: Value(
                Type.INT, a.value() // b.value()
            ),  # // for integer ops
            "%": lambda a, b: Value(Type.INT, a.value() % b.value()),
            "==": lambda a, b: Value(Type.BOOL, a.value() == b.value()),
            "!=": lambda a, b: Value(Type.BOOL, a.value() != b.value()),
            ">": lambda a, b: Value(Type.BOOL, a.value() > b.value()),
            "<": lambda a, b: Value(Type.BOOL, a.value() < b.value()),
            ">=": lambda a, b: Value(Type.BOOL, a.value() >= b.value()),
            "<=": lambda a, b: Value(Type.BOOL, a.value() <= b.value()),
        }
        self.binary_ops[Type.STRING] = {
            "+": lambda a, b: Value(Type.STRING, a.value() + b.value()),
            "==": lambda a, b: Value(Type.BOOL, a.value() == b.value()),
            "!=": lambda a, b: Value(Type.BOOL, a.value() != b.value()),
            ">": lambda a, b: Value(Type.BOOL, a.value() > b.value()),
            "<": lambda a, b: Value(Type.BOOL, a.value() < b.value()),
            ">=": lambda a, b: Value(Type.BOOL, a.value() >= b.value()),
            "<=": lambda a, b: Value(Type.BOOL, a.value() <= b.value()),
        }
        self.binary_ops[Type.BOOL] = {
            "&": lambda a, b: Value(Type.BOOL, a.value() and b.value()),
            "|": lambda a, b: Value(Type.BOOL, a.value() or b.value()),
            "==": lambda a, b: Value(Type.BOOL, a.value() == b.value()),
            "!=": lambda a, b: Value(Type.BOOL, a.value() != b.value()),
        }
        self.binary_ops[Type.CLASS] = {
            "==": lambda a, b: Value(Type.BOOL, a.value() == b.value()),
            "!=": lambda a, b: Value(Type.BOOL, a.value() != b.value()),
        }

        self.unary_ops = {}
        self.unary_ops[Type.BOOL] = {
            "!": lambda a: Value(Type.BOOL, not a.value()),
        }
