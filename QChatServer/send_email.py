import sys
import smtplib
from email.mime.text import MIMEText

if len(sys.argv) < 3:
    print("参数不足")
    sys.exit(1)

try:
    email = sys.argv[1].encode('utf-8').decode('utf-8')  # 确保传入的是 utf-8 编码
    code = sys.argv[2]
except Exception as e:
    print("编码错误:", e)
    sys.exit(1)

msg = MIMEText(f"Your verification code is: {code}\nUse within 5 minutes.", "plain", "utf-8")
msg['Subject'] = "Verification Code"
msg['From'] = "1017944978@qq.com"
msg['To'] = email

try:
    server = smtplib.SMTP_SSL("smtp.qq.com", 465)
    server.login("1017944978@qq.com", "bqofmefegrjjbedj")
    server.sendmail("1017944978@qq.com", [email], msg.as_string())
    server.quit()
    print("邮件已发送")
except Exception as e:
    print("发送失败:", e)