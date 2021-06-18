
import os
import sqlite3

# Create DB in current file
DEFAULT_PATH = os.path.join(os.path.dirname(__file__), '3S_Retail.db')
CREATE_SQL_FILE = os.path.join(os.path.dirname(__file__), '3S_Retail.sql')


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

    sql = ''' INSERT OR REPLACE INTO Member(UID, Name, Recently, Email, Redeem, Timestamp)
              VALUES(?,?,?,?,?,?) '''
    cur = conn.cursor()
    cur.execute(sql, student)
    conn.commit()
    return cur.lastrowid


def create_members():
    # create a database connection
    db_conn = db_connect()

    with db_conn:
        # members
        Member1 = ('DC-94-28-17', 'Adam Smith', 'APPLE', 'Adam@gmail.com', '30', '2021-01-08 17:22:24')
        Member2 = ('57-92-DB-C6', 'Steve Davidson', 'ORANGE', 'Steve@gmail.com', '10', '2021-01-17 16:58:16')
        Member3 = ('29-48-45-29', 'Kin Chan' , 'CHESSE',  '3sretail1@gmail.com', '200', '2021-04-01 17:32:24')
        Member4 = ('97-A7-B3-A3', 'Bob Li' , 'EGG',  'ee3070l03gp6@gmail.com', '30', '2021-04-01 12:12:31')
        Member5 = ('29-F8-48-F4', 'Carl Lee' , 'BUTTER',  '3sretail2@gmail.com', '22', '2021-04-01 14:12:32')
        Member6 = ('48-D2-F4-F8', 'Danny Cheung' , 'YOGURT',  'danny@gmail.com', '55', '2021-03-02 13:19:52')
        Member7 = ('67-99-E8-F1', 'Edison Lau' , 'RICE',  'edison@gmail.com', '58', '2021-02-21 12:59:30')
        Member8 = ('E8-55-F1-99', 'Emily Chan' , 'UDON',  'emily@gmail.com', '50', '2021-04-11 12:59:31')
        Member9 = ('F1-22-99-55', 'Nicole Yeung' , 'HAM',  'nicole@gmail.com', '39', '2021-04-09 19:15:42')
        Member10 = ('86-56-37-61', 'Helen Chiu' , 'JAM',  'helen@gmail.com', '68', '2021-01-15 16:58:16')
        Member11 = ('37-27-61-56', 'Iris Ng' , 'PIZZA',  'iris@gmail.com', '150', '2021-01-02 19:12:50')
        Member12 = ('61-58-56-27', 'Jason Chow' , 'BREAD',  'jason@gmail.com', '5', '2021-01-08 17:45:16')
        Member13 = ('56-18-27-58', 'Kelly Ma' , 'SAUSAGE',  'kelly@gmail.com', '50', '2021-01-04 17:53:22')
        Member14 = ('27-B6-58-18', 'Mike Wu' , 'DONUT',  'mike@gmail.com', '106', '2021-02-23 15:19:11')
        Member15 = ('58-F1-18-B6', 'Nathan Kwok' , 'SOY SAUCE',  'nathan@gmail.com', '40', '2021-01-01 12:17:36')
        Member16 = ('18-D3-B6-F1', 'Lily Ho ' , 'VINEGAR',  'lily@gmail.com', '34', '2021-03-07 13:28:20')
        Member17 = ('A1-86-62-A3', 'Karen Lam' , 'KETCHUP',  'karen@gmail.com', '61', '2021-04-10 13:28:21')
        Member18 = ('62-B7-A3-86', 'Patrick Ko ' , 'SALT',  'patrick@gmail.com', '10', '2021-04-07 22:42:51')
        Member19 = ('A3-22-86-B7', 'Mark Lo' , 'SUGAR',  'mark@gmail.com', '10', '2021-03-09 12:17:36')
        Member20 = ('86-84-B7-22', 'Rachel Leung' , 'FLOUR',  'rachel@gmail.com', '5', '2021-04-20 22:40:59')
        Member21 = ('18-B8-E2-45', 'Sara Tong ' , 'RICE',  'sara@gmail.com', '350', '2021-03-22 11:46:45')
        Member22 = ('E2-97-45-B8', 'Amber Tang' , 'COCONUT OIL',  'amber@gmail.com', '100', '2021-04-09 18:58:11')


        # create student
        create_student_task(db_conn, Member1)
        create_student_task(db_conn, Member2)
        create_student_task(db_conn, Member3)
        create_student_task(db_conn, Member4)
        create_student_task(db_conn, Member5)
        create_student_task(db_conn, Member6)
        create_student_task(db_conn, Member7)
        create_student_task(db_conn, Member8)
        create_student_task(db_conn, Member9)
        create_student_task(db_conn, Member10)
        create_student_task(db_conn, Member11)
        create_student_task(db_conn, Member12)
        create_student_task(db_conn, Member13)
        create_student_task(db_conn, Member14)
        create_student_task(db_conn, Member15)
        create_student_task(db_conn, Member16)
        create_student_task(db_conn, Member17)
        create_student_task(db_conn, Member18)
        create_student_task(db_conn, Member19)
        create_student_task(db_conn, Member20)
        create_student_task(db_conn, Member21)
        create_student_task(db_conn, Member22)


def create_product_task(conn, product):
    """
    Create a new student task
    :param conn:
    :param student:
    :return:
    """

    sql = ''' INSERT OR REPLACE INTO PRODUCT (PID, NAME, PRICE, LOCATION)
              VALUES(?,?,?,?) '''
    cur = conn.cursor()
    cur.execute(sql, product)
    conn.commit()
    return cur.lastrowid


def create_product():
    # create a database connection
    db_conn = db_connect()

    with db_conn:
        # product
        product1 = (1, 'APPLE', 5,'F1')
        product2 = (2, 'ORANGE', 8,'F3')
        product3 = (3, 'MILK', 25,'D1')
        product4 = (4, 'CHESSE', 200,'F5')
        product5 = (5, 'EGG', 30,'A5')
        product6 = (6, 'BUTTER', 22,'F4')
        product7 = (7, 'YOGURT', 55,'F1')
        product8 = (8, 'RICE', 58,'A5')
        product9 = (9, 'UDON', 50,'D1')
        product10 = (10, 'HAM', 39,'E6')
        product11 = (11, 'JAM', 68,'B6')
        product12 = (12, 'PIZZA', 150,'A9')
        product13 = (13, 'BREAD', 5,'D2')
        product14 = (14, 'SAUSAGE', 50,'A1')
        product15 = (15, 'DONUT', 106,'D9')
        product16 = (16, 'SOY SAUCE', 40,'B4')
        product17 = (17, 'VINEGAR', 34,'B4')
        product18 = (18, 'KETCHUP', 61,'C3')
        product19 = (19, 'SALT', 10,'A9')
        product20 = (20, 'SUGAR', 10,'C4')
        product21 = (21, 'FLOUR', 5,'E7')
        product22 = (22, 'RICE', 350,'B3')
        product23 = (23, 'COCONUT OIL', 100,'A3')
        product24 = (24, 'OLIVE OIL', 15,'A7')
        product25 = (25, 'CHOCOLATE', 50,'F5')


        #create product
        create_product_task (db_conn, product1) 
        create_product_task (db_conn, product2) 
        create_product_task (db_conn, product3) 
        create_product_task (db_conn, product4) 
        create_product_task (db_conn, product5) 
        create_product_task (db_conn, product6) 
        create_product_task (db_conn, product7) 
        create_product_task (db_conn, product8) 
        create_product_task (db_conn, product9) 
        create_product_task (db_conn, product10) 
        create_product_task (db_conn, product11) 
        create_product_task (db_conn, product12) 
        create_product_task (db_conn, product13) 
        create_product_task (db_conn, product14) 
        create_product_task (db_conn, product15) 
        create_product_task (db_conn, product16) 
        create_product_task (db_conn, product17) 
        create_product_task (db_conn, product18) 
        create_product_task (db_conn, product19) 
        create_product_task (db_conn, product20) 
        create_product_task (db_conn, product21) 
        create_product_task (db_conn, product22) 
        create_product_task (db_conn, product23) 
        create_product_task (db_conn, product24) 
        create_product_task (db_conn, product25) 


        
def main():
    create_table()
    create_members()
    create_product()


if __name__ == '__main__':
    main()