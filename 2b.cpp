#include<iostream>
#include<fstream>
#include<vector>
#include<string>
#include<math.h>
#include<stdio.h>
#include<algorithm>
#include<map>
#include<unordered_map>
#include<utility>
#include <time.h>　
#include<queue>

using namespace std;

struct people
{
	string id;
	int type; //1是副机长，2是主机长，3是均可
	string base;
	int dcost;
	int pcost;
	vector<int>hx;//存储航线编号，通过sj[编号]找到对应航班
	char cur_pos;    //C是正，F是副
	int tot_time;//执勤时长
	int duty_num;//执勤次数
	int fly_time; //飞行时长
};

struct flight
{
	int index;
	string ID;
	string des;
	string beg;
	int arr_date;
	int arr_time;
	int beg_date;
	int beg_time;
	int beg_tot;
	int arr_tot;
};
struct duty
{
	vector<flight>flis;//一个执勤所包含的所有航段
	vector<int>flies_index;
	int beg_time;
	int end_time;      //均是总时间
	string beg;
	string des;
	int duty_time_tot = 0;
	int duty_time_fly = 0;   //==end_time-beg.time

};
struct path
{
	vector<int>lx;
	int hb_num;
};
struct node
{
	int num_fli;
	int num_dh;
	vector<int>vf;
	vector<int>vd;
};
bool cmp(flight a, flight b)
{
	return a.beg_tot < b.beg_tot;
	//return a.arr_tot < b.arr_tot;
}
struct renwu
{
	vector<duty>dutys;
	int flis;
	int beg_time;
	int arr_time;
	int tot_duty_time;
};
bool cmp2(renwu a, renwu b)
{
	return a.tot_duty_time > b.tot_duty_time;
}

struct partner
{
	people a, b;
	int work_time = 0;
	int tot_money;
	vector<renwu>rws;
	int left = 0;
	int end = 0;

	//工作时长不相等，时长短的在前面，便于均衡，工作时间相等，钱少的在前面，成本低
	bool operator < (partner x) const
	{
		if (work_time == x.work_time)  return tot_money > x.tot_money;    //想让小的在前面，用>  
		return work_time > x.work_time;                               //想让大的在前面，用<   
	}

};
vector<people>Emp2;
vector<flight>Fli2;
vector<path>paths;
vector<path>paths_base1;
vector<path>paths_base2;
vector<flight>sj;    //sort过的Fli2
vector<bool>vis;
int min_date = 35;
int min_CT = 40;
int max_fly = 600;
int max_duty = 720;
int min_rest = 660;
string base1 = "HOM";
string base2 = "TGD";
vector<node>cjnum;
vector<flight>failed;
vector <people> T1_base1, T1_base2, T2_base1, T2_base2, T3_base1, T3_base2;
vector<path>valid_path_base1;
vector<renwu>renwus;
vector<partner>jz1; //放base1的员工
vector<partner>jz2;  //放base2的员工

vector<renwu>rw1;  //放基地1的任务
vector<renwu>rw2;  //放基地2的任务

vector<partner>p_base1;
vector<partner>p_base2;

void B_sldata_C()  //读入data_C的数据
{
	ifstream infile("data//Data_BC.csv", ios::in);
	if (!infile)
	{
		cout << "open error" << endl;
		exit(1);
	}
	string s, ss;
	while (!infile.eof())
	{
		people p;
		infile >> p.id;
		infile >> s >> ss;
		int num = 0;
		if (s == "Y") num += 2;
		if (ss == "Y") num += 1;
		p.type = num;
		infile >> s;//跳过
		infile >> p.base;
		infile >> s;
		p.dcost = stoi(s);
		infile >> s;
		p.pcost = stoi(s);
		if (p.id == "") continue;
		Emp2.push_back(p);
	}
	/*cout<<Emp2.size()<<endl;
	for(people x:Emp2)
	{
		cout<<x.id<<" "<<x.type<<" "<<x.base<<" "<<x.dcost<<" "<<x.pcost<<endl;
	}*/

}
int cal_date(int x)
{
	return x / 1440;
}
int str_to_date(string s)
{
	for (int i = 0; i < s.size(); i++)
	{
		if (s[i] == '/')
		{
			int j = i + 1;
			int data = 0;
			while (s[j] != '/')
			{
				data = data * 10 + (s[j] - '0');
				j++;
			}
			return data;
		}
	}
}
int str_to_time(string s)
{
	int res = 0;
	int num = 0;
	for (int i = 0; i < s.size(); i++)
	{
		if (s[i] == ':')
		{
			res += num * 60;
			num = 0;
		}
		else num = num * 10 + (s[i] - '0');
	}
	res += num;
	return res;
}

void B_sldata_F()   //读入data_F的数据
{
	ifstream infile("data//Data_BF.csv", ios::in);
	if (!infile)
	{
		cout << "open error" << endl;
		exit(1);
	}
	string s;
	int num = 1;
	while (!infile.eof())
	{
		flight f;
		f.index = num++;
		infile >> f.ID;
		infile >> s;
		f.beg_date = str_to_date(s);
		infile >> s;
		f.beg_time = str_to_time(s);
		infile >> f.beg;
		infile >> s;
		f.arr_date = str_to_date(s);
		infile >> s;
		f.arr_time = str_to_time(s);
		infile >> f.des;
		infile >> s;
		if (f.ID == "") continue;
		Fli2.push_back(f);
	}
	cout << Fli2.size() << endl;
	/*for (flight x : Fli2)
	{
		cout << x.index << " " << x.ID << " " << x.beg_date << " " << x.beg_time << " " << x.beg << " " << x.arr_date << " " << x.arr_time << " " << x.des << endl;
	}*/
}

void cal_totB()
{
	int len = Fli2.size();
	for (int i = 0; i < len; i++)
	{
		flight x = Fli2[i];
		Fli2[i].arr_tot = (x.arr_date) * 24 * 60 + x.arr_time;
		Fli2[i].beg_tot = (x.beg_date) * 24 * 60 + x.beg_time;
	}
	/*unordered_map<int, bool>hh;
	int num = 0;
	for (flight x : Fli2)
	{
		int k1 = cal_date(x.beg_tot);
		int k2 = cal_date(x.arr_tot);
		
		if (!hh[k1])
		{
			cout << k1 << endl;
			hh[k1] = 1;
			num++;
		}
		if (!hh[k2])
		{
			cout << k2 << endl;
			hh[k2] = 1;
			num++;
		}
	}
	cout << num << endl;*/
		
}
bool isok(int i, int j)
{
	if (sj[i].des == sj[j].beg && sj[j].beg_tot - sj[i].arr_tot >= min_CT && !vis[j])
		return 1;
	else return 0;
}
bool iserror(int i, int j)
{
	if (sj[i].des == sj[j].beg && sj[j].beg_tot - sj[i].arr_tot >= min_CT)
		return 0;
	else return 1;
}
bool isdutyok(duty e, int j)  //判断sj[j]放入duty中是否符合要求
{
	if (sj[j].arr_tot - e.beg_time > max_duty) return 0; //超过总的执勤时长
	if ((sj[j].arr_tot - sj[j].beg_tot) + e.duty_time_fly > max_fly)  return 0;//超过总的飞行时长
	if (cal_date(e.beg_time) != cal_date(sj[j].arr_tot)) return 0;  //加入的sj[j]和duty不在同一天
	return 1;
}

void crew_distrbution()
{
	vector <people> T1, T2, T3;
	for (auto x : Emp2)
		if (x.type == 1)      T1.push_back(x);
		else if (x.type == 2) T2.push_back(x);
		else if (x.type == 3) T3.push_back(x);
	cout << "每个type的人员分布情况" << endl;
	cout << T1.size() << " " << T2.size() << " " << T3.size() << endl;

	for (auto x : Emp2)
	{
		if (x.type == 1) if (x.dcost != 600) cout << x.id << endl;
		else if (x.type == 2) if (x.dcost != 680) cout << x.id << endl;
		else if (x.type == 3) if (x.dcost != 640) cout << x.id << endl;
	}

	for (auto x : Emp2)
	{
		if (x.type == 1 && x.base == base1) T1_base1.push_back(x);
		else if (x.type == 1 && x.base == base2) T1_base2.push_back(x);
		else if (x.type == 2 && x.base == base1) T2_base1.push_back(x);
		else if (x.type == 2 && x.base == base2) T2_base2.push_back(x);
		else if (x.type == 3 && x.base == base1) T3_base1.push_back(x);
		else if (x.type == 3 && x.base == base2) T3_base2.push_back(x);
	}
	cout << "每个type的人员分布和基地情况" << endl;
	cout << "基地1:" << endl;
	cout << T1_base1.size() << " " << T2_base1.size() << " " << T3_base1.size() << endl;
	cout << "基地2:" << endl;
	cout << T1_base2.size() << " " << T2_base2.size() << " " << T3_base2.size() << endl;


	//分配人员base1
	int i1 = 0, i2 = 0, i3 = 0;
	int num = 24;
	while (num--)
	{
		partner pp;
		people a = T2_base1[i2++];  //正
		a.cur_pos = 'C';
		people b = T1_base1[i1++];  //副
		b.cur_pos = 'F';
		pp.a = a, pp.b = b;
		pp.tot_money = (a.dcost + b.dcost);
		pp.work_time = 0;
		jz1.push_back(pp);
	}
	num = 4;
	while (num--)
	{
		partner pp;
		people a = T2_base1[i2++];
		a.cur_pos = 'C';
		people b = T3_base1[i3++];
		b.cur_pos = 'F';
		pp.a = a, pp.b = b;
		pp.tot_money = (a.dcost + b.dcost);
		pp.work_time = 0;
		jz1.push_back(pp);
	}
	num = 8;
	while (num--)
	{
		partner pp;
		people a = T3_base1[i3++];
		a.cur_pos = 'C';
		people b = T3_base1[i3++];
		b.cur_pos = 'F';
		pp.a = a, pp.b = b;
		pp.tot_money = (a.dcost + b.dcost);
		pp.work_time = 0;
		jz1.push_back(pp);
	}
	cout << "base1的机长对数:";
	cout << jz1.size() << endl;
	//检验是否有重复
	//unordered_map<string, bool>hash;
	//for (auto x:jz1)
	//{
	//	string a = x.a.id;
	//	string b = x.b.id;
	//	if (hash[a] || hash[b]) cout << "error" << endl;
	//	hash[a] = 1;
	//	hash[b] = 1;
	//}

	//分配人员base2
	i1 = 0, i2 = 0, i3 = 0;
	num = 59;
	while (num--)
	{
		partner pp;
		people a = T2_base2[i2++];
		a.cur_pos = 'C';
		people b = T1_base2[i1++];
		b.cur_pos = 'F';
		pp.a = a, pp.b = b;
		pp.tot_money = (a.dcost + b.dcost);
		pp.work_time = 0;
		jz2.push_back(pp);
	}
	num = 104;
	while (num--)
	{
		partner pp;
		people a = T3_base2[i3++];
		a.cur_pos = 'C';
		people b = T1_base2[i1++];
		b.cur_pos = 'F';
		pp.a = a, pp.b = b;
		pp.tot_money = (a.dcost + b.dcost);
		pp.work_time = 0;
		jz2.push_back(pp);
	}
	cout << "base2的机长对数:";
	cout << jz2.size() << endl;
	//检验是否有重复
	unordered_map<string, bool>hash;
	for (auto x : jz2)
	{
		string a = x.a.id;
		string b = x.b.id;
		if (hash[a] || hash[b]) cout << "error" << endl;
		hash[a] = 1;
		hash[b] = 1;
	}

}
void Find_min_cjnum()
{
	//统计所有的未乘机人员：放入left中
	int sum = 0;
	unordered_map<int, bool>hash;
	for (int i = 0; i < paths.size(); i++)
	{
		vector<int>lx = paths[i].lx;
		for (int j = 0; j < lx.size(); j++)
		{
			hash[lx[j]] = 1;
		}
	}
	vector <flight>left;
	for (int i = 0; i < sj.size(); i++)
	{
		if (hash[i] == 0) left.push_back(sj[i]);
	}
	cout << left.size() << endl;

	//处理left中的点使得连通的边变成一个对象
	sort(left.begin(), left.end(), cmp);
	vector<flight>new_left;
	vector<bool>vis(left.size(), 0);
	vector<vector<int>>com_path;   //通过left中的顺序访问
	for (int i = 0; i < left.size(); i++)
	{
		vector<int>v;
		flight	x = left[i];
		if (vis[i]) continue;
		vis[i] = 1;
		v.push_back(i);
		for (int j = i + 1; j < left.size(); j++)
		{
			flight y = left[j];
			if (x.des == y.beg && y.beg_tot - x.arr_tot >= min_CT && !vis[j]) //合并y给x
			{
				x.des = y.des;
				x.arr_tot = y.arr_tot;
				vis[j] = 1;
				v.push_back(j);
			}
		}
		new_left.push_back(x);
		com_path.push_back(v);
	}
	cout << " new_left.size()" << new_left.size() << endl;

	//测试存储的内容
	/*cout << com_path.size() << endl;
	for (int i = 0; i < com_path.size(); i++)
	{
		vector<int>v = com_path[i];
		cout << endl << endl;
		cout << new_left[i].beg << " " << new_left[i].des << " " << new_left[i].beg_tot << " " << new_left[i].arr_tot << endl;

		if (v.size() > 1)
		{
			cout << "-----------------" << endl;
			for (auto x : v)
			{
				flight ff = left[x];
				cout << ff.beg << " " << ff.des << " " << ff.beg_tot << " " << ff.arr_tot << endl;
			}
		}
	}*/

	//begin
	vector<vector<int>>cj_path = vector<vector<int>>(new_left.size());
	for (int i = 0; i < new_left.size(); i++) //对每个left单独考虑
	{
		flight fx = new_left[i];
		int res1 = 1000000;
		vector<int>v1;     //base1前
		int res2 = 1000000;
		vector <int> v2;   //base1后
		int res3 = 100000;
		vector<int>v3;     //base2前
		int res4 = 100000;
		vector<int>v4;     //base2后
		for (int j = 0; j < paths.size(); j++)
		{
			vector<int>lx = paths[j].lx;

			//查找前半段路的最小路径，长度存储在res1中，具体路径存储在v1中
			int pp = -1; //可以到达left出发点的节点
			for (int k = 0; k < lx.size(); k++)
			{
				flight x = sj[lx[k]];
				if (x.des == fx.beg && fx.beg_tot - x.arr_tot >= min_CT)
					pp = max(pp, k);
			}
			if (pp != -1)
			{
				for (int k = pp; k >= 0; k--)
				{
					flight cur_f = sj[lx[k]];
					if (cur_f.beg == base1)
					{
						int num = k - pp + 1;
						if (num < res1)
						{
							res1 = num;
							v1.clear();
							for (int u = k; u <= pp; u++)
							{
								v1.push_back(lx[u]);
							}
						}
						break;
					}
				}
				for (int k = pp; k >= 0; k--)
				{
					flight cur_f = sj[lx[k]];
					if (cur_f.beg == base2)
					{
						int num = k - pp + 1;
						if (num < res3)
						{
							res3 = num;
							v3.clear();
							for (int u = k; u <= pp; u++)
							{
								v3.push_back(lx[u]);
							}
						}
						break;
					}
				}
			}

			//查找后半段路的最小路径，长度存储在res2中，具体路径存储在v2中
			int qq = -1; //可以到达left出发点的节点
			for (int k = 0; k < lx.size(); k++)
			{
				flight x = sj[lx[k]];
				if (x.beg == fx.des && x.beg_tot - fx.arr_tot >= min_CT)
					qq = min(qq, k);
			}
			if (qq != -1)
			{
				for (int k = qq; k < lx.size(); k++)
				{
					flight cur_f = sj[lx[k]];
					if (cur_f.des == base1)
					{
						int num = k - qq + 1;
						if (num < res2)
						{
							res2 = num;
							v2.clear();
							for (int u = qq; u <= k; u++)
							{
								v2.push_back(lx[u]);
							}
						}
						break;
					}
				}

				for (int k = qq; k < lx.size(); k++)
				{
					flight cur_f = sj[lx[k]];
					if (cur_f.des == base2)
					{
						int num = k - qq + 1;
						if (num < res2)
						{
							res4 = num;
							v4.clear();
							for (int u = qq; u <= k; u++)
							{
								v4.push_back(lx[u]);
							}
						}
						break;
					}
				}
			}

		}
		cout << "第" << i << "组:" << endl;
		bool isvalid_1 = 1;
		bool isvalid_2 = 1;
		if (v1.size() == 0 || (v2.size() == 0 && fx.des != base1)) isvalid_1 = 0;
		if (v3.size() == 0 || (v4.size() == 0 && fx.des != base2)) isvalid_2 = 0;
		int print12 = 0;
		if (isvalid_1 && isvalid_2)
		{
			if (v1.size() + v2.size() < v3.size() + v4.size()) print12 = 1;
			else print12 = 2;
		}
		else if (isvalid_1 && !isvalid_2) print12 = 1;
		else if (!isvalid_1 && isvalid_2) print12 = 2;

		//输出详细信息
		//if (print12==1) //通过base1比base2更优
		//{
		//	for (int i = 0; i < v1.size(); i++)
		//	{
		//		flight x = sj[v1[i]];
		//		cout << x.beg << " " << x.des << " " << x.beg_tot << " " << x.arr_tot << endl;
		//	}
		//	cout << endl;
		//	cout << fx.beg << " " << fx.des << " " << fx.beg_tot << " " << fx.arr_tot << endl;
		//	cout << endl;

		//	for (int i = 0; i < v2.size(); i++)
		//	{
		//		flight x = sj[v2[i]];
		//		cout << x.beg << " " << x.des << " " << x.beg_tot << " " << x.arr_tot << endl;
		//	}

		//}
		//else if (print12 == 2) //通过base2比base1更优
		//{
		//	for (int i = 0; i < v3.size(); i++)
		//	{
		//		flight x = sj[v3[i]];
		//		cout << x.beg << " " << x.des << " " << x.beg_tot << " " << x.arr_tot << endl;
		//	}
		//	cout << endl;
		//	cout << fx.beg << " " << fx.des << " " << fx.beg_tot << " " << fx.arr_tot << endl;
		//	cout << endl;

		//	for (int i = 0; i < v4.size(); i++)
		//	{
		//		flight x = sj[v4[i]];
		//		cout << x.beg << " " << x.des << " " << x.beg_tot << " " << x.arr_tot << endl;
		//	}
		//}
		//else continue;

		if (print12 == 1)
		{
			node ee;
			ee.num_fli = com_path[i].size();
			ee.num_dh = (v1.size() + v2.size()) * 2;
			ee.vf = com_path[i];  //注意这个访问要用left
			vector<int>v;
			for (auto x : v1) v.push_back(x);
			for (int i = 0; i < sj.size(); i++) if (sj[i].index == fx.index) v.push_back(i);
			for (auto x : v2) v.push_back(x);
			ee.vd = v;     //要用sj访问
			cjnum.push_back(ee);

		}
		else if (print12 == 2)
		{
			node ee;
			ee.num_fli = com_path[i].size();
			ee.num_dh = (v3.size() + v4.size()) * 2;
			ee.vf = com_path[i];  //注意这个访问要用left
			vector<int>v;
			for (auto x : v3) v.push_back(x);
			for (int i = 0; i < sj.size(); i++) if (sj[i].index == fx.index) v.push_back(i);
			for (auto x : v4) v.push_back(x);
			ee.vd = v;     //要用sj访问
			cjnum.push_back(ee);
		}
		else
		{
			continue;
		}


	}
	cout << cjnum.size() << endl;
	for (int i = 0; i < cjnum.size(); i++)
	{
		node x = cjnum[i];
		cout << x.num_fli << " " << x.num_dh << endl;
	}

}
void path_base()
{
	for (int i = 0; i < paths.size(); i++)
	{
		path x = paths[i];
		vector<int>v = x.lx;
		if (sj[v[0]].beg == base1)
			paths_base1.push_back(x);
		else  paths_base2.push_back(x);
	}
	cout << paths_base1.size() << endl;
	cout << paths_base2.size() << endl;

	////测试path_base1
	//cout << "path_base1" << endl;
	//for (int i = 0; i < paths_base1.size(); i++)
	//{
	//	if (i % 20 != 0) continue;

	//	cout << "第" << i << "条有效路径" << endl;
	//	vector<int>v = paths_base1[i].lx;
	//	for (auto x : v)
	//	{
	//		flight f = sj[x];
	//		cout << f.beg << " " << f.des << " " << f.beg_tot << " " << f.arr_tot << endl;
	//	}
	//}
	////测试path_base2
	//cout << "path_base2" << endl;
	//for (int i = 0; i < paths_base2.size(); i++)
	//{
	//	if (i % 20 != 0) continue;

	//	cout << "第" << i << "条有效路径" << endl;
	//	vector<int>v = paths_base2[i].lx;
	//	for (auto x : v)
	//	{
	//		flight f = sj[x];
	//		cout << f.beg << " " << f.des << " " << f.beg_tot << " " << f.arr_tot << endl;
	//	}
	//}
}

void print_table()
{
	ofstream outfile("E://math_data//m2B_b___2.txt", ios::out);
	outfile << "员工" << " " << "航班" << " " << "日期" << " " << "类型" << endl;

	int tot_cost = 0;

	unordered_map<int, bool>ok_fli;
	vector<people>P_with_worktime;

	//先处理基地1的
	cout << "处理base1" << endl;
	cout << p_base1.size() << endl;
	for (auto x : p_base1)
	{
		partner pp = x;
		tot_cost += (pp.work_time*pp.tot_money);

		people a = pp.a;
		people b = pp.b;
		int fly_time = 0;
		int duty_num = 0;

		for (int i = 0; i < x.rws.size(); i++)
		{
			vector<duty> dutys = x.rws[i].dutys;
			duty_num += dutys.size();
			for (int k = 0; k < dutys.size(); k++)
			{
				duty dd = dutys[k];
				fly_time += dd.duty_time_fly;
				for (int u = 0; u < dd.flis.size(); u++)
				{
					flight f = dd.flis[u];
					//"员工" << " " << "航班" << " " << "日期" << " " << "类型" << endl;
					outfile << a.id << " " << f.ID << " " << cal_date(f.beg_tot) << " " << a.cur_pos << endl;
					outfile << b.id << " " << f.ID << " " << cal_date(f.beg_tot) << " " << b.cur_pos << endl;

					ok_fli[f.index] = 1;

				}
			}
		}
		a.tot_time = pp.work_time;
		b.tot_time = pp.work_time;

		a.fly_time = fly_time;
		b.fly_time = fly_time;

		a.duty_num = duty_num;
		b.duty_num = duty_num;

		P_with_worktime.push_back(a);
		P_with_worktime.push_back(b);
	}


	//先处理基地2的
	cout << "处理base2" << endl;
	cout << p_base2.size() << endl;
	for (auto x : p_base2)
	{
		partner pp = x;
		tot_cost += (pp.work_time*pp.tot_money);

		people a = pp.a;
		people b = pp.b;

		int fly_time = 0;
		int duty_num = 0;

		for (int i = 0; i < x.rws.size(); i++)
		{
			vector<duty> dutys = x.rws[i].dutys;
			duty_num += dutys.size();
			for (int k = 0; k < dutys.size(); k++)
			{
				duty dd = dutys[k];
				fly_time += dd.duty_time_fly;
				
				for (int u = 0; u < dd.flis.size(); u++)
				{
					flight f = dd.flis[u];
					//"员工" << " " << "航班" << " " << "日期" << " " << "类型" << endl;
					outfile << a.id << " " << f.ID << " " << cal_date(f.beg_tot) << " " << a.cur_pos << endl;
					outfile << b.id << " " << f.ID << " " << cal_date(f.beg_tot) << " " << b.cur_pos << endl;

					ok_fli[f.index] = 1;

				}
			}
		}
		a.tot_time = pp.work_time;
		b.tot_time = pp.work_time;

		a.fly_time = fly_time;
		b.fly_time = fly_time;

		a.duty_num = duty_num;
		b.duty_num = duty_num;

		P_with_worktime.push_back(a);
		P_with_worktime.push_back(b);
	}

	cout << "总成本： " << tot_cost << endl;

	int fin_flis = 0;       //成功分配的次数
	vector<flight>undistr;  //未分配的集合
	for (int i = 0; i < Fli2.size(); i++)
	{
		int index = Fli2[i].index;
		if (ok_fli[index] == 1) fin_flis++;
		else  undistr.push_back(Fli2[i]);
	}

	cout << "成功分配的航班数： " << fin_flis << endl;
	cout << "未分配的航班数：  " << undistr.size() << endl;


	ofstream outfile2("E://math_data//m2B_a___2.txt", ios::out);
	outfile2 << "航班号" << " " << "日期" << endl;
	for (auto x : undistr)
	{
		outfile2 << x.ID << " " << cal_date(x.beg_tot) << endl;
	}

	ofstream outfile3("E://math_data//m2B_c___2.txt", ios::out);
	outfile3 << "员工号" << " " << "执勤时长" << " " << " 飞行时长" << " " << "执勤天数" << endl;
	cout << "实际工作人数: " << P_with_worktime.size() << endl;
	cout << "机组总体利用率: " << double(P_with_worktime.size()) / Emp2.size() << endl;
	for (auto p : P_with_worktime)
	{
		outfile3 << p.id << " " << p.tot_time << " " << p.fly_time << " " << p.duty_num << endl;
	}

	int maxn = 10000;
	int max_flytime = 0, min_flytime = maxn, ave_flytime = 0;
	int max_dutytime = 0, min_dutytime = maxn, ave_dutytime = 0;
	int max_dutynum = 0, min_dutynum = maxn, ave_dutynum = 0;

	for (auto p : P_with_worktime)
	{
		//处理平均的
		ave_flytime += p.fly_time;
		ave_dutytime += p.tot_time;
		ave_dutynum += p.duty_num;
		//处理max
		max_flytime = max(max_flytime, p.fly_time);
		max_dutytime = max(max_dutytime, p.tot_time);
		max_dutynum = max(max_dutynum, p.duty_num);
		//处理min
		min_flytime = min(min_flytime, p.fly_time);
		min_dutytime = min(min_dutytime, p.tot_time);
		min_dutynum = min(min_dutynum, p.duty_num);
	}
	ave_flytime /= (P_with_worktime.size());
	ave_dutytime /= (P_with_worktime.size());
	ave_dutynum /= (P_with_worktime.size());

	cout << "max_flytime: " << max_flytime << endl;
	cout << "max_dutytime: " << max_dutytime << endl;
	cout << "max_dutynum: " << max_dutynum << endl;

	cout << "min_flytime: " << min_flytime << endl;
	cout << "min_dutytime: " << min_dutytime << endl;
	cout << "min_dutynum: " << min_dutynum << endl;

	cout << "ave_flytime: " << ave_flytime << endl;
	cout << "ave_dutytime: " << ave_dutytime << endl;
	cout << "ave_dutynum: " << ave_dutynum << endl;


	//输出一次时长：
	vector<partner>p_tot;
	for (auto x : p_base1) p_tot.push_back(x);
	for (auto y : p_base2) p_tot.push_back(y);

	int min_fly_time_1=maxn, max_fly_time_1=0, ave_fly_time_1=0;
	int min_duty_time_1=maxn, max_duty_time_1=0, ave_duty_time_1=0;
	int min_duty_num_1=maxn, max_duty_num_1=0, ave_duty_num_1=0;
	int num = 0;
	int date = 0;

	for (auto x : p_tot)
	{
		int duty_date = 0;
		for (auto y : x.rws)
		{
			vector<duty>dutys = y.dutys;
			duty_date += dutys.size();
			for (auto z : dutys) //一次执勤
			{
				num++;
				min_fly_time_1 = min(min_fly_time_1, z.duty_time_fly);
				min_duty_time_1 = min(min_duty_time_1, z.duty_time_tot);

				max_fly_time_1 = max(max_fly_time_1, z.duty_time_fly);
				max_duty_time_1 = max(max_duty_time_1, z.duty_time_tot);

				ave_duty_time_1 += z.duty_time_tot;
				ave_fly_time_1 += z.duty_time_fly;
			}
		}
		min_duty_num_1 = min(min_duty_num_1, duty_date);
		max_duty_num_1 = max(max_duty_num_1, duty_date);
		date += duty_date;
	}
	ave_fly_time_1 /= num;
	ave_duty_time_1 /= num;
	ave_duty_num_1 = date / p_tot.size();

	cout << "max_flytime_1: " << max_fly_time_1 << endl;
	cout << "max_dutytime_1: " << max_duty_time_1 << endl;
	cout << "max_dutynum_1: " << max_duty_num_1 << endl;

	cout << "min_flytime_1: " << min_fly_time_1 << endl;
	cout << "min_dutytime_1: " << min_duty_time_1 << endl;
	cout << "min_dutynum_1: " << min_duty_num_1 << endl;

	cout << "ave_flytime_1: " << ave_fly_time_1 << endl;
	cout << "ave_dutytime_1: " << ave_duty_time_1 << endl;
	cout << "ave_dutynum_1: " << ave_duty_num_1 << endl;

}
void Find_effe_duty()
{
	int num = 0;
	sj = Fli2;
	sort(sj.begin(), sj.end(), cmp);
	//cout << sj.size() << endl;
	vis = vector<bool>(sj.size(), 0);
	vector<duty>dutys;
	for (int i = 0; i < sj.size(); i++)
	{
		flight fx = sj[i];
		if (!vis[i] && (fx.beg == base1 || fx.beg == base2)) //begin find
		{
			cout << i << " " << num << endl;
			//cout << fx.beg << endl;
			string cur_base = fx.beg;
			duty dd;
			dd.beg = fx.beg;
			dd.des = fx.des;
			dd.beg_time = fx.beg_tot;
			dd.end_time = fx.arr_tot;
			dd.duty_time_fly += (fx.arr_tot - fx.beg_tot);
			dd.flis.push_back(fx);
			dd.flies_index.push_back(i);

			bool dd_empty = 0;
			vis[i] = 1;
			int k = i;
			while (k < sj.size())
			{
				//cout << k << endl;
				bool isfind = 0;
				for (int j = k + 1; j < sj.size(); j++)
				{
					if (!vis[j] && isok(k, j) && isdutyok(dd, j))
					{
						dd.des = sj[j].des;
						dd.end_time = sj[j].arr_tot;
						dd.duty_time_fly += (sj[j].arr_tot - sj[j].beg_tot);
						dd.flis.push_back(sj[j]);
						dd.flies_index.push_back(j);


						k = j;
						vis[j] = 1;
						isfind = 1;
						break;
					}
					else if (isok(k, j) && !isdutyok(dd, j)) //说明一个duty结束了
					{

						dd.duty_time_tot = dd.end_time - dd.beg_time;
						dutys.push_back(dd);

						//如果当前的执勤结束是基地，说明是一个有效的任务
						if (dd.des == cur_base)
						{
							renwu rw;
							rw.dutys = dutys;
							renwus.push_back(rw);
							dutys.clear();
						};
						num += dd.flis.size();
						//重新初始化dd 用sj[j]
						//因为isok==1，说明sj[j]的起点一定和dd的终点相等，也就是等于base，保证了每个dd的起点都是base
						dd.flis.clear();
						dd.flies_index.clear();
						dd.beg = sj[j].beg;
						dd.beg_time = sj[j].beg_tot;
						dd.des = sj[j].des;
						dd.end_time = sj[j].arr_tot;
						dd.duty_time_fly = 0;
						dd.duty_time_fly += (sj[j].arr_tot - sj[j].beg_tot);
						dd.flis.push_back(sj[j]);
						dd.flies_index.push_back(j);


						vis[j] = 1;
						k = j;
						isfind = 1;
						break;
					}
				}
				if (isfind == 0)  //说明找不到后继节点
				{
					//判断当前的dd是否符合要求 基地到基地
					int len = dd.flis.size();
					bool isnone = 1;
					for (int t = len - 1; t >= 0; t--)
					{
						if (dd.flis[t].des == cur_base)
						{
							isnone = 0;
							break;
						}
						else
						{
							vis[dd.flies_index[t]] = 0;
							dd.flies_index.pop_back();
							dd.flis.pop_back();
						}
					}
					if (isnone == 1)
					{
						for (int ll = 0; ll < dd.flies_index.size(); ll++) vis[dd.flies_index[ll]] = 0;
						dd.flies_index.clear();
						dd.flis.clear();
					}
					if (dd.flis.size() > 0) //说明当前的dd有base
					{
						dd.duty_time_tot = dd.end_time - dd.beg_time;
						dutys.push_back(dd);
						num += dd.flis.size();

						renwu rw;
						rw.dutys = dutys;
						renwus.push_back(rw);
						dutys.clear();
					}
					break;
				}
			}

		}
	}
	cout << dutys.size() << endl;
	cout << renwus.size() << endl;


	for (int i = 0; i < renwus.size(); i++)
	{
		if (i % 100 != 0) continue;
		cout << "任务" << i << endl;
		cout << renwus[i].dutys.size() << endl;
		vector<duty>dutys = renwus[i].dutys;
		for (int j = 0; j < dutys.size(); j++)
		{
			duty dt = dutys[j];
			cout << dt.beg << " " << dt.des << " " << dt.beg_time << " " << dt.end_time << " " << dt.flis.size() << endl;
		}

	}









	//vis = vector<bool>(sj.size(), 0);
	//for (int i = 0; i < sj.size(); i++)
	//{
	//	if (!vis[i] && (sj[i].beg == base1 || sj[i].beg == base2)) //说明找到了可以开始的点
	//	{
	//		string cur_base = sj[i].beg;
	//		vector<int>lx;
	//		lx.push_back(i);
	//		vis[i] = 1;
	//		int k = i;
	//		while (k < sj.size())
	//		{
	//			bool isfind = 0;
	//			for (int j = k + 1; j < sj.size(); j++)
	//			{
	//				if (isok(k, j))
	//				{
	//					isfind = 1;
	//					lx.push_back(j);
	//					vis[j] = 1;
	//					k = j;
	//					break;
	//				}
	//			}
	//			if (isfind == 0)  //当前节点已经没有可行的后继节点了,说明一条可行路径已经出来了
	//			{
	//				//剔除结尾不是基地的节点
	//				int end = lx.size() - 1;
	//				int i;
	//				for (i = end; i >= 0; i--)
	//				{
	//					string des = sj[lx[i]].des;
	//					if (des == cur_base) break;
	//					else vis[lx[i]] = 0;
	//				}
	//				int num = end - i;
	//				while (num--) lx.pop_back();

	//				if (lx.size() > 1)
	//				{
	//					path p;
	//					p.lx = lx;
	//					paths.push_back(p);
	//					lx.clear();
	//				}
	//				break;
	//			}
	//		}
	//		i++;
	//	}
	//}
}


void update_renwu()
{
	for (int i = 0; i < renwus.size(); i++)
	{
		renwu x = renwus[i];
		int len = x.dutys.size();
		renwus[i].beg_time = x.dutys[0].beg_time;
		renwus[i].arr_time = x.dutys[len - 1].end_time;
		int sum = 0;
		vector<duty>dutys = x.dutys;
		for (int j = 0; j < dutys.size(); j++)
		{
			sum += dutys[j].duty_time_tot;
		}
		renwus[i].tot_duty_time = sum;
	}
	for (auto x : renwus)
	{
		if (x.arr_time <= x.beg_time) cout << "error_update" << endl;
		//cout << x.tot_duty_time << endl;
	}
}
duty ini_dd(flight f)
{
	duty dd;
	dd.beg = f.beg;
	dd.des = f.des;
	dd.beg_time = f.beg_tot;
	dd.end_time = f.arr_tot;
	dd.duty_time_fly = (f.arr_tot - f.beg_tot);
	dd.flis.push_back(f);
	return dd;
}
duty duty_add(duty dd, flight f)
{
	dd.des = f.des;
	dd.end_time = f.arr_tot;
	dd.duty_time_fly += (f.arr_tot - f.beg_tot);
	dd.flis.push_back(f);
	return dd;
}

bool can_add_duty(duty dd, flight f) //f可以作为一个新的dd的起点  这里传进来的dd是之前的，因为要从之前的地点出发
{
	if (dd.des != f.beg) return 0;
	if (f.beg_tot - dd.end_time < min_CT) return 0;
	if (f.beg_tot - dd.end_time < min_rest) return 0;

	if (cal_date(dd.end_time) == cal_date(f.beg_tot)) return 0;

	return 1;
}
bool can_to_f(duty dd, flight f)
{
	if (dd.des == f.beg && f.beg_tot - dd.end_time >= min_CT) return 1;
	else return 0;
}
bool can_addf(duty dd, flight f)
{
	if (f.arr_tot - dd.beg_time > max_duty) return 0; //超过总的执勤时长
	if ((f.arr_tot - f.beg_tot) + dd.duty_time_fly > max_fly)  return 0;//超过总的飞行时长
	if (cal_date(dd.beg_time) != cal_date(f.beg_tot)) return 0;  //加入的sj[j]和duty不在同一天
	return 1;
}
void Find_renwu() //
{
	cout << "begin find renwu" << endl;
	vector<duty>dutys;
	sj = Fli2;
	sort(sj.begin(), sj.end(), cmp);
	//cout << sj.size() << endl;
	vis = vector<bool>(sj.size(), 0);
	for (int i = 0; i < sj.size(); i++)
	{
		if (!vis[i] && (sj[i].beg == base1 || sj[i].beg == base2)) //说明从i开始
		{
			dutys.clear();
			flight fx = sj[i];
			string cur_base = fx.beg;
			duty dd;
			dd = ini_dd(fx);
			bool dd_empty = 0;
			vis[i] = 1;

			for (int j = i + 1; j < sj.size(); j++)
			{
				if (vis[j]) continue;
				flight f = sj[j];
				if (dd_empty)
				{
					if (can_add_duty(dd, f))
					{
						dd = ini_dd(f);
						vis[j] = 1;
						dd_empty = 0;
					}
					continue;
				}
				//往下必须要求dd有效
				if (can_to_f(dd, f))   //dd可以到达F，但是不一定能够加入f
				{
					if (can_addf(dd, f)) //dd能够加入f
					{
						dd = duty_add(dd, f);  //更新dd
						vis[j] = 1;
						if (dd.des == cur_base) //说明当前已经找到了有效任务 ，放入renwus中
						{
							dd.duty_time_tot = dd.end_time - dd.beg_time;
							dutys.push_back(dd);

							renwu rw;
							rw.dutys = dutys;
							renwus.push_back(rw);
							dutys.clear();
							dd_empty = 1;
						}
					}
					else  //不能加入f，则结束当前dd，放入dutys中，重新找一个dd
					{
						dd.duty_time_tot = dd.end_time - dd.beg_time;
						dutys.push_back(dd);

						dd_empty = 1;
					}
				}
				//不能到达的话就直接看下一个节点

			}
		}
	}
	cout << "任务数量" << endl;
	cout << renwus.size() << endl;
	//for (int i = 0; i < renwus.size(); i++)
	//{
	//	if (i % 500 != 0) continue;
	//	cout << "任务" << i << endl;
	//	vector<duty>dutys = renwus[i].dutys;
	//	for (int j = 0; j < dutys.size(); j++)
	//	{
	//		duty dd = dutys[j];
	//		cout << dd.beg << " " << dd.des << " " << dd.beg_time << " " << dd.end_time << " " << dd.flis.size() << " " << dd.duty_time_fly << " " << dd.duty_time_tot << endl;
	//	}
	//	//检查是否匹配
	//	if (dutys[0].beg == dutys[dutys.size() - 1].des && (dutys[0].beg == base1 || dutys[0].beg == base2)) {}
	//	else cout << "error1" << endl;

	//	for (int j = 0; j < dutys.size() - 1; j++)
	//	{
	//		int k = j + 1;
	//		duty d1 = dutys[j];
	//		duty d2 = dutys[k];
	//		if (d1.des != d2.beg) cout << "error2" << endl;
	//	}
	//	for (int j = 0; j < dutys.size() - 1; j++)
	//	{
	//		int k = j + 1;
	//		duty d1 = dutys[j];
	//		duty d2 = dutys[k];
	//		if (d2.beg_time - d1.end_time < min_rest) cout << "error" << endl;
	//	}
	//}
	//
	//int sum = 0;
	//for (int i = 0; i < renwus.size(); i++)
	//{
	//	vector<duty>dutys = renwus[i].dutys;
	//	for (int j = 0; j < dutys.size(); j++)
	//	{
	//		sum += dutys[j].flis.size();
	//	}
	//}
	//cout << "总航班数" << endl;
	//cout << sum << endl;
}
void cal_ans()
{
	//求解base1:
	//结果放入p_base1 
	sort(jz1.begin(), jz1.end(), [=](partner a, partner b) {
		return a.tot_money < b.tot_money;
	});
	unordered_map<int, bool>is_choosed;
	sort(rw1.begin(), rw1.end(), [=](renwu a,renwu b) {
		return  a.beg_time < b.beg_time;
	});
	for (int i = 0; i < jz1.size(); i++)
	{
		partner x = jz1[i];   //拿出一对组合，给他们分配任务
		for (int j = 0; j < rw1.size(); j++)
		{
			if (is_choosed[j]) continue;
			renwu rw = rw1[j];
			if (rw.beg_time - x.end >= min_rest && cal_date(rw.beg_time) != cal_date(x.end))
			{
				is_choosed[j] = 1; //标记已选
				if (x.rws.size() == 0)  //更新
				{
					x.left = rw.beg_time;
				}
				x.end = rw.arr_time;
				x.work_time += rw.tot_duty_time;
				x.rws.push_back(rw);
			}
		}
		if (x.rws.size() > 0) p_base1.push_back(x);
	}
	cout <<"p_base1.size() : "<< p_base1.size() << endl;

	//求解base2:
	//结果放入p_base2 
	sort(jz2.begin(), jz2.end(), [=](partner a, partner b) {
		return a.tot_money < b.tot_money;
	});
	unordered_map<int, bool>is_choosed2;
	sort(rw2.begin(), rw2.end(), [=](renwu a, renwu b) {
		return  a.beg_time < b.beg_time;
	});
	for (int i = 0; i < jz2.size(); i++)
	{
		partner x = jz2[i];   //拿出一对组合，给他们分配任务
		for (int j = 0; j < rw2.size(); j++)
		{
			if (is_choosed2[j]) continue;
			renwu rw = rw2[j];
			if (rw.beg_time - x.end >= min_rest && cal_date(rw.beg_time) != cal_date(x.end))
			{
				is_choosed2[j] = 1; //标记已选
				if (x.rws.size() == 0)  //更新
				{
					x.left = rw.beg_time;
				}
				x.end = rw.arr_time;
				x.work_time += rw.tot_duty_time;
				x.rws.push_back(rw);
			}
		}
		if (x.rws.size() > 0) p_base2.push_back(x);
	}
	cout<< "p_base2.size() :"<< p_base2.size() << endl;
	
}
void renwu_distribution()
{
	for (auto x : renwus)
	{
		if (x.dutys[0].beg == base1)
		{
			rw1.push_back(x);
		}
		else if (x.dutys[0].beg == base2)
		{
			rw2.push_back(x);
		}
		else cout << "error" << endl;
	}
	cout << "任务1和任务2的数量" << endl;
	cout << rw1.size() << " " << rw2.size() << endl;
	//check
	/*cout << "check" << endl;
	for (int i = 0; i <5; i++)
	{
		cout << rw1[i].dutys[0].beg << endl;
	}
	for (int i = 0; i < 5; i++)
	{
		cout << rw2[i].dutys[0].beg << endl;
	}

	sort(rw1.begin(), rw1.end(), [=](renwu a, renwu b) {
		return a.flis > b.flis;
	});
	sort(rw2.begin(), rw2.end(), [=](renwu a, renwu b) {
		return a.flis > b.flis;
	});
	cout << "sorted" << endl;
	for (int i = 0; i < 5; i++)
	{
		cout << rw1[i].flis<<" ";
	}
	cout << endl;
	for (int i = 0; i < 5; i++)
	{
		cout << rw2[i].flis << " ";
	}
	cout << endl;*/
}
void cal_renwus_flis()
{
	for (int i = 0; i < renwus.size(); i++)
	{
		int sum = 0;
		vector<duty>dutys = renwus[i].dutys;
		for (auto y : dutys)
		{
			sum += y.flis.size();
		}
		renwus[i].flis = sum;
	}
	//check
	/*int num = 0;
	for (auto x : renwus)
	{
		num += x.flis;
	}
	cout << num << endl;*/
}
void check()
{
	long long zong_cost = 0;
	for (auto x : renwus)
	{
		for (auto y : x.dutys)
		{
			for (auto z : y.flis)
			{
				zong_cost += (z.arr_tot - z.beg_tot) * 680 * 2;
			}
		}
	}
	cout << "总成本为：" << zong_cost << endl;

	/*for (auto rw : p_base1)
	{
		vector<renwu>rws = rw.rws;
		for (int j = 0; j < rws.size(); j++)
		{

		}
	}*/
}
int main()
{
	//----------2B------
	clock_t start, end;

	B_sldata_C();
	B_sldata_F();

	start = clock();

	cal_totB();
	Find_renwu();
	update_renwu();
	crew_distrbution();
	cal_renwus_flis();
	renwu_distribution();
	cal_ans();

	end = clock();

	print_table();
	check();
	cout << "程序运行时长" << endl;
	cout << double(end - start) / CLOCKS_PER_SEC << endl;

	return 0;
}
