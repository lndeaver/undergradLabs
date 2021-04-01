/* PROBLEM 1 BEGIN */
CREATE TABLE label (
	lbltitle VARCHAR(25) NOT NULL,
	lblstreet VARCHAR(30) NOT NULL,
	lblcity VARCHAR(20) NOT NULL,
	lblstate VARCHAR(10) NOT NULL,
	lblpostcode VARCHAR(10) NOT NULL,
	lblnation VARCHAR(10) NOT NULL,
	PRIMARY KEY (lbltitle)
);

CREATE TABLE cd (
	cdid INTEGER,
	cdlblid VARCHAR(10) NOT NULL,
	cdtitle VARCHAR(25) NOT NULL,
	cdyear INTEGER,
	lbltitle VARCHAR(25) NOT NULL,
	PRIMARY KEY (cdid),
	FOREIGN KEY (lbltitle) REFERENCES label (lbltitle)
);

CREATE TABLE track (
	trkid INTEGER,
	trknum INTEGER,
	trktitle VARCHAR(35) NOT NULL,
	trklength DOUBLE,
	cdid INTEGER,
	PRIMARY KEY (trkid),
	FOREIGN KEY (cdid) REFERENCES cd (cdid)
);
	
/* PROBLEM 1 END */

/* PROBLEM 2 BEGIN */
SELECT track.trktitle, cd.cdtitle, track.trklength 
FROM track, cd 
WHERE track.cdid = cd.cdid 
ORDER BY cd.cdtitle, track.trklength;
/* PROBLEM 2 END */

/* PROBLEM 3 BEGIN */

SELECT trktitle, trklength FROM track WHERE track.cdid = (SELECT cdid FROM cd WHERE cdtitle = 'Swing');
/* PROBLEM 3 END */

/* PROBLEM 4 BEGIN */
SELECT cd.cdtitle, track.trktitle, track.trklength 
FROM cd NATURAL JOIN track
WHERE trklength = (SELECT MAX(trklength) FROM track WHERE cd.cdid = track.cdid);
/* PROBLEM 4 END */

/* PROBLEM 5 BEGIN */
SELECT cdtitle, MAX(trknum), SUM(trklength)
FROM track NATURAL JOIN cd
GROUP BY track.cdid
ORDER BY MAX(trknum) DESC;
/* PROBLEM 5 END */

/* PROBLEM 6 BEGIN */
SELECT label.lbltitle, label.lblnation, cd.cdtitle, SUM(track.trklength) AS cdlength
FROM label, track, cd
WHERE label.lbltitle = cd.lbltitle AND track.cdid = cd.cdid
GROUP BY cd.cdid
HAVING cdlength > 40;
/* PROBLEM 6 END */

/* PROBLEM 7 BEGIN */
SELECT cd.cdtitle, track.trktitle, track.trklength
FROM cd NATURAL JOIN track
ORDER BY track.trklength LIMIT 3;
/* PROBLEM 7 END */

/* PROBLEM 8 BEGIN */
CREATE VIEW CDView (cdid, cdlblid, cdtitle, cdyear, cdlength) AS
SELECT cd.cdid, cd.cdlblid, cd.cdtitle, cd.cdyear, SUM(track.trklength)
FROM cd NATURAL JOIN track
GROUP BY cd.cdid;
/* PROBLEM 8 END */

/* PROBLEM 9 BEGIN */
SELECT track.trktitle, track.trklength, cd.cdtitle
FROM cd NATURAL JOIN track
WHERE track.trktitle REGEXP '^C.';
/* PROBLEM 9 END */
