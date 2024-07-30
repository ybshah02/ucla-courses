from classv3 import ClassDef
from intbase import InterpreterBase, ErrorType
from bparser import BParser
from objectv3 import ObjectDef
from type_valuev3 import TypeManager

# need to document that each class has at least one method guaranteed

# Main interpreter class
class Interpreter(InterpreterBase):
    def __init__(self, console_output=True, inp=None, trace_output=False):
        super().__init__(console_output, inp)
        self.trace_output = trace_output

    # run a program, provided in an array of strings, one string per line of source code
    # usese the provided BParser class found in parser.py to parse the program into lists
    def run(self, program):
        status, parsed_program = BParser.parse(program)
        print(parsed_program)
        if not status:
            super().error(
                ErrorType.SYNTAX_ERROR, f"Parse error on program: {parsed_program}"
            )
        self.__add_all_class_types_to_type_manager(parsed_program)
        self.__map_class_names_to_class_defs(parsed_program)

        # instantiate main class
        invalid_line_num_of_caller = None
        self.main_object = self.instantiate(
            InterpreterBase.MAIN_CLASS_DEF, invalid_line_num_of_caller
        )

        # call main function in main class; return value is ignored from main
        self.main_object.call_method(
            InterpreterBase.MAIN_FUNC_DEF, [], False, invalid_line_num_of_caller
        )

        # program terminates!

    # user passes in the line number of the statement that performed the new command so we can generate an error
    # if the user tries to new an class name that does not exist. This will report the line number of the statement
    # with the new command
    def instantiate(self, class_name, line_num_of_statement):

        if '@' in class_name and class_name.split('@')[0] in self.class_index:
            cdef = class_name.split('@')
            template_obj = self.class_index[cdef[0]].class_source
            template_obj[1] = class_name
            if len(template_obj[2]) != len(cdef[1:]):
                super().error(
                    ErrorType.TYPE_ERROR,
                    f"Too little or too many parameterized types for template.",
                    line_num_of_statement
                )
            for line in template_obj[2:]:
                if line[0] == self.FIELD_DEF:
                    if line[1] in template_obj[2] or '@' in line[1]:
                        if '@' in line[1]:
                            temp_line = [line[1].split('@')[0]]
                            for elem in line[1].split('@')[1:]:
                                temp_line.append(cdef[1:][template_obj[2].index(elem)])
                            line[1] = '@'.join(temp_line)
                        else:
                            line[1] = cdef[1:][template_obj[2].index(line[1])]
                
                if line[0] == self.METHOD_DEF:
                    if line[1] in template_obj[2] or '@' in line[1]:
                        if '@' in line[1]:
                            temp_line = [line[1].split('@')[0]]
                            for elem in line[1].split('@')[1:]:
                                if elem in template_obj[2]:
                                    temp_line.append(cdef[1:][template_obj[2].index(elem)])
                                else:
                                    temp_line.append(elem)
                            line[1] = '@'.join(temp_line)
                        else:
                            line[1] = cdef[1:][template_obj[2].index(line[1])]

                    for item in line[3]:
                        if item[0] in template_obj[2] or '@' in line[1] or '@' in item[0]:
                            if '@' in item[0]:
                                temp_line = [item[0].split('@')[0]]
                                for elem in item[0].split('@')[1:]:
                                    temp_line.append(cdef[1:][template_obj[2].index(elem)])
                                item[0] = '@'.join(temp_line)
                            else:
                                item[0] = cdef[1:][template_obj[2].index(item[0])]

                    for item in line[4]:
                        if item == InterpreterBase.LET_DEF:
                            for item in line[4][line[4].index(item) + 1]:
                                if '@' in item[0]:
                                    temp_line = [item[0].split('@')[0]]
                                    for elem in item[0].split('@')[1:]:
                                        if elem not in template_obj[2]:
                                            template_obj.append(elem)
                                        else:
                                            temp_line.append(cdef[1:][template_obj[2].index(elem)])
                                    item[0] = '@'.join(temp_line)
                                else:
                                    item[0] = cdef[1:][template_obj[2].index(item[0])]

            template_obj[2] = cdef[1:]
            #print(template_obj)
            self.class_index[class_name] = ClassDef(template_obj, self)

        if class_name not in self.class_index:
            super().error(
                ErrorType.TYPE_ERROR,
                f"No class named {class_name} found",
                line_num_of_statement,
            )
        class_def = self.class_index[class_name]
        obj = ObjectDef(
            self, class_def, None, self.trace_output
        )  # Create an object based on this class definition
        return obj

    # returns a ClassDef object
    def get_class_def(self, class_name, line_number_of_statement):
        if class_name not in self.class_index:
            super().error(
                ErrorType.TYPE_ERROR,
                f"No class named {class_name} found",
                line_number_of_statement,
            )
        return self.class_index[class_name]

    def is_template(self, class_name, line_number_of_statement):
        if '@' in class_name:
            split_name = class_name.split('@')
            params = self.class_index[split_name[0]].get_params()
            if self.get_class_def(split_name[0], line_number_of_statement) and len(params) == len(split_name[1:]):
                return True
            else:
                return self.error(ErrorType.TYPE_ERROR)
        else:
            return False

    # returns a bool
    def is_valid_type(self, typename):
        return self.type_manager.is_valid_type(typename)

    # returns a bool
    def is_a_subtype(self, suspected_supertype, suspected_subtype):
        return self.type_manager.is_a_subtype(suspected_supertype, suspected_subtype)

    # typea and typeb are Type objects; returns true if the two type are compatible
    # for assignments typea is the type of the left-hand-side variable, and typeb is the type of the
    # right-hand-side variable, e.g., (set person_obj_ref (new teacher))
    def check_type_compatibility(self, typea, typeb, for_assignment=False):
        return self.type_manager.check_type_compatibility(typea, typeb, for_assignment)

    def __map_class_names_to_class_defs(self, program):
        self.class_index = {}
        for item in program:
            if item[0] == InterpreterBase.CLASS_DEF or item[0] == InterpreterBase.TEMPLATE_CLASS_DEF:
                if item[1] in self.class_index:
                    super().error(
                        ErrorType.TYPE_ERROR,
                        f"Duplicate class name {item[1]}",
                        item[0].line_num,
                    )
                self.class_index[item[1]] = ClassDef(item, self)

    # [class classname inherits superclassname [items]]
    def __add_all_class_types_to_type_manager(self, parsed_program):
        self.type_manager = TypeManager()
        for item in parsed_program:
            if item[0] == InterpreterBase.CLASS_DEF or item[0] == InterpreterBase.TEMPLATE_CLASS_DEF:
                class_name = item[1]
                superclass_name = None
                if item[2] == InterpreterBase.INHERITS_DEF:
                    superclass_name = item[3]
                self.type_manager.add_class_type(class_name, superclass_name)
                if item[0] == InterpreterBase.TEMPLATE_CLASS_DEF:
                    for it in item[2]:
                        self.type_manager.add_class_type(it, None)
