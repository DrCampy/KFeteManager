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
    name TEXT NOT NULL UNIQUE COLLATE NOCASE, --Functions with the same name but different case will be considered identical
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
Transactions(
    Id INTEGER PRIMARY KEY,
    sessionTime DATE NOT NULL REFERENCES SalesSessions(OpeningTime) ON DELETE CASCADE,
    lineNumber INT NOT NULL,
    total REAL NOT NULL,
    UNIQUE(sessionTime, lineNumber));

CREATE TABLE IF NOT EXISTS
IsOrder(
    Id INTEGER PRIMARY KEY REFERENCES Transactions(Id) ON DELETE CASCADE,
    price TEXT NOT NULL,
    CHECK (price IN('normal', 'reduced', 'free')));

CREATE TABLE IF NOT EXISTS
OrderContent(
    Id INTEGER PRIMARY KEY REFERENCES IsOrder(Id) ON DELETE CASCADE,
    article TEXT NOT NULL REFERENCES Articles(name) ON DELETE NO ACTION,
    quantity INTEGER NOT NULL);

CREATE TABLE IF NOT EXISTS
OrderClient(
    Id INTEGER PRIMARY KEY REFERENCES IsOrder(Id) ON DELETE CASCADE,
    client TEXT NOT NULL REFERENCES Clients(Name) ON DELETE NO ACTION);

CREATE TABLE IF NOT EXISTS
CashMoves(
    Id INTEGER PRIMARY KEY REFERENCES Transactions(Id) ON DELETE CASCADE,
    client TEXT REFERENCES Clients(Name) ON DELETE NO ACTION);

CREATE TABLE IF NOT EXISTS
Config(
    Field TEXT NOT NULL PRIMARY KEY,
    value TEXT);

INSERT OR REPLACE INTO Functions VALUES('No Function', 0);
INSERT OR IGNORE INTO Config VALUES('CurrentSession', '');
INSERT OR IGNORE INTO Config VALUES('CurrentSessionOrderId', 0);
INSERT OR IGNORE INTO Config VALUES('PinHash', ''); --TODO Edit for real default hash
INSERT OR IGNORE INTO Config VALUES('FirstStart', 1);
INSERT OR IGNORE INTO Config VALUES('ManagerHash', ''); --TODO Edit for real default hash
INSERT OR IGNORE INTO Config VALUES('TreasurerHash', ''); --TODO Edit for real default hash

--These statements have to be hardcoded because our parser is shitty
--CREATE TRIGGER order_check
--BEFORE INSERT ON IsOrder
--WHEN NEW.Id IN (SELECT Id FROM CashMoves)
--BEGIN
--    SELECT Raise(FAIL, 'Line number is already a cash move (withdraw/deposit)');
--END;

--CREATE TRIGGER cashMove_check
--BEFORE INSERT ON CashMoves
--WHEN NEW.Id IN (SELECT Id FROM IsOrder)
--BEGIN
--    SELECT Raise(FAIL, 'Line number is already an order.');
--END;

-- For the maybe to come history database
--CREATE TABLE IF NOT EXISTS
--SaleSessions(
--    OpeningTime DATE NOT NULL PRIMARY KEY,
--    closingTime DATE,
--    jShare NUMERIC NOT NULL, --not in day to day database
--    openAmount NUMERIC,
--    closeAmount NUMERIC,
--    soldAmount NUMERIC);
