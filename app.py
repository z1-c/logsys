from flask import Flask, render_template, request, redirect, url_for, flash
import sqlite3
import hashlib

app = Flask(__name__)
app.secret_key = 'your_secret_key'

# 数据库初始化
def init_db():
    conn = sqlite3.connect('logsys.db')
    cursor = conn.cursor()
    cursor.execute('''
        CREATE TABLE IF NOT EXISTS users (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            username TEXT UNIQUE NOT NULL,
            password_hash TEXT NOT NULL
        )
    ''')
    cursor.execute('''
        CREATE TABLE IF NOT EXISTS logs (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            filename TEXT NOT NULL,
            content TEXT NOT NULL
        )
    ''')
    conn.commit()
    conn.close()

init_db()

# 用户注册
@app.route('/register', methods=['GET', 'POST'])
def register():
    if request.method == 'POST':
        username = request.form['username']
        password = request.form['password']
        confirm_password = request.form['confirm_password']

        if password != confirm_password:
            flash('密码和确认密码不一致！')
            return redirect(url_for('register'))

        # 对密码进行哈希处理
        password_hash = hashlib.sha256(password.encode()).hexdigest()

        conn = sqlite3.connect('logsys.db')
        cursor = conn.cursor()
        try:
            cursor.execute("INSERT INTO users (username, password_hash) VALUES (?, ?)", (username, password_hash))
            conn.commit()
            flash('注册成功！请登录。')
            return redirect(url_for('login'))
        except sqlite3.IntegrityError:
            flash('用户名已存在！')
        finally:
            conn.close()
    return render_template('register.html')

# 用户登录
@app.route('/login', methods=['GET', 'POST'])
def login():
    if request.method == 'POST':
        username = request.form['username']
        password = request.form['password']
        password_hash = hashlib.sha256(password.encode()).hexdigest()

        conn = sqlite3.connect('logsys.db')
        cursor = conn.cursor()
        cursor.execute("SELECT password_hash FROM users WHERE username = ?", (username,))
        user = cursor.fetchone()
        conn.close()

        if user and user[0] == password_hash:
            return redirect(url_for('upload'))
        else:
            flash('用户名或密码错误！')
    return render_template('login.html')

# 日志上传
@app.route('/upload', methods=['GET', 'POST'])
def upload():
    if request.method == 'POST':
        file = request.files['file']
        if file:
            content = file.read()
            filename = file.filename
            conn = sqlite3.connect('logsys.db')
            cursor = conn.cursor()
            cursor.execute("INSERT INTO logs (filename, content) VALUES (?, ?)", (filename, content))
            conn.commit()
            conn.close()
            flash('日志上传成功！')
            return redirect(url_for('upload'))
    return render_template('upload.html')



if __name__ == '__main__':
    app.run(debug=True, host='127.0.0.1', port=5000)
