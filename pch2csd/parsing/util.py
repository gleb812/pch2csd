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
