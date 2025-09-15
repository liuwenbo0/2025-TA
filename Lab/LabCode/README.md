# LabCode 实验代码管理系统使用指南

(本文档主要部分由 AI 生成，可能存在错误)

## 概述

本目录是数据结构与算法设计课程的实验代码管理核心，包含学生代码收集、查重分析、成绩统计等完整功能。系统基于HydroOJ平台，提供自动化的代码管理和分析工具。

## 目录结构详解

### 📁 核心工具文件

#### `select_all_problems.py` - 核心查重分析脚本
**功能**：自动化代码查重和统计分析的主要工具

**主要配置参数**：
```python
# 需要分析的实验名称
lab_names = ['lab2']  # 可配置为 ['lab1', 'lab2', 'lab3'] 等

# 结果输出文件名
RESULT_FILE_NAME = 'lab2_pcnt.xlsx'

# 不参与查重的管理员账户
FORBIDDEN_UID = ['U2', 'U126']

# 查重阈值设置（百分比）
DETECT_THRESHOLD = {
    'lab1': {'P14': 30, 'P15': 30, 'P82': 30},
    'lab2': {'P90': 30, 'P91': 30},
    'lab3': {'P40': 30, 'P92': 30}
}
```

**工作流程**：
1. 读取用户信息（`user.csv`）建立ID-学号映射
2. 扫描各实验目录下的学生代码文件
3. 按题目分类整理代码文件
4. 调用`sim_c++.exe`进行代码相似度检测
5. 解析查重结果，统计违规情况
6. 生成Excel报告（summary、detail、statistic三个sheet）

#### `requirements.txt` - Python依赖
```
openpyxl  # Excel文件操作库
```

#### `user.csv` / `user.xlsx` - 学生用户信息
**格式**：`_id,mail,mailLower,uname`
- `_id`：HydroOJ系统内部用户ID
- `uname`：学生学号（如U202311539）

### 📁 学生代码目录

#### `Lab1/`, `lab2/`, `lab3/` - 各实验学生提交代码
**文件命名规则**：
```
U{用户ID}_P{题目ID}_R{提交记录ID}_S{}@{}.{扩展名}
```

**示例**：
- `U216_P2_R66f7b6e6ec56fd55436a0ec3_S1@100.cc` 
  - 用户ID：216
  - 题目ID：P2
  - 提交记录：R66f7b6e6ec56fd55436a0ec3
  - 文件类型：C++源码

**目录结构**：
```
Lab1/
├── P14/                    # 按题目分类的代码文件
│   ├── U136_P14_..._.c
│   ├── U149_P14_..._.c
│   └── log.txt            # 查重工具输出日志
├── P15/
└── 各学生代码文件（根目录）
```

### 📁 查重工具

#### `similarity/sim_c++.exe` - C++代码相似度检测工具
**使用方法**：
```bash
sim_c++.exe -R -o log.txt -p -s
```

**参数说明**：
- `-R`：递归处理子目录
- `-o log.txt`：输出结果到log.txt
- `-p`：显示百分比相似度
- `-s`：简化输出格式

**输出格式示例**：
```
Total input files: 45
./U127_P14_..._.cc: 294 tokens, 55 lines
./U128_P14_..._.cc: 340 tokens, 56 lines
./U127_P14_..._.cc ./U128_P14_..._.cc (85%)
```

#### `Electron-SIMGUI.V1.1.0/` - 图形界面查重工具
包含`代码查重.exe`，提供可视化的代码相似度检测界面。

### 📁 成绩管理

#### 完成率统计文件
- `all_pcnt.xlsx` - 所有实验完成率汇总
- `lab2_pcnt.xlsx`, `lab3_pcnt.xlsx` - 各实验完成率详情
- `p_cnt.xlsx` - 查重统计汇总

#### 成绩目录
- `scores/` - 历史成绩数据
- `new_scores/` - 最新成绩数据
- `final_scores/` - 最终成绩汇总

#### `new_scores/summary.py` - 成绩汇总脚本
**功能**：
- 读取各实验CSV成绩文件
- 计算基础题和拓展题分数
- 应用查重扣分规则
- 生成最终成绩Excel报告

**关键配置**：
```python
# 实验列表
lab_names = ['lab1', 'lab2', 'lab3', 'lab4', 'lab5', 'lab6']

# 各实验基础题数量
lab_base_problem_num = {
    'lab1': 4, 'lab2': 3, 'lab3': 2, 
    'lab4': 2, 'lab5': 2, 'lab6': 2
}

# 特殊学生分数调整
special_student_base = {
    'lab6': {'U202311554': 200}
}
```

### 📁 检查和验证

#### `Checks/` - 实验检查数据
- `lab1_judge.csv` - 实验1人工评判结果
- `lab2_judge.csv` - 实验2人工评判结果
- `check.xlsx` - 检查汇总表

**评判等级**：
- `a` - 优秀
- `b` - 良好  
- `c` - 及格

### 📁 辅助文件

#### `导出users方法.txt` - HydroOJ用户数据导出方法
```javascript
// 连接数据库
hydrooj db

// 建立游标
var cursor = db.user.find({});

// 导出用户信息
cursor.forEach(function(doc) {
    print(doc._id + "," + doc.mail + "," + doc.mailLower + "," + doc.uname);
});
```

## 使用流程指南

### 🚀 新助教快速上手

#### 1. 环境准备
```bash
# 安装Python依赖
cd Lab/LabCode
pip install -r requirements.txt
```

#### 2. 用户信息管理
1. **导出用户数据**：
   - 登录HydroOJ管理后台
   - 使用`导出users方法.txt`中的脚本导出用户信息
   - 保存为`user.csv`文件

2. **验证用户信息**：
   ```python
   import pandas as pd
   users = pd.read_csv('user.csv')
   print(users.head())
   ```

### 📥 实验代码收集流程

#### 每次实验结束后的操作步骤：

1. **从HydroOJ下载代码**：
   - 登录HydroOJ管理后台
   - 进入对应实验的管理页面
   - 选择"下载所有提交"功能
   - 下载得到压缩包（如`lab2.zip`）

2. **解压和整理代码**：
   ```bash
   # 解压到对应目录
   unzip lab2.zip -d lab2/
   
   # 检查文件结构
   ls lab2/
   ```

3. **验证文件命名**：
   确保文件遵循命名规则：`U{ID}_P{题目}_R{记录}_S{得分}@{满分}.{扩展名}`

### 🔍 代码查重分析

#### 运行查重分析：

1. **配置分析参数**：
   编辑`select_all_problems.py`：
   ```python
   # 设置要分析的实验
   lab_names = ['lab2']
   
   # 设置输出文件名
   RESULT_FILE_NAME = 'lab2_pcnt.xlsx'
   
   # 调整查重阈值（可选）
   DETECT_THRESHOLD = {
       'lab2': {
           'P90': 30,  # P90题目30%相似度阈值
           'P91': 30   # P91题目30%相似度阈值
       }
   }
   ```

2. **执行查重分析**：
   ```bash
   cd Lab/LabCode
   python select_all_problems.py
   ```

3. **查看分析结果**：
   - 打开生成的Excel文件（如`lab2_pcnt.xlsx`）
   - **summary** sheet：学生查重次数汇总
   - **detail** sheet：详细的查重记录
   - **statistic** sheet：各题目查重统计

#### 查重结果解读：

**Summary Sheet**：
- `_id`：学生用户ID
- `学号`：学生学号
- `总数`：该学生被查重的题目总数

**Detail Sheet**：
- 每列代表一个题目（如`lab2_P90`）
- `1`表示该学生在此题目上被查重
- `0`表示未被查重

**Statistic Sheet**：
- `Problem`：题目名称
- `p_cnt`：被查重的学生数量
- `tot_submit`：总提交人数
- `p_rate`：查重率（被查重人数/总提交人数）

### 📊 成绩统计与管理

#### 1. 准备成绩数据
从HydroOJ导出各实验的成绩CSV文件：
- `lab1.csv`, `lab2.csv`, `lab3.csv` 等

#### 2. 配置成绩计算
编辑`new_scores/summary.py`：
```python
# 确认实验列表
lab_names = ['lab1', 'lab2', 'lab3', 'lab4', 'lab5', 'lab6']

# 确认各实验基础题数量
lab_base_problem_num = {
    'lab1': 4,  # lab1有4道基础题
    'lab2': 3,  # lab2有3道基础题
    # ...
}
```

#### 3. 生成最终成绩
```bash
cd Lab/LabCode/new_scores
python summary.py
```

生成的`lab_scores.xlsx`包含：
- **scores** sheet：按总分排序的成绩单
- **statistic** sheet：分数段统计
- **scores_by_uid** sheet：按学号排序的成绩单
- **scores_by_class** sheet：按班级排序的成绩单

### ⚠️ 重要注意事项

#### 查重分析注意事项：
1. **阈值设置**：不同题目应设置不同的查重阈值
   - 简单题目（如基础数据结构）：建议30-50%
   - 复杂题目（如算法实现）：建议60-80%

2. **禁用题目**：某些过于简单的题目可以禁用查重
   ```python
   FORBIDDEN = [2, 4, 7, 16, 17]  # 禁用的题目ID列表
   ```

3. **管理员账户**：确保管理员和助教账户不参与查重
   ```python
   FORBIDDEN_UID = ['U2', 'U126']  # 管理员用户ID
   ```

#### 文件管理注意事项：
1. **备份重要数据**：定期备份`user.csv`和成绩文件
2. **版本控制**：保留历史版本的查重结果和成绩数据
3. **权限控制**：确保敏感数据的访问权限

#### 成绩计算注意事项：
1. **基础题识别**：确保正确配置各实验的基础题数量
2. **特殊情况处理**：使用`special_student_base`处理特殊学生的分数调整
3. **查重扣分**：查重次数会影响最终成绩，需要合理设置扣分规则

### 🛠️ 故障排除

#### 常见问题及解决方案：

1. **用户ID不存在错误**：
   ```
   uid 为 XXX 的学生不存在，请检查信息是否导入完整
   ```
   **解决**：重新导出并更新`user.csv`文件

2. **查重工具无法运行**：
   **解决**：检查`similarity/sim_c++.exe`是否存在且有执行权限

3. **Excel文件生成失败**：
   **解决**：确保安装了`openpyxl`库：`pip install openpyxl`

4. **代码文件格式错误**：
   **解决**：检查从HydroOJ下载的文件命名是否符合规范

### 📈 高级功能

#### 自定义查重规则：
可以根据具体需求修改`select_all_problems.py`中的查重逻辑：

```python
# 自定义相似度计算
def custom_similarity_check(file1, file2):
    # 实现自定义的相似度检测逻辑
    pass

# 自定义结果过滤
def filter_results(results):
    # 实现自定义的结果过滤逻辑
    pass
```

#### 批量处理多个实验：
```python
# 一次性处理多个实验
lab_names = ['lab1', 'lab2', 'lab3', 'lab4', 'lab5']
RESULT_FILE_NAME = 'all_labs_pcnt.xlsx'
```

### 📞 技术支持

如遇到技术问题，请检查：
1. Python环境和依赖库是否正确安装
2. 文件路径和权限是否正确
3. HydroOJ导出的数据格式是否符合预期
4. 查重工具是否能正常运行

---

**最后更新**：2025年
**维护者**：数据结构与算法设计课程组
**适用版本**：HydroOJ + Python 3.x