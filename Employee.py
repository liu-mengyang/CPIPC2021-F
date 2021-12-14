class Employee(object):
    def __init__(self, Index, EmpNo, Caption, FirstOfficer,
                       Deadhead, Base, DutyCostPerHour,
                       ParingCostPerHour):
        super(Employee).__init__()
        self.index = Index
        self.number = EmpNo
        self.cap_enable = Caption
        self.fo_enable = FirstOfficer
        self.dh_enable = Deadhead
        self.base = Base
        self.d_cost = DutyCostPerHour
        self.p_cost = ParingCostPerHour

    def show(self):
        print("EmpNo: ", self.number, ' (', type(self.number,), ')')
        print("Caption: ", self.cap_enable, ' (', type(self.cap_enable,), ')')
        print("FirstOfficer: ", self.fo_enable, ' (', type(self.fo_enable,), ')')
        print("DeadHead: ", self.dh_enable, ' (', type(self.dh_enable,), ')')
        print("Base: ", self.base, ' (', type(self.base,), ')')
        print("DutyCostPerHour: ", self.d_cost, ' (', type(self.d_cost,), ')')
        print("ParingCostPerHour: ", self.p_cost, ' (', type(self.p_cost,), ')')
        print("----------------------------------")