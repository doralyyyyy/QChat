import jieba
from wordcloud import WordCloud
import matplotlib.pyplot as plt

with open("chat_log.txt", "r", encoding="utf-8") as f:
    text = f.read()

# 使用 jieba 分词
words = jieba.lcut(text)
words = [w for w in words if len(w) > 1 and w != "我"]

# 生成词频字典
from collections import Counter
counts = Counter(words)

# 创建词云对象
wc = WordCloud(
    font_path="msyh.ttc",  # 微软雅黑
    width=800,
    height=600,
    background_color="white",
    colormap="viridis",  # 色系可选：viridis、plasma、inferno、cool、spring 等
    max_words=200,
    max_font_size=100,
    random_state=42
)

wc.generate_from_frequencies(counts)

# 显示图像
plt.figure(figsize=(10, 8))
plt.imshow(wc, interpolation="bilinear")
plt.axis("off")
plt.tight_layout()
plt.show()