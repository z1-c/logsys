import sqlite3
import hashlib
import os
from flask import Flask, render_template, request, redirect, url_for, flash, session
from gmssl.sm4 import CryptSM4, SM4_ENCRYPT, SM4_DECRYPT
import logging

logging.basicConfig(level=logging.INFO)

app = Flask(__name__)
app.secret_key = '000000'

ADMIN_USERNAME = 'admin'
ADMIN_PASSWORD = '000000'

DATABASE = 'logsys.db'


def init_db():
    conn = sqlite3.connect(DATABASE)
    cursor = conn.cursor()
    cursor.execute('''
        CREATE TABLE IF NOT EXISTS users (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            username TEXT UNIQUE NOT NULL,
            password_hash TEXT NOT NULL,
            salt TEXT NOT NULL
        )
    ''')
    cursor.execute('''
        CREATE TABLE IF NOT EXISTS logs (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            filename TEXT NOT NULL,
            content BLOB NOT NULL,
            username TEXT NOT NULL,
            key TEXT
        )
    ''')
    conn.commit()
    conn.close()
    print("数据库初始化完成！")
    
    
# 管理员登录
@app.route('/admin/login', methods=['GET', 'POST'])
def admin_login():
    if request.method == 'POST':
        username = request.form['username']
        password = request.form['password']
        if username == ADMIN_USERNAME and password == ADMIN_PASSWORD:
            session['admin'] = True
            flash('登录成功！')
            return redirect(url_for('admin_dashboard'))
        else:
            flash('用户名或密码错误！')
    return render_template('admin_login.html')

# 管理员登出
@app.route('/admin/logout')
def admin_logout():
    session.pop('admin', None)
    flash('您已成功登出！')
    return redirect(url_for('admin_login'))

# 管理员仪表盘
@app.route('/admin/dashboard')
def admin_dashboard():
    if 'admin' not in session:
        flash('请先登录！')
        return redirect(url_for('admin_login'))
    
    conn = sqlite3.connect('logsys.db')
    cursor = conn.cursor()
    cursor.execute('SELECT * FROM logs')
    logs = cursor.fetchall()
    cursor.execute('SELECT * FROM users')
    users = cursor.fetchall()
    conn.close()
    
    return render_template('admin_dashboard.html', logs=logs, users=users)

# 使用标志变量确保初始化代码只执行一次
initialized = False

@app.before_request
def initialize_database():
    global initialized
    if not initialized:
        init_db()
        initialized = True

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
        salt = os.urandom(16).hex()
        password_hash = hashlib.sha256((password + salt).encode()).hexdigest()
        conn = sqlite3.connect('logsys.db')
        cursor = conn.cursor()
        try:
            cursor.execute("SELECT id FROM users WHERE username = ?", (username,))
            if cursor.fetchone():
                flash('用户名已存在！')
                return redirect(url_for('register'))
            cursor.execute("INSERT INTO users (username, password_hash, salt) VALUES (?, ?, ?)", (username, password_hash, salt))
            conn.commit()
            flash('注册成功！请登录。')
            return redirect(url_for('login'))
        except Exception as e:
            flash(f'注册失败：{str(e)}')
        finally:
            conn.close()
    return render_template('register.html')


# 用户登录
@app.route('/login', methods=['GET', 'POST'])
def login():
    if request.method == 'POST':
        username = request.form['username']
        password = request.form['password']
        conn = sqlite3.connect('logsys.db')
        cursor = conn.cursor()
        cursor.execute("SELECT id, password_hash, salt FROM users WHERE username = ?", (username,))
        user = cursor.fetchone()
        conn.close()
        if user:
            password_hash = hashlib.sha256((password + user[2]).encode()).hexdigest()
            if user[1] == password_hash:
                session['username'] = username
                flash('登录成功！')
                return redirect(url_for('upload'))
            else:
                flash('密码错误！')
        else:
            flash('用户名不存在！')
    return render_template('login.html')


# 日志上传
@app.route('/upload', methods=['GET', 'POST'])
def upload():
    if 'username' not in session:
        flash('请先登录！')
        return redirect(url_for('login'))

    if request.method == 'POST':
        file = request.files['file']
        encrypt = request.form.get('encrypt', 'no')
        if file:
            content = file.read()
            if encrypt == 'yes':
                key = os.urandom(16)
                crypt_sm4 = CryptSM4()
                crypt_sm4.set_key(key, SM4_ENCRYPT)
                encrypted_bytes = crypt_sm4.crypt_ecb(content)
                encrypted_content = encrypted_bytes.hex()
                key_hex = key.hex()
            else:
                encrypted_content = content.hex()
                key_hex = None

            conn = sqlite3.connect('logsys.db')
            cursor = conn.cursor()
            cursor.execute("INSERT INTO logs (filename, content, username, key) VALUES (?, ?, ?, ?)",
                           (file.filename, encrypted_content, session['username'], key_hex))
            conn.commit()
            conn.close()
            flash('日志上传成功！')
            return redirect(url_for('upload'))
    return render_template('upload.html')
    
# 用户查询自己发布的日志
@app.route('/mylogs', methods=['GET'])
def mylogs():
    if 'username' not in session:
        flash('请先登录！')
        return redirect(url_for('login'))

    username = session['username']
    conn = sqlite3.connect('logsys.db')
    cursor = conn.cursor()
    cursor.execute("SELECT id, filename FROM logs WHERE username = ?", (username,))
    logs = cursor.fetchall()
    conn.close()
    return render_template('mylogs.html', logs=logs)
    


# 查看某个日志内容
@app.route('/log/<int:log_id>')
def view_log(log_id):
    if 'username' not in session:
        flash('请先登录！')
        return redirect(url_for('login'))

    conn = sqlite3.connect('logsys.db')
    cursor = conn.cursor()

    # 查询所有日志用于显示列表
    cursor.execute("SELECT id, filename FROM logs WHERE username = ?", (session['username'],))
    logs = cursor.fetchall()

    # 查询当前日志详情（加密内容+key）
    cursor.execute("SELECT filename, content, key FROM logs WHERE id = ?", (log_id,))
    row = cursor.fetchone()

    if row:
        filename, encrypted_content_hex, key_hex = row

        # 如果key_hex为空，说明日志没加密，直接解码content
        if key_hex:
            key_bytes = bytes.fromhex(key_hex)
            encrypted_content = bytes.fromhex(encrypted_content_hex)  # 转bytes
            crypt_sm4 = CryptSM4()
            crypt_sm4.set_key(key_bytes, SM4_DECRYPT)
            iv = b'\x00' * 16  # 这里替换成实际用的IV，如果没有用IV请确认加密模式
            decrypted_bytes = crypt_sm4.crypt_cbc(iv, encrypted_content)
            content = decrypted_bytes.decode('utf-8', errors='ignore')
        else:
            # 没加密，直接把hex转bytes，再转字符串
            content = bytes.fromhex(encrypted_content_hex).decode('utf-8', errors='ignore')

        log = (filename,)
    else:
        flash("日志不存在或已删除")
        return redirect(url_for('mylogs'))

    conn.close()
    return render_template('mylogs.html', logs=logs, log=log, content=content)


        
# 删除日志
@app.route('/delete_log/<int:log_id>', methods=['POST'])
def delete_log(log_id):
    if 'username' not in session:
        flash('请先登录！')
        return redirect(url_for('login'))

    username = session['username']
    try:
        conn = sqlite3.connect('logsys.db')
        cursor = conn.cursor()
        # 检查日志是否存在以及是否属于当前用户
        cursor.execute("SELECT username FROM logs WHERE id = ?", (log_id,))
        log = cursor.fetchone()
        if log and log[0] == username:
            cursor.execute("DELETE FROM logs WHERE id = ?", (log_id,))
            conn.commit()
            flash('日志删除成功！')
        else:
            flash('日志不存在或您无权删除！')
        conn.close()
    except Exception as e:
        logging.error(f"Error deleting log: {e}")
        flash('日志删除失败！')
    return redirect(url_for('mylogs'))
    

# 管理员查看日志内容
@app.route('/admin/view_log/<int:log_id>', methods=['GET'])
def admin_view_log(log_id):
    if 'admin' not in session:
        flash('请先登录！')
        return redirect(url_for('admin_login'))

    try:
        conn = sqlite3.connect('logsys.db')
        cursor = conn.cursor()
        cursor.execute("SELECT content FROM logs WHERE id = ?", (log_id,))
        log_content = cursor.fetchone()
        conn.close()

        if log_content:
            content = log_content[0]
            return render_template('admin_log.html', content=content)
        else:
            flash('日志不存在！')
            return redirect(url_for('admin_dashboard'))
    except Exception as e:
        logging.error(f"Error viewing log: {e}")
        flash('日志查看失败！')
        return redirect(url_for('admin_dashboard'))

# 管理员编辑日志
@app.route('/admin/edit_log/<int:log_id>', methods=['GET', 'POST'])
def admin_edit_log(log_id):
    if 'admin' not in session:
        flash('请先登录！')
        return redirect(url_for('admin_login'))

    try:
        conn = sqlite3.connect('logsys.db')
        cursor = conn.cursor()
        cursor.execute("SELECT content FROM logs WHERE id = ?", (log_id,))
        log_content = cursor.fetchone()
        conn.close()

        if request.method == 'POST':
            new_content = request.form['content']
            conn = sqlite3.connect('logsys.db')
            cursor = conn.cursor()
            cursor.execute("UPDATE logs SET content = ? WHERE id = ?", (new_content, log_id))
            conn.commit()
            conn.close()
            flash('日志更新成功！')
            return redirect(url_for('admin_dashboard'))

        return render_template('admin_edit_log.html', content=log_content[0] if log_content else '')
    except Exception as e:
        logging.error(f"Error editing log: {e}")
        flash('日志编辑失败！')
        return redirect(url_for('admin_dashboard'))
        
# 管理员删除日志        
@app.route('/delete_admin_log/<int:log_id>', methods=['POST'])
def delete_admin_log(log_id):
    if 'admin' not in session:
        flash('请先登录！')
        return redirect(url_for('admin_login'))

    try:
        conn = sqlite3.connect('logsys.db')
        cursor = conn.cursor()
        cursor.execute("DELETE FROM logs WHERE id = ?", (log_id,))
        conn.commit()
        conn.close()
        flash('日志删除成功！')
    except Exception as e:
       logging.error(f"Error deleting log: {e}")
       flash('日志删除失败！')
    return redirect(url_for('admin_dashboard'))
        
if __name__ == '__main__':
    app.run(debug=True, host='127.0.0.1', port=5000)
