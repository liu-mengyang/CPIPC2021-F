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
vector<people>Emp2;
vector<flight>Fli2;
vector<path>paths;
vector<path>paths_base1;
vector<path>paths_base2;
vector<flight>sj;    //sort过的Fli2
vector<bool>vis;
int min_date = 35;
int min_CT = 40;
string base1 = "HOM";
string base2 = "TGD";
vector<node>cjnum; 
vector<flight>failed;
vector <people> T1_base1, T1_base2, T2_base1, T2_base2, T3_base1, T3_base2;
vector<path>valid_path_base1;

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
	/*for (flight x : Fli2)
	{
		cout << x.index << " " << x.ID << " " << x.beg_date << " " << x.beg_time << " " << x.beg_tot << " " << x.arr_date << " " << x.arr_time << " " << x.arr_tot << endl;
	}*/
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
void Find_effe_path()
{
	sj = Fli2;
	sort(sj.begin(), sj.end(), cmp);
	/*for (auto x : sj)
	{
		cout<< x.beg_tot <<" "<< x.arr_tot << endl;
	}*/

	//cout << sj.size() << endl;
	vis = vector<bool>(sj.size(), 0);
	for (int i = 0; i < sj.size(); i++)
	{
		if (!vis[i] && (sj[i].beg == base1 || sj[i].beg == base2 ) ) //说明找到了可以开始的点
		{
			string cur_base = sj[i].beg;
			vector<int>lx;
			lx.push_back(i);
			vis[i] = 1;
			int k = i;
			while (k < sj.size())
			{
				bool isfind = 0;
				for (int j = k + 1; j < sj.size(); j++)
				{
					if (isok(k, j))
					{
						isfind = 1;
						lx.push_back(j);
						vis[j] = 1;
						k = j;
						break;
					}
				}
				if (isfind == 0)  //当前节点已经没有可行的后继节点了,说明一条可行路径已经出来了
				{
					//剔除结尾不是基地的节点
					int end = lx.size() - 1;
					int i;
					for (i = end; i >= 0; i--)
					{
						string des = sj[lx[i]].des;
						if (des == cur_base) break;
						else vis[lx[i]] = 0;
					}
					int num = end - i;
					while (num--) lx.pop_back();

					if (lx.size() > 1)
					{
						path p;
						p.lx = lx;
						paths.push_back(p);
						lx.clear();
					}
					break;
				}
			}
			i++;
		}
	}
	
	cout << "可行路径数量" << paths.size() << endl;
	int num = 0;
	for (int i = 0; i < paths.size(); i++)
	{
		num += paths[i].lx.size();
	}
	cout << "分配路径数量" << num << endl;


	//查看具体路径：
	//for (int j = 0; j < paths.size(); j++)
	//{
	//	if (j % 50 != 0) continue;
	//	cout << "第" << j << "条路径" << endl;
	//	vector<int>lx = paths[j].lx;
	//	for (int i = 0; i < lx.size(); i++)
	//	{
	//		int idx = lx[i];
	//		cout << sj[idx].beg << " " << sj[idx].des << " " << sj[idx].beg_tot << " " << sj[idx].arr_tot << endl;
	//	}
	//}
	
	//unordered_map<int, bool>hash;

	//检验是否有重复的
	//for (int i = 0; i < paths.size(); i++)
	//{
	//	vector<int>lx = paths[i].lx;
	//	for (int j = 0; j < lx.size(); j++)
	//	{
	//		if (!hash[lx[j]]) hash[lx[j]] = 1;
	//		else cout << "error" << endl;
	//	}
	//}
	////输出未分配的航班
	//cout << "未分配的航班数" << endl;
	//for (int i = 0; i < sj.size(); i++)
	//{
	//	if (hash[i] == 0)
	//		cout << sj[i].ID << " " << sj[i].beg << " " << sj[i].des << " " << sj[i].beg_tot << " " << sj[i].arr_tot << endl;
	//}
	//检验所有的有效路径是否满足要求
	//for (int i = 0; i < paths.size(); i++)
	//{
	//	vector<int>lx = paths[i].lx;
	//	for (int j = 0; j < lx.size() - 1; j++)
	//	{
	//		int k = j + 1;
	//		if (iserror(lx[j], lx[k]))
	//		{
	//			cout << "error" << endl;
	//		}
	//	}
	//	int end = lx.size() - 1;
	//	if (sj[lx[0]].beg != sj[lx[end]].des || !(sj[lx[end]].des == base1 || sj[lx[end]].des == base2)) cout << "error" << endl;
	//}
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
			ee.num_dh = (v1.size() + v2.size())*2;
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
void cal_sumfli()
{
	/*
	base1: 24正+24副  4副+4正副 8正副+8正副  共计36组(替补次数12)
	base2: 59正+59副  104正副+104副 剩余67副 共计163组
	*/
	int sum = 0;
	int base1_crew = 36;
	
	for (int i = 0; i < paths_base1.size(); i++)
	{
		paths_base1[i].hb_num = paths_base1[i].lx.size();
	}
	sort(paths_base1.begin(), paths_base1.end(), [=](path a,path b) 
	{     return a.hb_num > b.hb_num; });
	for (int i = 0; i < base1_crew; i++)
	{
		sum += paths_base1[i].hb_num;
		valid_path_base1.push_back(paths_base1[i]);
	}
	//未分配的航班添加到failed中
	for (int i = base1_crew; i < paths_base1.size(); i++)
	{
		vector<int>v = paths_base1[i].lx;
		for (auto x : v) failed.push_back(sj[x]);
	}
	for (int i = 0; i < paths_base2.size(); i++)
		sum += paths_base2[i].lx.size();
	cout << "完成的总航班数：" << endl;
	cout << sum << endl;

}
int cal_date(int x)
{
	return x / 1440;
}
void print_table1() //输出未成功分配的航班   航班号+日
{
	unordered_map<int, bool>hash;

	for (int i = 0; i < paths.size(); i++)
	{
		vector<int>lx = paths[i].lx;
		for (int j = 0; j < lx.size(); j++)
		{
			if (!hash[lx[j]]) hash[lx[j]] = 1;
			else cout << "error" << endl;
		}
	}
	for (int i = 0; i < sj.size(); i++)
	{
		if (hash[i] == 0)
			failed.push_back(sj[i]);
	}
	cout << "未成功分配的数量" << endl;
	cout << failed.size() <<endl;

	ofstream outfile("E://math_data//m1B_a.txt", ios::out);
	outfile << "航班号" << " " << "日期" << endl;
	for (int i = 0; i < failed.size(); i++)
	{
		flight f = failed[i];
		outfile << f.ID << " " << cal_date(f.beg_tot) << endl;
	}
	cout << "table1 finish" << endl;
}
void print_table2()
{
	vector<people>distrbuted;
	int p_cap = 0, p_fo = 0, p_cf = 0;
	//分配base2的航班给合适的人  
	for (int i = 0; i < paths_base2.size(); i++)
	{
		vector<int>lx = paths_base2[i].lx;  //get 路径
		//分配给合适的两个人
		if (p_cap < T2_base2.size() && p_fo<T1_base2.size())
		{
			people cap = T2_base2[p_cap];
			people fo = T1_base2[p_fo];
			cap.hx = lx;
			cap.cur_pos = 'C';
			fo.hx = lx;
			fo.cur_pos = 'F';
			distrbuted.push_back(cap);
			distrbuted.push_back(fo);
			p_cap++, p_fo++;
		}
		else if(p_fo<T1_base2.size() && p_cf <T3_base2.size())
		{
			people cf = T3_base2[p_cf];
			people fo = T1_base2[p_fo];
			cf.hx = lx;
			cf.cur_pos = 'C';
			fo.hx = lx;
			fo.cur_pos = 'F';
			distrbuted.push_back(cf);
			distrbuted.push_back(fo);
			p_cf++, p_fo++;
		}
	}
	//cout << distrbuted.size() << endl;
	//分配base1的航班给合适的人 
	p_cap = 0, p_fo = 0, p_cf = 0;
	for (int i = 0; i < valid_path_base1.size(); i++)
	{
		vector<int>lx = valid_path_base1[i].lx;  //get 路径
		//分配给合适的两个人
		if (p_cap < T2_base1.size() && p_fo < T1_base1.size())
		{
			people cap = T2_base1[p_cap];
			people fo = T1_base1[p_fo];
			cap.hx = lx;
			cap.cur_pos = 'C';
			fo.hx = lx;
			fo.cur_pos = 'F';
			distrbuted.push_back(cap);
			distrbuted.push_back(fo);
			p_cap++, p_fo++;
		}
		else if (p_cap < T2_base1.size() && p_cf < T3_base1.size())
		{
			people cf = T3_base1[p_cf];
			people cap = T2_base1[p_cap];
			cf.hx = lx;
			cf.cur_pos = 'F';
			cap.hx = lx;
			cap.cur_pos = 'C';
			distrbuted.push_back(cf);
			distrbuted.push_back(cap);
			p_cap++, p_cf++;
		}
		else  if(p_cf<T3_base1.size())//只剩下全是正副机长的 两两配对
		{
			people cf1 = T3_base1[p_cf];
			p_cf++;
			people cf2 = T3_base1[p_cf];
			p_cf++;
			cf1.hx = lx;
			cf1.cur_pos = 'F';
			cf2.hx = lx;
			cf2.cur_pos = 'C';
			distrbuted.push_back(cf1);
			distrbuted.push_back(cf2);
		}
	}
	cout << "已分配的人员的数量" << endl;
	cout << distrbuted.size() << endl;
	//检验是否人员有重复：
	//unordered_map<string, bool>hash;
	//for (auto x : distrbuted)
	//{
	//	string id = x.id;
	//	if (hash[id]) cout << "error" << endl;
	//	else hash[id] = 1;
	//}

	ofstream outfile("E://math_data//m1B_b.txt", ios::out);
	outfile << "员工" << " " << "航班" <<" "<<"类型" <<endl;
	for (int i = 0; i < distrbuted.size(); i++)
	{
		people person = distrbuted[i];
		vector<int>lx = person.hx;
		for (int j = 0; j < person.hx.size(); j++)
		{
			flight f = sj[lx[j]];
			outfile << person.id << " " << f.ID << " " << cal_date(f.beg_tot) << " " << person.cur_pos<< endl;
		}
	}
	cout << "table2 finish" << endl;
}
int main()
{
	//----------B------
	clock_t start, end;
	B_sldata_C();
	B_sldata_F();
	start = clock();
	cal_totB();
	Find_effe_path();
	crew_distrbution();
	path_base();
	//Find_min_cjnum();
	cal_sumfli();
	end = clock();
	print_table1();
	print_table2();
	cout << "程序运行时长" << endl;
	cout << double(end - start) / CLOCKS_PER_SEC << endl;
	return 0;
}
