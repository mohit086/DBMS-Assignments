drop table employee;
drop table department;
drop table dept_locations;
drop table works_on;
drop table dependent;
drop table project;

alter table employee
drop FOREIGN KEY fk_super_ssn;

alter table employee
drop FOREIGN KEY fk__dno;

alter table department
drop FOREIGN KEY fk_mgr;

alter table dept_locations
drop FOREIGN KEY fk_dnum;

alter table project
drop FOREIGN KEY fk_dnum2;

alter table works_on
drop FOREIGN KEY fk_pno;

alter table works_on
drop FOREIGN KEY fk_essn;

alter table dependent
drop FOREIGN KEY dep_ssn;