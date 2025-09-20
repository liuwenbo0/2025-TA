import pandas as pd
import os
import math

pd.set_option('display.notebook_repr_html', False)

# 统计的实验名
lab_names = ['lab1', 'lab2', 'lab3', 'lab4', 'lab5', 'lab6']

# 每个实验中基础题的个数
lab_base_problem_num = {
    'lab1': 4,
    'lab2': 3,
    'lab3': 2,
    'lab4': 2,
    'lab5': 2,
    'lab6': 2,
}

# 扣分系数
PUNISH_RATIO = 2

# 计算所有实验中，基础题的总分
tot_base_score = 0
tot_base_cnt = 0
for lab_name, lab_base_score in lab_base_problem_num.items():
    tot_base_score += lab_base_score * 100
    tot_base_cnt += 1

print(f'tot_base_score: {tot_base_score}')

# 手动修改实验分数
# 手动修改基础题分数
special_student_base = {
    'lab1': {
        'U202311619': 200,
        'U202311611': 200,
        'U202311602': 100,
        'U202312075': 400,
        'U202311621': 400,
        'U202311618': 400,
        'U202315980': 300,
        'U202311557': 300,
    },
    'lab2': {
        'U202311575': 200,
        'U202311611': 200,
        'U202311602': 100,
        'U202312075': 300,
        'U202311604': 300,
        'U202311621': 300,
        'U202311618': 200,
    },
    'lab3': {
        'U202311611': 200,
        'U202311602': 100,
        'U202312075': 200,
        'U202311604': 100,
        'U202311621': 200,
        'U202311618': 80,
        'U202315980': 100,
    },
    'lab4': {
        'U202312075': 200,
        'U202311611': 200,
        'U202311602': 100,
        'U202311621': 200,
    },
    'lab5': {
        'U202311575': 200,
        'U202311611': 200,
        'U202311602': 200,
        'U202312075': 200,
        'U202311621': 200,
        'U202311568': 200,
    },
    'lab6': {
        'U202311553': 200,
        'U202311554': 200,
        'U202311578': 200,
        'U202311579': 200,
        'U202311587': 200,
        'U202311611': 200,
        'U202311602': 100,
        'U202312075': 200,
        'U202311621': 200,
    },
}

# 手动修改拓展题分数
special_student_advanced = {
    'lab1': {'U202311619': 100},
    'lab5': {
        'U202311568': 300,
    },
    'lab6': {
        'U202311553': 100,
        'U202311554': 100,
    },
}

# 手动修改学生总分，优先级高于上面两个
special_student_tot = {'U202311582': tot_base_score, 'U202311577': 500}


# 手动修改代码查重次数
special_plagiarism_cnt = {
    # 'U202215239': 0
}

# 读取xls（绝对路径）
first_class = pd.read_excel(io='./accounts.xlsx', sheet_name=0, usecols='A,F')

second_class = pd.read_excel(io='./accounts.xlsx', sheet_name=1, usecols='A,F')

others_class = pd.read_excel(io='./accounts.xlsx', sheet_name=2, usecols='A,F')


id_name_dict = {}

all_class = [first_class, second_class, others_class]

for class_x in all_class:
    for idx, data in class_x.iterrows():
        id_name_dict[data[0]] = data[1]
        # print(f'data[0]: {data[0]} data[1]: {data[1]}')

print(f'id_name_dict: {id_name_dict}')

lab_scores = {}
lab_detail_scores = {}

tot_scores_dict = {}

for key in id_name_dict.keys():
    tot_scores_dict[key] = 0


for lab_name in lab_names:
    lab_score_file = f'./{lab_name}.csv'

    if not os.path.exists(lab_score_file):
        continue

    lab_scores[lab_name] = {}

    # 统计总分
    lab_score = pd.read_csv(
        lab_score_file, usecols=['排名', '用户', '分数']
    )  # 使用列索引来选择列
    print(f'lab_score: {lab_score}')

    for idx, data in lab_score.iterrows():
        lab_scores[lab_name][data[1]] = data[2]

        if tot_scores_dict.get(data[1]) is not None:
            tot_scores_dict[data[1]] += float(data[2])

    # 统计小分
    lab_score = pd.read_csv(lab_score_file)
    lab_detail_scores[lab_name] = {}
    col_size = lab_score.shape[1]

    for idx, data in lab_score.iterrows():
        lab_detail_scores[lab_name][data[1]] = []

        # 9 为第一题得分的列数，每 3 列可得到一道题的得分
        for i in range(9, col_size, 3):
            if pd.isnull(data[i]):
                lab_detail_scores[lab_name][data[1]].append(float(0))
            else:
                lab_detail_scores[lab_name][data[1]].append(float(data[i]))

    # 原始代码为统计 xlsx 格式，已改为 csv 格式
    # # 统计总分
    # lab_score = pd.read_excel(io=lab_score_file, usecols="A,B,C")
    # for idx, data in lab_score.iterrows():
    #     # print(f'data: {data}')
    #     lab_scores[lab_name][data[1]] = data[2]

    #     if tot_scores_dict.get(data[1]) is not None:
    #         tot_scores_dict[data[1]] += float(data[2])

    # # 统计小分
    # lab_score = pd.read_excel(io=lab_score_file)
    # lab_detail_scores[lab_name] = {}
    # col_size = lab_score.shape[1]

    # for idx, data in lab_score.iterrows():
    #     # print(f'data: {data}')
    #     lab_detail_scores[lab_name][data[1]] = []
    #     for i in range(5, col_size, 3):
    #         if pd.isnull(data[i]):
    #             lab_detail_scores[lab_name][data[1]].append(float(0))
    #         else:
    #             lab_detail_scores[lab_name][data[1]].append(float(data[i]))


print_excel_dict = {}

lab_scores_dict = {}

for lab_name in lab_names:
    base_scores = []
    advanced_scores = []

    if lab_detail_scores.get(lab_name) is None:
        continue

    for sid in id_name_dict.keys():
        scores = [0] * tot_base_cnt
        # if lab_detail_scores[lab_name].get(sid) is None:
        # base_scores.append(0.0)
        # advanced_scores.append(0.0)
        # continue

        if lab_detail_scores[lab_name].get(sid) is not None:
            scores = lab_detail_scores[lab_name][sid]

        if len(scores) <= 0:
            continue
        base_score = 0
        advanced_score = 0

        for i in range(0, lab_base_problem_num[lab_name]):
            base_score += scores[i]
        for i in range(lab_base_problem_num[lab_name], len(scores)):
            advanced_score += scores[i]

        # 特殊处理
        if lab_name in special_student_base:
            if sid in special_student_base[lab_name]:
                special_base_score = special_student_base[lab_name][sid]

                tot_scores_dict[sid] -= base_score
                tot_scores_dict[sid] += special_base_score

                base_score = special_base_score

        # 特殊处理
        if lab_name in special_student_advanced:
            if sid in special_student_advanced[lab_name]:
                special_advanced_score = special_student_advanced[lab_name][sid]

                tot_scores_dict[sid] -= advanced_score
                tot_scores_dict[sid] += special_advanced_score

                advanced_score = special_advanced_score

        base_scores.append(base_score)
        advanced_scores.append(advanced_score)

    lab_scores_dict[f'{lab_name}_基础'] = base_scores
    lab_scores_dict[f'{lab_name}_拓展'] = advanced_scores


tot_scores = []
for sid in id_name_dict.keys():
    if tot_scores_dict.get(sid) is None:
        tot_scores.append(0.0)
    else:
        # 手动修改总分
        if sid in special_student_tot:
            tot_scores.append(special_student_tot[sid])
        else:
            tot_scores.append(tot_scores_dict[sid])

s_ids = id_name_dict.keys()
# print(f's_ids: {s_ids}')


print_excel_dict['学号'] = s_ids

print_excel_dict['总分'] = tot_scores


for lab_name in lab_names:
    print_excel_dict[f'{lab_name}_基础'] = lab_scores_dict[f'{lab_name}_基础']
    print_excel_dict[f'{lab_name}_拓展'] = lab_scores_dict[f'{lab_name}_拓展']


p_cnt = pd.read_excel(io='./p_cnt.xlsx', sheet_name='summary', usecols='B, C')

p_cnt_dict = {}

for idx, data in p_cnt.iterrows():
    p_cnt_dict[data[0]] = data[1]

print(f'p_cnt: {p_cnt_dict}')


# 特殊处理
# p_cnt_dict['U202215239'] = 0

for sid, p_cnt in special_plagiarism_cnt:
    p_cnt_dict[sid] = p_cnt

p_cnt_list = []

for sid in id_name_dict.keys():
    if p_cnt_dict.get(sid) is None:
        p_cnt_dict[sid] = 0
        p_cnt_list.append(0)
        continue
    p_cnt_list.append(p_cnt_dict[sid])


punish_scores_dict = {}

for sid in id_name_dict.keys():
    p_cnt = p_cnt_dict[sid]

    # punish_scores_dict[sid] = (p_cnt) // 3 * 3
    # 扣分
    punish_scores_dict[sid] = p_cnt * PUNISH_RATIO

print_excel_dict['查重数'] = p_cnt_list


punish_scores = []

for sid in id_name_dict.keys():
    punish_scores.append(punish_scores_dict[sid])

print_excel_dict['扣分'] = punish_scores


# 分类
sorted_tot_score = sorted(tot_scores, reverse=True)

tot_size = len(sorted_tot_score)


# 基础题总分之上，划分几个等级
ABOVE_CLASS_NUM = 2

print(f'{sorted_tot_score}')

type_std_score_dict = {}

# 寻找第一个 <= 基础题总分同学的位置
base_score_idx = -1
for i in range(0, len(sorted_tot_score)):
    if sorted_tot_score[i] <= tot_base_score:
        base_score_idx = i
        break

if base_score_idx == -1:
    print('找不到总分小于等于基础题总分的同学！')
    exit(0)

# 处理高于基础题总分的等级
curr_score = 0
for i in range(ABOVE_CLASS_NUM - 1):
    curr_score += base_score_idx // ABOVE_CLASS_NUM
    print(f'curr_score: {curr_score}')
    type_std_score_dict[i] = sorted_tot_score[curr_score]

type_std_score_dict[ABOVE_CLASS_NUM - 1] = sorted_tot_score[base_score_idx - 1]

# 等于基础题总分
type_std_score_dict[ABOVE_CLASS_NUM] = tot_base_score


# 低于基础题总分
# 基础题总分之下，划分几个等级
BELOW_CLASS_NUM = 2

# 找到第一个小于基础题总分的分数
below_base_idx = 0
for i in range(0, len(sorted_tot_score)):
    if sorted_tot_score[i] < tot_base_score:
        below_base_idx = i
        break

# 找到第一个小于基础题总分 * 0.7 的分数
below_0_7_base_idx = 0
for i in range(0, len(sorted_tot_score)):
    if sorted_tot_score[i] < tot_base_score * 0.7:
        below_0_7_base_idx = i
        break
# below_0_7_base_idx = below_0_7_base_idx - 1

curr_std_idx = 0
curr_score = below_base_idx
for i in range(BELOW_CLASS_NUM - 1):
    # 最后的等级分阈值应该从 ABOVE_CLASS_NUM + 1 开始
    curr_std_idx = i + ABOVE_CLASS_NUM + 1

    curr_score += (below_0_7_base_idx - below_base_idx) // BELOW_CLASS_NUM
    curr_score = min(curr_score, len(sorted_tot_score) - 1)
    print(f'curr_score: {curr_score}')
    type_std_score_dict[curr_std_idx] = sorted_tot_score[curr_score]

# 补一个 0.7 base
curr_std_idx += 1
type_std_score_dict[curr_std_idx] = tot_base_score * 0.7

print(f'type_std_score_dict: {type_std_score_dict}')

type_scores_dict = {}

for sid in id_name_dict.keys():
    tot_score = tot_scores_dict[sid]

    # 如果当前分数 >= type_std_score_dict 的分数，则说明处于对应的等级
    for i, score in type_std_score_dict.items():
        if tot_score >= score:
            type_scores_dict[sid] = 100 - i * 5
            break

type_scores = []
for sid in id_name_dict.keys():
    type_score = 0
    if sid in type_scores_dict:
        type_score = type_scores_dict[sid]
    type_scores.append(type_score)

print_excel_dict['等级分'] = type_scores


final_scores_dict = {}
for sid in id_name_dict.keys():
    final_scores_dict[sid] = 0
    if sid in type_scores_dict:
        final_scores_dict[sid] = type_scores_dict[sid] - punish_scores_dict[sid]

final_scores = []
for sid in id_name_dict.keys():
    final_scores.append(final_scores_dict[sid])

final_scores_cnt = {}

for final_score in final_scores:
    final_score = math.ceil(final_score / 10) * 10
    # final_score = round(final_score, -1)
    if final_scores_cnt.get(final_score) is not None:
        final_scores_cnt[final_score] += 1
    else:
        final_scores_cnt[final_score] = 1

print(f'final_scores_cnt: {final_scores_cnt}')

print_excel_dict['最终得分'] = final_scores


for k, v in print_excel_dict.items():
    print(f'{k}_len: {len(v)}')


# 获取按照班级排序的学号
class_ordered_sid = []
class_ordered_final_scores = []
class_ordered_pcnt = []

class_order = pd.read_excel(io='./class_order.xlsx', sheet_name=0, usecols='A')
for idx, data in class_order.iterrows():
    # print(f'{data}')
    # class_ordered_sid.append(data[0])
    class_ordered_sid.append(data.iloc[0])


for sid in class_ordered_sid:
    class_ordered_final_scores.append(final_scores_dict[sid])

for sid in class_ordered_sid:
    class_ordered_pcnt.append(p_cnt_dict[sid])

class_ordered_scores_dict = {}
class_ordered_scores_dict['学号'] = class_ordered_sid
class_ordered_scores_dict['总分'] = class_ordered_final_scores
class_ordered_scores_dict['重复次数'] = class_ordered_pcnt


class_ordered_detail = pd.DataFrame(class_ordered_scores_dict)


detail = pd.DataFrame(print_excel_dict)
detail_sorted_by_uid = detail.sort_values('学号', ascending=True)
detail = detail.sort_values('总分', ascending=False)

# 按照在线表格排序

print_statistic_dict = {}
print_statistic_dict['分数段(<=)'] = final_scores_cnt.keys()
print_statistic_dict['人数'] = final_scores_cnt.values()
statistic = pd.DataFrame(print_statistic_dict)
statistic = statistic.sort_values('分数段(<=)')

with pd.ExcelWriter('lab_scores.xlsx') as writer:
    detail.to_excel(writer, sheet_name='scores', index=False)
    statistic.to_excel(writer, sheet_name='statistic', index=False)
    detail_sorted_by_uid.to_excel(writer, sheet_name='scores_by_uid', index=False)
    detail_sorted_by_uid.to_excel(writer, sheet_name='scores_by_uid', index=False)
    class_ordered_detail.to_excel(writer, sheet_name='scores_by_class', index=False)


# print(f'lab_scores: {lab_scores}')
# print(f'lab_detail_scores: {lab_detail_scores}')


# print(first_class)
# print(second_class)
