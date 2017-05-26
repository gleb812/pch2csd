class AttrEqMixin:
    def attrs_equal(self, other):
        if type(self) != type(other):
            return False
        attrs_self = self.__dict__
        attrs_other = other.__dict__
        if set(attrs_self.keys()) != set(attrs_other.keys()):
            return False
        for key in attrs_self.keys():
            if attrs_self[key] != attrs_other[key]:
                return False
        return True


class ReprStrMixin:
    def __repr__(self):
        return f"{type(self).__name__}({', '.join([f'{k}={v}' for k, v in self.__dict__.items()])})"

    def __str__(self):
        return self.__repr__()
