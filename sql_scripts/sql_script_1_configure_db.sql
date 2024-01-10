create table public.sockets (
	id int not null primary key,
	name varchar(30) not null,
	ipaddress inet not null,
	port integer not null,
	constraint port check (port >= 0 and port <= 65535)
);
