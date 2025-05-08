import sys
import io
sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding='utf-8')
import speech_recognition as sr

if len(sys.argv) < 2:
    print("No file provided")
    sys.exit(1)

audio_path = sys.argv[1]

r = sr.Recognizer()
with sr.AudioFile(audio_path) as source:
    audio = r.record(source)

# print(f"音频路径：{audio_path}", file=sys.stderr)

try:
    text = r.recognize_google(audio, language="zh-CN")
    print(text)
except sr.UnknownValueError:
    print("无法识别语音")
except sr.RequestError as e:
    print(f"识别服务出错: {e}")