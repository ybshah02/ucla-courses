# pylint: disable=too-few-public-methods

"""
Module with classes for class, field, and method definitions.

In P1, we don't allow overloading within a class;
two methods cannot have the same name with different parameters.
"""

from intbase import InterpreterBase, ErrorType
from type_valuev1 import Type
from enum import Enum

class MethodDef:
    """
    Wrapper struct for the definition of a member method.
    """

    def __init__(self, method_def):
        self.return_type = method_def[1]
        self.method_name = method_def[2]
        self.formal_params = method_def[3]
        self.code = method_def[4]

    def get_method_name(self):
        return self.method_name
    
    def get_formal_params(self):
        return self.formal_params

    def get_return_type(self):
        return self.return_type


class FieldDef:
    """
    Wrapper struct for the definition of a member field.
    """

    def __init__(self, field_def):
        self.field_type = field_def[1]
        self.field_name = field_def[2]
        self.default_field_value = field_def[3]

    def get_field_name(self):
        return self.field_name

    def get_field_type(self):
        return self.field_type
    
    def get_field_value(self):
        return self.default_field_value.value()
    
    def set_field_value(self, value):
        self.default_field_value = value

class ClassDef:
    """
    Holds definition for a class:
        - list of fields (and default values)
        - list of methods

    class definition: [class classname [field1 field2 ... method1 method2 ...]]
    """

    def __init__(self, class_def, interpreter, curr_classes, all_class_names):
        self.interpreter = interpreter
        self.name = class_def[1]
        self.all_classes = all_class_names
        self.curr_classes = curr_classes
        if len(class_def) > 2 and class_def[2] == 'inherits':
            self.parent = curr_classes.get(class_def[3])
        else:
            self.parent = None
        self.__create_field_list(class_def[2:])
        self.__create_method_list(class_def[2:])

    def get_fields(self):
        """
        Get a list of FieldDefs for *all* fields in the class.
        """
        return self.fields
    
    def set_field(self, field, value):
        for f in self.fields:
            if f.get_field_name() == field:
                f.set_field_value(value)

    def get_methods(self):
        """
        Get a list of MethodDefs for *all* fields in the class.
        """
        return self.methods
    
    def get_name(self):
        return self.name
    
    def get_parent(self):
        return self.parent

    def __create_field_list(self, class_body):
        self.fields = []
        fields_defined_so_far = set()
        for member in class_body:
            if member[0] == InterpreterBase.FIELD_DEF:
                if not self.check_field_type(member[1], member[3]):
                    self.interpreter.error(ErrorType.TYPE_ERROR, f"Incomplete value {member[1]} to type {member[3]}")
                if member[2] in fields_defined_so_far:  # redefinition
                    self.interpreter.error(
                        ErrorType.NAME_ERROR,
                        "duplicate field " + member[2],
                        member[0].line_num,
                    )
                self.fields.append(FieldDef(member))
                fields_defined_so_far.add(member[2])
    
    def check_field_type(self, type, val):
        
        if (type == 'bool' or type == Type.BOOL) and (val == InterpreterBase.TRUE_DEF or val == InterpreterBase.FALSE_DEF):
            return True
        if (type == 'string' or type == Type.STRING) and val[0] == '"':
            return True
        if (type == 'int' or type == Type.INT) and val.lstrip('-').isnumeric():
            return True
        if (type in self.all_classes or type == self.name or type == Type.CLASS) and val == InterpreterBase.NULL_DEF:
            return True
        
        return False

    def __create_method_list(self, class_body):
        self.methods = []
        methods_defined_so_far = set()
        for member in class_body:
            
            if member[0] == InterpreterBase.METHOD_DEF:
                if member[2] in methods_defined_so_far:  # redefinition
                    self.interpreter.error(
                        ErrorType.NAME_ERROR,
                        "duplicate method " + member[2],
                        member[0].line_num,
                    )
                params_defined = []
                for param in member[3]:
                    if param[1] in params_defined:
                        self.interpreter.error(
                            ErrorType.NAME_ERROR,
                            f"duplicate formal param name {param[1]}"
                        )
                    else:
                        params_defined.append(param[1])
                self.methods.append(MethodDef(member))
                methods_defined_so_far.add(member[2])
        
