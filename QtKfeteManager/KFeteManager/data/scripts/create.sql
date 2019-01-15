--Notes file is parsed by the program.
--Notes:
--File does not support multiline comments.
--Each statement has to start with a comment line containing 'Statement' with no space and with that exact CaSe.

--Statement
--1
PRAGMA foreign_keys = ON;

--Statement
--2
CREATE TABLE IF NOT EXISTS
Articles(
    Name TEXT NOT NULL PRIMARY KEY,
    sellPrice NUMERIC NOT NULL,
    jShare NUMERIC NOT NULL,
    bPrice NUMERIC NOT NULL,
    reducedPrice NUMERIC NOT NULL,
    function INTEGER DEFAULT 0 REFERENCES Functions(Id) ON UPDATE CASCADE ON DELETE SET DEFAULT);

--Statement
--3
CREATE TABLE IF NOT EXISTS
Functions(
    name TEXT NOT NULL UNIQUE COLLATE NOCASE, --Functions with the same name but different case will be considered identical
    Id INTEGER PRIMARY KEY);

--Statement
--4
CREATE TABLE IF NOT EXISTS
Clients(
    Name TEXT NOT NULL PRIMARY KEY,
    phone TEXT DEFAULT '',
    address TEXT DEFAULT '',
    email TEXT DEFAULT '',
    negLimit NUMERIC DEFAULT 0 CHECK(negLimit >= -10),
    isJobist INTEGER DEFAULT 0,
    balance NUMERIC NOT NULL DEFAULT 0);

--Statement
--5
CREATE TABLE IF NOT EXISTS
SaleSessions(
    OpeningTime DATE NOT NULL PRIMARY KEY,
    closingTime DATE,
    state TEXT NOT NULL DEFAULT 'opened' CHECK(state IN('opened', 'closed')),
    openAmount NUMERIC,
    closeAmount NUMERIC,
    cashSoldAmount NUMERIC DEFAULT 0,
    CHECK (((closingTime IS NULL) AND state = 'opened') OR ((closingTime > OpeningTime) AND state = 'closed')),
    CHECK (((closeAmount IS NULL) AND state = 'opened') OR ((closeAmount IS NOT NULL) AND state = 'closed'))); --To be stored in case prices changed after the sale

--Statement
--6
CREATE TABLE IF NOT EXISTS
HeldSession(
    Name TEXT NOT NULL REFERENCES Clients(Name) ON DELETE CASCADE ON UPDATE CASCADE,
    SessionTime DATE NOT NULL REFERENCES SaleSessions(OpeningTime) ON DELETE CASCADE ON UPDATE CASCADE,
    PRIMARY KEY(Name, SessionTime));

--Statement
--7
CREATE TABLE IF NOT EXISTS
Transactions(
    Id INTEGER PRIMARY KEY,
    sessionTime DATE NOT NULL REFERENCES SaleSessions(OpeningTime) ON DELETE CASCADE,
    lineNumber INT NOT NULL,
    processTime DATE NOT NULL,
    total REAL NOT NULL,
    UNIQUE(sessionTime, lineNumber));

--Statement
--8
CREATE TABLE IF NOT EXISTS
IsOrder(
    Id INTEGER PRIMARY KEY REFERENCES Transactions(Id) ON DELETE CASCADE,
    price TEXT NOT NULL,
    CHECK (price IN('normal', 'reduced', 'free')));

--Statement
--9
CREATE TABLE IF NOT EXISTS
OrderContent(
    Id INTEGER REFERENCES IsOrder(Id) ON DELETE CASCADE,
    article TEXT NOT NULL REFERENCES Articles(name) ON DELETE NO ACTION,
    quantity INTEGER NOT NULL,
    PRIMARY KEY(Id, article));

--Statement
--10
CREATE TABLE IF NOT EXISTS
OrderClient(
    Id INTEGER PRIMARY KEY REFERENCES IsOrder(Id) ON DELETE CASCADE,
    client TEXT NOT NULL REFERENCES Clients(Name) ON DELETE NO ACTION);

--Statement
--11
CREATE TABLE IF NOT EXISTS
CashMoves(
    Id INTEGER PRIMARY KEY REFERENCES Transactions(Id) ON DELETE CASCADE,
    client TEXT REFERENCES Clients(Name) ON DELETE NO ACTION);

--Statement
--12
CREATE TABLE IF NOT EXISTS
Config(
    Field TEXT NOT NULL PRIMARY KEY,
    value TEXT);

--Statement
--13
CREATE TRIGGER order_check
BEFORE INSERT ON IsOrder
WHEN NEW.Id IN (SELECT Id FROM CashMoves)
BEGIN
    SELECT Raise(FAIL, 'Line number is already a cash move (withdraw/deposit).');
END;

--Statement
--14
CREATE TRIGGER cashMove_check
BEFORE INSERT ON CashMoves
WHEN NEW.Id IN (SELECT Id FROM IsOrder)
BEGIN
    SELECT Raise(FAIL, 'Line number is already an order.');
END;

--Statement
--15
CREATE TRIGGER transaction_time
BEFORE INSERT ON Transactions
WHEN NEW.processTime < NEW.sessionTime
BEGIN
SELECT Raise(Fail, 'Added Transaction happened before the begining of the session.');
END;

--Statement
--16
CREATE TRIGGER transaction_session_state
BEFORE INSERT ON Transactions
WHEN (SELECT state FROM SaleSessions WHERE SaleSessions.OpeningTime = NEW.sessionTime) = 'closed'
BEGIN
SELECT Raise(Fail, 'Cannot add a transaction to a closed sale session.');
END;

--Statement
--17
CREATE TRIGGER CurrentSession_exists
BEFORE UPDATE OF value ON Config
WHEN (NEW.Field='CurrentSession' AND ((NEW.value NOT IN (SELECT OpeningTime FROM SaleSessions)) AND (New.value NOT NULL)))
BEGIN
SELECT Raise(Fail, 'Current Session must be an existing session or null.');
END;

--Statement
--18
CREATE TRIGGER CurrentSessionOrderId_valid
BEFORE UPDATE OF value ON Config
WHEN (NEW.Field='CurrentSessionOrderId' AND (NEW.value < 0 OR NEW.value IS NULL))
BEGIN
SELECT Raise(Fail, 'Current Session Order Id must be a positive Integer.');
END;

--19 to 25
--Statement
INSERT OR REPLACE INTO Functions VALUES('No Function', 0);

--Statement
INSERT OR IGNORE INTO Config VALUES('CurrentSession', NULL);

--Statement
INSERT OR IGNORE INTO Config VALUES('CurrentSessionOrderId', 1);

--Statement
INSERT OR IGNORE INTO Config VALUES('PinHash', ''); --TODO Edit for real default hash

--Statement
INSERT OR IGNORE INTO Config VALUES('FirstStart', 1);

--Statement
INSERT OR IGNORE INTO Config VALUES('ManagerHash', ''); --TODO Edit for real default hash

--Statement
INSERT OR IGNORE INTO Config VALUES('TreasurerHash', ''); --TODO Edit for real default hash

-- For the maybe to come history database
--CREATE TABLE IF NOT EXISTS
--SaleSessions(
--    OpeningTime DATE NOT NULL PRIMARY KEY,
--    closingTime DATE,
--    jShare NUMERIC NOT NULL, --not in day to day database
--    openAmount NUMERIC,
--    closeAmount NUMERIC,
--    soldAmount NUMERIC);
