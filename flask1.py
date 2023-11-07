from flask import Flask,request,jsonify
import sqlite3
app=Flask(__name__)
def create_db():
    db=db_make()
    cur=db.cursor()
    sql='''CREATE TABLE IF NOT EXISTS VALS(IND INT,ANALOG_INPUT FLOAT,ANALOG_OUTPUT 
    FLOAT,DIGITAL_INPUT FLOAT,DIGITAL_OUTPUT FLOAT,BINARY_INPUT BOOL,BINARY_OUTPUT BOOL)'''
    cur.execute(sql)
    db.commit()
    db.close()
@app.route('/fill_all')
def fill():
    id=request.args.get('ind')
    aip=float(request.args.get('analog_input'))
    aop=float(request.args.get('analog_output'))
    dip=float(request.args.get('digital_input'))
    dop=float(request.args.get('digital_output'))
    bip=bool(request.args.get('binary_input'))
    bop=bool(request.args.get('binary_output'))
    db=db_make()
    cur=db.cursor()
    sql1='''SELECT * from VALS where ind=?'''
    cur.execute(sql1,(id))
    db.commit()
    #s=str(len(cur.fetchall()))
    t='0'
    if(len(cur.fetchall())==0):
        sql='''INSERT into VALS VALUES (?,?,?,?,?,?,?)'''
        cur.execute(sql,(id,aip,aop,dip,dop,bip,bop,))
        t='1'
    else:
        sql='''Update VALS set analog_input=?, ANALOG_OUTPUT=? , digital_input=? ,
          digital_output=? , binary_input=? , binary_output=? Where ind=?'''
        cur.execute(sql,(aip,aop,dip,dop,bip,bop,id))
        t='2'
    db.commit()
    db.close()
    return t
@app.route('/read/<name>')
def read(name):
    id=(request.args.get('id'))
    db=db_make()
    cur=db.cursor()
    sql='''select '''+name+''' from VALS where ind=?'''
    cur.execute(sql,(id))
    record=cur.fetchall()
    db.commit()
    db.close()
    #print(record)
    return str(record[0][0])
@app.route('/edit/<name>')
def edit(name):
    id=request.args.get('id')
    val=float(request.args.get('val'))
    db=db_make()
    cur=db.cursor()
    sql='''update VALS set '''+name+'''=? where ind=?'''
    cur.execute(sql,(val,id,))
    db.commit()
    db.close()
    return '1'
def db_make():
    db=sqlite3.connect('db_vals.db')
    return db
if __name__=='__main__':
    create_db()
    app.run()