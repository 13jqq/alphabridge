#SELECT count(*) FROM bridgedb.banker
#UNION
#SELECT count(*) FROM bridgedb.breaker;

drop database bridgedb;
create database bridgedb;

SET SQL_SAFE_UPDATES = 0;

CREATE TABLE bridgedb.banker (
    infoSet CHAR(30) NOT NULL, timestamp bigint, lastUtil double,
    regret0 DOUBLE, regret1 DOUBLE, regret2 DOUBLE, regret3 DOUBLE, regret4 DOUBLE, regret5 DOUBLE, regret6 DOUBLE,
    regret7 DOUBLE, regret8 DOUBLE, regret9 DOUBLE, regret10 DOUBLE, regret11 DOUBLE, regret12 DOUBLE,
    PRIMARY KEY (infoSet)
);

CREATE TABLE bridgedb.breaker (
    infoSet CHAR(30) NOT NULL, timestamp bigint, lastUtil double, 
    regret0 DOUBLE, regret1 DOUBLE, regret2 DOUBLE, regret3 DOUBLE, regret4 DOUBLE, regret5 DOUBLE, regret6 DOUBLE,
    regret7 DOUBLE, regret8 DOUBLE, regret9 DOUBLE, regret10 DOUBLE, regret11 DOUBLE, regret12 DOUBLE,
    PRIMARY KEY (infoSet)
);

CREATE TABLE bridgedb.gameInfo (
	timestamp bigint, stone int,
    player0_0 int, player0_1 int, player0_2 int, player0_3 int, player0_4 int, player0_5 int, player0_6 int,
    player0_7 int, player0_8 int, player0_9 int, player0_10 int, player0_11 int, player0_12 int, 
    player1_0 int, player1_1 int, player1_2 int, player1_3 int, player1_4 int, player1_5 int, player1_6 int,
    player1_7 int, player1_8 int, player1_9 int, player1_10 int, player1_11 int, player1_12 int,
    player2_0 int, player2_1 int, player2_2 int, player2_3 int, player2_4 int, player2_5 int, player2_6 int,
    player2_7 int, player2_8 int, player2_9 int, player2_10 int, player2_11 int, player2_12 int,
    player3_0 int, player3_1 int, player3_2 int, player3_3 int, player3_4 int, player3_5 int, player3_6 int,
    player3_7 int, player3_8 int, player3_9 int, player3_10 int, player3_11 int, player3_12 int,
    PRIMARY KEY (timestamp)
);

DELIMITER // 
create procedure bridgedb.INSERT_FOR_BANKER
(
    IN vinfoSet char(30), vtimestamp bigint, vlastUtil double,
    vregret0 double, vregret1 double, vregret2 double, vregret3 double, vregret4 double, vregret5 double, vregret6 double,
    vregret7 double, vregret8 double, vregret9 double, vregret10 double, vregret11 double, vregret12 double
)
begin
     insert into bridgedb.banker 
     (infoSet, timestamp, lastUtil, 
     regret0, regret1, regret2, regret3, regret4, regret5, regret6, 
     regret7, regret8, regret9, regret10, regret11, regret12) 
     values 
     (vinfoSet, vtimestamp, vlastUtil, 
     vregret0, vregret1, vregret2, vregret3, vregret4, vregret5, vregret6, 
     vregret7, vregret8, vregret9, vregret10, vregret11, vregret12);
end //
DELIMITER ;

DELIMITER // 
create procedure bridgedb.INSERT_FOR_BREAKER
(
    IN vinfoSet char(30), vtimestamp bigint, vlastUtil double,
    vregret0 double, vregret1 double, vregret2 double, vregret3 double, vregret4 double, vregret5 double, vregret6 double,
    vregret7 double, vregret8 double, vregret9 double, vregret10 double, vregret11 double, vregret12 double
)
begin
     insert into breaker 
     (infoSet, timestamp, lastUtil, 
     regret0, regret1, regret2, regret3, regret4, regret5, regret6, 
     regret7, regret8, regret9, regret10, regret11, regret12) 
     values 
     (vinfoSet, vtimestamp, vlastUtil, 
     vregret0, vregret1, vregret2, vregret3, vregret4, vregret5, vregret6, 
     vregret7, vregret8, vregret9, vregret10, vregret11, vregret12);
end //
DELIMITER ;

DELIMITER // 
create procedure bridgedb.UPDATE_FOR_BANKER
(
    IN vinfoSet char(30), vtimestamp bigint, vlastUtil double,
    vregret0 double, vregret1 double, vregret2 double, vregret3 double, vregret4 double, vregret5 double, vregret6 double,
    vregret7 double, vregret8 double, vregret9 double, vregret10 double, vregret11 double, vregret12 double
)
begin
     update bridgedb.banker 
     set
     timestamp=vtimestamp, lastUtil=vlastUtil, 
     regret0=vregret0, regret1=vregret1, regret2=vregret2, regret3=vregret3, regret4=vregret4, regret5=vregret5, regret6=vregret6, 
     regret7=vregret7, regret8=vregret8, regret9=vregret9, regret10=vregret10, regret11=vregret11, regret12=vregret12 
     where
     infoSet=vinfoSet;
end //
DELIMITER ;

DELIMITER // 
create procedure bridgedb.UPDATE_FOR_BREAKER
(
    IN vinfoSet char(30), vtimestamp bigint, vlastUtil double,
    vregret0 double, vregret1 double, vregret2 double, vregret3 double, vregret4 double, vregret5 double, vregret6 double,
    vregret7 double, vregret8 double, vregret9 double, vregret10 double, vregret11 double, vregret12 double
)
begin
     update bridgedb.breaker 
     set
     timestamp=vtimestamp, lastUtil=vlastUtil, 
     regret0=vregret0, regret1=vregret1, regret2=vregret2, regret3=vregret3, regret4=vregret4, regret5=vregret5, regret6=vregret6, 
     regret7=vregret7, regret8=vregret8, regret9=vregret9, regret10=vregret10, regret11=vregret11, regret12=vregret12 
     where
     infoSet=vinfoSet;
end //
DELIMITER ;

DELIMITER // 
create procedure bridgedb.STOREGAMEINFO
(
    IN vtimestamp bigint, IN vstone int,
    IN vplayer0_0 int, IN vplayer0_1 int, IN vplayer0_2 int, IN vplayer0_3 int, IN vplayer0_4 int, IN vplayer0_5 int, IN vplayer0_6 int,
    IN vplayer0_7 int, IN vplayer0_8 int, IN vplayer0_9 int, IN vplayer0_10 int, IN vplayer0_11 int, IN vplayer0_12 int,
    IN vplayer1_0 int, IN vplayer1_1 int, IN vplayer1_2 int, IN vplayer1_3 int, IN vplayer1_4 int, IN vplayer1_5 int, IN vplayer1_6 int,
    IN vplayer1_7 int, IN vplayer1_8 int, IN vplayer1_9 int, IN vplayer1_10 int, IN vplayer1_11 int, IN vplayer1_12 int,
    IN vplayer2_0 int, IN vplayer2_1 int, IN vplayer2_2 int, IN vplayer2_3 int, IN vplayer2_4 int, IN vplayer2_5 int, IN vplayer2_6 int,
    IN vplayer2_7 int, IN vplayer2_8 int, IN vplayer2_9 int, IN vplayer2_10 int, IN vplayer2_11 int, IN vplayer2_12 int,
    IN vplayer3_0 int, IN vplayer3_1 int, IN vplayer3_2 int, IN vplayer3_3 int, IN vplayer3_4 int, IN vplayer3_5 int, IN vplayer3_6 int,
    IN vplayer3_7 int, IN vplayer3_8 int, IN vplayer3_9 int, IN vplayer3_10 int, IN vplayer3_11 int, IN vplayer3_12 int
)
begin
     insert into bridgedb.gameInfo 
     (timestamp, stone,
     player0_0, player0_1, player0_2, player0_3, player0_4, player0_5, player0_6, 
     player0_7, player0_8, player0_9, player0_10, player0_11, player0_12,
     player1_0, player1_1, player1_2, player1_3, player1_4, player1_5, player1_6, 
     player1_7, player1_8, player1_9, player1_10, player1_11, player1_12,
     player2_0, player2_1, player2_2, player2_3, player2_4, player2_5, player2_6, 
     player2_7, player2_8, player2_9, player2_10, player2_11, player2_12,
     player3_0, player3_1, player3_2, player3_3, player3_4, player3_5, player3_6, 
     player3_7, player3_8, player3_9, player3_10, player3_11, player3_12) 
     values 
     (vtimestamp, vstone,
     vplayer0_0, vplayer0_1, vplayer0_2, vplayer0_3, vplayer0_4, vplayer0_5, vplayer0_6, 
     vplayer0_7, vplayer0_8, vplayer0_9, vplayer0_10, vplayer0_11, vplayer0_12,
     vplayer1_0, vplayer1_1, vplayer1_2, vplayer1_3, vplayer1_4, vplayer1_5, vplayer1_6, 
     vplayer1_7, vplayer1_8, vplayer1_9, vplayer1_10, vplayer1_11, vplayer1_12,
     vplayer2_0, vplayer2_1, vplayer2_2, vplayer2_3, vplayer2_4, vplayer2_5, vplayer2_6, 
     vplayer2_7, vplayer2_8, vplayer2_9, vplayer2_10, vplayer2_11, vplayer2_12,
     vplayer3_0, vplayer3_1, vplayer3_2, vplayer3_3, vplayer3_4, vplayer3_5, vplayer3_6, 
     vplayer3_7, vplayer3_8, vplayer3_9, vplayer3_10, vplayer3_11, vplayer3_12);
end //
DELIMITER ;