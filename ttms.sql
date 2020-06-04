#下面是本项目数据库创建语句,相应的主外键关系,级联删除,级联更新,索引,唯一约束,非空约束都在建表语句中已包含.
#user 用户表
CREATE TABLE `user` (                                    
   `uid` int(11) NOT NULL AUTO_INCREMENT,                 
   `userName` varchar(30) NOT NULL,    
   `passWord` varchar(32) NOT NULL,    
   `sex` enum('w','m') DEFAULT 'm',                       
   `phoneNumber` varchar(12) NOT NULL, 
   `mibao` varchar(20)  DEFAULT NULL,   
   `power` tinyint(4) NOT NULL,                           
   PRIMARY KEY (`uid`)                                    
 ) ENGINE=InnoDB DEFAULT CHARSET=utf8; 

#studio 影院表
CREATE TABLE `studio` (                                                   
   `sid` int(11) NOT NULL AUTO_INCREMENT,                                  
   `studioName` varchar(50) NOT NULL,                   
   `row` int(11) NOT NULL,                                                 
   `col` int(11) NOT NULL,                                                 
   `introduce` varchar(128) DEFAULT 'this is a studio', 
   PRIMARY KEY (`sid`)                                                     
 ) ENGINE=InnoDB DEFAULT CHARSET=utf8;

#seat 影院座位表
CREATE TABLE `seat` (                                                                                 
   `seatid` int(11) NOT NULL AUTO_INCREMENT,                                                           
   `studioid` int(11) NOT NULL,                                                                        
   `row` int(11) NOT NULL,                                                                             
   `col` int(11) NOT NULL,                                                                             
   `status` tinyint(4) DEFAULT '1',                                                                    
   PRIMARY KEY (`seatid`),                                                                             
   KEY `fk_studio_major` (`studioid`),                                                                 
   CONSTRAINT `fk_studio_major` FOREIGN KEY (`studioid`) REFERENCES `studio` (`sid`) ON DELETE CASCADE 
 ) ENGINE=InnoDB DEFAULT CHARSET=utf8;

#movie 电影表
CREATE TABLE `movie` (                                                                                                   
   `mid` int(11) NOT NULL AUTO_INCREMENT,                                                                                 
   `movieName` varchar(30) NOT NULL,                                                                                      
   `releaseTime` date NOT NULL,                                                                                           
   `projectionTime` date NOT NULL,                                                                                        
   `price` float NOT NULL,                                                                                                
   `introduce` varchar(128) DEFAULT 'this is a movie',                                                                    
   `pic` varchar(512) DEFAULT 'http://secretkeycrm.digifilm.com.cn/upload/20180530/e676f667c4cdfc7e074898adab2622f2.jpg', 
   `length` int(11) DEFAULT '90',                                                                                         
   PRIMARY KEY (`mid`),                                                                                                   
   KEY `idx_movie_projtime` (`projectionTime`)                                                                            
 ) ENGINE=InnoDB  DEFAULT CHARSET=utf8 ;
# CREATE INDEX idx_movie_projtime ON movie(projectionTime); 

#schedule 电影计划表
CREATE TABLE `schedule` (                                                                                
   `schid` int(11) NOT NULL AUTO_INCREMENT,                                                               
   `studioid` int(11) NOT NULL,                                                                           
   `movieid` int(11) NOT NULL,                                                                            
   `datetime` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,                                               
   PRIMARY KEY (`schid`),                                                                                 
   KEY `fk_moviesch_major` (`movieid`),                                                                   
   KEY `fk_studiosch_major` (`studioid`),                                                                 
   CONSTRAINT `fk_moviesch_major` FOREIGN KEY (`movieid`) REFERENCES `movie` (`mid`) ON DELETE CASCADE,   
   CONSTRAINT `fk_studiosch_major` FOREIGN KEY (`studioid`) REFERENCES `studio` (`sid`) ON DELETE CASCADE 
 ) ENGINE=InnoDB DEFAULT CHARSET=utf8;
    
#ticket 售票信息表
CREATE TABLE `ticket` (
	tickid int(11) NOT NULL AUTO_INCREMENT,
    userid int(11),
    studioid int(11),
    scheduleid int(11),
    movieid int(11),
    row INT NOT NULL,
    col INT NOT NULL,
    price FLOAT NOT NULL,
    status tinyint DEFAULT '1',
    PRIMARY KEY (tickid),
    UNIQUE KEY `scheduleid_row_col` (`scheduleid`,`row`,`col`), 
    KEY fk_ticketuserid_major(userid),
    KEY fk_ticketstudioid_major(studioid),
    KEY fk_ticketscheduleid_major(scheduleid),
    KEY fk_ticketmovieid_major(movieid),
    CONSTRAINT fk_ticketuserid_major FOREIGN KEY (userid) REFERENCES user(uid) ON DELETE SET NULL,
    CONSTRAINT fk_ticketstudioid_major FOREIGN KEY (studioid) REFERENCES studio(sid) ON DELETE SET NULL,
    CONSTRAINT fk_ticketscheduleid_major FOREIGN KEY (scheduleid) REFERENCES schedule(schid) ON DELETE SET NULL,
    CONSTRAINT fk_ticketmovieid_major FOREIGN KEY(movieid) REFERENCES movie(mid) ON DELETE SET NULL
    ) ENGINE=InnoDB DEFAULT CHARSET=utf8; 
            
#视图 查看电影计划完整信息
CREATE VIEW view_schedule AS SELECT schid,studioid,studioName,movieid,movieName,datetime FROM schedule
	INNER JOIN movie ON movie.`mid` = movieid
    INNER JOIN studio ON studio.sid = studioid; 

#视图 统计票房
CREATE VIEW view_movie_record AS SELECT movieid,movieName,sum(ticket.price) box_office FROM ticket
	INNER JOIN movie ON movie.mid = movieid GROUP BY movieid ORDER BY box_office DESC;  

#触发器 当新生成影厅时自动生成该影厅座位
DELIMITER //
CREATE TRIGGER insert_seat AFTER INSERT ON studio FOR EACH ROW
BEGIN
	DECLARE  rownum SMALLINT DEFAULT NEW.row;
    DECLARE  colnum SMALLINT DEFAULT NEW.col;
    DECLARE  studioid SMALLINT DEFAULT NEW.sid;
    DECLARE i SMALLINT DEFAULT 1;
    DECLARE j SMALLINT DEFAULT 1;
    WHILE i <= rownum DO
		SET j = 1;
		WHILE j <= colnum DO
            INSERT INTO seat values(NULL,studioid,i,j,1);
            SET j = j+1;
		END WHILE;
		SET i=i+1;
	END WHILE;
END //
DELIMITER ;
