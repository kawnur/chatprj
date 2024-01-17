create table public.companions (
	id int not null primary key,
	name varchar(30) not null
);
	
create table public.sockets (
	id int not null primary key references public.companions (id),
	ipaddress inet not null,
	port integer not null,
	constraint port check (port >= 0 and port <= 65535)
);

create table public.messages (
	id int not null primary key,
	companion_id int not null references public.companions (id),	
	time timestamp NOT null,	
	message text,
	isSent boolean NOT null
);
