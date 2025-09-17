# OJ 用户管理

## 批量导入用户
[参考hydrooj文档在前端批量导入用户](https://hydro.js.org/docs/Hydro/system/import-user)

步骤:
1. 根据学生名单,构建导入所用csv或excel,复制(注意直接使用csv进行导入时,用户信息列不可用,应该用excel打开csv文件再进行复制)
2. 在前端导入用户页面粘贴数据,进行预览,确认无误后导入
3. 检查前端提示信息中成功导入的用户的数量和表格中的数量是否一致
4. 在管理域界面中查看导入的用户的小组信息是否正确

## 在后台服务器上列出所有用户

打开数据库
```bash
hydrooj db
```

```javascript
// 以json格式列出user的所有属性
db.user.find({}).forEach(doc => {
    printjson(doc);
});

// 输出 CSV 的特定的值
db.user.find({}).forEach(doc =>
      print(`${doc._id},${doc.mail},${doc.uname},${doc.ip},${doc.school}`)
);
```

## 在后台服务器上操作数据库

打开数据库
```bash
hydrooj db
```
hydrooj的数据库为mongo db,可以搜索mongo db相关的代码进行数据库操作
其中用户相关的表名为是user
```javascript
// 删除id大于9527的所有用户
db.user.deleteMany({
    _id: { $gt: 9527 }
  })
```
