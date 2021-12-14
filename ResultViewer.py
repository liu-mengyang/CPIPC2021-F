import matplotlib.pyplot as plt
import pandas as pd
from collections import OrderedDict


class RViewer(object):
    def __init__(self, dataloader, data_cls="a"):
        super(object).__init__()
        self.dl = dataloader
        self.mathched_e_f = []
        self.mathched_e_f_dic = {'em_no':[], 'fl_no':[], 'cls':[]}
        self.mathched_cap_dic = {'em_no':[], 'fl_no':[]}
        self.matched_fo_dic = {'em_no':[], 'fl_no':[]}
        self.matched_dh_dic = {'em_no':[], 'fl_no':[]}
        self.dics = []
        self.data_cls = data_cls

        self.duty_flight_time_dic = {}
        self.duty_time_dic = {}
        self.duty_dates_dic = {}


    def load_results(self, matched_e_f):
        self.mathched_e_f = matched_e_f
        self.generate_dics()

    def generate_dics(self):
        for (em, fl, cls) in self.mathched_e_f:
            self.mathched_e_f_dic['em_no'].append(em)
            self.mathched_e_f_dic['fl_no'].append(fl)
            self.mathched_e_f_dic['cls'].append(cls)
            if cls == 1:
                self.mathched_cap_dic['em_no'].append(em)
                self.mathched_cap_dic['fl_no'].append(fl)
            elif cls == 2:
                self.matched_fo_dic['em_no'].append(em)kb
                self.matched_fo_dic['fl_no'].append(fl)
            elif cls == 3:
                self.matched_dh_dic['em_no'].append(em)
                self.matched_dh_dic['fl_no'].append(fl)
        self.dics = [self.mathched_cap_dic, self.matched_fo_dic, self.matched_dh_dic]

    def draw_ef_gantt(self, duration=None, people=None, save=None):
        plt.rcParams['font.sans-serif'] = ['SimHei']
        plt.rcParams['axes.unicode_minus'] = False
        plt.figure(figsize=(20,8),dpi=80)
        ax = plt.gca()
        ax.invert_yaxis()
        colors = ['#DC143C', '#87CEFA', '#EE82EE']
        labels = ['Caption', 'First-officer', 'Dead-head']
        for (em_no, fl_no, cls) in self.mathched_e_f:
            if duration is not None:
                if self.dl.leavedate[fl_no] > duration:
                    continue
            if people is not None:
                if em_no > people:
                    continue
            plt.barh(em_no,
                    self.count_flight_time(fl_no),
                    left=self.dl.leavetime[fl_no],
                    color=colors[cls-1],
                    label=labels[cls-1])
        handles, labels = plt.gca().get_legend_handles_labels()

        by_label = OrderedDict(zip(labels, handles))

        plt.legend(by_label.values(), by_label.keys())
        plt.title("员工排班航班甘特图")

        if save is not None:
            plt.savefig(save)

    def load_results_from_str_df(self, df):
        self.mathched_e_f = []
        for row in df.iterrows():
            em_no = self.find_emp_no(row[1][0])
            fl_no = self.find_fl_no(str(row[1][1])+"/"+str(row[1][2]))
            if row[1][3] == 'C':
                cls = 1
            elif row[1][3] == 'F':
                cls = 2
            elif row[1][3] == 'D':
                cls = 3
            self.mathched_e_f.append((em_no, fl_no, cls))
        self.generate_dics()

    def load_results_from_df(self, df):
        self.mathched_e_f = []
        for row in df.iterrows():
            em_no = row[1][0]
            fl_no = row[1][1]
            cls = row[1][2]
            self.mathched_e_f.append((em_no, fl_no, cls))
        self.generate_dics()

    def count_flight_time(self, fl_no):
        return self.dl.arrivetime[fl_no]-self.dl.leavetime[fl_no]

    def find_emp_no(self, emp_name):
        for key, value in self.dl.C_dic.items():
                if value == emp_name:
                    return key

    def find_fl_no(self, fl_name):
        for key, value in self.dl.F_dic.items():
            if value == fl_name:
                return key
        print("not find"+str(fl_name))

    def get_results_df_b(self):
        results_dic = {'em_no':[],
                       'leg_no':[],
                       'fl_no':[],
                       'dpt_date':[],
                       'dpt_time':[],
                       'dpt_ap':[],
                       'arr_date':[],
                       'arr_time':[],
                       'arr_ap':[],
                       'cls':[]}
        for i in range(len(self.dl.C)):
            fl_list = []
            for (em_no, fl_no, cls) in self.mathched_e_f:
                if em_no == i:
                    fl_list.append((fl_no,cls))
            for j in range(len(fl_list)):
                for k in range(j+1,len(fl_list)):
                    if self.dl.leavetime[fl_list[k][0]] < self.dl.leavetime[fl_list[j][0]]:
                        temp = fl_list[k]
                        fl_list[k] = fl_list[j]
                        fl_list[j] = temp
            m = 1
            for (fl_no, cls) in fl_list:
                results_dic['em_no'].append(self.dl.C_dic[i])
                results_dic['leg_no'].append(m)
                results_dic['fl_no'].append(self.dl.F_dic[fl_no].split('/')[0])
                if self.data_cls == "b":
                    results_dic['dpt_date'].append("8/"+str(self.dl.leavedate[fl_no])+"/2019")
                elif self.data_cls == "a":
                    results_dic['dpt_date'].append("8/"+str(self.dl.leavedate[fl_no])+"/2021")
                results_dic['dpt_time'].append(self.dl.leavetimestr[fl_no])
                results_dic['dpt_ap'].append(self.dl.AP_dic[self.dl.F_ap_dpt_dic[fl_no]])
                if self.data_cls == "b":
                    results_dic['arr_date'].append("8/"+str(self.dl.arrivedate[fl_no])+"/2019")
                elif self.data_cls == "a":
                    results_dic['arr_date'].append("8/"+str(self.dl.arrivedate[fl_no])+"/2021")
                results_dic['arr_time'].append(self.dl.arrivetimestr[fl_no])
                results_dic['arr_ap'].append(self.dl.AP_dic[self.dl.F_ap_arr_dic[fl_no]])
                if cls == 1:
                    results_dic['cls'].append('C')
                elif cls == 2:
                    results_dic['cls'].append('F')
                elif cls == 3:
                    results_dic['cls'].append('D')
                m+=1
        return pd.DataFrame(results_dic)

    def get_results_df_a(self):
        results_dic = {'fl_no':[],
                       'dpt_date':[],
                       'dpt_time':[],
                       'dpt_ap':[],
                       'arr_date':[],
                       'arr_time':[],
                       'arr_ap':[],
                       'comp':[]}
        fl_list = []
        fl_leave_list = []
        for (em_no, fl_no, cls) in self.mathched_e_f:
            if fl_no not in fl_list:
                fl_list.append(fl_no)
        for fl in self.dl.F:
            if fl not in fl_list:
                fl_leave_list.append(fl)
        for i in range(len(fl_leave_list)):
            for j in range(i+1,len(fl_leave_list)):
                if self.dl.leavedate[fl_leave_list[j]] < self.dl.leavedate[fl_leave_list[i]]:
                    temp = fl_leave_list[j]
                    fl_leave_list[j] = fl_leave_list[i]
                    fl_leave_list[i] = temp
                elif self.dl.leavedate[fl_leave_list[j]] == self.dl.leavedate[fl_leave_list[i]]:
                    if self.dl.leavetime[fl_leave_list[j]] < self.dl.leavetime[fl_leave_list[i]]:
                        temp = fl_leave_list[j]
                        fl_leave_list[j] = fl_leave_list[i]
                        fl_leave_list[i] = temp
                    elif self.dl.leavetime[fl_leave_list[j]] == self.dl.leavetime[fl_leave_list[i]]:
                        if self.dl.F_ap_dpt_dic[fl_leave_list[j]] < self.dl.F_ap_dpt_dic[fl_leave_list[i]]:
                            temp = fl_leave_list[j]
                            fl_leave_list[j] = fl_leave_list[i]
                            fl_leave_list[i] = temp
                        elif self.dl.F_ap_dpt_dic[fl_leave_list[j]] == self.dl.F_ap_dpt_dic[fl_leave_list[i]]:
                            if self.dl.F_ap_arr_dic[fl_leave_list[j]] < self.dl.F_ap_arr_dic[fl_leave_list[i]]:
                                temp = fl_leave_list[j]
                                fl_leave_list[j] = fl_leave_list[i]
                                fl_leave_list[i] = temp
        for fl_no in fl_leave_list:
            results_dic['fl_no'].append(self.dl.F_dic[fl_no].split('/')[0])
            if self.data_cls == "b":
                results_dic['dpt_date'].append("8/"+str(self.dl.leavedate[fl_no])+"/2019")
            elif self.data_cls == "a":
                results_dic['dpt_date'].append("8/"+str(self.dl.leavedate[fl_no])+"/2021")
            results_dic['dpt_time'].append(self.dl.leavetimestr[fl_no])
            results_dic['dpt_ap'].append(self.dl.AP_dic[self.dl.F_ap_dpt_dic[fl_no]])
            if self.data_cls == "b":
                results_dic['arr_date'].append("8/"+str(self.dl.arrivedate[fl_no])+"/2019")
            elif self.data_cls == "a":
                results_dic['dpt_date'].append("8/"+str(self.dl.leavedate[fl_no])+"/2021")
            results_dic['arr_time'].append(self.dl.arrivetimestr[fl_no])
            results_dic['arr_ap'].append(self.dl.AP_dic[self.dl.F_ap_arr_dic[fl_no]])
            results_dic['comp'].append('C1F1')
        return pd.DataFrame(results_dic)

    def count_duty_time(self):
        self.duty_flight_time_dic = {}
        self.duty_time_dic = {}
        self.duty_dates_dic = {}
        for i in range(len(self.dl.C)):
            fl_list = []
            for (em_no, fl_no, cls) in self.mathched_e_f:
                if em_no == i:
                    fl_list.append((fl_no,cls))
            for j in range(len(fl_list)):
                for k in range(j+1,len(fl_list)):
                    if self.dl.leavetime[fl_list[k][0]] < self.dl.leavetime[fl_list[j][0]]:
                        temp = fl_list[k]
                        fl_list[k] = fl_list[j]
                        fl_list[j] = temp
            duty_flight_time = {}
            duty_time = {}
            duty_dates = []
            duty_dpt_time = {}
            duty_arr_time = {}
            for (fl_no, cls) in fl_list:
                if self.dl.leavedate[fl_no] not in duty_dates:
                    duty_dates.append(self.dl.leavedate[fl_no])
                    duty_flight_time[self.dl.leavedate[fl_no]] = self.dl.arrivetime[fl_no] - self.dl.leavetime[fl_no]
                    duty_dpt_time[self.dl.leavedate[fl_no]] = [self.dl.leavetime[fl_no]]
                    duty_arr_time[self.dl.leavedate[fl_no]] = [self.dl.arrivetime[fl_no]]
                else:
                    duty_flight_time[self.dl.leavedate[fl_no]] += self.dl.arrivetime[fl_no] - self.dl.leavetime[fl_no]
                    duty_dpt_time[self.dl.leavedate[fl_no]].append(self.dl.leavetime[fl_no])
                    duty_arr_time[self.dl.leavedate[fl_no]].append(self.dl.arrivetime[fl_no])
            for day in duty_dates:
                duty_time[day] = max(duty_arr_time[day]) - min(duty_dpt_time[day])
            self.duty_flight_time_dic[i] = duty_flight_time
            self.duty_time_dic[i] = duty_time
            self.duty_dates_dic[i] = duty_dates