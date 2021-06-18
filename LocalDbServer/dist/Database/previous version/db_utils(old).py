"""
Created by donsky for www.donskytech.com
"""
import os
import sqlite3

# Create DB in current file
DEFAULT_PATH = os.path.join(os.path.dirname(__file__), '3S_Retail.db')
CREATE_SQL_FILE = os.path.join(os.path.dirname(__file__), 'MemberDB.sql')


def db_connect(db_path=DEFAULT_PATH):
    con = sqlite3.connect(db_path)
    return con


def create_table():
    db_conn = db_connect()

    with db_conn:
        try:
            db_conn = db_connect()
            cursor = db_conn.cursor()
            print("Successfully Connected to SQLite")

            with open(CREATE_SQL_FILE, 'r') as sqlite_file:
                sql_script = sqlite_file.read()

            cursor.executescript(sql_script)
            print("SQLite script executed successfully")
            cursor.close()

        except sqlite3.Error as error:
            print("Error while executing sqlite script", error)

    print("Successfully created table!")


def create_student_task(conn, student):
    """
    Create a new student task
    :param conn:
    :param student:
    :return:
    """

    sql = ''' INSERT INTO Member(UID, Name, Recently, Email, Redeem, Time)
              VALUES(?,?,?,?,?,?) '''
    cur = conn.cursor()
    cur.execute(sql, student)
    conn.commit()
    return cur.lastrowid


def create_members():
    # create a database connection
    db_conn = db_connect()

    with db_conn:
        # students
        Member1 = ('FC-C8-13-31', 'Adam Smith', 'Apple', 'Adam@gmail.com', '30', '13:00:45')
        Member2 = ('57-92-DB-C6', 'Steve Davidson', 'Orange', 'Steve@gmail.com', '10', '14:20:35')

        # create student
        create_student_task(db_conn, Member1)
        create_student_task(db_conn, Member2)


def main():
    create_table()
    create_members()


if __name__ == '__main__':
    main()
