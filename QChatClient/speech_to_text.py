# google版 要翻墙
# import sys
# import io
# sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding='utf-8')
# import speech_recognition as sr
#
# if len(sys.argv) < 2:
#     print("No file provided")
#     sys.exit(1)
#
# audio_path = sys.argv[1]
#
# r = sr.Recognizer()
# with sr.AudioFile(audio_path) as source:
#     audio = r.record(source)
#
# # print(f"音频路径：{audio_path}", file=sys.stderr)
#
# try:
#     text = r.recognize_google(audio, language="zh-CN")
#     print(text)
# except sr.UnknownValueError:
#     print("无法识别语音")
# except sr.RequestError as e:
#     print(f"识别服务出错: {e}")


# vosk版  不需要翻墙，就用这个了
import sys, os, wave, json
from vosk import Model, KaldiRecognizer
from pydub import AudioSegment
from pydub.utils import which

sys.stdout.reconfigure(encoding='utf-8')

ffmpeg_path = which("ffmpeg")
if not ffmpeg_path:
    ffmpeg_path = os.path.join(os.path.dirname(__file__), "ffmpeg", "bin", "ffmpeg.exe")
    if not os.path.exists(ffmpeg_path):
        print("未找到 ffmpeg，可将其放在项目的 ffmpeg/bin/ 目录下")
        sys.exit(1)

AudioSegment.converter = ffmpeg_path

if len(sys.argv) != 2:
    print("请提供音频文件路径")
    sys.exit(1)

input_path = sys.argv[1]
converted_path = input_path.replace(".wav", "_converted.wav")

# 转换为 16kHz, 单声道, 16位
try:
    sound = AudioSegment.from_file(input_path)
    sound = sound.set_frame_rate(16000).set_channels(1).set_sample_width(2)
    sound.export(converted_path, format="wav")
except Exception as e:
    print(f"音频转换失败: {e}")
    sys.exit(1)

# 模型加载
script_dir = os.path.dirname(os.path.abspath(__file__))
model_path = os.path.join(script_dir, "model")
if not os.path.exists(model_path) or not os.path.isdir(model_path):
    print("模型路径无效")
    sys.exit(1)

model = Model(model_path)

# 打开 WAV 文件
wf = wave.open(converted_path, "rb")

if wf.getnchannels() != 1 or wf.getsampwidth() != 2 or wf.getframerate() != 16000:
    print("无法识别语音")
    sys.exit(1)

rec = KaldiRecognizer(model, wf.getframerate())

text = ""
while True:
    data = wf.readframes(4000)
    if len(data) == 0:
        break
    if rec.AcceptWaveform(data):
        try:
            partial = json.loads(rec.Result())["text"]
            text += partial + " "
        except:
            continue

try:
    final = json.loads(rec.FinalResult())["text"]
    text += final
except:
    pass

print((text.strip().replace(' ','') if text.strip() else "无法识别语音"))