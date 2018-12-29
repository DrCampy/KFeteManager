PRAGMA foreign_keys = ON;

CREATE TABLE IF NOT EXISTS
Articles(
    Name TEXT NOT NULL PRIMARY KEY,
    sellPrice NUMERIC NOT NULL,
    jShare NUMERIC NOT NULL,
    bPrice NUMERIC NOT NULL,
    reducedPrice NUMERIC NOT NULL,
    function INTEGER DEFAULT 0 REFERENCES Functions(Id) ON UPDATE CASCADE ON DELETE SET DEFAULT);

CREATE TABLE IF NOT EXISTS
Functions(
    Name TEXT NOT NULL UNIQUE COLLATE NOCASE, --Functions with the same name but different case will be considered identical
    Id INTEGER PRIMARY KEY);

CREATE TABLE IF NOT EXISTS
Clients(
    Name TEXT NOT NULL PRIMARY KEY,
    phone TEXT,
    address TEXT,
    email TEXT,
    negLimit NUMERIC,
    isJobist INTEGER,
    balance NUMERIC NOT NULL DEFAULT 0);

CREATE TABLE IF NOT EXISTS
SaleSessions(
    OpeningTime DATE NOT NULL PRIMARY KEY,
    closingTime DATE,
    openAmount NUMERIC,
    closeAmount NUMERIC,
    soldAmount NUMERIC); --To be stored in case prices changed after the sale

CREATE TABLE IF NOT EXISTS
HeldSession(
    Name TEXT NOT NULL REFERENCES Clients(Name) ON DELETE CASCADE ON UPDATE CASCADE,
    SessionTime DATE NOT NULL REFERENCES SaleSessions(OpeningTime) ON DELETE CASCADE ON UPDATE CASCADE,
    PRIMARY KEY(Name, SessionTime));

CREATE TABLE IF NOT EXISTS
OrderDetails(
    OrderId INTEGER PRIMARY KEY,
    sessionTime DATE NOT NULL REFERENCES SaleSessions(OpeningTime) ON DELETE CASCADE,
    orderNumber INTEGER NOT NULL,
    UNIQUE(sessionTime, orderNumber));

CREATE TABLE IF NOT EXISTS
OrderContent(
    OrderId INTEGER NOT NULL REFERENCES OrderDetails(OrderId) ON DELETE CASCADE,
    Article TEXT NOT NULL REFERENCES Articles(Name) ON DELETE NO ACTION,
    amount INTEGER NOT NULL CHECK(amount > 0),
    PRIMARY KEY(OrderId, Article));

CREATE TABLE IF NOT EXISTS
OrderClient(
    OrderId INTEGER NOT NULL REFERENCES OrderDetails(OrderId) ON DELETE CASCADE,
    Client TEXT NOT NULL REFERENCES Clients(Name) ON DELETE NO ACTION,
    PRIMARY KEY(OrderId, Client));

CREATE TABLE IF NOT EXISTS
Config(
    Field TEXT NOT NULL PRIMARY KEY,
    value TEXT);

CREATE TABLE IF NOT EXISTS
Prices(
    Id INTEGER PRIMARY KEY,
    name TEXT NOT NULL UNIQUE);

INSERT OR REPLACE INTO Functions VALUES('Unknown Function', 0);
INSERT OR IGNORE INTO Config VALUES("CurrentSession", "");
INSERT OR IGNORE INTO Config VALUES("CurrentSessionOrderId", 0);
INSERT OR IGNORE INTO Config VALUES("PinHash", ""); --TODO Edit for real default hash
INSERT OR IGNORE INTO Config VALUES("FirstStart", 1);
INSERT OR IGNORE INTO Config VALUES("ManagerHash", ""); --TODO Edit for real default hash
INSERT OR IGNORE INTO Config VALUES("TreasurerHash", ""); --TODO Edit for real default hash
INSERT OR REPLACE INTO Prices(name) VALUES("normal");
INSERT OR REPLACE INTO Prices(name) VALUES("reduced");
INSERT OR REPLACE INTO Prices(name) VALUES("free");

-- For the maybe to come history database
--CREATE TABLE IF NOT EXISTS
--SaleSessions(
--    OpeningTime DATE NOT NULL PRIMARY KEY,
--    closingTime DATE,
--    jShare NUMERIC NOT NULL, --not in day to day database
--    openAmount NUMERIC,
--    closeAmount NUMERIC,
--    soldAmount NUMERIC);
