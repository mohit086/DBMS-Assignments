create table department(
dname varchar(30) NOT NULL,
dnumber int,
mgr_ssn char(9),
mgr_start_date date,
constraint pk_department PRIMARY KEY (dnumber)
);

create table employee(
fname varchar(30) NOT NULL,
minit varchar(30),
lname varchar(30) NOT NULL,
ssn char(9),
bdate Date,
address varchar(50),
sex char(1),
salary int,
super_ssn char(9),
dno int,
constraint emp_ssn PRIMARY KEY (ssn)
);

create table dept_locations(
dnumber int,
dlocation varchar(30),
constraint dept_no PRIMARY KEY (dnumber, dlocation)
);

create table works_on(
essn char(9),
pno int,
hours decimal(3,1),
constraint work_hours PRIMARY KEY (essn, pno)
);

create table project(
pname varchar(40) NOT NULL,
pnumber int,
plocation varchar(20),
dnum int,
constraint p_num PRIMARY KEY (pnumber)
);

create table dependent(
essn char(9),
dependent_name varchar(20),
sex char(1),
bdate date,
relationship varchar(20),
constraint d_name PRIMARY KEY (essn, dependent_name)
);