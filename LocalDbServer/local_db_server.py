#!/usr/bin/env python3
from bottle import route, run, request, get, response, default_app
from paste import httpserver
import sqlite3
import json
import requests
from pathlib import Path
import os

db_folder = os.path.join(os.path.dirname("Database/"), '3S_Retail.db')

application = default_app()

@get('/UpdateOrSignup')
def message0():

    UID = request.query.UID.lstrip().rstrip()
    Email = request.query.Email.lstrip().rstrip()
    U_name = request.query.U_name.lstrip().rstrip().replace('_',' ')
    print(f"Received the following query parameter UID = {UID}")

    conn = sqlite3.connect(db_folder)     #Connect to the SQLlite database
    cursor = conn.cursor()
    cursor.execute("SELECT COUNT(*) FROM MEMBER WHERE UID=?", (UID,))
    Valid_uid = cursor.fetchone()
    count = Valid_uid[0]

    if (count > 0):
        cursor.execute("UPDATE MEMBER SET Name=? WHERE UID=?", (U_name, UID))
        cursor.execute("UPDATE MEMBER SET Email=? WHERE UID=?", (Email, UID))
        conn.commit()
        Mode = "Update"
    else:
        cursor.execute("INSERT INTO MEMBER(UID, Name, Email, Redeem) VALUES(?,?,?,0)", (UID, U_name, Email))
        conn.commit()
        Mode = "Signup"
    cursor.close()

    response.headers['Content-Type'] = 'application/json'
    response.headers['Cache-Control'] = 'no-cache'

    Mode = {"Mode": Mode}

    print(f"Mode :: {Mode}")
    return json.dumps(Mode)


    

@get('/membership')
def message():

    UID = request.query.UID.lstrip().rstrip()
    print(f"Received the following query parameter UID = {UID}")

    conn = sqlite3.connect(db_folder)     #Connect to the SQLlite database
    cursor = conn.cursor()

    cursor.execute("SELECT COUNT(*) FROM MEMBER WHERE UID=?", (UID,))     #SQL query for membership validation
    Valid = cursor.fetchone()
    row_count = Valid[0]
    #print(f"query result :: ", row_count)

    if (row_count > 0):         #There is a record of the extracted UID
        cursor.execute("SELECT Name FROM MEMBER WHERE UID=?", (UID,))    #Further extracted its recently bought item, price and location
        Name = cursor.fetchone()
        cursor.execute("SELECT Recently FROM MEMBER WHERE UID=?", (UID,))    #Further extracted its recently bought item, price and location
        Bought = cursor.fetchone()
        cursor.execute("SELECT Price FROM PRODUCT,MEMBER WHERE PRODUCT.Name = MEMBER.Recently AND Recently = ?", Bought) 
        PInfo = cursor.fetchone()
        cursor.execute("SELECT Location FROM PRODUCT,MEMBER WHERE PRODUCT.Name = MEMBER.Recently AND Recently = ?", Bought)
        LInfo = cursor.fetchone()
        cursor.close()
        conn.close()
        conn = sqlite3.connect(db_folder)
        cursor = conn.cursor()
        cursor.execute("UPDATE MEMBER SET Timestamp =DATETIME('now', '+8 hours') WHERE UID = ?", (UID,))
        conn.commit()
        cursor.execute("SELECT Timestamp FROM MEMBER WHERE UID=?", (UID,))
        dateTime = cursor.fetchone()

    cursor.close()

    #Set Response Header to JSON
    response.headers['Content-Type'] = 'application/json'
    response.headers['Cache-Control'] = 'no-cache'

    ALL_INFO = {}

    if(row_count > 0):
        print(Bought)
        ALL_INFO["is_member"] = "true"
        ALL_INFO["last_login"] = str(dateTime)[1:-1].replace(',', '')
        ALL_INFO["name"] = str(Name)[1:-1].replace(',', '')
        ALL_INFO["Price"] = str(PInfo)[1:-1].replace(',', '')
        ALL_INFO["Location"] = str(LInfo)[1:-1].replace(',', '').replace("'", "")
        ALL_INFO["Item"] =  str(Bought)[1:-2].replace(',', '').replace("'", "")
        print(f"{str(Name)[2:-3]} successfully logged in on {str(dateTime)[2:-2].replace(',', '')}")


    else:
        ALL_INFO["is_member"] = "false"
        print(f"Record not found")

    return json.dumps(ALL_INFO)

@get('/buyitem&UID')
def message2():

    UID = request.query.UID.lstrip().rstrip()
    Name = request.query.Name.lstrip().rstrip().replace('_',' ')
    print(f"Received the following query parameter Name={Name}")
    
    conn = sqlite3.connect(db_folder)     #Connect to the SQLlite database
    cursor = conn.cursor()

    cursor.execute("SELECT COUNT(*) FROM PRODUCT WHERE Name=?", (Name,))     #SQL query for membership validation
    Valid_item = cursor.fetchone()
    item_exist = Valid_item[0]

    if (item_exist > 0):
        cursor.execute("UPDATE MEMBER SET Recently=? WHERE UID=?", (Name, UID))
        cursor.execute("SELECT PRICE FROM PRODUCT WHERE Name=?", (Name,))   
        conn.commit()
        item_price = cursor.fetchone()
        cursor.close()
    else:
        item_price = 'NULL'

    response.headers['Content-Type'] = 'application/json'
    response.headers['Cache-Control'] = 'no-cache'

    price_result = {"Price": str(item_price)[1:-1].replace(',', '').replace("'", "")}

    print(f"price_result :: {price_result}")
    print(f"buy::{Name}")
    return json.dumps(price_result)

@get('/redeem&UID')
def message3():
    
    UID = request.query.UID.lstrip().rstrip()
    Redeem = request.query.Redeem.lstrip().rstrip()
    print(f"Received the following query parameter Redeem={Redeem}")

    conn = sqlite3.connect(db_folder)     #Connect to the SQLlite database
    cursor = conn.cursor()

    cursor.execute("UPDATE MEMBER SET Redeem=Redeem+? WHERE UID=?", (Redeem,UID))
    cursor.execute("SELECT Redeem FROM MEMBER WHERE UID=?", (UID,))
    conn.commit()
    user_redeem = cursor.fetchone()
    cursor.close()

    response.headers['Content-Type'] = 'application/json'
    response.headers['Cache-Control'] = 'no-cache'

    redeem_result = {"Redeem": str(user_redeem)[1:-1].replace(',', '').replace("'", "")}

    print(f"UID :: {UID}")
    print(f"Redeem point :: {Redeem}")
    return json.dumps(redeem_result)


@get('/email')
def message4():
    email_list = []
    dateTime = request.query.dateTime.lstrip().rstrip()
    dateTime = dateTime[0:4]+'-'+dateTime[4:6]+'-'+dateTime[6:8]+' '+dateTime[8:10]+':'+dateTime[10:12]+':'+dateTime[12:14]
    print(f"Received the following query parameter Name={dateTime}")

    conn = sqlite3.connect(db_folder)  # Connect to the SQLlite database
    cursor = conn.cursor()

    cursor.execute("SELECT Email FROM MEMBER WHERE Timestamp BETWEEN ? and datetime(?,'+6 hours')", (dateTime,dateTime,))  # SQL query for membership validation
    email_result = cursor.fetchall()
    if (len(email_result) > 0):
        for i in range(len(email_result)):
            email_list.append(str(email_result[i])[2:-3])
            url = "https://maker.ifttt.com/trigger/email/with/key/n5kTSAZZqJtesohD9kygGI_xAxpYgjLcmUQDVVxLLY3?value1={email_address}&value2={dateAndtime}".format(email_address=email_list[i],dateAndtime=dateTime)
            print(f"Received the following query parameter URL={url}")
            r = requests.get(url)
            print(r.text)
    else:
        email_result = 'NULL'

    response.headers['Content-Type'] = 'application/json'
    response.headers['Cache-Control'] = 'no-cache'
    email_address_result = {"Email": str(email_list)}
    return json.dumps(email_address_result)


httpserver.serve(application, host='0.0.0.0', port=8080)