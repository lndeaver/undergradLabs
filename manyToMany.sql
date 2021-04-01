/* Lab 03 */
/* PROBLEM 1 BEGIN */
CREATE TABLE person (
    psnid INTEGER,
    psnfname VARCHAR(10),
    psnlname VARCHAR(15),
    PRIMARY KEY (psnid)

);

CREATE TABLE label (
    lbltitle VARCHAR(15),
    lblstreet VARCHAR(30),
    lblcity VARCHAR(10),
    lblstate VARCHAR(5),
    lblpostcode INTEGER,
    lblnation VARCHAR(5),
    PRIMARY KEY (lbltitle)
);

CREATE TABLE cd (
    cdid INTEGER,
    cdlblid VARCHAR(10),
    cdtitle VARCHAR(20),
    cdyear INTEGER,
    lbltitle VARCHAR(15),
    PRIMARY KEY (cdid),
    FOREIGN KEY (lbltitle) REFERENCES label (lbltitle)
);

CREATE TABLE composition (
    compid INTEGER,
    comptitle VARCHAR(35),
    compyear VARCHAR(5),
    PRIMARY KEY (compid)
);

CREATE TABLE person_cd (
    psncdorder INTEGER,
    psnid INTEGER,
    cdid INTEGER,
    PRIMARY KEY (psnid, cdid),
    FOREIGN KEY (psnid) REFERENCES person (psnid),
    FOREIGN KEY (cdid) REFERENCES cd (cdid)
);

CREATE TABLE person_composition (
    psncomprole VARCHAR(10),
    psncomporder INTEGER,
    psnid INTEGER,
    compid INTEGER,
    PRIMARY KEY (psncomprole, psnid, compid),
    FOREIGN KEY (psnid) REFERENCES person (psnid),
    FOREIGN KEY (compid) REFERENCES composition (compid)
);

CREATE TABLE recording (
    rcdid INTEGER,
    rcdlength DOUBLE,
    rcddate VARCHAR(15),
    compid INTEGER,
    PRIMARY KEY (rcdid, compid),
    FOREIGN KEY (compid) REFERENCES composition (compid)
);

CREATE TABLE track (
    cdid INTEGER,
    trknum INTEGER,
    rcdid INTEGER,
    compid INTEGER,
    PRIMARY KEY (trknum, cdid),
    FOREIGN KEY (cdid) REFERENCES cd (cdid),
    FOREIGN KEY (rcdid, compid) REFERENCES recording (rcdid, compid)
);

CREATE TABLE person_recording (
    psnrcdrole VARCHAR(10),
    psnid INTEGER,
    rcdid INTEGER,
    compid INTEGER,
    PRIMARY KEY (psnrcdrole, psnid, rcdid), 
    FOREIGN KEY (psnid) REFERENCES person (psnid),
    FOREIGN KEY (rcdid, compid) REFERENCES recording (rcdid, compid)
);
/* PROBLEM 1 END */

/* PROBLEM 2 BEGIN */
SELECT trknum, comptitle 
FROM track, composition, cd 
WHERE track.cdid = cd.cdid AND track.compid = composition.compid
AND cd.cdtitle = "Giant Steps";
/* PROBLEM 2 END */

/* PROBLEM 3 BEGIN */
SELECT DISTINCT psnfname, psnlname, psnrcdrole
FROM person, person_recording, recording, composition
WHERE person_recording.psnid = person.psnid
AND recording.rcdid = person_recording.rcdid
AND recording.compid = composition.compid
AND recording.rcddate = "1959-05-04"
AND composition.comptitle = "Giant Steps";
/* PROBLEM 3 END */

/* PROBLEM 4 BEGIN */
SELECT DISTINCT psnfname, psnlname
FROM person, person_recording, person_composition
WHERE person_composition.psnid = person.psnid
AND person_recording.psnid = person.psnid
AND psncomprole = "music"
AND psnrcdrole = "tenor sax";
/* PROBLEM 4 END */

/* PROBLEM 5 BEGIN */
SELECT comptitle, trknum, cdtitle
FROM (SELECT cdid, compid
FROM composition NATURAL JOIN track NATURAL JOIN cd NATURAL JOIN recording
WHERE cd.cdid = track.cdid
AND composition.compid = track.compid
GROUP BY track.compid, track.cdid
HAVING COUNT(*) > 1) AS duplicates
NATURAL JOIN track 
NATURAL JOIN cd
NATURAL JOIN composition
ORDER BY comptitle ASC, trknum ASC;
/* PROBLEM 5 END */

/* PROBLEM 6 BEGIN */
SELECT rcdid, rcddate
FROM recording
WHERE NOT EXISTS 
(SELECT * FROM cd WHERE NOT EXISTS
(SELECT * FROM track WHERE track.rcdid = recording.rcdid
AND track.cdid = cd.cdid));
/* PROBLEM 6 END */

/* PROBLEM 7 BEGIN */
SELECT recording.rcdid, rcddate 
FROM recording NATURAL JOIN track
GROUP BY recording.rcdid, rcddate
HAVING COUNT(DISTINCT cdid) =
(SELECT COUNT(DISTINCT cdid) FROM cd);
/* PROBLEM 7 END */