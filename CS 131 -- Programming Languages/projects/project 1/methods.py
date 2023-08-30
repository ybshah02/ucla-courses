class MethodDefinition:
    def __init__(self, method_name, method_params, method_body):
        self.name = method_name
        self.params = {key: None for key in method_params}
        self.body = method_body

    def get_name(self):
        return self.name
    
    def get_params(self):
        return self.params
    
    def get_param_value(self, param):
        return self.params[param]
    
    def get_body(self):
        return self.body
    
    def update_params(self, param_values):
        for val, key in zip(param_values, self.params):
            self.params[key] = val