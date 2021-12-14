import pandas as pd
import numpy as np
import copy

from Flight import Flight
from Employee import Employee


class DataLoader():
    def __init__(self, path_employees, path_flights):
        self.df_employees = pd.read_csv(path_employees).fillna(0)
        self.df_flights = pd.read_csv(path_flights).fillna(0)
        self.employees = []
        self.flights = []
        
        self.C = []
        self.C1 = []
        self.C2 = []
        self.C3 = []
        self.C1_base = []
        self.C2_base = []
        self.C3_base = []
        self.C_base = []
        self.C_dic = {}
        
        self.F = []
        self.F_dic = {}
        self.F_ap_dpt_dic = {}
        self.F_ap_arr_dic = {}
        self.F_leave_base = []
        self.F_arrive_base = []
        self.nonF_leave_base = []
        self.nonF_arrive_base = []

        self.FF = []
        self.FF1 = []
        self.FF2 = []
        self.FD = {}

        self.AP = []
        self.AP_dic = {}
        self.AP_f_dpt_dic = {}
        self.AP_f_arr_dic = {}

        self.Base = []
        self.Base_dic = {}

        self.arrivedate = {}
        self.leavedate = {}
        self.arrivetime = {}
        self.leavetime = {}
        self.leavetimestr = {}
        self.arrivetimestr = {}

        self.Dates = []

        self.DCost = {}
        self.PCost = {}

        self.min_day = self.get_min_day()
        self.min_time = 10000

    def dump_data(self, cropped_date=None, use_optimize=True):
        # dump employees
        self.employees = []
        for i, row in enumerate(self.df_employees.iterrows()):
            EmpNo = row[1][0]
            if row[1][1] == 'Y':
                Caption = True
            else:
                Caption = False
            if row[1][2] == 'Y':
                FirstOfficer = True
            else:
                FirstOfficer = False
            if row[1][3] == 'Y':
                Deadhead = True
            else:
                Deadhead = False
            Base = row[1][4]
            DutyCostPerHour = row[1][5]
            ParingCostPerHour = row[1][6]
            empl = Employee(i, EmpNo, Caption, FirstOfficer, Deadhead, Base, 
                                    DutyCostPerHour, ParingCostPerHour)
            self.employees.append(empl)
            self.C_dic[i] = EmpNo
            
            self.DCost[i] = DutyCostPerHour
            self.PCost[i] = ParingCostPerHour
        # dump flights
        self.flights = []
        i = 0
        for row in self.df_flights.iterrows():
            FltNum = row[1][0]
            DptrDate= int(row[1][1].split('/')[1])
            DptrTime = 60*int(row[1][2].split(':')[0])+int(row[1][2].split(':')[1])
            DptrStn = row[1][3]
            ArrvDate = int(row[1][4].split('/')[1])
            ArrvTime = 60*int(row[1][5].split(':')[0])+int(row[1][5].split(':')[1])
            ArrvStn = row[1][6]
            if cropped_date is None:
                flight = Flight(i, FltNum, DptrDate, DptrTime, DptrStn,
                                        ArrvDate, ArrvTime, ArrvStn)
                self.flights.append(flight)
                self.F_dic[i] = flight.get_unique_number()
                if DptrDate not in self.FD:
                    self.FD[DptrDate] = [i]
                else:
                    self.FD[DptrDate].append(i)
                self.leavedate[i] = DptrDate
                self.arrivedate[i] = ArrvDate
                self.leavetimestr[i] = row[1][2]
                self.arrivetimestr[i] = row[1][5]
                i+=1
            else:
                if DptrDate >= cropped_date[0] and DptrDate <=cropped_date[1]:
                    flight = Flight(i, FltNum, DptrDate, DptrTime, DptrStn,
                                        ArrvDate, ArrvTime, ArrvStn)
                    self.flights.append(flight)
                    self.F_dic[i] = flight.get_unique_number()
                    if DptrDate not in self.FD:
                        self.FD[DptrDate] = [i]
                    else:
                        self.FD[DptrDate].append(i)
                    self.leavedate[i] = DptrDate
                    self.arrivedate[i] = ArrvDate
                    self.leavetimestr[i] = row[1][2]
                    self.arrivetimestr[i] = row[1][5]
                    i+=1

        self.min_time = self.get_min_time()
        self.get_times()
        self.get_C()
        self.get_AP()
        self.get_Base()
        self.get_F()
        if use_optimize:
            self.get_FF()
        self.get_non_F()
        self.get_times()

    def get_times(self):
        fl_len = len(self.flights)
        i = 0
        for flight in self.flights:
            if i % 100 == 0:
                print("Counting times...({}/{})".format(i,fl_len))
            i+=1
            self.leavetime[flight.index] = 24*60*(flight.dpt_date-1)+flight.dpt_time-self.min_time
            self.arrivetime[flight.index] = 24*60*(flight.arr_date-1)+flight.arr_time-self.min_time

    # get different crew
    def get_C(self):
        print("## Loading C")
        self.C = list(self.C_dic.keys()) # all crew
        self.C1 = [] # crew only with caption
        for empl in self.employees:
            if empl.cap_enable and not empl.fo_enable:
                self.C1.append(empl.index)
        self.C2 = [] # crew with caption and first officer
        for empl in self.employees:
            if empl.cap_enable and empl.fo_enable:
                self.C2.append(empl.index)
        self.C3 = [] # crew only with first officer
        for empl in self.employees:
            if empl.fo_enable and not empl.cap_enable:
                self.C3.append(empl.index)
        print("## C loaded")

    # get airport
    def get_AP(self):
        print("## Loading AP")
        self.AP = [] # all airports
        i = 0
        for flight in self.flights:
            if flight.dpt_stn not in self.AP_dic.values():
                self.AP_dic[i] = flight.dpt_stn
                self.AP.append(i)
                i+=1
            if flight.arr_stn not in self.AP_dic.values():
                self.AP_dic[i] = flight.arr_stn
                self.AP.append(i)
                i+=1

        print("## AP loaded")

    # get base
    def get_Base(self):
        print("## Loading Base")
        self.Base = [] # all bases
        self.C_base = [] # crew with different bases
        i = 0
        for emplo in self.employees:
            if emplo.base not in self.Base_dic.values():
                self.Base.append(i)
                self.Base_dic[i] = emplo.base
                i+=1
                temp_a = []
                temp_b = []
                temp_c = []
                temp_d = []
                self.C_base.append(temp_a)
                self.C1_base.append(temp_b)
                self.C2_base.append(temp_c)
                self.C3_base.append(temp_d)
            for key, value in self.Base_dic.items():
                if value == emplo.base:
                    base_index = key
            self.C_base[base_index].append(emplo.index)
            if emplo.cap_enable and not emplo.fo_enable:
                self.C1_base[base_index].append(emplo.index)
            elif emplo.cap_enable and emplo.fo_enable:
                self.C2_base[base_index].append(emplo.index)
            elif emplo.fo_enable and not emplo.cap_enable:
                self.C3_base[base_index].append(emplo.index)
        print("## Base loaded")

    # get flight
    def get_F(self):
        print("## Loading F")
        self.F = [] # all flights
        self.F_leave_base = [] # all leave base flights
        self.F_arrive_base = [] # all arrive base flights
        self.Dates = []
        for i in range(len(self.Base)):
            temp_a = []
            temp_b = []
            self.F_leave_base.append(temp_a)
            self.F_arrive_base.append(temp_b)

        for flight in self.flights:
            self.F.append(flight.index)
            for i in range(len(self.Base)):
                if (flight.dpt_stn == self.Base_dic[i]):
                    self.F_leave_base[i].append(flight.index)
                if (flight.arr_stn == self.Base_dic[i]):
                    self.F_arrive_base[i].append(flight.index)
                if (flight.dpt_date not in self.Dates):
                    self.Dates.append(flight.dpt_date)

            self.F_ap_dpt_dic[flight.index] = self.get_AP_index(flight.dpt_stn)
            self.F_ap_arr_dic[flight.index] = self.get_AP_index(flight.arr_stn)
            if self.get_AP_index(flight.dpt_stn) not in self.AP_f_dpt_dic.keys():
                self.AP_f_dpt_dic[self.get_AP_index(flight.dpt_stn)] = [flight.index]
            else:
                self.AP_f_dpt_dic[self.get_AP_index(flight.dpt_stn)].append(flight.index)
            if self.get_AP_index(flight.arr_stn) not in self.AP_f_arr_dic.keys():
                self.AP_f_arr_dic[self.get_AP_index(flight.arr_stn)] = [flight.index]
            else:
                self.AP_f_arr_dic[self.get_AP_index(flight.arr_stn)].append(flight.index)
        print("## F loaded")

    # get the minimize day in dataset.
    def get_min_day(self):
        day_min = 1000
        for row in self.df_flights.iterrows():
            this_day = int(row[1][1].split('/')[1])
            if this_day < day_min:
                day_min = this_day
        return day_min

    def get_max_day(self):
        day_max = 0
        for row in self.df_flights.iterrows():
            this_day = int(row[1][1].split('/')[1])
            if this_day > day_max:
                day_max = this_day
        return day_max

    def divide_days(self):
        return (self.get_max_day - self.get_min_day + 1) / 2

    def get_min_time(self):
        min_day = self.min_day
        min_time = 1000000
        for flight in self.flights:
            if flight.dpt_time < min_time and flight.dpt_date <= min_day:
                min_time = flight.dpt_time
        return 24*60*(min_day-1) + min_time

    # get False Flights in matrix
    def get_FF(self, minCT=40, MinRest=660):
        print("## Loading FF")
        self.FF = []
        # FF_matrix = np.zeros([len(self.flights),len(self.flights)])
        len_FF = len(self.flights) * len(self.flights)
        count = 0
        for i, flight_a in enumerate(self.flights):
            for j, flight_b in enumerate(self.flights):
                count += 1
                if count % 10000 == 0:
                    print("Loading data... ({}/{})".format(count, len_FF))
                arrv_time_i = flight_a.get_arrv_gap()
                dptr_time_j = flight_b.get_dptr_gap()
                if flight_a.arr_stn == flight_b.dpt_stn and dptr_time_j - arrv_time_i >= minCT:
                    continue

                else:
                    # FF_matrix[i][j]=1
                    self.FF.append((i,j))
        print("## FF loaded")
        print("## Loading FF1")
        self.FF1 = []
        count = 0
        for i, flight_a in enumerate(self.flights):
            for j, flight_b in enumerate(self.flights):
                count += 1
                if count % 10000 == 0:
                    print("Loading data... ({}/{})".format(count, len_FF))
                arrv_time_i = flight_a.get_arrv_gap()
                dptr_time_j = flight_b.get_dptr_gap()
                if flight_b.dpt_date > flight_a.dpt_date and flight_a.arr_stn == flight_b.dpt_stn and dptr_time_j - arrv_time_i >= MinRest:
                    continue
                else:
                    # FF_matrix[i][j]=1
                    self.FF1.append((i,j))
        print("## FF1 loaded")
        print("## Loading FF2")
        self.FF2 = []
        count = 0
        for i, flight_a in enumerate(self.flights):
            for j, flight_b in enumerate(self.flights):
                count += 1
                if count % 10000 == 0:
                    print("Loading data... ({}/{})".format(count, len_FF))
                arrv_time_i = flight_a.get_arrv_gap()
                dptr_time_j = flight_b.get_dptr_gap()
                if flight_b.dpt_date - flight_a.dpt_date > 2 and flight_a.arr_stn == flight_b.dpt_stn and flight_a.arr_stn in self.Base_dic.values():
                    continue
                else:
                    # FF_matrix[i][j]=1
                    self.FF2.append((i,j))
        print("## FF2 loaded")


    def get_non_F(self):
        for i in range (len(self.Base)):
            temp_a = []
            temp_b = []
            self.nonF_leave_base.append(temp_a)
            self.nonF_arrive_base.append(temp_b)
            self.nonF_leave_base[i] = list(set(self.F) - set(self.F_leave_base[i]))
            self.nonF_arrive_base[i] = list(set(self.F) - set(self.F_arrive_base[i]))

    def get_AP_index(self, ap_name):
        for key, value in self.AP_dic.items():
            if value == ap_name:
                return key
