
class zzz:

    def __init__(self):
        self.var1 = 1337
        self.var2 = "var2_value"

    def sub(self):
        return "sub_value"
    def woei(self):
        print("test")
        self.var1 = 5555
        self.var2 = self.sub()
        return self.var2

i = 0
while(i != 1000):
    c1 = zzz()
    print("VAR1 ", c1.var1)
    print("VAR2 ", c1.var2)
    g = c1.woei()
    print(g)


    i += 1
