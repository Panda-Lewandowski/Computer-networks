import smtplib
import sys
import os
from email.mime.application import MIMEApplication
from email.mime.multipart import MIMEMultipart
from email.mime.base import MIMEBase
from email.utils import COMMASPACE, formatdate
from email import encoders

# python3 smtp.py you@gmail.com passwd mail.to.send@gmail.com path keyword
if __name__ == "__main__":
    gmail_user = sys.argv[1] 
    gmail_password = sys.argv[2]
    send_from = gmail_user  
    send_to = [sys.argv[3]]
    files_folder = sys.argv[4]
    keyword = sys.argv[5]

    os.chdir(files_folder)

    files = os.listdir()
    print(f'{len(files)} files in folder')

    part = MIMEBase('application', "octet-stream")
    for filename in files:
        with open(filename, 'r') as f:
            text = f.read()
            if keyword in text:
                print(f'Find keyword "{keyword}" in {filename}...')
                to_send = filename
                break

    part.set_payload(open(to_send, "rb").read())
    encoders.encode_base64(part)
    part.add_header('Content-Disposition', 'attachment; filename="{}"'.format(filename))

    msg = MIMEMultipart()
    msg['From'] = send_from
    msg['To'] = COMMASPACE.join(send_to)
    msg['Date'] = formatdate(localtime=True)
    msg['Subject'] = 'OMG Super Important Message'
    msg.attach(part)
 
    try:
        server_ssl = smtplib.SMTP_SSL('smtp.gmail.com', 465)
        server_ssl.ehlo()
        server_ssl.login(gmail_user, gmail_password)
        server_ssl.sendmail(send_from, send_to, msg.as_string())
        server_ssl.close()

        print('Email sent!')
    except Exception:
        print("Somothing went wrong!")
