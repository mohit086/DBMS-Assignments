insert into employee(fname, minit, lname, ssn, bdate, address,
sex, salary, super_ssn, dno)
VALUES ('John','B','Smith','123456789','1965-01-09','731 Fondren, Houston','M',30000,NULL,NULL);

insert into employee(fname, minit, lname, ssn, bdate, address,
sex, salary, super_ssn, dno)
VALUES ('Alice','P','Ford','223456789','1965-11-12','541 New Enclave, Stanford','F',40000,NULL,NULL);

insert into department(dname,dnumber,mgr_ssn,mgr_start_date)
VALUES ('Research',3,NULL,'1965-02-15');

insert into dept_locations(dnumber,dlocation)
VALUES (3,'Stanford');

insert into project(pname,pnumber,plocation,dnum)
VALUES ('ProductX',5,'Sugarland',NULL);

insert into works_on(essn,pno,hours)
VALUES ('123456789',5,30.5);

insert into dependent(essn,dependent_name,sex,bdate,relationship)
VALUES ('223456789','Roger','M','1987-05-07','Son');