from methods import MethodDefinition

class ClassDefinition:
    def __init__(self, name):
        self.name = name
        self.fields = {}
        self.methods = {}

    def get_name(self):
        name = self.name
        return name

    def add_field(self, field_name, initial_value):
        self.fields[field_name] = initial_value
    
    def add_method(self, method_name, method_params, method_body):
        self.methods[method_name] = MethodDefinition(method_name, method_params, method_body)

    def get_all_fields(self):
        fields = self.fields.copy()
        return fields
    
    def get_all_methods(self):
        methods = self.methods.copy()
        return methods

    def get_field_names(self):
        field_names = self.fields.copy().keys()
        return field_names

    def get_method_names(self):
        method_names = self.methods.copy().keys()
        return method_names

    def get_field(self, field_name):
        field = self.fields.get(field_name)
        return field
    
    def get_method(self, method_name):
        method = self.methods.get(method_name)
        return method
    
    def update_field(self, field_name, new_value):
        self[field_name] = new_value