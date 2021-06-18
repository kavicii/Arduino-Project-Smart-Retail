/*
* Author: donsky
* Org: www.donskytech.com
* Purpose:  Test Database for IOT Projects
*/
CREATE TABLE Member (
 UID TEXT PRIMARY KEY,
 Name TEXT NOT NULL,
 Recently TEXT,
 Email TEXT NOT NULL,
 Redeem INTEGER NOT NULL,
 Time TEXT
);