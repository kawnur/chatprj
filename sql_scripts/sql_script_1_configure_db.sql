create table public.companions (
    id int not null primary key generated always as identity,
    name varchar(30) not null unique
);
	
create table public.sockets (
    id int not null primary key references public.companions (id),
    ipaddress inet not null,
    port integer not null,
    constraint port check (port >= 0 and port <= 65535)
);

create table public.messages (
    id int not null primary key generated always as identity,
    companion_id int not null references public.companions (id),
    author_id int not null references public.companions (id),
    timestamp_tz timestamp with time zone not null,
    message text,
    isSent boolean not null
);
