import sys
import os

def analyze_relationship(text):
    keywords = {
        "情侣": [
            "爱你", "亲爱的", "宝贝", "想你", "吻", "亲亲", "拥抱", "在干嘛", "晚安", "早安", "我也爱你", "喜欢你",
            "心动", "等你", "生气", "道歉", "不理我", "想见你", "约会", "甜蜜", "牵手", "分手", "我错了"
        ],
        "朋友": [
            "哈哈", "一起", "兄弟", "朋友", "姐妹", "聚会", "打游戏", "开黑", "吃饭", "周末", "聊", "出去玩",
            "喝酒", "逛街", "过来", "搓一顿", "没事干", "拍照", "发小", "老同学", "笑死", "叙旧"
        ],
        "同事": [
            "项目", "会议", "报告", "上班", "老板", "加班", "任务", "合作", "进度", "工作", "出差",
            "开会", "绩效", "团队", "文档", "资料", "表格", "客户", "沟通", "工位", "方案", "周报"
        ],
        "家人": [
            "妈妈", "爸爸", "儿子", "女儿", "爷爷", "奶奶", "家", "回家", "吃饭了", "晚点回来", "家庭", "照顾",
            "吃药", "身体", "关心", "早点睡", "我爱你", "回来吧", "外婆", "早点回家", "保重", "小心"
        ],
        "敌对": [
            "傻逼", "滚", "别说了", "闭嘴", "有病", "吵", "烦死了", "你妈", "神经病", "报警", "脑瘫",
            "打你", "恶心", "废物", "别烦我", "操你", "神经", "贱人", "死开", "白痴", "傻叉"
        ],
        "恋人未满": [
            "好喜欢你", "我也有点喜欢你", "偷偷想你", "你很特别", "你觉得我怎么样", "有点心动",
            "你想不想我", "我们合适吗", "喜欢就说", "暧昧", "如果我们在一起", "你单身吗"
        ],
        "陌生人": [
            "你好", "请问", "打扰了", "不认识", "加个好友", "谁啊", "我们聊过吗", "请教一下",
            "认识一下", "交个朋友", "你是谁", "你在哪工作", "怎么称呼", "谢谢"
        ],
        "商业合作": [
            "合作", "洽谈", "报价", "需求", "合同", "发票", "项目对接", "会议纪要", "商讨", "提案",
            "客户需求", "商务合作", "协议", "对接人", "商务洽谈"
        ],
        "师生": [
            "老师", "学生", "作业", "上课", "成绩", "讲题", "批改", "考试", "答疑", "补课",
            "讲解", "教育", "学习", "练习题", "课堂", "校规", "课程", "报告", "期末"
        ]
    }

    scores = {k: 0 for k in keywords}

    for relation, words in keywords.items():
        for w in words:
            if w in text:
                scores[relation] += 1

    sorted_scores = sorted(scores.items(), key=lambda x: x[1], reverse=True)
    top_relation, score = sorted_scores[0]

    if score == 0:
        return "未能从聊天中推断出有效的关系，可能缺乏足够信息。"
    else:
        return f"根据聊天内容，推测关系为：{top_relation}（关键词匹配数：{score}）"

def main():
    if len(sys.argv) < 2:
        print("请提供聊天记录文件路径。")
        return

    chat_file = sys.argv[1]
    if not os.path.exists(chat_file):
        print("聊天记录文件不存在。")
        return

    with open(chat_file, "r", encoding="utf-8") as f:
        chat_text = f.read()

    result = analyze_relationship(chat_text)

    with open("relationship_result.txt", "w", encoding="utf-8") as f:
        f.write(result)

    print("分析完成")

if __name__ == "__main__":
    main()