/* PROBLEM 1 BEGIN */
/* Perform the following SQL query and paste your results below: */
SELECT * from track WHERE trklength > 6;

/* PASTE RESULTS HERE */
trkid,trknum,trktitle,trklength
5,5,Syeeda's song flute,7.00
7,7,Mr. P.C.,6.95
12,12,Syeeda's song flute,7.03
22,10,Clouds,7.20
/* PROBLEM 1 END */

/* PROBLEM 2 BEGIN */
SELECT * FROM track WHERE trklength >= 3 AND trklength <= 4;
/* PROBLEM 2 END */

/* PROBLEM 3 BEGIN */
SELECT trknum, trklength FROM track WHERE trktitle = 'Java Jive';
/* PROBLEM 3 END */

/* PROBLEM 4 BEGIN */
SELECT AVG(trklength) FROM track;
/* PROBLEM 4 END */

/* PROBLEM 5 BEGIN */
CREATE TABLE ship (
	RegCode VARCHAR(10) NOT NULL,
    ShipName VARCHAR(25) NOT NULL,
    Tonnage INTEGER,
    Year INTEGER,
    Type VARCHAR (25) NOT NULL,
    PRIMARY KEY (RegCode)
);
/* PROBLEM 5 END */

/* PROBLEM 6 BEGIN */
SELECT RegCode from ship WHERE ShipName REGEXP '.*[a|A].*';
/* PROBLEM 6 END */

/* PROBLEM 7 BEGIN */
 SELECT ShipName FROM ship WHERE Tonnage = (SELECT MAX(Tonnage) FROM ship);
/* PROBLEM 7 END */
