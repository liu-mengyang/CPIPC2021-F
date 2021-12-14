class Flight(object):
    def __init__(self, Index, FltNum, 
                       DptrDate, DptrTime, DptrStn,
                       ArrvDate, ArrvTime, ArrvStn):
        super(Flight).__init__()
        self.index = Index
        self.number = FltNum
        self.dpt_date = DptrDate
        self.dpt_time = DptrTime
        self.dpt_stn = DptrStn
        self.arr_date = ArrvDate
        self.arr_time = ArrvTime
        self.arr_stn = ArrvStn

    def show(self):
        print("FltNum: " + self.number, ' (', type(self.number,), ')')
        print("DptrDate: " + str(self.dpt_date), ' (', type(self.dpt_date,), ')')
        print("DptrTime: " + str(self.dpt_time), ' (', type(self.dpt_time,), ')')
        print("DptrStn: " + self.dpt_stn, ' (', type(self.dpt_stn,), ')')
        print("ArrvDate: " + str(self.arr_date), ' (', type(self.arr_date,), ')')
        print("ArrvTime: " + str(self.arr_time), ' (', type(self.arr_time,), ')')
        print("ArrvStn: " + self.arr_stn, ' (', type(self.arr_stn,), ')')
        print("----------------------------------")

    # not minus min_day in dataset
    def get_dptr_gap(self):
        return 60*24*self.dpt_date + self.dpt_time
    
    def get_arrv_gap(self):
        return 60*24*self.arr_date + self.arr_time

    def get_unique_number(self):
        return self.number+'/'+str(self.dpt_date)
