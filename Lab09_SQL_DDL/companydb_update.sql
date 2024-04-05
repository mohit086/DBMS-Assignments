update employee
set dno = 3
where ssn = '123456789';

update employee
set super_ssn = '123456789', dno = 3
where ssn = '223456789';

update department
set mgr_ssn = '123456789'
where dnumber = 3;

update project
set dnum = 3
where pnumber = 5;