#!/usr/bin/env python3
from flask import Flask, jsonify,request
#from flask_cors import CORS
import sqlite3

app=Flask(__name__)
#CORS(app, resources={r"/*": {"origins": "*"}})

@app.route('/api/dht/readings', methods=['GET'])
def api_get_dht_readings():
    return jsonify(get_dht_reading())

@app.route('/api/dht/readings/<readingNum>', methods=['GET'])
def api_get_dht_reading(readingNum):
    return jsonify(get_dht_reading_by_id(readingNum))

@app.route('/api/dht/readings/insert',  methods = ['POST'])
def api_add_dht_reading():
    reading = request.get_json(force=True)
    ts = reading['timestamp']
    temp = reading['temperature']
    hum = reading['humidity']
    return jsonify(insert_dht_reading(ts,temp,hum))

@app.route('/api/arduino/readings', methods=['GET'])
def api_get_arduino_readings():
    return jsonify(get_arduino_reading())

@app.route('/api/arduino/readings/<readingNum>', methods=['GET'])
def api_get_arduino_reading(readingNum):
    return jsonify(get_arduino_reading_by_id(readingNum))

@app.route('/api/arduino/readings/insert',  methods = ['POST'])
def api_add_arduino_reading():
    reading = request.get_json(force=True)
    ts = reading['timestamp']
    wg = reading['windGen']
    sg = reading['solarGen']
    return jsonify(insert_arduino_reading(ts,wg,sg))

def connect_to_db():
    conn = sqlite3.connect('readings_db.db')
    return conn

def create_db_table_dht():
    conn=connect_to_db()
    cur=conn.cursor()
    cur.execute(''' SELECT count(name) FROM sqlite_master WHERE type='table' AND name='dht' ''')
    if cur.fetchone()[0]==1:
        return
    else:
        conn.execute('''CREATE TABLE dht (readingNum INTEGER PRIMARY KEY NOT NULL , timestamp TEXT, temperature REAL, humidity REAL)''')
    conn.commit()
    
def create_db_table_arduino():
    conn=connect_to_db()
    cur=conn.cursor()
    cur.execute(''' SELECT count(name) FROM sqlite_master WHERE type='table' AND name='arduino' ''')
    if cur.fetchone()[0]==1:
        return
    else:
        conn.execute('''CREATE TABLE arduino (readingNum INTEGER PRIMARY KEY NOT NULL , timestamp TEXT, windGen REAL, solarGen REAL)''')
    conn.commit()
    
def insert_dht_reading(ts,temp,hum):
    inserted_reading = {}
    
    conn=connect_to_db()
    cur = conn.cursor()
    cur.execute("INSERT INTO dht (timestamp,temperature,humidity) VALUES (?,?,?)",(ts,temp,hum))
    conn.commit()
    inserted_reading = get_dht_reading_by_id(cur.lastrowid)
    conn.close()
    return True

def get_dht_reading():
    readings=[]
    conn = connect_to_db()
    cur = conn.cursor()
    cur.execute("SELECT* FROM dht")
    rows = cur.fetchall()
    
    for i in rows:
        reading = {}
        reading["readingNum"] = i[0]
        reading["timestamp"] = i[1]
        reading["temperature"] = i[2]
        reading["humidity"] = i[3]
        readings.append(reading)
        
    return readings

def get_dht_reading_by_id(readingNum):
    reading = {}
    try:
        conn = connect_to_db()
        conn.row_factory = sqlite3.Row
        cur = conn.cursor()
        cur.execute("SELECT * FROM dht WHERE readingNum = ?", (readingNum,))
        row = cur.fetchone()
        reading["timestamp"] = row["timestamp"]
        reading["temperature"] = row["temperature"]
        reading["humidity"] = row["humidity"]
        
    except:
        reading = {}

    return reading
    
def insert_arduino_reading(ts,wg,sg):
    inserted_reading = {}
    conn=connect_to_db()
    cur = conn.cursor()
    cur.execute("INSERT INTO arduino (timestamp,windGen,solarGen) VALUES (?,?,?)",(ts,wg,sg))
    conn.commit()
    inserted_reading = get_arduino_reading_by_id(cur.lastrowid)
    conn.close()
    return True

def get_arduino_reading():
    readings=[]
    conn = connect_to_db()
    cur = conn.cursor()
    cur.execute("SELECT* FROM arduino")
    rows = cur.fetchall()
    
    for i in rows:
        reading = {}
        reading["readingNum"] = i[0]
        reading["timestamp"] = i[1]
        reading["windGen"] = i[2]
        reading["solarGen"] = i[3]
        readings.append(reading)
        
    return readings

def get_arduino_reading_by_id(readingNum):
    reading = {}
    try:
        conn = connect_to_db()
        conn.row_factory = sqlite3.Row
        cur = conn.cursor()
        cur.execute("SELECT * FROM arduino WHERE readingNum = ?", (readingNum,))
        row = cur.fetchone()        
        reading["timestamp"] = row["timestamp"]
        reading["windGen"] = row["windGen"]
        reading["solarGen"] = row["solarGen"]
        
    except:
        reading = {}

    return reading

if __name__ == '__main__':
    connect_to_db()
    create_db_table_dht()
    create_db_table_arduino()
    app.run()
