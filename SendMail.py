#!/usr/bin/env python

import smtplib

from optparse import OptionParser

from email.MIMEMultipart import MIMEMultipart
from email.MIMEText import MIMEText
from email.MIMEImage import MIMEImage

help = "Usage: ./SendMail.py -c <config_file_name> [-p <photo_file_name>]"

def parse_config(config_file):
    params = {}
    with open(config_file) as config:
        for line in config:
            pair = line.split(':=')
            if len(pair) != 2:
                raise NameError("Can't parse parameters")
            params[pair[0].strip()] = pair[1].strip()
    return params

def check_config_errors(params):
    if 'sender' not in params:
        print 'Config file has no "sender" field'
        exit()
    if 'password' not in params:
        print 'Config file has no "password" field'
        exit()
    if 'recipient' not in params:
        print 'Config file has no "recipient" field'
        exit()
    if 'smtp' not in params:
        print 'Config file has no "smtp" field'
        exit()
    if 'port' not in params:
        print 'Config file has no "port" field'
        exit()
    if 'subject' not in params:
        print 'Config file has no "subject" field'
        exit()
    if 'message_photo' and 'message' not in params:
        print 'Config file has no "message" or "message_photo" field'
        exit()

parser = OptionParser()

parser.add_option("-c", "--config", dest = "config")
parser.add_option("-p", "--photo", dest = "photo")

(options, args) = parser.parse_args()

if options.config == None:
    print help
    exit()

try:
    params = parse_config(options.config)
    check_config_errors(params)
except NameError("Can't parse parameters"):
    print 'Config file has wrong format'
    exit()

fromaddr = params['sender']
toaddr = params['recipient']

msgRoot = MIMEMultipart('related')
msgRoot['Subject'] = params['subject']
msgRoot['From'] = fromaddr
msgRoot['To'] = toaddr
msgRoot.preamble = 'This is a multi-part message in MIME format.'

msgAlternative = MIMEMultipart('alternative')
msgRoot.attach(msgAlternative)

msgText = MIMEText('This is the alternative plain text message.')
msgAlternative.attach(msgText)

msgText = ""
if options.photo != None and 'message_photo' in params:
    msgText = MIMEText(params['message_photo'], 'html')
else:
    msgText = MIMEText(params['message'], 'html')
msgAlternative.attach(msgText)

if options.photo != None:
    fp = open(options.photo, 'rb')
    msgImage = MIMEImage(fp.read())
    fp.close()

    msgImage.add_header('Content-ID', '<image1>')
    msgRoot.attach(msgImage)

username = params['sender'][:(params['sender']).find('@')]
password = params['password']

server = smtplib.SMTP(params['smtp'], params['port'])
server.starttls()
server.login(username, password)
server.sendmail(fromaddr, toaddr, msgRoot.as_string())
server.quit()
