import pandas as pd

import os
import shutil
import subprocess

curr_path = os.getcwd()
print(f'当前目录: {curr_path}')

# 查重目录名
lab_names = ['lab2']
# lab_names = ['lab1', 'lab2', 'lab3']
# lab_names = ['Lab1', 'Lab2', 'Lab3', 'Lab4', 'Lab5', 'Lab6']

# 结果文件名
RESULT_FILE_NAME = 'lab2_pcnt.xlsx'

tool_path = f'{curr_path}/similarity/sim_c++.exe'
tool_name = 'sim_c++.exe'

# 不参加查重的 OJ 账户 uid
FORBIDDEN_UID = [
    'U2', 'U126'
]

# 默认查重阈值（%）
DEFAULT_DETECT_THRESHOLD = 100

# 手动设置题目的查重阈值（%）
DETECT_THRESHOLD = {
    'lab1': {
        'P14': 30,
        'P15': 30,
        'P82': 30
    },
    'lab2': {
        'P90': 30,
        'P91': 30
    },

    'lab3': {
        'P40': 30,
        'P92': 30
    }
}


# print(f'tool in {tool_path} exists: {os.path.exists(tool_path)}')
# pd.set_option('display.notebook_repr_html',False)
USE_CSV = True

id_uname_dict = {}

# 为 id 和 学号 建立关系，因为导出的代码是以用户的 id 为标记
if USE_CSV:
    users = pd.read_csv('user.csv')

    for row in users.itertuples():
        id_uname_dict[str(row[1])] = row[4]
else:
    users = pd.read_excel(io=f'user.xlsx', usecols="A,D")


    for idx, data in users.iterrows():
        id_uname_dict[str(data[0])] = data[1]

problem_submit_cnt = {}
plagiarism_cnt = {}

sid_set = set()

# 禁用所有基础题的查重，pid 为题号
forbidden_all_base_pid = [2, 4, 7, 16, 17, 37, 38, 51, 52, 57, 58, 64, 66]

# 17?
forbidden_30_pid = [2, 4, 7, 17, 35, 37, 38, 39, 52, 57, 58, 59, 60, 64]

# FORBIDDEN = forbidden_30_pid
# 选择禁用判重的题目集合
FORBIDDEN = []


for lab_name in lab_names:
    root_path = f'{curr_path}/{lab_name}'

    if not os.path.exists(root_path):
        continue

    os.chdir(f'{root_path}')

    print(f'当前目录: {os.getcwd()}')

    filename_list = os.listdir(root_path)

    pid_set = set()

    for filename in filename_list:
        if len(filename.split('_')) < 4:
            continue

        uid, pid, _, _ = filename.split('_')

        if uid[1:] not in id_uname_dict:
            print(f'uid 为 {uid[1:]} 的学生不存在，请检查信息是否导入完整，流程中止')
            exit(0)

        if id_uname_dict[uid[1:]].startswith('U'):
            sid_set.add(uid)
        pid_set.add(pid)

        # print(f'check path: {root_path}/{pid}')
        if not os.path.exists(f'{root_path}/{pid}'):
            os.makedirs(f'{root_path}/{pid}')

        shutil.copy(filename, f'{root_path}/{pid}')

    plagiarism_cnt[lab_name] = {}

    for pid in pid_set:
        print(f'当前目录: {os.getcwd()}')

        plagiarism_cnt[lab_name][pid] = {}
        # 某个问题的所有代码目录
        problem_path = f'{root_path}/{pid}'
        print(f'检查问题目录:{problem_path}')
        # print(f'run: copy {tool_path} {problem_path}/{tool_name}')
        # shutil.copy({tool_path}, f'{problem_path}/{tool_name}')
        # subprocess.run(f'copy {tool_path} {problem_path}')

        os.chdir(f'{problem_path}')

        print(f'run: {tool_path} -R -o log.txt -p -s')
        subprocess.run(f'{tool_path} -R -o log.txt -p -s')
        with open('log.txt', 'r') as f:
            lines = f.readlines()
            for line in lines:
                if line.startswith('Total input'):
                    problem_submit_cnt[pid] = int(line.split(' ')[2])

                if not line.startswith('.'):
                    continue
                if len(line.split(' ')) != 8:
                    continue
                u1, _, _,  percent, _, _, u2, _ = line.split(' ')

                u1 = u1.split('_')[0][2:]
                u2 = u2.split('_')[0][2:]

                if u1 in FORBIDDEN_UID or u2 in FORBIDDEN_UID:
                    print(f'Forbidden UID {u1} {u2} pid: {pid}')
                    continue

                
                threshold = DEFAULT_DETECT_THRESHOLD
                if lab_name in DETECT_THRESHOLD and pid in DETECT_THRESHOLD[lab_name]:
                    threshold = DETECT_THRESHOLD[lab_name][pid] 

                if int(percent) >= threshold:
                    print(f'problem: {pid} threshold: {threshold}')

                    if int(pid[1:]) not in FORBIDDEN:
                        plagiarism_cnt[lab_name][pid][u1] = 1
                        plagiarism_cnt[lab_name][pid][u2] = 1

                    # if plagiarism_cnt[lab_name][pid].get(u1) is None:
                    #     plagiarism_cnt[u1] = 1
                    # else:
                    #     plagiarism_cnt[u1] += 1
                    # if plagiarism_cnt.get(u1) is None:
                    #     plagiarism_cnt[u1] = 1
                    # else:
                    #     plagiarism_cnt[u1] += 1

    print(f'plagiarism_cnt: {plagiarism_cnt}')

    os.chdir(f'{curr_path}')
    print(f'当前目录: {os.getcwd()}')



tot_cnt          = {}
for sid in sid_set:
    # print(f'sid: {sid}')
    tot_cnt[sid] = 0

    for lab_cnt in plagiarism_cnt.values():
        for pid, problem_cnt in lab_cnt.items():
            # print(f'pid: {pid}, problem_cnt: {problem_cnt}')
            if sid in problem_cnt.keys():
                tot_cnt[sid] += 1




# print(f'id_uname_dict: {id_uname_dict}')
print(f'tot_cnt: {tot_cnt}')
print(f'tot_cnt size: {len(tot_cnt.keys())}')
uids = tot_cnt.keys()

unames = []
p_cnt = []

for uid in uids:
    unames.append(id_uname_dict[uid[1:]])
    p_cnt.append(tot_cnt[uid])

print_dict = {}
print_dict['_id'] = uids
print_dict['学号'] = unames
print_dict['总数'] = p_cnt

print(f'print_dict: {print_dict}')

summary = pd.DataFrame(print_dict)
summary = summary.sort_values('总数', ascending=False)

detail_print_dict = {}
detail_print_dict['_id'] = uids
detail_print_dict['学号'] = unames

statistic_print_dict = {}

statistic_print_dict['Problem'] = []
statistic_print_dict['p_cnt'] = []
statistic_print_dict['tot_sumbit'] = []
statistic_print_dict['p_rate'] = []


detail_print_dict['tot'] = p_cnt


for k, lab_p_cnts in plagiarism_cnt.items():
    
    for p, problems in lab_p_cnts.items():
        statistic_print_dict['Problem'].append(f'{k}_{p}')

        print(f'problems: {problems}')
        detail_print_dict[f'{k}_{p}'] = []

        p_cnt = 0

        for uid in uids:
            # print(f'uid: {uid} {problems.get(uid)}')

            if problems.get(uid) is not None:
                p_cnt += 1
                detail_print_dict[f'{k}_{p}'].append(problems[uid])
            else:
                detail_print_dict[f'{k}_{p}'].append(0)

        statistic_print_dict['p_cnt'].append(p_cnt)
        statistic_print_dict['tot_sumbit'].append(problem_submit_cnt[p])


        statistic_print_dict['p_rate'].append(1.0 * p_cnt / problem_submit_cnt[p])


details = pd.DataFrame(detail_print_dict)
details = details.sort_values('tot', ascending=False)

statistic = pd.DataFrame(statistic_print_dict)

with pd.ExcelWriter(RESULT_FILE_NAME) as writer:
    summary.to_excel(writer, sheet_name='summary', index=False)
    details.to_excel(writer, sheet_name='detail', index=False)
    statistic.to_excel(writer, sheet_name='statistic', index=False)



