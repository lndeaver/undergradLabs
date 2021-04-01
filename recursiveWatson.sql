/* Lab 04 */

/* PROBLEM 1 BEGIN */
SELECT empno, empfname FROM emp WHERE bossno IS NULL;
/* PROBLEM 1 END */

/* PROBLEM 2 BEGIN */
SELECT wrk.empno, wrk.empfname, boss.empno AS mgrno, boss.empfname AS mgrfname
FROM emp wrk, emp boss
WHERE wrk.bossno = boss.empno
ORDER BY wrk.empfname;
/* PROBLEM 2 END */

/* PROBLEM 3 BEGIN */
SELECT emp.deptname
FROM emp
WHERE emp.empno NOT IN (SELECT dept.empno
FROM dept
WHERE emp.deptname = dept.deptname)
GROUP BY emp.deptname
HAVING AVG(empsalary) > 25000;
/* PROBLEM 3 END */

/* PROBLEM 4 BEGIN */
SELECT oth.empno, oth.empfname
FROM emp anr, emp oth
WHERE anr.empfname = 'Andrew'
AND anr.bossno = oth.bossno;
/* PROBLEM 4 END */

/* PROBLEM 5 BEGIN */
SELECT empno, empfname, empsalary
FROM emp 
WHERE empsalary = (SELECT MAX(oth.empsalary)
					FROM emp anr, emp oth 
					WHERE anr.empfname = 'Andrew' 
					AND anr.bossno = oth.bossno)
AND bossno = (SELECT bossno
				FROM emp 
				WHERE empfname = 'Andrew');
/* PROBLEM 5 END */

/* PROBLEM 6 BEGIN */
SELECT DISTINCT boss.empno, boss.empfname
FROM emp wrk, emp boss, dept
WHERE boss.empno NOT IN (SELECT dept.empno
                        FROM dept
                        WHERE boss.empno = dept.empno)
AND wrk.bossno = boss.empno;
/* PROBLEM 6 END */

/* PROBLEM 7 BEGIN */
SELECT prodid, proddesc, prodprice
FROM product
WHERE prodprice = (SELECT MAX(prodprice)
FROM product
WHERE prodid IN (SELECT subprodid
FROM product, assembly
WHERE proddesc = 'Animal photography kit'
AND product.prodid = assembly.prodid))
AND prodid IN (SELECT subprodid
FROM product, assembly
WHERE proddesc = 'Animal photography kit'
AND product.prodid = assembly.prodid);

/*find max subproduct*/
/* PROBLEM 7 END */
